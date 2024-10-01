#include "utilities.h"

#include "core/hex_mesh.h"
#include "core/tile_mesh.h"
#include "core/utils.h"
#include "misc/types.h"
#include "primitives/hexagon.h"
#include "primitives/pentagon.h"
#include "tal/reference.h"

namespace sota {

bool is_water_mesh(TileMesh* mesh) { return dynamic_cast<WaterMesh*>(mesh); }
bool is_plain_mesh(TileMesh* mesh) { return dynamic_cast<PlainMesh*>(mesh); }
bool is_hill_mesh(TileMesh* mesh) { return dynamic_cast<HillMesh*>(mesh); }
bool is_mountain_mesh(TileMesh* mesh) { return dynamic_cast<MountainMesh*>(mesh); }

std::optional<Biome> get_biome(TileMesh* mesh) {
  if (is_water_mesh(mesh)) {
    return Biome::WATER;
  } else if (is_plain_mesh(mesh)) {
    return Biome::PLAIN;
  } else if (is_hill_mesh(mesh)) {
    return Biome::HILL;
  } else if (is_mountain_mesh(mesh)) {
    return Biome::MOUNTAIN;
  }
  return {};
}

}  // namespace sota
