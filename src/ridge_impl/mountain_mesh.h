#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams

namespace sota {

class MountainMesh : public RidgeMesh {
  GDCLASS(MountainMesh, RidgeMesh)
 public:
  MountainMesh() = default;  // existence is 'must' for Godot
  MountainMesh(const MountainMesh& other) = delete;
  MountainMesh(MountainMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  MountainMesh& operator=(MountainMesh&& rhs) = delete;

  MountainMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  MountainMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) override;

 protected:
  static void _bind_methods() {}
};

}  // namespace sota
