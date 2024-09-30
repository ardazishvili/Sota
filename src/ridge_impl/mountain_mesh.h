#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams

namespace sota {

class MountainMesh : public RidgeMesh {
 public:
  MountainMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  MountainMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(std::map<std::pair<int, int>, float>& distance_keeper, float diameter,
                               int divisions) override;
};

}  // namespace sota
