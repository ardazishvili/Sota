#include "core/mesh.h"

#include "core/dummy_mesher.h"
#include "tal/arrays.h"
#include "tal/godot_core.h"
#include "tal/vector3.h"

namespace sota {

void SotaMesh::init() {
  init_impl();
  request_update();
}

void SotaMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_divisions"), &SotaMesh::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &SotaMesh::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_vertices"), &SotaMesh::get_vertices);
  ClassDB::bind_method(D_METHOD("set_vertices", "vertices"), &SotaMesh::set_vertices);
}

void SotaMesh::set_divisions(const int p_divisions) {
  _divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

int SotaMesh::get_divisions() const { return _divisions; }

void SotaMesh::tesselate_into_triangles(Vector3 a, Vector3 b, Vector3 c, int level) {
  --level;
  if (!level) {
    vertices_.append_array(Vector3Array{a, b, c});
    return;
  }
  Vector3 ab = (a + b) / 2;
  Vector3 bc = (b + c) / 2;
  Vector3 ca = (c + a) / 2;
  tesselate_into_triangles(a, ab, ca, level);
  tesselate_into_triangles(ca, bc, c, level);
  tesselate_into_triangles(bc, ab, b, level);
  tesselate_into_triangles(ca, ab, bc, level);
}

void SotaMesh::calculate_normals() {
  normals_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    const Vector3& p0 = vertices_[i];
    const Vector3& p1 = vertices_[i + 1];
    const Vector3& p2 = vertices_[i + 2];

    Vector3 normal = (p0 - p1).cross(p2 - p1);
    normal = normal.normalized();

    normals_.insert(normals_.end(), {normal, normal, normal});
  }
}

void SotaMesh::calculate_tangents() { tangents_ = DummyMesher::calculate_tangents(vertices_.size()); }

void SotaMesh::calculate_colors() { colors_ = DummyMesher::calculate_colors(vertices_.size()); }

void SotaMesh::calculate_indices() {
  indices_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    indices_.append_array({i, i + 1, i + 2});
  }
}
void SotaMesh::calculate_tex_uv2() { tex_uv2_ = DummyMesher::calculate_tex_uv2(vertices_.size()); }

void SotaMesh::calculate_color_custom() {
  color_custom0_ = DummyMesher::calculate_color_custom0(vertices_.size());
  color_custom1_ = DummyMesher::calculate_color_custom0(vertices_.size());
  color_custom2_ = DummyMesher::calculate_color_custom0(vertices_.size());
  color_custom3_ = DummyMesher::calculate_color_custom0(vertices_.size());
}

void SotaMesh::calculate_bones_weights() {
  bones_ = DummyMesher::calculate_bones(vertices_.size());
  weights_ = DummyMesher::calculate_weights(vertices_.size());
}

#ifdef SOTA_GDEXTENSION
Array SotaMesh::_create_mesh_array() const {
  Array res;
  res.append(vertices_);
  res.append(normals_to_godot_fmt());
  res.append(tangents_);
  res.append(colors_);
  res.append(tex_uv1_);
  res.append(tex_uv2_);
  res.append(color_custom0_);
  res.append(color_custom1_);
  res.append(color_custom2_);
  res.append(color_custom3_);
  res.append(bones_);
  res.append(weights_);
  res.append(indices_);
  return res;
}
#else
void SotaMesh::_create_mesh_array(Array& res) const {
  res[RS::ARRAY_VERTEX] = vertices_;
  res[RS::ARRAY_NORMAL] = normals_to_godot_fmt();
  res[RS::ARRAY_TANGENT] = tangents_;
  res[RS::ARRAY_TEX_UV] = tex_uv1_;
  res[RS::ARRAY_INDEX] = indices_;
}
#endif

void SotaMesh::recalculate_all_except_vertices() {
  calculate_indices();
  calculate_normals();
  calculate_tex_uv1();
  calculate_tangents();
  calculate_colors();
  calculate_tex_uv2();
  calculate_color_custom();
  calculate_bones_weights();
}

Vector3Array SotaMesh::get_vertices() const { return vertices_; }

void SotaMesh::set_vertices(Vector3Array vertices) {
  vertices_ = vertices;
  recalculate_all_except_vertices();
  request_update();
}

Vector3Array SotaMesh::normals_to_godot_fmt() const {
  Vector3Array normals;
  for (const auto& n : normals_) {
    normals.append(n);
  }
  return normals;
}

void SotaMesh::update() { request_update(); }

}  // namespace sota
