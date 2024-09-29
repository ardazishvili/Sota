#include "water_mesh.h"

#include "core/utils.h"

namespace sota {

// TODO globals
constexpr float bottom_y_offset = 0.5;

void WaterMesh::calculate_final_heights(std::map<std::pair<int, int>, float>& distance_keeper, float diameter,
                                        int divisions) {
  calculate_ridge_based_heights(cosrp, bottom_y_offset, distance_keeper, divisions);
}

}  // namespace sota
