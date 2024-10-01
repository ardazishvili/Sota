#pragma once

#include <functional>  // for function
#include <limits>      // for numeric_limits
#include <map>         // for map
#include <memory>      // for make_unique, unique_ptr
#include <set>         // for set
#include <utility>     // for pair
#include <vector>      // for vector

#include "core/general_utility.h"  // for FlatMeshProcessor, MeshProc...
#include "core/hex_mesh.h"         // for HexMeshParams, HexMesh
#include "core/mesh.h"             // for SotaMesh, Orientation, Orie...
#include "core/pent_mesh.h"        // for PentagonMeshParams, PentMesh
#include "core/tile_mesh.h"        // for TileMesh
#include "core/utils.h"
#include "misc/discretizer.h"
#include "misc/types.h"           // for Neighbours, GroupedMeshVert...
#include "primitives/hexagon.h"   // for Hexagon
#include "primitives/pentagon.h"  // for Pentagon
#include "ridge_impl/ridge.h"
#include "tal/arrays.h"     // for Vector3Array
#include "tal/noise.h"      // for FastNoiseLite
#include "tal/reference.h"  // for Ref
#include "tal/vector3.h"    // for Vector3

namespace sota {
class Ridge;

struct RidgeHexMeshParams {
  HexMeshParams hex_mesh_params;
  Ref<FastNoiseLite> plain_noise{nullptr};
  Ref<FastNoiseLite> ridge_noise{nullptr};
};

struct RidgePentagonMeshParams {
  PentagonMeshParams pentagon_mesh_params;
  Ref<FastNoiseLite> plain_noise{nullptr};
  Ref<FastNoiseLite> ridge_noise{nullptr};
};

class RidgeMesh : public TileMesh {
  GDCLASS(RidgeMesh, TileMesh)
 public:
  RidgeMesh() = default;  // existence is 'must' for Godot
  RidgeMesh(const RidgeMesh& other) = delete;
  RidgeMesh(RidgeMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  RidgeMesh& operator=(RidgeMesh&& rhs) = delete;

  // getters
  std::vector<TileMesh*> get_neighbours() const;
  std::pair<float, float> get_min_max_height() const { return {_min_height, _max_height}; }

  // setters
  void set_plain_noise(Ref<FastNoiseLite> plain_noise);
  void set_ridge_noise(Ref<FastNoiseLite> ridge_noise);
  void set_neighbours(Neighbours p_neighbours) { _neighbours = p_neighbours; }
  void set_ridges(std::vector<Ridge*> r) { _ridges = r; }
  void set_shift_compress(float y_shift, float y_compress);

  // calculation
  void calculate_corner_points_distances_to_border(DiscreteVertexToDistance& distance_map, int divisions);
  void calculate_initial_heights();
  virtual void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) = 0;

  void calculate_normals() { _mesh->calculate_normals(); }
  void update() { _mesh->update(); }
  void recalculate_all_except_vertices() { _mesh->recalculate_all_except_vertices(); }
  void init() { _mesh->init(); }
  Vector3 get_center() { return _mesh->get_center(); }
  SotaMesh* inner_mesh() override { return _mesh.ptr(); }

  int get_id() override { return _mesh->get_id(); }

 protected:
  RidgeMesh(Hexagon hex, RidgeHexMeshParams params)
      : _mesh(Ref<SotaMesh>(memnew(HexMesh(hex, params.hex_mesh_params)))) {
    _plain_noise = params.plain_noise;
    _ridge_noise = params.ridge_noise;
  }

  RidgeMesh(Pentagon pentagon, RidgePentagonMeshParams params)
      : _mesh(Ref<SotaMesh>(memnew(PentMesh(pentagon, params.pentagon_mesh_params)))) {
    _plain_noise = params.plain_noise;
    _ridge_noise = params.ridge_noise;
  }

  static void _bind_methods();

  Vector3Array _initial_vertices;

  void shift_compress();
  void calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                     float ridge_offset, DiscreteVertexToDistance& distance_map, int divisions);

  Ref<FastNoiseLite> _plain_noise;
  Ref<FastNoiseLite> _ridge_noise;
  std::vector<Ridge*> _ridges;
  Neighbours _neighbours = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  float _min_height = std::numeric_limits<float>::max();
  float _max_height = std::numeric_limits<float>::min();
  float _y_shift = 0.0f;     // no shift
  float _y_compress = 1.0f;  // no compress

  Ref<SotaMesh> _mesh;
  std::unique_ptr<MeshProcessor> _processor;

 private:
  template <typename T>
  friend Ref<RidgeMesh> make_ridge_pentagon_mesh(Pentagon pentagon, RidgePentagonMeshParams params);

  template <typename T>
  friend Ref<RidgeMesh> make_ridge_hex_mesh(Hexagon hex, RidgeHexMeshParams params);

  // TODO make const. inner_mesh have to be const?
  std::set<int> get_exclude_list();
};

template <typename T>
Ref<RidgeMesh> make_ridge_hex_mesh(Hexagon hex, RidgeHexMeshParams params) {
  auto res = Ref<T>(memnew(T(hex, params)));
  res->init();
  if (params.hex_mesh_params.orientation == Orientation::Plane) {
    res->_processor = std::make_unique<FlatMeshProcessor>(FlatMeshProcessor());
  } else {
    res->_processor = std::make_unique<VolumeMeshProcessor>(VolumeMeshProcessor(res->inner_mesh()->get_vertices()));
  }
  return res;
}

template <typename T>
Ref<RidgeMesh> make_ridge_pentagon_mesh(Pentagon pentagon, RidgePentagonMeshParams params) {
  auto res = Ref<T>(memnew(T(pentagon, params)));
  res->init();
  if (params.pentagon_mesh_params.orientation == Orientation::Plane) {
    res->_processor = std::make_unique<FlatMeshProcessor>(FlatMeshProcessor());
  } else {
    res->_processor = std::make_unique<VolumeMeshProcessor>(VolumeMeshProcessor(res->inner_mesh()->get_vertices()));
  }
  return res;
}

}  // namespace sota
