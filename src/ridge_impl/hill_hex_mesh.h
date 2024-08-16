#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

class HillHexMesh : public RidgeHexMesh {
 public:
  void calculate_final_heights() override;
};

}  // namespace sota
