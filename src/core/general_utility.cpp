#include "core/general_utility.h"

#include <cmath>       // for lerp
#include <functional>  // for function
#include <optional>

#include "core/utils.h"          // for epsilonEqual
#include "misc/discretizer.h"    // for Discretizer
#include "misc/types.h"          // for GroupedMeshVer...
#include "misc/utilities.h"      // for to_point_divis...
#include "primitives/polygon.h"  // for RegularPolygon
#include "ridge_impl/ridge.h"    // for Ridge
#include "tal/arrays.h"          // for Vector3Array
#include "tal/godot_core.h"
#include "tal/noise.h"      // for FastNoiseLite
#include "tal/reference.h"  // for Ref
#include "tal/vector2.h"    // for Vector2
#include "tal/vector3.h"    // for Vector3

namespace sota {

void GeneralUtility::make_smooth_normals(std::vector<DiscreteVertexToNormals>& vertex_groups) {
  DiscreteVertexToNormals all;
  for (auto& g : vertex_groups) {
    for (auto it = g.begin(); it != g.end(); ++it) {
      all[it->first].insert(all[it->first].end(), it->second.begin(), it->second.end());
    }
  }

  for (auto it = all.begin(); it != all.end(); ++it) {
    auto& series_of_normals = it->second;
    Vector3 normal(0.0, 0.0, 0.0);
    for (auto* n : series_of_normals) {
      normal += *n;
    }

    normal /= series_of_normals.size();
    normal.normalize();
    for (auto* n : series_of_normals) {
      *n = normal;
    }
  }
}

std::pair<std::vector<std::array<float, 3>>, std::vector<float>> GeneralUtility::get_border_line_coeffs(
    float R, float r, std::set<int> exclude_list) {
  auto get_coeffs = [R, r, exclude_list]() -> std::vector<std::array<float, 3>> {
    std::array<std::array<float, 3>, 6> all = HexBorderLineParams(R, r).get_coeffs();

    std::vector<std::array<float, 3>> _coeffs;
    for (int i = 0; i < 6; ++i) {
      if (!exclude_list.empty() && exclude_list.contains(i)) {
        continue;
      }
      _coeffs.push_back(all[i]);
    }
    return _coeffs;
  };
  std::vector<std::array<float, 3>> _coeffs = get_coeffs();
  unsigned int coeffs_size = _coeffs.size();
  std::vector<float> coeffs_precalc(coeffs_size);
  for (unsigned int i = 0; i < coeffs_size; ++i) {
    coeffs_precalc[i] = Vector2(_coeffs[i][0], _coeffs[i][1]).length();
  }

  return {_coeffs, coeffs_precalc};
}

// FlatMeshProcessor definitions
Vector3Array FlatMeshProcessor::shift_compress(Vector3Array vertices, float shift, float compress, float offset) {
  Vector3Array result;
  int n = vertices.size();
  for (int i = 0; i < n; ++i) {
    Vector3 p = vertices[i];
    p.y += shift;
    p.y *= compress;
    p.y += offset;
    result.push_back(p);
  }
  return result;
}
void FlatMeshProcessor::calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height,
                                                  float& max_height, Vector3 normal) {
  for (auto& v : vertices) {
    float n = noise.ptr() ? noise->get_noise_2d(v.x, v.z) : 0.0;
    v += n * normal;
    min_height = std::min(min_height, v.y);
    max_height = std::max(max_height, v.y);
  }
}
void FlatMeshProcessor::calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) {
  auto t = [r, this](float dist_to_center_axis) -> float { return (r - std::min(r, dist_to_center_axis)) / r; };
  for (auto& v : vertices) {
    v.y *= std::lerp(1.0f, 3.0f, t(Vector2(0, 0).distance_to(Vector2(v.x, v.z))));
  }
}

Vector3Array FlatMeshProcessor::calculate_ridge_based_heights(
    Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
    std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_list, float diameter, int divisions,
    DiscreteVertexToDistance& distance_map, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
    std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) {
  auto divisioned = [diameter, divisions](Vector3 point) {
    return VertexToNormalDiscretizer::get_discrete_vertex(point, diameter / (divisions * 2));
  };
  Vector3Array result;
  for (int i = 0; i < vertices.size(); ++i) {
    Vector3 v = vertices[i];
    auto center = base.center();
    std::vector<Vector3> ridge_points;
    for (const Ridge* ridge : ridges) {
      if (ridge->start().distance_to(center) < 2 * R || ridge->end().distance_to(center) < 2 * R) {
        auto p = ridge->get_points();
        ridge_points.insert(ridge_points.end(), p.begin(), p.end());
      }
    }

    PointToLineDistance_VectorMultBased calculator(exclude_list, base.points());
    float distance_to_border = calculator.calc(Vector3(v.x, 0, v.z));
    for (const auto& point : neighbours_corner_points) {
      distance_to_border = std::min(distance_to_border, Vector2(v.x, v.z).distance_to(Vector2(point.x, point.z)) +
                                                            distance_map[divisioned(point)]);
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

    float n = ridge_noise.ptr() ? std::abs(ridge_noise->get_noise_2d(v.x, v.z)) * 0.289 : 0;
    auto t_perlin = [distance_to_border, ridge_offset](float y) -> float {
      if (epsilonEqual(distance_to_border, 0.0f)) {
        return 0.0f;
      }
      return 1 - (ridge_offset - y * 2) / ridge_offset;
    };
    v.y = center.y + interpolation_func(v.y, approx_end, t(distance_to_border, distance_to_ridge_projection));
    v.y -= std::lerp(0, n, t_perlin(v.y));

    min_height = std::min(min_height, v.y);
    max_height = std::max(max_height, v.y);

    result.push_back(v);
  }

  return result;
}

// VolumeMeshProcessor definitions
Vector3Array VolumeMeshProcessor::shift_compress(Vector3Array vertices, float shift, float compress, float offset) {
  Vector3Array result;
  int n = vertices.size();
  for (int i = 0; i < n; ++i) {
    Vector3 dir = _initial_vertices[i].normalized();
    result.push_back(_initial_vertices[i] + dir * (vertices[i].length() - _initial_vertices[i].length()) * compress);
  }
  return result;
}

void VolumeMeshProcessor::calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height,
                                                    float& max_height, Vector3 normal) {
  for (auto& v : vertices) {
    v += v.normalized() * (1 - v.length());
  }
  for (auto& v : vertices) {
    float n = noise.ptr() ? noise->get_noise_3d(v.x, v.y, v.z) : 0.0;
    Vector3 old = v;
    v += n * v.normalized();
    min_height = std::min(min_height, v.length() - old.length());
    max_height = std::max(max_height, v.length() - old.length());
  }
}

void VolumeMeshProcessor::calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) {
  auto t = [r](float dist_to_center_axis) -> float { return (r - std::min(r, dist_to_center_axis)) / r; };
  for (auto& v : vertices) {
    float dist_to_center_axis = center.cross(v - center).length() / center.length();

    Vector3 normal = v.normalized();
    v += normal * std::lerp(0.0f, R / 4.0f, t(dist_to_center_axis * 1.2));
  }
}

Vector3Array VolumeMeshProcessor::calculate_ridge_based_heights(
    Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
    std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_list, float diameter, int divisions,
    DiscreteVertexToDistance& distance_map, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
    std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) {
  auto divisioned = [diameter, divisions](Vector3 point) {
    return VertexToNormalDiscretizer::get_discrete_vertex(point, diameter / (divisions * 2));
  };
  Vector3Array result;
  for (int i = 0; i < vertices.size(); ++i) {
    Vector3 v = _initial_vertices[i];
    auto center = base.center();
    std::vector<Vector3> ridge_points;
    for (const Ridge* ridge : ridges) {
      if (ridge->start().distance_to(center) < 2 * ridge_offset ||
          ridge->end().distance_to(center) < 2 * ridge_offset) {
        auto p = ridge->get_points();
        ridge_points.insert(ridge_points.end(), p.begin(), p.end());
      }
    }

    PointToLineDistance_VectorMultBased calculator(exclude_list, base.points());
    float distance_to_border = calculator.calc(v);
    for (const auto& point : neighbours_corner_points) {
      distance_to_border =
          std::min(distance_to_border, (v - point.normalized()).length() + distance_map[divisioned(point)]);
    }
    auto closestRidgePoint = [ridge_points](Vector3 p) -> std::optional<Vector3> {
      auto it = std::min_element(ridge_points.begin(), ridge_points.end(),
                                 [p](Vector3 lhs, Vector3 rhs) { return p.distance_to(lhs) < p.distance_to(rhs); });
      return it != ridge_points.end() ? *it : std::optional<Vector3>();
    };
    std::optional<Vector3> crp_opt = closestRidgePoint(v);
    if (!crp_opt) {
      printerr("Can't find closest ridge point, yield initial point");
      result.push_back(v);
      continue;
    }
    Vector3 crp = crp_opt.value();

    float distance_to_ridge_projection = v.cross(crp).length() / crp.length();
    Vector3 approx_end = crp;
    auto t = [](float to_border, float to_projection) { return to_border / (to_border + to_projection); };
    Vector3 direction = v.normalized() * (crp.length() - v.length() > 0 ? 1 : -1);
    float length = (approx_end - v).length();
    auto t_result = t(distance_to_border, distance_to_ridge_projection);

    // TODO add noise
    result.push_back(vertices[i] + t_result * direction * length);
  }

  return result;
}
}  // namespace sota
