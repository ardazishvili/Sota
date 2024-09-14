#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

class PlainHexMesh : public RidgeHexMesh {
 public:
  PlainHexMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeHexMesh(hex, params) {}
  void calculate_final_heights() override;
};

}  // namespace sota
