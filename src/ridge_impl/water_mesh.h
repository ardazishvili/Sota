#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams

namespace sota {

class WaterMesh : public RidgeMesh {
  GDCLASS(WaterMesh, RidgeMesh)
 public:
  WaterMesh() = default;  // existence is 'must' for Godot
  WaterMesh(const WaterMesh& other) = delete;
  WaterMesh(WaterMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  WaterMesh& operator=(WaterMesh&& rhs) = delete;

  WaterMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  WaterMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) override;

 protected:
  static void _bind_methods() {}
};

}  // namespace sota
