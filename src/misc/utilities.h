#pragma once

#include <optional>

#include "misc/types.h"
#include "primitives/pentagon.h"
#include "ridge_impl/ridge_mesh.h"
#include "tal/reference.h"
#include "tile_mesh.h"

namespace sota {

Ref<RidgeMesh> create_hex_mesh(Biome biome, Hexagon hex, RidgeHexMeshParams params);
Ref<RidgeMesh> create_pentagon_mesh(Biome biome, Pentagon pentagon, RidgePentagonMeshParams params);

using PointDivisionedPosition = std::pair<int, int>;

PointDivisionedPosition to_point_divisioned_position(Vector3 v, float diameter, int divisions);

bool is_water_mesh(TileMesh* mesh);
bool is_plain_mesh(TileMesh* mesh);
bool is_hill_mesh(TileMesh* mesh);
bool is_mountain_mesh(TileMesh* mesh);

std::optional<Biome> get_biome(TileMesh* mesh);

}  // namespace sota
