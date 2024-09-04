#pragma once

#include "ridge_impl/ridge_hex_mesh.h"

namespace sota {

class WaterHexMesh : public RidgeHexMesh {
 public:
  WaterHexMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeHexMesh(hex, params) {}
  void calculate_final_heights() override;
};

}  // namespace sota
