#include "prism_impl/prism_hex_mesh.h"

#if SOTA_MODULE
#include "core/os/memory.h"
#else
#include "godot_cpp/core/memory.hpp"
#endif
#include "hex_mesh.h"
#include "misc/types.h"
#include "primitives/Triangle.h"
#include "tal/godot_core.h"

namespace sota {
PrismHexMesh::PrismHexMesh(Hexagon hex, PrismHexMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
  _height = params.height;
}

void PrismHexMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &PrismHexMesh::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &PrismHexMesh::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height"), "set_height", "get_height");
}

void PrismHexMesh::set_height(const float p_height) {
  if (p_height < 0) {
    return;
  }
  _height = p_height;
  init();
  calculate_heights();
  recalculate_all_except_vertices();
  update();
}

float PrismHexMesh::get_height() const { return _height; }

void PrismHexMesh::calculate_heights() {
  for (auto& v : vertices_) {
    if (_tesselation_type == Orientation::Polyhedron) {
      v += v.normalized() * _height;
    } else if (_tesselation_type == Orientation::Plane) {
      v += _hex.normal() * _height;
    }
  }

  for (int i = 0; i < 6; ++i) {
    auto corner_points = _hex.points();

    add_face_to_base_hex(Edge{.a = corner_points[i], .b = corner_points[(i + 1) % 6]}, _height);
  }
}

Ref<PrismHexMesh> make_prism_mesh(Hexagon hex, PrismHexMeshParams params) {
  auto res = Ref<PrismHexMesh>(memnew(PrismHexMesh(hex, params)));
  res->init();
  return res;
}

}  // namespace sota
