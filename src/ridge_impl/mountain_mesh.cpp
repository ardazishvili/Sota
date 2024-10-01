#include "mountain_mesh.h"

#include <cmath>

namespace sota {

// TODO globals
constexpr float top_y_offset = 0.5;

void MountainMesh::calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) {
  calculate_ridge_based_heights([](double a, double b, double c) { return std::lerp(a, b, c); }, top_y_offset,
                                distance_map, divisions);
}

}  // namespace sota
