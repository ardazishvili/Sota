#pragma once

#include "ridge_hex_mesh.h"

namespace sota {

using namespace gd;

class PlainHexMesh : public RidgeHexMesh {
 public:
  void calculate_final_heights() override;
};

}  // namespace sota
