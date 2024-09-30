#include "prism_impl/prism_pent_mesh.h"

#include "core/pent_mesh.h"  // for PentMesh
#include "tal/godot_core.h"  // for D_METHOD, ClassDB, PropertyInfo

namespace sota {
PrismPentMesh::PrismPentMesh(Pentagon pent, PrismPentMeshParams params) : PentMesh(pent, params.pent_mesh_params) {
  _height = params.height;
}

void PrismPentMesh::init_impl() {
  PentMesh::init_impl();
  add_faces(_height);
  recalculate_all_except_vertices();
  update();
}

void PrismPentMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &PrismPentMesh::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &PrismPentMesh::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height"), "set_height", "get_height");
}

void PrismPentMesh::set_height(const float p_height) {
  if (p_height < 0) {
    return;
  }
  _height = p_height;
  init();
}

float PrismPentMesh::get_height() const { return _height; }

}  // namespace sota
