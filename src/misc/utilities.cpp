#include "utilities.h"

#include "core/hex_mesh.h"
#include "core/utils.h"
#include "misc/types.h"
#include "primitives/Hexagon.h"
#include "ridge_impl/hill_hex_mesh.h"
#include "ridge_impl/mountain_hex_mesh.h"
#include "ridge_impl/plain_hex_mesh.h"
#include "ridge_impl/ridge_hex_mesh.h"
#include "ridge_impl/water_hex_mesh.h"
#include "tal/godot_core.h"
#include "tal/reference.h"

namespace sota {

Ref<HexMesh> create_hex_mesh(Biome biome, Hexagon hex, RidgeHexMeshParams params) {
  switch (biome) {
    case Biome::MOUNTAIN:
      return make_impl<MountainHexMesh>(hex, params);
    case Biome::PLAIN:
      return make_impl<PlainHexMesh>(hex, params);
    case Biome::HILL:
      return make_impl<HillHexMesh>(hex, params);
    case Biome::WATER:
      return make_impl<WaterHexMesh>(hex, params);
    default:
      printerr("Unreachable biome");
  }
  // Unreachable, prevent "control reached end of non-void funcion" warning
  return Ref<MountainHexMesh>();
}

PointDivisionedPosition to_point_divisioned_position(Vector3 v, float diameter, int divisions) {
  float R = radius(diameter);
  float r = small_radius(diameter);
  float z_step = R / (2 * divisions);  // middle vertex of triangle is displaced at half or "R / 2"
  float x_step = r / divisions;

  return std::make_pair<int, int>(std::round(v.x / x_step), std::round(v.z / z_step));
};

}  // namespace sota
