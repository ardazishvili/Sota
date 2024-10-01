#pragma once

#include <map>      // for map
#include <utility>  // for pair

#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh, RidgeHexMeshParams
#include "tal/vector3i.h"

namespace sota {

class PlainMesh : public RidgeMesh {
 public:
  PlainMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  PlainMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) override;
};

}  // namespace sota
