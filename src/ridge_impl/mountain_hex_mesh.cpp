#include "mountain_hex_mesh.h"

#include <cmath>

namespace sota {

// TODO globals
constexpr float top_y_offset = 0.5;

void MountainHexMesh::calculate_final_heights() {
  calculate_ridge_based_heights([](double a, double b, double c) { return std::lerp(a, b, c); }, top_y_offset);
}

}  // namespace sota
