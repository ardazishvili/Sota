#pragma once

#include "ridge_impl/ridge_hex_mesh.h"
#include "types.h"

namespace sota {

gd::Ref<HexMesh> create_hex_mesh(Biome biome, Hexagon hex, RidgeHexMeshParams params);

using PointDivisionedPosition = std::pair<int, int>;

PointDivisionedPosition to_point_divisioned_position(gd::Vector3 v, float diameter, int divisions);

template <typename T>
godot::Ref<RidgeHexMesh> make_impl(Hexagon hex, RidgeHexMeshParams params) {
  auto res = godot::Ref(memnew(T(hex, params)));
  res->init();
  return res;
}
}  // namespace sota
