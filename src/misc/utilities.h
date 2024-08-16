#pragma once

#include "core/hex_mesh.h"
#include "types.h"

namespace sota {

gd::Ref<HexMesh> create_hex_mesh(Biome biome);

using PointDivisionedPosition = std::pair<int, int>;

PointDivisionedPosition to_point_divisioned_position(gd::Vector3 v, float diameter, int divisions);

}  // namespace sota
