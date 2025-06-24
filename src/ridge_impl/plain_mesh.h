#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams
#include "tal/vector3i.h"

namespace sota {

class PlainMesh : public RidgeMesh {
  GDCLASS(PlainMesh, RidgeMesh)
 public:
  PlainMesh() = default;  // existence is 'must' for Godot
  PlainMesh(const PlainMesh& other) = delete;
  PlainMesh(PlainMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  PlainMesh& operator=(PlainMesh&& rhs) = delete;

  PlainMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  PlainMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) override;

 protected:
  static void _bind_methods() {}
};

}  // namespace sota
