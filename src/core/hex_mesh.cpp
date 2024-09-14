#include "hex_mesh.h"

#include "Hexagon.h"
#include "core/utils.h"
#include "tal/arrays.h"
#include "tal/godot_core.h"
#include "tal/vector2.h"
#include "tal/vector3.h"

namespace sota {

void HexMesh::init_from_hex(Hexagon hex) {
  hex.check();
  _hex = hex;
}

HexMesh::HexMesh() {
  _hex = make_unit_hexagon();
  _R = radius(_diameter);
  _r = small_radius(_diameter);

  init();  // for init in godot
}

HexMesh::HexMesh(Hexagon hex, HexMeshParams params) {
  init_from_hex(hex);
  _R = radius(_diameter);
  _r = small_radius(_diameter);

  _id = params.id;
  _diameter = params.diameter;
  _frame_state = params.frame_state;
  _frame_offset = params.frame_offset;
  set_material(params.material);
  _divisions = params.divisions;
  _clip_options = params.clip_options;
}

HexMesh::HexMesh(Hexagon hex) {
  auto params = HexMeshParams{};
  init_from_hex(hex);
  _R = radius(_diameter);
  _r = small_radius(_diameter);

  _id = params.id;
  _diameter = params.diameter;
  _frame_state = params.frame_state;
  _frame_offset = params.frame_offset;
  set_material(params.material);
  _divisions = params.divisions;
  _clip_options = params.clip_options;
}

void HexMesh::init_impl() {
  if (tesselation_mode == TesselationMode::Iterative) {
    calculate_vertices_iteration();
    if (_frame_state) {
      add_frame();
    }
  } else if (tesselation_mode == TesselationMode::Recursive) {
    calculate_vertices_recursion();
  }
  recalculate_all_except_vertices();
}

void HexMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_diameter"), &HexMesh::get_diameter);
  ClassDB::bind_method(D_METHOD("set_diameter", "p_diameter"), &HexMesh::set_diameter);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "diameter"), "set_diameter", "get_diameter");

  // Interface
  ClassDB::bind_method(D_METHOD("get_id"), &HexMesh::get_id);
}

void HexMesh::add_frame() const {
  for (int i = 0; i < 6; ++i) {
    // rectangle consists from 4 points: a, b, c, d => 2 triangles
    auto corner_points = _hex.points();
    auto a = corner_points[i];
    auto b = corner_points[(i + 1) % 6];

    auto c = corner_points[i] + Vector3(0, -_frame_offset, 0);
    auto d = corner_points[(i + 1) % 6] + Vector3(0, -_frame_offset, 0);
    vertices_.push_back(b);
    vertices_.push_back(a);
    vertices_.push_back(c);

    vertices_.push_back(b);
    vertices_.push_back(c);
    vertices_.push_back(d);
  }
}

void HexMesh::set_diameter(const float p_diameter) {
  _diameter = p_diameter > 0 ? p_diameter : 0;
  _R = radius(_diameter);
  _r = small_radius(_diameter);

  init_from_hex(make_hexagon_at_position(_hex.center(), p_diameter));
  init();
}

float HexMesh::get_diameter() const { return _diameter; }

void HexMesh::z_clip(float boundary) const {
  float z_step = _R / _divisions;
  float half_z_step = z_step / 2;

  Vector3Array filtered;
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

void HexMesh::calculate_vertices_recursion() {
  vertices_.clear();
  auto corner_points = _hex.points();
  Vector3 c = (corner_points[0] + corner_points[3]) / 2;
  for (int i = 0; i < 6; ++i) {
    tesselate_into_triangles(corner_points[i], corner_points[(i + 1) % 6], c, _divisions);
  }
  return;
}

void HexMesh::calculate_vertices_iteration() const {
  vertices_.clear();

  auto corner_points = _hex.points();
  auto center = _hex.center();
  auto normal = _hex.normal();

  Vector3 pivot = corner_points[2];
  unsigned int triangles_count = _divisions * 2 + (_divisions * 2 - 1);
  float d0_step = (pivot - center).length() / _divisions;
  float half_d0_step = d0_step / 2;
  float d1_step = d0_step * std::sqrt(3) / 2;

  Vector3 direction0 = (center - pivot).normalized();
  Vector3 direction1 = direction0.cross(normal).normalized();

  Vector3 d0_inc = d0_step * direction0;
  Vector3 d1_inc = d1_step * direction1;
  Vector3 half_d0_inc = d0_inc / 2;

  Vector3 start_point_even = pivot;
  Vector3 start_point_odd = pivot + half_d0_inc + d1_inc;

  for (int layer = 0; layer < _divisions; ++layer, triangles_count -= 2) {
    Vector3 point_even = start_point_even + (layer * half_d0_inc) + (layer * d1_inc);
    Vector3 point_odd = start_point_odd + (layer * half_d0_inc) + (layer * d1_inc);
    for (unsigned int i = 0; i < triangles_count; ++i) {
      if (is_odd(i)) {
        vertices_.push_back(point_odd);
        vertices_.push_back(point_odd + half_d0_inc - d1_inc);
        vertices_.push_back(point_odd + d0_inc);

        point_odd += d0_inc;

      } else {
        vertices_.push_back(point_even);
        vertices_.push_back(point_even + d0_inc);
        vertices_.push_back(point_even + half_d0_inc + d1_inc);

        point_even += d0_inc;
      }
    }
  }

  if (_clip_options.down) {
    z_clip(-_R / 2);
  } else if (_clip_options.up) {
    z_clip(_R / 2);
  }

  if (_clip_options.left) {
    return;
  }

  Vector3Array copy = vertices_;
  int n = vertices_.size();

  auto reflected = [center, direction0](Vector3 v) -> Vector3 { return (v - center).reflect(direction0) + center; };

  if (_clip_options.right) {
    vertices_.clear();
  }
  for (int i = 0; i < n; i += 3) {
    Vector3 p0 = reflected(copy[i]);
    Vector3 p1 = reflected(copy[i + 1]);
    Vector3 p2 = reflected(copy[i + 2]);
    vertices_.append_array({p0, p2, p1});
  }
}

void HexMesh::calculate_tex_uv1() const {
  tex_uv1_.clear();

  auto corner_points = _hex.points();
  auto normal = _hex.normal();

  Vector3 c = (corner_points[0] + corner_points[3]) / 2;
  float diameter = (corner_points[3] - corner_points[0]).length();
  float R = radius(diameter);
  float r = small_radius(diameter);
  Vector3 direction0 = (corner_points[3] - corner_points[0]).normalized();
  Vector3 direction1 =
      tesselation_type == TesselationType::Plane ? direction0.cross(normal) : direction0.cross(c.normalized());

  Vector3 p0 = c - direction1 * r - direction0 * R;
  Vector3 p1 = c - direction1 * r + direction0 * R;
  Vector3 p2 = c + direction1 * r - direction0 * R;

  Vector3 r_line = (p2 - p0).normalized();
  Vector3 R_line = (p1 - p0).normalized();

  auto tex_coord_mapper = [R, r, r_line, R_line, p0](Vector3 v) -> Vector2 {
    Vector3 v_rel_p0 = v - p0;
    return {r_line.dot(v_rel_p0) / (2 * r), R_line.dot(v_rel_p0) / (2 * R)};
  };
  for (const auto& v : vertices_) {
    tex_uv1_.push_back(tex_coord_mapper(v));
  }
}

void HexMesh::update() { request_update(); }

Ref<HexMesh> make_hex_mesh(Hexagon hex, HexMeshParams params) {
  Ref<HexMesh> mesh = memnew(HexMesh(hex, params));
  mesh->init();
  return mesh;
}

}  // namespace sota
