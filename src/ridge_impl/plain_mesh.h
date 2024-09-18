#pragma once

#include "primitives/pentagon.h"
#include "ridge_impl/ridge_mesh.h"

namespace sota {

class PlainMesh : public RidgeMesh {
 public:
  PlainMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  PlainMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(std::map<std::pair<int, int>, float>& distance_keeper, float diameter,
                               int divisions) override;
};

}  // namespace sota
