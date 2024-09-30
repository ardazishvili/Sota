#include "honeycomb/honeycomb_honey.h"

#include <algorithm>  // for max, min
#include <memory>     // for make_unique
#include <vector>     // for vector

#include "core/general_utility.h"  // for VolumeMeshProc...
#include "core/hex_mesh.h"         // for HexMesh, HexMe...
#include "core/mesh.h"             // for Orientation
#include "misc/types.h"            // for GroupedMeshVer...
#include "misc/utilities.h"        // for to_point_divis...
#include "primitives/hexagon.h"    // for Hexagon, make_...
#include "primitives/polygon.h"    // for RegularPolygon
#include "tal/callable.h"          // for Callable
#include "tal/godot_core.h"        // for D_METHOD, ClassDB
#include "tal/noise.h"             // for FastNoiseLite
#include "tal/reference.h"         // for Ref
#include "tal/vector3.h"           // for Vector3

namespace sota {

HoneycombHoney::HoneycombHoney(Hexagon hex, HoneycombHoneyMeshParams params)
    : _hex_mesh(Ref<HexMesh>(memnew(HexMesh(hex, params.hex_mesh_params)))) {
  _hex_mesh->init();
  _noise = params.noise;
  _max_level = params.max_level;
  _fill_delta = params.fill_delta;
  _min_offset = params.min_offset;
  if (params.hex_mesh_params.orientation == Orientation::Plane) {
    _processor = std::make_unique<FlatMeshProcessor>(FlatMeshProcessor());
  } else {
    _processor = std::make_unique<VolumeMeshProcessor>(VolumeMeshProcessor(inner_mesh()->get_vertices()));
  }
}

void HoneycombHoney::_bind_methods() {
  ClassDB::bind_method(D_METHOD("lock"), &HoneycombHoney::lock);
  ClassDB::bind_method(D_METHOD("unlock"), &HoneycombHoney::unlock);
  ClassDB::bind_method(D_METHOD("is_locked"), &HoneycombHoney::is_locked);
  ClassDB::bind_method(D_METHOD("fill"), &HoneycombHoney::fill);
  ClassDB::bind_method(D_METHOD("clear"), &HoneycombHoney::clear);
  ClassDB::bind_method(D_METHOD("is_full"), &HoneycombHoney::is_full);
  ClassDB::bind_method(D_METHOD("is_empty"), &HoneycombHoney::is_empty);
}

void HoneycombHoney::set_noise(Ref<FastNoiseLite> p_noise) {
  _noise = p_noise;
  if (_noise.ptr()) {
    _noise->connect("changed", Callable(this, "request_update"));
    _hex_mesh->update();
  }
}

void HoneycombHoney::set_min_offset(float p_offset) { _min_offset = p_offset; }

void HoneycombHoney::set_max_level(float p_max_level) { _max_level = p_max_level; }

void HoneycombHoney::set_fill_delta(float d) { _fill_delta = d; }

void HoneycombHoney::set_level(int p_level) {
  _level = p_level;
  auto new_center = _hex_mesh->base().center() + Vector3(0, _min_offset + p_level * _fill_delta, 0);
  _hex_mesh->set_base(std::make_unique<Hexagon>(make_hexagon_at_position(new_center, _hex_mesh->get_R() * 2)));
}

void HoneycombHoney::fill() {
  if (_level == _max_level) {
    return;
  }
  _level += 1;
  recalculate_vertices_update(_fill_delta);
}

bool HoneycombHoney::is_full() const { return _level == _max_level; }
bool HoneycombHoney::is_empty() const { return _level == 0; }

void HoneycombHoney::clear() {
  _level = 0;
  auto vertices = _hex_mesh->get_vertices();
  for (auto& v : vertices) {
    v.y = _min_offset;
  }
  _hex_mesh->set_vertices(vertices);
  _hex_mesh->calculate_normals();
  _hex_mesh->update();
}

int HoneycombHoney::get_level() const { return _level; }

void HoneycombHoney::calculate_initial_heights() {
  auto vertices = _hex_mesh->get_vertices();
  for (auto& v : vertices) {
    float n = _noise.ptr() ? _noise->get_noise_2d(v.x, v.z) : 0.0;
    v.y = _hex_mesh->base().center().y + n;
    _min_y = std::min(_min_y, v.y);
    _max_y = std::max(_max_y, v.y);
  }
  _hex_mesh->set_vertices(vertices);
}

void HoneycombHoney::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

void HoneycombHoney::calculate_heights() {
  if (!_processor) {
    print("processor of HoneycombHoney object is nullptr");
  }
  _hex_mesh->set_vertices(
      _processor->shift_compress(_hex_mesh->get_vertices(), _y_shift, _y_compress, _hex_mesh->base().center().y));
}

GroupedMeshVertices HoneycombHoney::get_grouped_vertices() {
  GroupedMeshVertices vertex_groups;
  auto vertices = _hex_mesh->get_vertices();
  auto& normals = _hex_mesh->get_normals();
  int size = vertices.size();
  for (int i = 0; i < size; ++i) {
    Vector3 v = vertices[i];
    Vector3& n = normals[i];
    auto p = to_point_divisioned_position(v, _hex_mesh->get_R() * 2, _hex_mesh->get_divisions());
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}

void HoneycombHoney::recalculate_vertices_update(float surplus) {
  auto vertices = _hex_mesh->get_vertices();
  for (auto& v : vertices) {
    v.y += surplus;
  }
  _hex_mesh->set_vertices(vertices);
  _hex_mesh->calculate_normals();
  _hex_mesh->update();
}

}  // namespace sota
