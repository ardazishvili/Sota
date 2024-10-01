#pragma once

#include <optional>
#include <type_traits>

#include "misc/types.h"
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

bool is_water_mesh(TileMesh* mesh);
bool is_plain_mesh(TileMesh* mesh);
bool is_hill_mesh(TileMesh* mesh);
bool is_mountain_mesh(TileMesh* mesh);

std::optional<Biome> get_biome(TileMesh* mesh);

template <typename T, typename POLY, typename PAR>
Ref<RidgeMesh> create_mesh_impl(POLY polygon, PAR params) {
  if constexpr (std::is_same_v<POLY, Hexagon>) {
    return make_ridge_hex_mesh<T>(polygon, params);
  } else {
    return make_ridge_pentagon_mesh<T>(polygon, params);
  }
}
template <typename POLY, typename PAR>
Ref<RidgeMesh> create_ridge_mesh(Biome biome, POLY polygon, PAR params) {
  switch (biome) {
    case Biome::MOUNTAIN:
      return create_mesh_impl<MountainMesh>(polygon, params);
    case Biome::PLAIN:
      return create_mesh_impl<PlainMesh>(polygon, params);
    case Biome::HILL:
      return create_mesh_impl<HillMesh>(polygon, params);
    case Biome::WATER:
      return create_mesh_impl<WaterMesh>(polygon, params);
    default:
      printerr("Unreachable biome");
  }
  // Unreachable, prevent "control reached end of non-void funcion" warning
  return Ref<MountainMesh>();
}

}  // namespace sota
