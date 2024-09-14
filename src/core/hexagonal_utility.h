#pragma once

#include <vector>

#include "tal/vector3i.h"

namespace sota {

class HexagonalUtility {
 public:
  static std::vector<std::vector<Vector3i>> get_offset_coords_layout(int size);
  static int calculate_id(int row, int col, int size);
};

}  // namespace sota
