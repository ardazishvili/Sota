#include "honeycomb/honeycomb_honey.h"

#include "core/general_utility.h"
#include "misc/types.h"
#include "misc/utilities.h"
#include "primitives/Hexagon.h"
#include "tal/callable.h"
#include "tal/godot_core.h"
#include "tal/vector3.h"

namespace sota {

HoneycombHoney::HoneycombHoney(Hexagon hex, HoneycombHoneyMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
  _noise = params.noise;
  _max_level = params.max_level;
  _fill_delta = params.fill_delta;
  _min_offset = params.min_offset;
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
    request_update();
  }
}

void HoneycombHoney::set_min_offset(float p_offset) { _min_offset = p_offset; }

void HoneycombHoney::set_max_level(float p_max_level) { _max_level = p_max_level; }

void HoneycombHoney::set_fill_delta(float d) { _fill_delta = d; }

void HoneycombHoney::set_level(int p_level) {
  _level = p_level;
  auto new_center = _hex.center() + Vector3(0, _min_offset + p_level * _fill_delta, 0);
  _hex = make_hexagon_at_position(new_center, _diameter);
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
  for (auto& v : vertices_) {
    v.y = _min_offset;
  }
  calculate_normals();
  request_update();
}

int HoneycombHoney::get_level() const { return _level; }

void HoneycombHoney::calculate_initial_heights() {
  for (auto& v : vertices_) {
    float n = _noise.ptr() ? _noise->get_noise_2d(v.x, v.z) : 0.0;
    v.y = _hex.center().y + n;
    _min_y = std::min(_min_y, v.y);
    _max_y = std::max(_max_y, v.y);
  }
}

void HoneycombHoney::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

void HoneycombHoney::calculate_heights() {
  GeneralUtility::shift_compress(vertices_, _y_shift, _y_compress, _hex.center().y);
}

GroupedHexagonMeshVertices HoneycombHoney::get_grouped_vertices() {
  GroupedHexagonMeshVertices vertex_groups;
  int size = vertices_.size();
  for (int i = 0; i < size; ++i) {
    Vector3 v = vertices_[i];
    Vector3& n = normals_[i];
    auto p = to_point_divisioned_position(v, _diameter, _divisions);
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}

void HoneycombHoney::recalculate_vertices_update(float surplus) {
  for (auto& v : vertices_) {
    v.y += surplus;
  }
  calculate_normals();
  request_update();
}

Ref<HoneycombHoney> make_honeycomb_honey(Hexagon hex, HoneycombHoneyMeshParams params) {
  Ref<HoneycombHoney> mesh = memnew(HoneycombHoney(hex, params));
  mesh->init();
  return mesh;
}
}  // namespace sota
