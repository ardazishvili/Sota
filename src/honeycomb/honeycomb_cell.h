#pragma once

#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "hex_mesh.h"
namespace sota {

class HoneycombCell : public HexMesh {
  GDCLASS(HoneycombCell, HexMesh)

 public:
  // getters
  GroupedHexagonMeshVertices get_grouped_vertices();
  gd::Vector3 get_offset() const { return offset; }

  // setters
  void set_noise(gd::Ref<gd::FastNoiseLite> noise);
  void set_offset(gd::Vector3 offset);
  void set_selection_material(gd::Ref<gd::ShaderMaterial> p_selection_material);

  void calculate_heights(float bottom_offset);

 protected:
  static void _bind_methods();

 private:
  gd::Ref<gd::FastNoiseLite> noise;
  gd::Vector3 offset;
  gd::Ref<gd::ShaderMaterial> selection_material;

  void handle_mouse_entered();
  void handle_mouse_exited();
  void handle_input_event(gd::Camera3D* p_camera, const gd::Ref<gd::InputEvent>& p_event,
                          const gd::Vector3& p_event_position, const gd::Vector3& p_normal, int32_t p_shape_idx);
};

}  // namespace sota
