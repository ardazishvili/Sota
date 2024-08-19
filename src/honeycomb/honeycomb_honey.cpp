#include "honeycomb/honeycomb_honey.h"

#include "general_utility.h"
#include "godot_cpp/core/class_db.hpp"
#include "misc/utilities.h"

namespace sota {

using namespace gd;

void HoneycombHoney::_bind_methods() {
  ClassDB::bind_method(D_METHOD("lock"), &HoneycombHoney::lock);
  ClassDB::bind_method(D_METHOD("unlock"), &HoneycombHoney::unlock);
  ClassDB::bind_method(D_METHOD("is_locked"), &HoneycombHoney::is_locked);
  ClassDB::bind_method(D_METHOD("get_offset"), &HoneycombHoney::get_offset);
  ClassDB::bind_method(D_METHOD("fill"), &HoneycombHoney::fill);
  ClassDB::bind_method(D_METHOD("clear"), &HoneycombHoney::clear);
  ClassDB::bind_method(D_METHOD("is_full"), &HoneycombHoney::is_full);
  ClassDB::bind_method(D_METHOD("is_empty"), &HoneycombHoney::is_empty);
}

void HoneycombHoney::set_noise(Ref<FastNoiseLite> p_noise) {
  noise = p_noise;
  if (noise.ptr()) {
    noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void HoneycombHoney::set_min_offset(float p_offset) { min_offset = p_offset; }

void HoneycombHoney::set_max_level(float p_max_level) { max_level = p_max_level; }

void HoneycombHoney::set_xz_offset(Vector2 p_offset) {
  offset.x = p_offset.x;
  offset.z = p_offset.y;
  request_update();
}

void HoneycombHoney::set_fill_delta(float d) { fill_delta = d; }

void HoneycombHoney::set_level(int p_level, float y_offset) {
  level = p_level;
  offset.y = y_offset;
}

void HoneycombHoney::fill() {
  if (level == max_level) {
    return;
  }
  level += 1;
  recalculate_vertices_update(fill_delta);
}

bool HoneycombHoney::is_full() const { return level == max_level; }
bool HoneycombHoney::is_empty() const { return level == 0; }

void HoneycombHoney::clear() {
  level = 0;
  for (auto& v : vertices_) {
    v.y = min_offset;
  }
  calculate_normals();
  request_update();
}

int HoneycombHoney::get_level() const { return level; }

void HoneycombHoney::calculate_initial_heights() {
  for (auto& v : vertices_) {
    float n = noise.ptr() ? noise->get_noise_2d(v.x, +v.z) : 0.0;
    v.y = offset.y + n;
    _min_y = std::min(_min_y, v.y);
    _max_y = std::max(_max_y, v.y);
  }
}

void HoneycombHoney::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

void HoneycombHoney::calculate_heights() { GeneralUtility::shift_compress(vertices_, _y_shift, _y_compress, offset.y); }

GroupedHexagonMeshVertices HoneycombHoney::get_grouped_vertices() {
  GroupedHexagonMeshVertices vertex_groups;
  int size = vertices_.size();
  for (int i = 0; i < size; ++i) {
    Vector3& v = vertices_[i];
    Vector3& n = normals_[i];
    auto p = to_point_divisioned_position(Vector3(offset.x + v.x, offset.y + v.y, offset.z + v.z), diameter, divisions);
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

}  // namespace sota
