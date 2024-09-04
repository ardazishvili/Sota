#include "core/mesh.h"

#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"

namespace sota {

using namespace godot;

void SotaMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_divisions"), &SotaMesh::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &SotaMesh::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");
}

void SotaMesh::set_divisions(const int p_divisions) {
  divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

int SotaMesh::get_divisions() const { return divisions; }

void SotaMesh::tesselate_into_triangles(Vector3 a, Vector3 b, Vector3 c, int level) const {
  --level;
  if (!level) {
    vertices_.append_array(PackedVector3Array{a, b, c});
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

void SotaMesh::calculate_normals() const {
  normals_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    Vector3& p0 = vertices_[i];
    Vector3& p1 = vertices_[i + 1];
    Vector3& p2 = vertices_[i + 2];

    Vector3 normal = (p0 - p1).cross(p2 - p1);
    normal = normal.normalized();

    normals_.append_array({normal, normal, normal});
  }
}

void SotaMesh::calculate_tangents() const {
  tangents_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    tangents_.append_array({1, 0, 0, 1});
  }
}

void SotaMesh::calculate_colors() const {
  colors_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    colors_.push_back(Color(0, 0, 0));
  }
}

void SotaMesh::calculate_indices() const {
  indices_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    indices_.append_array({i, i + 1, i + 2});
  }
}
void SotaMesh::calculate_tex_uv2() const {
  tex_uv2_.clear();
  for (const auto& v : vertices_) {
    tex_uv2_.push_back(Vector2{0, 0});
  }
}

void SotaMesh::calculate_color_custom() const {
  color_custom0_.clear();
  color_custom1_.clear();
  color_custom2_.clear();
  color_custom3_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    color_custom0_.append_array({0, 0, 0, 0});
    color_custom1_.append_array({0, 0, 0, 0});
    color_custom2_.append_array({0, 0, 0, 0});
    color_custom3_.append_array({0, 0, 0, 0});
  }
}

void SotaMesh::calculate_bones_weights() const {
  bones_.clear();
  weights_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    bones_.append_array({0, 0, 0, 0});
    weights_.append_array({0, 0, 0, 0});
  }
}

Array SotaMesh::_create_mesh_array() const {
  Array res;
  res.append(vertices_);
  res.append(normals_);
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

void SotaMesh::recalculate_all_except_vertices() const {
  calculate_indices();  // normals depends on indices. Calculate them first
  calculate_normals();
  calculate_tangents();
  calculate_colors();
  calculate_tex_uv1();
  calculate_tex_uv2();
  calculate_color_custom();
  calculate_bones_weights();
}

}  // namespace sota