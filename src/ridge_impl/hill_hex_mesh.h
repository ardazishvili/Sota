#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

class HillHexMesh : public RidgeHexMesh {
 public:
  HillHexMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeHexMesh(hex, params) {}
  void calculate_final_heights() override;
};

}  // namespace sota
