#pragma once

#include "misc/types.h"
#include "ridge_impl/ridge_hex_mesh.h"
#include "tal/reference.h"

namespace sota {

Ref<HexMesh> create_hex_mesh(Biome biome, Hexagon hex, RidgeHexMeshParams params);

using PointDivisionedPosition = std::pair<int, int>;

PointDivisionedPosition to_point_divisioned_position(Vector3 v, float diameter, int divisions);

template <typename T>
Ref<RidgeHexMesh> make_impl(Hexagon hex, RidgeHexMeshParams params) {
  auto res = Ref<T>(memnew(T(hex, params)));
  res->init();
  return res;
}
}  // namespace sota
