#include "mountain_hex_mesh.h"

#include <cmath>

namespace sota {

using namespace gd;

// TODO globals
constexpr float top_y_offset = 0.5;

void MountainHexMesh::calculate_final_heights() {
  calculate_ridge_based_heights(std::lerp<double, double, double>, top_y_offset);
}

}  // namespace sota
