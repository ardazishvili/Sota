#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams

namespace sota {

class HillMesh : public RidgeMesh {
  GDCLASS(HillMesh, RidgeMesh)
 public:
  HillMesh() = default;  // existence is 'must' for Godot
  HillMesh(const HillMesh& other) = delete;
  HillMesh(HillMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  HillMesh& operator=(HillMesh&& rhs) = delete;

  HillMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  HillMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) override;

 protected:
  static void _bind_methods() {}
};

}  // namespace sota
