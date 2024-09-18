#include "prism_impl/prism_hex_mesh.h"

#include "hex_mesh.h"
#include "misc/types.h"
#include "primitives/triangle.h"
#include "tal/godot_core.h"

namespace sota {
PrismHexMesh::PrismHexMesh(Hexagon hex, PrismHexMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
  _height = params.height;
}

void PrismHexMesh::init_impl() {
  HexMesh::init_impl();
  add_faces(_height);
  recalculate_all_except_vertices();
  update();
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
}

float PrismHexMesh::get_height() const { return _height; }

}  // namespace sota
