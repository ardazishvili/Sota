#pragma once

#include "primitives/pentagon.h"
#include "ridge_impl/ridge_mesh.h"

namespace sota {

class HillMesh : public RidgeMesh {
 public:
  HillMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeMesh(hex, params) {}
  HillMesh(Pentagon pentagon, RidgePentagonMeshParams params) : RidgeMesh(pentagon, params) {}
  void calculate_final_heights(std::map<std::pair<int, int>, float>& distance_keeper, float diameter,
                               int divisions) override;
};

}  // namespace sota
