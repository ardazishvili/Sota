#pragma once

#include <vector>

#include "godot_cpp/variant/vector3i.hpp"

namespace sota {

class HexagonalUtility {
 public:
  static std::vector<std::vector<godot::Vector3i>> get_offset_coords_layout(int size);
  static int calculate_id(int row, int col, int size);
};

}  // namespace sota
