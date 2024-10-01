#include "water_mesh.h"

#include "core/utils.h"

namespace sota {

// TODO globals
constexpr float bottom_y_offset = 0.5;

void WaterMesh::calculate_final_heights(DiscreteVertexToDistance& distance_map, float diameter, int divisions) {
  calculate_ridge_based_heights(cosrp, bottom_y_offset, distance_map, divisions);
}

}  // namespace sota
