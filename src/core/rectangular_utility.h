#pragma once
#include <vector>

#include "tal/vector3i.h"

namespace sota {

/**
 * @brief Builds offset-coordinate layouts and tile IDs for rectangular footprints.
 *
 * Each layout entry stores the row in X and the column in Z. Tile IDs use row-major order.
 */
class RectangularUtility {
 public:
  static std::vector<std::vector<Vector3i>> get_offset_coords_layout(int height, int width);
  static int calculate_id(int row, int col, int width);
};

}  // namespace sota
