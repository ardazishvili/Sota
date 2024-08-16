#include "ridge_hex_mesh.h"

#include "core/utils.h"
#include "misc/utilities.h"

namespace sota {

using namespace gd;

void RidgeHexMesh::_bind_methods() {}

void RidgeHexMesh::set_plain_noise(Ref<FastNoiseLite> p_noise) {
  plain_noise = p_noise;
  if (plain_noise.ptr()) {
    plain_noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void RidgeHexMesh::set_ridge_noise(Ref<FastNoiseLite> p_ridge_noise) {
  ridge_noise = p_ridge_noise;
  if (ridge_noise.ptr()) {
    ridge_noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void RidgeHexMesh::set_offset(Vector3 p_offset) {
  offset = p_offset;
  request_update();
}

void RidgeHexMesh::calculate_corner_points_distances_to_border() {
  auto [coeffs, coeffs_precalc] = get_border_line_coeffs();

  std::map<Vector3, float> neighbours_corner_points_distances_to_border;
  for (const auto& n : neighbours) {
    if (n) {
      auto tmp = dynamic_cast<RidgeHexMesh*>(n)->get_corner_points_distances_to_border();
      neighbours_corner_points_distances_to_border.insert(tmp.begin(), tmp.end());
    }
  }

  float distance_to_border;
  for (auto& v : _corner_points) {
    distance_to_border = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < coeffs.size(); ++i) {
      distance_to_border = std::min(
          distance_to_border, std::abs(coeffs[i][0] * v.x + coeffs[i][1] * v.z + coeffs[i][2]) / coeffs_precalc[i]);
    }
    for (const auto& [point, d] : neighbours_corner_points_distances_to_border) {
      distance_to_border = std::min(distance_to_border,
                                    (Vector2(offset.x + v.x, offset.z + v.z) - Vector2(point.x, point.z)).length() + d);
    }
    _corner_points_to_border_dist[Vector3(offset.x + v.x, offset.y + v.y, offset.z + v.z)] = distance_to_border;
  }
}

std::pair<std::vector<std::array<float, 3>>, std::vector<float>> RidgeHexMesh::get_border_line_coeffs() {
  auto get_coeffs = [this]() -> std::vector<std::array<float, 3>> {
    std::array<std::array<float, 3>, 6> all = HexBorderLineParams(R, r).get_coeffs();

    std::vector<std::array<float, 3>> coeffs;
    for (int i = 0; i < 6; ++i) {
      if (!neighbours[i]) {
        coeffs.push_back(all[i]);
      }
    }
    return coeffs;
  };
  std::vector<std::array<float, 3>> coeffs = get_coeffs();
  unsigned int coeffs_size = coeffs.size();
  std::vector<float> coeffs_precalc(coeffs_size);
  for (unsigned int i = 0; i < coeffs_size; ++i) {
    coeffs_precalc[i] = Vector2(coeffs[i][0], coeffs[i][1]).length();
  }

  return {coeffs, coeffs_precalc};
}

void RidgeHexMesh::shift_compress() {
  for (auto& v : vertices_) {
    v.y += _y_shift;
    v.y *= _y_compress;
    v.y += offset.y;
  }
}

void RidgeHexMesh::calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                                 float ridge_offset) {
  shift_compress();

  std::vector<Vector3> ridge_points;
  for (const Ridge* ridge : ridges) {
    Vector3 s = ridge->start();
    Vector3 e = ridge->end();

    if ((Vector2(s.x, s.z) - Vector2(offset.x, offset.z)).length() < 2 * R ||
        (Vector2(e.x, e.z) - Vector2(offset.x, offset.z)).length() < 2 * R) {
      auto p = ridge->get_points();
      ridge_points.insert(ridge_points.end(), p.begin(), p.end());
    }
  }
  auto [coeffs, coeffs_precalc] = get_border_line_coeffs();

  std::map<Vector3, float> neighbours_corner_points_distances_to_border;
  for (const HexMesh* n : neighbours) {
    if (n) {
      std::map<Vector3, float> tmp = dynamic_cast<const RidgeHexMesh*>(n)->get_corner_points_distances_to_border();
      neighbours_corner_points_distances_to_border.insert(tmp.begin(), tmp.end());
    }
  }

  float distance_to_border;
  unsigned int coeffs_size = coeffs.size();
  for (auto& v : vertices_) {
    distance_to_border = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < coeffs_size; ++i) {
      distance_to_border = std::min(
          distance_to_border, std::abs(coeffs[i][0] * v.x + coeffs[i][1] * v.z + coeffs[i][2]) / coeffs_precalc[i]);
    }
    for (const auto& [point, d] : neighbours_corner_points_distances_to_border) {
      distance_to_border = std::min(distance_to_border,
                                    (Vector2(offset.x + v.x, offset.z + v.z) - Vector2(point.x, point.z)).length() + d);
    }
    auto closestRidgePoint = [ridge_points](Vector2 p) -> Vector3 {
      auto it = std::min_element(ridge_points.begin(), ridge_points.end(), [p](Vector3 lhs, Vector3 rhs) {
        return p.distance_to(Vector2(lhs.x, lhs.z)) < p.distance_to(Vector2(rhs.x, rhs.z));
      });
      return it != ridge_points.end() ? *it
                                      : Vector3{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                                std::numeric_limits<float>::max()};
    };

    Vector3 crp = closestRidgePoint(Vector2(offset.x + v.x, offset.z + v.z));

    float distance_to_ridge_projection = Vector2(crp.x, crp.z).distance_to(Vector2(offset.x + v.x, offset.z + v.z));
    float approx_end = crp.y;
    auto t = [](float to_border, float to_projection) { return to_border / (to_border + to_projection); };

    float n = ridge_noise.ptr() ? std::abs(ridge_noise->get_noise_2d(offset.x + v.x, offset.z + v.z)) * 0.289 : 0;
    auto t_perlin = [distance_to_border, ridge_offset](float y) -> float {
      if (epsilonEqual(distance_to_border, 0.0f)) {
        return 0.0f;
      }
      return 1 - (ridge_offset - y * 2) / ridge_offset;
    };

    v.y = offset.y + interpolation_func(v.y, approx_end, t(distance_to_border, distance_to_ridge_projection));
    v.y -= std::lerp(0, n, t_perlin(v.y));
  }

  _min_y = std::min_element(vertices_.begin(), vertices_.end(), [](const auto& v1, const auto& v2) {
             return v1.y < v2.y;
           })->y;
  _max_y = std::max_element(vertices_.begin(), vertices_.end(), [](const auto& v1, const auto& v2) {
             return v1.y < v2.y;
           })->y;

  calculate_normals();
  request_update();
}

void RidgeHexMesh::calculate_initial_heights() {
  for (auto& v : vertices_) {
    float n = plain_noise.ptr() ? plain_noise->get_noise_2d(offset.x + v.x, offset.z + v.z) : 0.0;
    v.y = offset.y + n;
    _min_y = std::min(_min_y, v.y);
    _max_y = std::max(_max_y, v.y);
  }

  request_update();
}

GroupedHexagonMeshVertices RidgeHexMesh::get_grouped_vertices() {
  GroupedHexagonMeshVertices vertex_groups;
  int size = vertices_.size();
  for (int i = 0; i < size; ++i) {
    Vector3& v = vertices_[i];
    Vector3& n = normals_[i];
    auto p = to_point_divisioned_position(Vector3(offset.x + v.x, offset.y + v.y, offset.z + v.z), diameter, divisions);
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}
std::vector<HexMesh*> RidgeHexMesh::get_neighbours() const {
  std::vector<HexMesh*> res;
  std::copy_if(neighbours.begin(), neighbours.end(), std::back_inserter(res), [](HexMesh* n) { return n != nullptr; });
  return res;
}

void RidgeHexMesh::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

}  // namespace sota
