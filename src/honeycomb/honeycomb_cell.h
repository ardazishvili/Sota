#pragma once

#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "hex_mesh.h"
#include "types.h"
namespace sota {

struct HoneycombCellMeshParams {
  HexMeshParams hex_mesh_params;
  godot::Ref<godot::FastNoiseLite> noise{nullptr};
  godot::Ref<godot::ShaderMaterial> selection_material{nullptr};
};

class HoneycombCell : public HexMesh {
  GDCLASS(HoneycombCell, HexMesh)

 public:
  HoneycombCell() : HexMesh() {}
  HoneycombCell(Hexagon hex, HoneycombCellMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
    _noise = params.noise;
    _selection_material = params.selection_material;
  }

  // getters
  GroupedHexagonMeshVertices get_grouped_vertices();

  // setters
  void set_noise(gd::Ref<gd::FastNoiseLite> noise);
  void set_selection_material(gd::Ref<gd::ShaderMaterial> p_selection_material);

  void calculate_heights(float bottom_offset);

 protected:
  static void _bind_methods();

 private:
  gd::Ref<gd::FastNoiseLite> _noise;
  gd::Ref<gd::ShaderMaterial> _selection_material;

  void handle_mouse_entered();
  void handle_mouse_exited();
  void handle_input_event(gd::Camera3D* p_camera, const gd::Ref<gd::InputEvent>& p_event,
                          const gd::Vector3& p_event_position, const gd::Vector3& p_normal, int32_t p_shape_idx);
};

godot::Ref<HoneycombCell> make_honeycomb_cell(Hexagon hex, HoneycombCellMeshParams params);

}  // namespace sota
