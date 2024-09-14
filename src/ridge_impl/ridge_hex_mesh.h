#pragma once

#include <functional>
#include <limits>
#include <map>
#include <vector>

#include "core/hex_mesh.h"
#include "tal/noise.h"
#include "tal/reference.h"
#include "tal/vector3.h"
#include "misc/types.h"
#include "ridge_impl/ridge.h"

namespace sota {

struct RidgeHexMeshParams {
  HexMeshParams hex_mesh_params;
  Ref<FastNoiseLite> plain_noise{nullptr};
  Ref<FastNoiseLite> ridge_noise{nullptr};
};

class RidgeHexMesh : public HexMesh {
  GDCLASS(RidgeHexMesh, HexMesh)
 public:
  RidgeHexMesh() : HexMesh() {}
  RidgeHexMesh(Hexagon hex, RidgeHexMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
    _id = params.hex_mesh_params.id;
    _plain_noise = params.plain_noise;
    _ridge_noise = params.ridge_noise;
    _diameter = params.hex_mesh_params.diameter;
    _frame_state = params.hex_mesh_params.frame_state;
    _frame_offset = params.hex_mesh_params.frame_offset;
    _divisions = params.hex_mesh_params.divisions;
    set_material(params.hex_mesh_params.material);
    _clip_options = params.hex_mesh_params.clip_options;
  }

  // getters
  std::vector<HexMesh*> get_neighbours() const;
  std::map<Vector3, float> get_corner_points_distances_to_border() const { return _corner_points_to_border_dist; }
  GroupedHexagonMeshVertices get_grouped_vertices();
  std::pair<float, float> get_min_max_height() const { return {_min_y, _max_y}; }

  // setters
  void set_plain_noise(Ref<FastNoiseLite> plain_noise);
  void set_ridge_noise(Ref<FastNoiseLite> ridge_noise);
  void set_neighbours(HexagonNeighbours p_neighbours) { _neighbours = p_neighbours; }
  void set_ridges(std::vector<Ridge*> r) { _ridges = r; }
  void set_shift_compress(float y_shift, float y_compress);

  // calculation
  void calculate_corner_points_distances_to_border();
  void calculate_initial_heights();
  virtual void calculate_final_heights() {}

 protected:
  static void _bind_methods();

  Vector3Array _initial_vertices;

  void shift_compress();
  void calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                     float ridge_offset);

  Ref<FastNoiseLite> _plain_noise;
  Ref<FastNoiseLite> _ridge_noise;
  std::vector<Ridge*> _ridges;
  HexagonNeighbours _neighbours = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  float _min_y = std::numeric_limits<float>::max();
  float _max_y = std::numeric_limits<float>::min();
  float _y_shift = 0.0f;     // no shift
  float _y_compress = 1.0f;  // no compress

 private:
  std::map<Vector3, float> _corner_points_to_border_dist;
};

}  // namespace sota
