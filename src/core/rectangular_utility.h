#pragma once
#include <vector>

#include "tal/vector3i.h"

namespace sota {

class RectangularUtility {
 public:
  static std::vector<std::vector<Vector3i>> get_offset_coords_layout(int height, int width);
  static int calculate_id(int row, int col, int width);
};

}  // namespace sota
