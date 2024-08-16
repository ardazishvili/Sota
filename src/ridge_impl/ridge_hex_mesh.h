#pragma once

#include <functional>
#include <map>
#include <vector>

#include "core/hex_mesh.h"
#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "misc/types.h"
#include "ridge_impl/ridge.h"

namespace sota {

//'vertices_' stored without offset. But client code get any vertice with offset applied
class RidgeHexMesh : public HexMesh {
  GDCLASS(RidgeHexMesh, HexMesh)
 public:
  // getters
  std::vector<HexMesh*> get_neighbours() const;
  std::map<gd::Vector3, float> get_corner_points_distances_to_border() const { return _corner_points_to_border_dist; }
  gd::Vector3 center() const { return offset; }
  GroupedHexagonMeshVertices get_grouped_vertices();
  std::pair<float, float> get_min_max_height() const { return {_min_y, _max_y}; }

  // setters
  void set_plain_noise(gd::Ref<gd::FastNoiseLite> plain_noise);
  void set_ridge_noise(gd::Ref<gd::FastNoiseLite> ridge_noise);
  void set_offset(gd::Vector3 offset);
  void set_neighbours(HexagonNeighbours p_neighbours) { neighbours = p_neighbours; }
  void set_ridges(std::vector<Ridge*> r) { ridges = r; }
  void set_shift_compress(float y_shift, float y_compress);

  // calculation
  void calculate_corner_points_distances_to_border();
  void calculate_initial_heights();
  virtual void calculate_final_heights() {}

 protected:
  static void _bind_methods();

  void shift_compress();
  void calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                     float ridge_offset);

  gd::Vector3 offset;
  gd::Ref<gd::FastNoiseLite> plain_noise;
  gd::Ref<gd::FastNoiseLite> ridge_noise;
  std::vector<Ridge*> ridges;
  HexagonNeighbours neighbours = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  float _min_y = std::numeric_limits<float>::max();
  float _max_y = std::numeric_limits<float>::min();
  float _y_shift = 0.0f;     // no shift
  float _y_compress = 1.0f;  // no compress

 private:
  std::map<gd::Vector3, float> _corner_points_to_border_dist;
  std::pair<std::vector<std::array<float, 3>>, std::vector<float>>
  get_border_line_coeffs();  // to calculate distance from vertice to line (border)
};

struct HexBorderLineParams {
  HexBorderLineParams(float R, float r) {
    coeffs = {std::array<float, 3>{-R / (2 * r), 1.0f, R}, std::array<float, 3>{1.0f, 0.0f, -r},
              std::array<float, 3>{R / (2 * r), 1.0f, -R}, std::array<float, 3>{-R / (2 * r), 1.0f, -R},
              std::array<float, 3>{1.0f, 0.0f, r},         std::array<float, 3>{R / (2 * r), 1.0f, R}};
  }
  std::array<std::array<float, 3>, 6> get_coeffs() { return coeffs; }

 private:
  std::array<std::array<float, 3>, 6> coeffs;
};

}  // namespace sota
