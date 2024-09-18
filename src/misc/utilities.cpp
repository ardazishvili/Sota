#include "utilities.h"

#include "core/hex_mesh.h"
#include "core/utils.h"
#include "misc/types.h"
#include "primitives/hexagon.h"
#include "primitives/pentagon.h"
#include "ridge_impl/hill_mesh.h"
#include "ridge_impl/mountain_mesh.h"
#include "ridge_impl/plain_mesh.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/water_mesh.h"
#include "tal/godot_core.h"
#include "tal/reference.h"
#include "tile_mesh.h"

namespace sota {

Ref<RidgeMesh> create_hex_mesh(Biome biome, Hexagon hex, RidgeHexMeshParams params) {
  switch (biome) {
    case Biome::MOUNTAIN:
      return make_ridge_hex_mesh<MountainMesh>(hex, params);
    case Biome::PLAIN:
      return make_ridge_hex_mesh<PlainMesh>(hex, params);
    case Biome::HILL:
      return make_ridge_hex_mesh<HillMesh>(hex, params);
    case Biome::WATER:
      return make_ridge_hex_mesh<WaterMesh>(hex, params);
    default:
      printerr("Unreachable biome");
  }
  // Unreachable, prevent "control reached end of non-void funcion" warning
  return Ref<MountainMesh>();
}

// TODO unify with above
Ref<RidgeMesh> create_pentagon_mesh(Biome biome, Pentagon pentagon, RidgePentagonMeshParams params) {
  switch (biome) {
    case Biome::MOUNTAIN:
      return make_ridge_pentagon_mesh<MountainMesh>(pentagon, params);
    case Biome::PLAIN:
      return make_ridge_pentagon_mesh<PlainMesh>(pentagon, params);
    case Biome::HILL:
      return make_ridge_pentagon_mesh<HillMesh>(pentagon, params);
    case Biome::WATER:
      return make_ridge_pentagon_mesh<WaterMesh>(pentagon, params);
    default:
      printerr("Unreachable biome");
  }
  // Unreachable, prevent "control reached end of non-void funcion" warning
  return Ref<MountainMesh>();
}

PointDivisionedPosition to_point_divisioned_position(Vector3 v, float diameter, int divisions) {
  float R = radius(diameter);
  float r = small_radius(diameter);
  float z_step = R / (2 * divisions);  // middle vertex of triangle is displaced at half or "R / 2"
  float x_step = r / divisions;

  return std::make_pair<int, int>(std::round(v.x / x_step), std::round(v.z / z_step));
};

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
