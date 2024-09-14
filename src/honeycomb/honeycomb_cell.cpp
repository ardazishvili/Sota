#include "honeycomb/honeycomb_cell.h"

#include "core/utils.h"
#include "tal/callable.h"
#include "tal/camera.h"
#include "tal/event.h"
#include "tal/godot_core.h"
#include "tal/vector2.h"
#include "general_utility.h"
#include "misc/utilities.h"

namespace sota {

void HoneycombCell::_bind_methods() {
  ClassDB::bind_method(D_METHOD("handle_mouse_entered"), &HoneycombCell::handle_mouse_entered);
  ClassDB::bind_method(D_METHOD("handle_mouse_exited"), &HoneycombCell::handle_mouse_exited);
  ClassDB::bind_method(D_METHOD("handle_input_event"), &HoneycombCell::handle_input_event);
}

void HoneycombCell::handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event,
                                       const Vector3& p_event_position, const Vector3& p_normal, int32_t p_shape_idx) {
  if (auto* mouse_event = dynamic_cast<InputEventMouse*>(p_event.ptr()); mouse_event) {
    if (mouse_event->get_button_mask().has_flag(MOUSE_BUTTON_MASK_LEFT) && mouse_event->is_pressed()) {
      set_material(_selection_material);
    }
  }
}

void HoneycombCell::handle_mouse_entered() {
  // placeholder
}
void HoneycombCell::handle_mouse_exited() {
  // placeholder
}

void HoneycombCell::set_noise(Ref<FastNoiseLite> p_noise) {
  _noise = p_noise;
  if (_noise.ptr()) {
    _noise->connect("changed", Callable(this, "request_update"));
    request_update();
  }
}

void HoneycombCell::set_selection_material(Ref<ShaderMaterial> p_selection_material) {
  _selection_material = p_selection_material;
}

void HoneycombCell::calculate_heights(float bottom_offset) {
  auto [coeffs, coeffs_precalc] = GeneralUtility::get_border_line_coeffs(_R, _r, {});

  auto center = _hex.center();

  float distance_to_border;
  unsigned int coeffs_size = coeffs.size();
  for (auto& v : vertices_) {
    distance_to_border = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < coeffs_size; ++i) {
      distance_to_border = std::min(distance_to_border, std::abs(coeffs[i][0] * (v.x - center.x) +
                                                                 coeffs[i][1] * (v.z - center.z) + coeffs[i][2]) /
                                                            coeffs_precalc[i]);
    }

    Vector3 center_point = center + Vector3(0, bottom_offset, 0);

    float distance_to_center = Vector2(center_point.x, center_point.z).distance_to(Vector2(v.x, v.z));
    float approx_end = center_point.y;
    auto t = [](float to_border, float to_projection) { return to_border / (to_border + to_projection); };

    v.y = center.y + cosrp(v.y, approx_end, t(distance_to_border, distance_to_center));
  }
}

// TODO factor out to super-class?
GroupedHexagonMeshVertices HoneycombCell::get_grouped_vertices() {
  GroupedHexagonMeshVertices vertex_groups;
  int size = vertices_.size();
  for (int i = 0; i < size; ++i) {
    Vector3& v = vertices_[i];
    Vector3& n = normals_[i];
    auto p = to_point_divisioned_position(v, _diameter, _divisions);
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}

Ref<HoneycombCell> make_honeycomb_cell(Hexagon hex, HoneycombCellMeshParams params) {
  Ref<HoneycombCell> mesh = memnew(HoneycombCell(hex, params));
  mesh->init();
  return mesh;
}

}  // namespace sota
