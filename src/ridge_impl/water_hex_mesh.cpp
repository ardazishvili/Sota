#include "water_hex_mesh.h"

#include "core/utils.h"

namespace sota {

using namespace godot;

// TODO globals
constexpr float bottom_y_offset = 0.5;

void WaterHexMesh::calculate_final_heights() { calculate_ridge_based_heights(cosrp, bottom_y_offset); }

}  // namespace sota
