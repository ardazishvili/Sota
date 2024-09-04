#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

class MountainHexMesh : public RidgeHexMesh {
 public:
  MountainHexMesh(Hexagon hex, RidgeHexMeshParams params) : RidgeHexMesh(hex, params) {}
  void calculate_final_heights() override;
};

}  // namespace sota
