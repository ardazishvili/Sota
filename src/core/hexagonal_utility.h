#pragma once

#include <vector>

#include "tal/vector3i.h"

namespace sota {

/**
 * @brief Builds offset-coordinate layouts and tile IDs for hexagonal footprints.
 *
 * Size is the number of rings around the center. Layout entries store the row in X and
 * the column in Z. IDs use a row stride of twice the size plus one and may have gaps.
 */
class HexagonalUtility {
 public:
  static std::vector<std::vector<Vector3i>> get_offset_coords_layout(int size);
  static int calculate_id(int row, int col, int size);
};

}  // namespace sota
