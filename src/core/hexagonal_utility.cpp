#include "core/hexagonal_utility.h"

#include <memory>  // for allocator_traits<>::value_type

#include "core/utils.h"             // for is_odd
#include "misc/cube_coordinates.h"  // for OffsetCoordinates, cubeToOffset
#include "tal/vector3i.h"           // for Vector3i

namespace sota {

std::vector<std::vector<Vector3i>> HexagonalUtility::get_offset_coords_layout(int size) {
  std::vector<std::vector<Vector3i>> col_row_layout(size * 2 + 1, std::vector<Vector3i>());
  auto r_start = [size](int q) { return q <= 0 ? (-size - q) : -size; };
  auto r_end = [size](int q) { return q <= 0 ? size : size - q; };
  for (int q = -size; q <= size; ++q) {
    for (int r = r_start(q); r <= r_end(q); ++r) {
      auto cube = CubeCoordinates{.q = q, .r = r, .s = -q - r};
      auto offset_coord = cubeToOffset(cube);
      if (is_odd(size) && is_odd(offset_coord.row)) {
        ++offset_coord.col;
      }
      col_row_layout[offset_coord.row + size].push_back(Vector3i(offset_coord.row + size, 0, offset_coord.col + size));
    }
  }

  return col_row_layout;
}

int HexagonalUtility::calculate_id(int row, int col, int size) { return row * (size * 2 + 1) + col; }
}  // namespace sota
