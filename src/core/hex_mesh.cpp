#include "hex_mesh.h"

#include <array>
#include <numbers>

#include "core/utils.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"

namespace sota {

using namespace gd;

HexMesh::HexMesh() {
  R = radius(diameter);
  r = small_radius(diameter);

  init();
}

void HexMesh::init(bool clip_left, bool clip_right, bool clip_up, bool clip_bottom) {
  calculate_vertices(clip_left, clip_right, clip_up, clip_bottom);
  if (frame_state) {
    add_frame();
  }
  calculate_indices();  // normals depends on indices. Calculate them first
  calculate_normals();
  calculate_tangents();
  calculate_colors();
  calculate_tex_uv1();
  calculate_tex_uv2();
  calculate_color_custom();
  calculate_bones_weights();
}

void HexMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_divisions"), &HexMesh::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &HexMesh::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_diameter"), &HexMesh::get_diameter);
  ClassDB::bind_method(D_METHOD("set_diameter", "p_diameter"), &HexMesh::set_diameter);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "diameter"), "set_diameter", "get_diameter");

  // Interface
  ClassDB::bind_method(D_METHOD("get_id"), &HexMesh::get_id);
}

void HexMesh::set_divisions(const int p_divisions) {
  divisions = p_divisions > 1 ? p_divisions : 1;
  init();
  request_update();
}

void HexMesh::add_frame() const {
  for (int i = 0; i < 6; ++i) {
    // rectangle consists from 4 points: a, b, c, d => 2 triangles
    auto a = _corner_points[i];
    auto b = _corner_points[(i + 1) % 6];

    auto c = _corner_points[i] + Vector3(0, -frame_offset, 0);
    auto d = _corner_points[(i + 1) % 6] + Vector3(0, -frame_offset, 0);
    vertices_.push_back(b);
    vertices_.push_back(a);
    vertices_.push_back(c);

    vertices_.push_back(b);
    vertices_.push_back(c);
    vertices_.push_back(d);
  }
}

void HexMesh::set_diameter(const float p_diameter) {
  diameter = p_diameter > 0 ? p_diameter : 0;
  R = radius(diameter);
  r = small_radius(diameter);

  constexpr double pi = std::numbers::pi;
  for (float i = -pi / 6; i < 11 * pi / 6; i += pi / 3) {
    _corner_points[i] = {std::cos(i) * R, 0, std::sin(i) * R};
  }

  init();
  request_update();
}

float HexMesh::get_diameter() const { return diameter; }
int HexMesh::get_divisions() const { return divisions; }

void HexMesh::z_clip(float boundary) const {
  float z_step = R / divisions;
  float half_z_step = z_step / 2;

  PackedVector3Array filtered;
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    Vector3& p0 = vertices_[i];
    Vector3& p1 = vertices_[i + 1];
    Vector3& p2 = vertices_[i + 2];
    int vertices_to_fix = 0;
    Vector3* to_fix;
    if (boundary > 0) {
      if (p0.z > (boundary + EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p0;
      }
      if (p1.z > (boundary + EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p1;
      }
      if (p2.z > (boundary + EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p2;
      }
    } else {
      if (p0.z < (boundary - EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p0;
      }
      if (p1.z < (boundary - EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p1;
      }
      if (p2.z < (boundary - EPSILON)) {
        ++vertices_to_fix;
        to_fix = &p2;
      }
    }

    if (vertices_to_fix > 1) {
      // don't add vertices of triangle to filtered
      continue;
    } else if (vertices_to_fix == 1) {
      // just 1 vertex, fix it
      if (boundary > 0) {
        to_fix->z -= half_z_step;
      } else {
        to_fix->z += half_z_step;
      }
    }
    filtered.append_array({p0, p1, p2});
  }
  vertices_ = filtered;
}

void HexMesh::calculate_vertices(bool clip_left, bool clip_right, bool clip_up, bool clip_bottom) const {
  vertices_.clear();

  float z_step = R / divisions;
  float half_z_step = z_step / 2;
  float x_step = z_step * std::sqrt(3) / 2;

  float z_init_even = diameter / 2;
  float z_init_odd = diameter / 2 - half_z_step;
  unsigned int triangles_count = divisions * 2 + (divisions * 2 - 1);

  for (int layer = 0; layer < divisions; ++layer, triangles_count -= 2) {
    float z_even = z_init_even - (layer * half_z_step);
    float z_odd = z_init_odd - (layer * half_z_step);
    int even_cnt = 0;
    int odd_cnt = 0;
    for (unsigned int i = 0; i < triangles_count; ++i) {
      if (is_odd(i)) {
        Vector3 v1;
        v1.x = x_step * layer + x_step;
        v1.z = z_odd;
        vertices_.push_back(v1);

        Vector3 v2;
        v2.x = x_step * layer;
        v2.z = z_odd - half_z_step;
        vertices_.push_back(v2);

        Vector3 v3;
        v3.x = x_step * layer + x_step;
        v3.z = z_odd - z_step;
        vertices_.push_back(v3);

        z_odd -= z_step;

        ++odd_cnt;
      } else {
        Vector3 v1;
        v1.x = x_step * layer;
        v1.z = z_even;
        vertices_.push_back(v1);

        Vector3 v3;
        v3.x = x_step * layer;
        v3.z = z_even - z_step;
        vertices_.push_back(v3);

        Vector3 v2;
        v2.x = x_step * layer + x_step;
        v2.z = z_even - half_z_step;
        vertices_.push_back(v2);

        z_even -= z_step;

        ++even_cnt;
      }
    }
  }

  if (clip_bottom) {
    z_clip(-R / 2);
  } else if (clip_up) {
    z_clip(R / 2);
  }

  if (clip_left) {
    return;
  }

  PackedVector3Array copy = vertices_;
  int n = vertices_.size();

  auto left = [this](Vector3 v) -> Vector3 {
    v.x = -v.x;
    return v;
  };

  if (clip_right) {
    vertices_.clear();
  }
  for (int i = 0; i < n; i += 3) {
    Vector3 p0 = left(copy[i]);
    Vector3 p1 = left(copy[i + 1]);
    Vector3 p2 = left(copy[i + 2]);
    vertices_.append_array({p0, p2, p1});
  }
}

void HexMesh::calculate_indices() const {
  indices_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; i += 3) {
    indices_.append_array({i, i + 1, i + 2});
  }
}

void HexMesh::calculate_normals() const {
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

void HexMesh::calculate_tangents() const {
  tangents_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    tangents_.append_array({1, 0, 0, 1});
  }
}

void HexMesh::calculate_colors() const {
  colors_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    colors_.push_back(Color(0, 0, 0));
  }
}

void HexMesh::calculate_tex_uv1() const {
  tex_uv1_.clear();
  Vector3 offset(0, 0, 0);
  auto tex_coord_mapper = [this](Vector3 v) -> Vector2 { return {(v.x + r) / (2 * r), (v.z + R) / (2 * R)}; };
  for (const auto& v : vertices_) {
    tex_uv1_.push_back(tex_coord_mapper(v));
  }
}

void HexMesh::calculate_tex_uv2() const {
  tex_uv2_.clear();
  Vector3 offset(0, 0, 0);
  auto tex_coord_mapper = [this](Vector3 v) -> Vector2 { return {(v.x + r) / (2 * r), (v.z + R) / (2 * R)}; };
  for (const auto& v : vertices_) {
    tex_uv2_.push_back(tex_coord_mapper(v));
  }
}

void HexMesh::calculate_color_custom() const {
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

void HexMesh::calculate_bones_weights() const {
  bones_.clear();
  weights_.clear();
  int n = vertices_.size();
  for (int i = 0; i < n; ++i) {
    bones_.append_array({0, 0, 0, 0});
    weights_.append_array({0, 0, 0, 0});
  }
}

Array HexMesh::_create_mesh_array() const {
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

void HexMesh::update() { request_update(); }

}  // namespace sota
