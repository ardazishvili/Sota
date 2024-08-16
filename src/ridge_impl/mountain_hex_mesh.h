#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

class MountainHexMesh : public RidgeHexMesh {
 public:
  void calculate_final_heights() override;
};

}  // namespace sota
