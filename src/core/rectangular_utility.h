#pragma once
#include <vector>

#include "godot_cpp/variant/vector3i.hpp"

namespace sota {

class RectangularUtility {
 public:
  static std::vector<std::vector<godot::Vector3i>> get_offset_coords_layout(int height, int width);
  static int calculate_id(int row, int col, int width);
};

}  // namespace sota
