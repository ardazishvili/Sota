#include "ridge_impl/ridge_hex_mesh.h"

#include <unordered_set>

#include "core/general_utility.h"
#include "core/utils.h"
#include "misc/utilities.h"
#include "tal/callable.h"
#include "tal/godot_core.h"
#include "tal/vector2.h"
#include "tal/vector3.h"

namespace sota {

void RidgeHexMesh::_bind_methods() {}

void RidgeHexMesh::set_plain_noise(Ref<FastNoiseLite> p_noise) {
  _plain_noise = p_noise;
  if (_plain_noise.ptr()) {
    _plain_noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void RidgeHexMesh::set_ridge_noise(Ref<FastNoiseLite> p_ridge_noise) {
  _ridge_noise = p_ridge_noise;
  if (_ridge_noise.ptr()) {
    _ridge_noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void RidgeHexMesh::calculate_corner_points_distances_to_border() {
  std::unordered_set<int> exclude_list;
  for (int i = 0; i < 6; ++i) {
    if (_neighbours[i]) {
      exclude_list.insert(i);
    }
  }
  auto [coeffs, coeffs_precalc] = GeneralUtility::get_border_line_coeffs(_R, _r, exclude_list);

  std::map<Vector3, float> neighbours_corner_points_distances_to_border;
  for (const auto& n : _neighbours) {
    if (n) {
      auto tmp = dynamic_cast<RidgeHexMesh*>(n)->get_corner_points_distances_to_border();
      neighbours_corner_points_distances_to_border.insert(tmp.begin(), tmp.end());
    }
  }

  auto corner_points = _hex.points();
  auto center = _hex.center();

  float distance_to_border;
  for (auto& v : corner_points) {
    distance_to_border = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < coeffs.size(); ++i) {
      distance_to_border = std::min(distance_to_border, std::abs(coeffs[i][0] * (v.x - center.x) +
                                                                 coeffs[i][1] * (v.z - center.z) + coeffs[i][2]) /
                                                            coeffs_precalc[i]);
    }
    for (const auto& [point, d] : neighbours_corner_points_distances_to_border) {
      distance_to_border = std::min(distance_to_border, (Vector2(v.x, v.z) - Vector2(point.x, point.z)).length() + d);
    }
    _corner_points_to_border_dist[v] = distance_to_border;
  }
}

void RidgeHexMesh::shift_compress() {
  auto center = _hex.center();

  if (tesselation_type == HexMesh::TesselationType::Plane) {
    GeneralUtility::shift_compress(vertices_, _y_shift, _y_compress, center.y);
  } else if (tesselation_type == TesselationType::Polyhedron) {
    vertices_ =
        GeneralUtility::shift_compress_polyhedron(vertices_, _initial_vertices, _y_shift, _y_compress, center.y);
  } else {
    printerr("unknown type of RidgeHexMesh");
  }
}

void RidgeHexMesh::calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                                 float ridge_offset) {
  auto center = _hex.center();
  shift_compress();

  std::vector<Vector3> ridge_points;
  for (const Ridge* ridge : _ridges) {
    Vector3 s = ridge->start();
    Vector3 e = ridge->end();

    if ((Vector2(s.x, s.z) - Vector2(center.x, center.z)).length() < 2 * _R ||
        (Vector2(e.x, e.z) - Vector2(center.x, center.z)).length() < 2 * _R) {
      auto p = ridge->get_points();
      ridge_points.insert(ridge_points.end(), p.begin(), p.end());
    }
  }
  std::unordered_set<int> exclude_list;
  for (int i = 0; i < 6; ++i) {
    if (_neighbours[i]) {
      exclude_list.insert(i);
    }
  }
  auto [coeffs, coeffs_precalc] = GeneralUtility::get_border_line_coeffs(_R, _r, exclude_list);

  std::map<Vector3, float> neighbours_corner_points_distances_to_border;
  for (const HexMesh* n : _neighbours) {
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
      distance_to_border = std::min(distance_to_border, std::abs(coeffs[i][0] * (v.x - center.x) +
                                                                 coeffs[i][1] * (v.z - center.z) + coeffs[i][2]) /
                                                            coeffs_precalc[i]);
    }
    for (const auto& [point, d] : neighbours_corner_points_distances_to_border) {
      distance_to_border = std::min(distance_to_border, (Vector2(v.x, v.z) - Vector2(point.x, point.z)).length() + d);
    }
    auto closestRidgePoint = [ridge_points](Vector2 p) -> Vector3 {
      auto it = std::min_element(ridge_points.begin(), ridge_points.end(), [p](Vector3 lhs, Vector3 rhs) {
        return p.distance_to(Vector2(lhs.x, lhs.z)) < p.distance_to(Vector2(rhs.x, rhs.z));
      });
      return it != ridge_points.end() ? *it
                                      : Vector3{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                                std::numeric_limits<float>::max()};
    };

    Vector3 crp = closestRidgePoint(Vector2(v.x, v.z));

    float distance_to_ridge_projection = Vector2(crp.x, crp.z).distance_to(Vector2(v.x, v.z));
    float approx_end = crp.y;
    auto t = [](float to_border, float to_projection) { return to_border / (to_border + to_projection); };

    float n = _ridge_noise.ptr() ? std::abs(_ridge_noise->get_noise_2d(v.x, v.z)) * 0.289 : 0;
    auto t_perlin = [distance_to_border, ridge_offset](float y) -> float {
      if (epsilonEqual(distance_to_border, 0.0f)) {
        return 0.0f;
      }
      return 1 - (ridge_offset - y * 2) / ridge_offset;
    };

    v.y = center.y + interpolation_func(v.y, approx_end, t(distance_to_border, distance_to_ridge_projection));
    v.y -= std::lerp(0, n, t_perlin(v.y));
  }

  for (auto& v : vertices_) {
    _min_y = std::min(_min_y, v.y);
  }
  for (auto& v : vertices_) {
    _max_y = std::max(_max_y, v.y);
  }
}

void RidgeHexMesh::calculate_initial_heights() {
  auto normal = _hex.normal();

  if (tesselation_type == HexMesh::TesselationType::Plane) {
    for (auto& v : vertices_) {
      float n = _plain_noise.ptr() ? _plain_noise->get_noise_2d(v.x, v.z) : 0.0;
      v += n * normal;
      _min_y = std::min(_min_y, v.y);
      _max_y = std::max(_max_y, v.y);
    }
  } else if (tesselation_type == HexMesh::TesselationType::Polyhedron) {
    _initial_vertices = vertices_;
    for (auto& v : vertices_) {
      float n = _plain_noise.ptr() ? _plain_noise->get_noise_3d(v.x, v.y, v.z) : 0.0;
      Vector3 old = v;
      v += n * v.normalized();
      _min_y = std::min(_min_y, v.length() - old.length());
      _max_y = std::max(_max_y, v.length() - old.length());
    }
  } else {
    printerr("unknown type of RidgeHexMesh");
  }

  request_update();
}

GroupedHexagonMeshVertices RidgeHexMesh::get_grouped_vertices() {
  GroupedHexagonMeshVertices vertex_groups;
  int size = vertices_.size();
  for (int i = 0; i < size; ++i) {
    Vector3 v = vertices_[i];
    Vector3& n = normals_[i];
    auto p = to_point_divisioned_position(v, _diameter, _divisions);
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}

std::vector<HexMesh*> RidgeHexMesh::get_neighbours() const {
  std::vector<HexMesh*> res;
  std::copy_if(_neighbours.begin(), _neighbours.end(), std::back_inserter(res),
               [](HexMesh* n) { return n != nullptr; });
  return res;
}

void RidgeHexMesh::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

}  // namespace sota
