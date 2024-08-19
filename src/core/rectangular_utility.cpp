#include "core/rectangular_utility.h"

#include "misc/types.h"

namespace sota {

using namespace gd;

std::vector<std::vector<Vector3i>> RectangularUtility::get_offset_coords_layout(int height, int width) {
  std::vector<std::vector<Vector3i>> col_row_layout(height, std::vector<Vector3i>(width));
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      col_row_layout[row][col] = Vector3i(row, 0, col);
    }
  }
  return col_row_layout;
}

int RectangularUtility::calculate_id(int row, int col, int width) { return row * width + col; }
}  // namespace sota
