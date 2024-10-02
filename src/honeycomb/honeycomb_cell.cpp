#include "honeycomb/honeycomb_cell.h"

#include <algorithm>  // for min
#include <cmath>      // for abs
#include <limits>     // for numeric_limits
#include <utility>    // for tuple_element<...
#include <vector>     // for vector

#include "core/general_utility.h"  // for GeneralUtility
#include "core/hex_mesh.h"         // for HexMesh
#include "core/utils.h"            // for cosrp
#include "misc/discretizer.h"      // for Dicretizer
#include "misc/types.h"            // for GroupedMeshVer...
#include "misc/utilities.h"        // for to_point_divis...
#include "primitives/hexagon.h"    // for Hexagon
#include "primitives/polygon.h"    // for RegularPolygon
#include "tal/callable.h"          // for Callable
#include "tal/camera.h"            // for Camera3D
#include "tal/event.h"             // for InputEventMouse
#include "tal/godot_core.h"        // for D_METHOD, ClassDB
#include "tal/material.h"          // for ShaderMaterial
#include "tal/noise.h"             // for FastNoiseLite
#include "tal/vector2.h"           // for Vector2

namespace sota {

HoneycombCell::HoneycombCell(Hexagon hex, HoneycombCellMeshParams params)
    : _hex_mesh(Ref<HexMesh>(memnew(HexMesh(hex, params.hex_mesh_params)))) {
  _hex_mesh->init();
  _noise = params.noise;
  _selection_material = params.selection_material;
}

void HoneycombCell::_bind_methods() {
  ClassDB::bind_method(D_METHOD("handle_mouse_entered"), &HoneycombCell::handle_mouse_entered);
  ClassDB::bind_method(D_METHOD("handle_mouse_exited"), &HoneycombCell::handle_mouse_exited);
  ClassDB::bind_method(D_METHOD("handle_input_event"), &HoneycombCell::handle_input_event);
}

void HoneycombCell::handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event,
                                       const Vector3& p_event_position, const Vector3& p_normal, int32_t p_shape_idx) {
  if (auto* mouse_event = dynamic_cast<InputEventMouse*>(p_event.ptr()); mouse_event) {
    if (mouse_event->get_button_mask().has_flag(MOUSE_BUTTON_MASK_LEFT) && mouse_event->is_pressed()) {
      _hex_mesh->set_material(_selection_material);
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
    _hex_mesh->update();
  }
}

void HoneycombCell::set_selection_material(Ref<ShaderMaterial> p_selection_material) {
  _selection_material = p_selection_material;
}

void HoneycombCell::calculate_heights(float bottom_offset) {
  auto [coeffs, coeffs_precalc] =
      PointToLineDistance_EquationBased::get_border_line_coeffs(_hex_mesh->get_R(), _hex_mesh->get_r(), {});

  auto center = _hex_mesh->base().center();

  float distance_to_border;
  unsigned int coeffs_size = coeffs.size();
  auto vertices = _hex_mesh->get_vertices();
  for (auto& v : vertices) {
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
  _hex_mesh->set_vertices(vertices);
}

}  // namespace sota
