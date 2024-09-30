#pragma once

#include <set>
#include <vector>
#include <functional>

#include "misc/types.h"
#include "noise.h"
#include "primitives/polygon.h"
#include "ridge_impl/ridge.h"
#include "tal/arrays.h"
#include "tal/reference.h"
#include "tal/vector3.h"
namespace sota {

class MeshProcessor {
 public:
  virtual ~MeshProcessor() = default;
  virtual Vector3Array shift_compress(Vector3Array vertices, float shift, float compress, float offset) = 0;
  virtual void calculate_initial_heights(Vector3Array& vertices, Ref<FastNoiseLite> noise, float& min_height,
                                         float& max_height, Vector3 normal) = 0;
  virtual void calculate_hill_heights(Vector3Array& vertices, float r, float R, Vector3 center) = 0;
  virtual Vector3Array calculate_ridge_based_heights(
      Vector3Array vertices, const RegularPolygon& base, const std::vector<Ridge*> ridges,
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_list, float diameter, int divisions,
      std::map<std::pair<int, int>, float>& distance_keeper, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
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
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_list, float diameter, int divisions,
      std::map<std::pair<int, int>, float>& distance_keeper, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
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
      std::vector<Vector3> neighbours_corner_points, float R, std::set<int> exclude_list, float diameter, int divisions,
      std::map<std::pair<int, int>, float>& distance_keeper, Ref<FastNoiseLite> ridge_noise, float ridge_offset,
      std::function<double(double, double, double)> interpolation_func, float& min_height, float& max_height) override;

 private:
  Vector3Array _initial_vertices;
};

class GeneralUtility {
 public:
  static void make_smooth_normals(std::vector<GroupedMeshVertices>& vertex_groups);
  static std::pair<std::vector<std::array<float, 3>>, std::vector<float>> get_border_line_coeffs(
      float R, float r, std::set<int> exclude_list);
};

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

class PointToLineDistance_EquationBased {
 public:
  PointToLineDistance_EquationBased(std::set<int> exclude_list, float R, float r)
      : _exclude_list(exclude_list), _R(R), _r(r) {
    auto [coeffs, coeffs_precalc] = GeneralUtility::get_border_line_coeffs(_R, _r, _exclude_list);
    _coeffs = coeffs;
    _coeffs_precalc = coeffs_precalc;
  }

  float calc(float x, float z) {
    float distance_to_border = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < _coeffs.size(); ++i) {
      distance_to_border = std::min(
          distance_to_border, std::abs(_coeffs[i][0] * x + _coeffs[i][1] * z + _coeffs[i][2]) / _coeffs_precalc[i]);
    }
    return distance_to_border;
  }

 private:
  std::set<int> _exclude_list;
  float _R;
  float _r;
  std::vector<std::array<float, 3>> _coeffs;
  std::vector<float> _coeffs_precalc;
};

class PointToLineDistance_VectorMultBased {
 public:
  PointToLineDistance_VectorMultBased(std::set<int> exclude_list, std::vector<Vector3> corner_points)
      : _exclude_list(exclude_list), _corner_points(corner_points) {}

  float calc(Vector3 p) {
    float distance_to_border = std::numeric_limits<float>::max();
    unsigned int points_num = _corner_points.size();
    for (unsigned int i = 0; i < points_num; ++i) {
      if (!_exclude_list.empty() && _exclude_list.contains(i)) {
        continue;
      }
      Vector3 line = _corner_points[i] - _corner_points[(i + 1) % points_num];
      float dist = (p - _corner_points[(i + 1) % points_num]).cross(line).length() / line.length();
      distance_to_border = std::min(distance_to_border, dist);
    }
    return distance_to_border;
  }

 private:
  std::set<int> _exclude_list;
  std::vector<Vector3> _corner_points;
};

}  // namespace sota
