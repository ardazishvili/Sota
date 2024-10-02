#pragma once

#include <algorithm>   // for min
#include <array>       // for array
#include <cmath>       // for abs
#include <functional>  // for function
#include <limits>      // for numeric_limits
#include <map>         // for map
#include <set>         // for set
#include <utility>     // for pair
#include <vector>      // for vector

#include "misc/discretizer.h"
#include "primitives/polygon.h"
#include "ridge_impl/ridge.h"
#include "tal/arrays.h"     // for Vector3Array
#include "tal/noise.h"      // for FastNoiseLite
#include "tal/reference.h"  // for Ref
#include "tal/vector3.h"    // for Vector3
#include "tal/vector3i.h"

namespace sota {
class RegularPolygon;
class Ridge;

class MeshProcessor {
 public:
  virtual ~MeshProcessor() = default;
  virtual Vector3Array shift_compress(Vector3Array vertices, float shift, float compress, float offset) = 0;
  virtual void calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height,
                                         float& max_height, Vector3 normal) = 0;
  virtual void calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) = 0;
  virtual Vector3Array calculate_ridge_based_heights(
      Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_border_set, float diameter,
      int divisions, DiscreteVertexToDistance& distance_map, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
      std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) = 0;

 private:
};

class FlatMeshProcessor : public MeshProcessor {
 public:
  Vector3Array shift_compress(Vector3Array vertices, float shift, float compress, float offset) override;
  void calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height, float& max_height,
                                 Vector3 normal) override;
  void calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) override;
  Vector3Array calculate_ridge_based_heights(
      Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_border_set, float diameter,
      int divisions, DiscreteVertexToDistance& distance_map, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
      std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) override;

 private:
};

class VolumeMeshProcessor : public MeshProcessor {
 public:
  VolumeMeshProcessor(Vector3Array initial_vertices) : MeshProcessor(), _initial_vertices(initial_vertices) {}

  Vector3Array shift_compress(Vector3Array vertices, float shift, float compress, float offset) override;
  void calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height, float& max_height,
                                 Vector3 normal) override;
  void calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) override;
  Vector3Array calculate_ridge_based_heights(
      Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_border_set, float diameter,
      int divisions, DiscreteVertexToDistance& distance_map, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
      std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) override;

 private:
  Vector3Array _initial_vertices;
};

class GeneralUtility {
 public:
  static void make_smooth_normals(std::vector<DiscreteVertexToNormals>& normal_groups);
};

/**
 * @brief Parameters of line equations for Hexagon with center 0.0 and excircle radius = 1.0
 */
struct HexBorderLineParams {
  HexBorderLineParams(float R, float r) {
    _coeffs = {std::array<float, 3>{-R / (2 * r), 1.0f, R}, std::array<float, 3>{1.0f, 0.0f, -r},
               std::array<float, 3>{R / (2 * r), 1.0f, -R}, std::array<float, 3>{-R / (2 * r), 1.0f, -R},
               std::array<float, 3>{1.0f, 0.0f, r},         std::array<float, 3>{R / (2 * r), 1.0f, R}};
  }
  std::array<std::array<float, 3>, 6> get_coeffs() { return _coeffs; }

 private:
  std::array<std::array<float, 3>, 6> _coeffs;
};

/**
 * @brief Calculate distances to borders of regular hexagon
 *
 * Calculation is based on line equations => radiuses of incircle(r) and excircle(R) are used
 * If particular line must be excluded from calculation, pass its number in `exclude_border_set`
 */
class PointToLineDistance_EquationBased {
 public:
  PointToLineDistance_EquationBased(std::set<int> exclude_border_set, float R, float r)
      : _exclude_border_set(exclude_border_set), _R(R), _r(r) {
    auto [coeffs, coeffs_precalc] = get_border_line_coeffs(_R, _r, _exclude_border_set);
    _coeffs = coeffs;
    _coeffs_precalc = coeffs_precalc;
  }

  float calc(float x, float z) {
    float distance = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < _coeffs.size(); ++i) {
      distance =
          std::min(distance, std::abs(_coeffs[i][0] * x + _coeffs[i][1] * z + _coeffs[i][2]) / _coeffs_precalc[i]);
    }
    return distance;
  }
  static std::pair<std::vector<std::array<float, 3>>, std::vector<float>> get_border_line_coeffs(
      float R, float r, std::set<int> exclude_border_set);

 private:
  std::set<int> _exclude_border_set;
  float _R;
  float _r;
  std::vector<std::array<float, 3>> _coeffs;
  std::vector<float> _coeffs_precalc;
};

/**
 * @brief Calculate distances to lines via vector product
 *
 * Lines presented as list of corner points p0, p1, ..., pN of polygon: there are N lines in total.
 * If particular line must be excluded from calculation, pass its number in `exclude_border_set`
 */
class PointToLineDistance_VectorMultBased {
 public:
  PointToLineDistance_VectorMultBased(std::set<int> exclude_border_set, std::vector<Vector3> points)
      : _exclude_border_set(exclude_border_set), _points(points) {}

  float calc(Vector3 p) {
    float distance = std::numeric_limits<float>::max();
    unsigned int points_num = _points.size();
    for (unsigned int i = 0; i < points_num; ++i) {
      if (!_exclude_border_set.empty() && _exclude_border_set.contains(i)) {
        continue;
      }
      Vector3 line = _points[i] - _points[(i + 1) % points_num];
      float dist = (p - _points[(i + 1) % points_num]).cross(line).length() / line.length();
      distance = std::min(distance, dist);
    }
    return distance;
  }

 private:
  std::set<int> _exclude_border_set;
  std::vector<Vector3> _points;
};

}  // namespace sota
