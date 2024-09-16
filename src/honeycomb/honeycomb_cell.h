#pragma once

#include "core/hex_mesh.h"
#include "misc/types.h"
#include "tal/camera.h"
#include "tal/event.h"
#include "tal/material.h"
#include "tal/noise.h"
namespace sota {

struct HoneycombCellMeshParams {
  HexMeshParams hex_mesh_params;
  Ref<FastNoiseLite> noise{nullptr};
  Ref<ShaderMaterial> selection_material{nullptr};
};

class HoneycombCell : public HexMesh {
  GDCLASS(HoneycombCell, HexMesh)

 public:
  HoneycombCell() = default;  // existence is 'must' for Godot
  HoneycombCell(const HoneycombCell& other) = delete;
  HoneycombCell(HoneycombCell&& other) = delete;
  // copying operator= defined inside GDCLASS
  HoneycombCell& operator=(HoneycombCell&& rhs) = delete;

  // getters
  GroupedHexagonMeshVertices get_grouped_vertices();

  // setters
  void set_noise(Ref<FastNoiseLite> noise);
  void set_selection_material(Ref<ShaderMaterial> p_selection_material);

  void calculate_heights(float bottom_offset);

 protected:
  HoneycombCell(Hexagon hex, HoneycombCellMeshParams params);
  static void _bind_methods();

 private:
  Ref<FastNoiseLite> _noise;
  Ref<ShaderMaterial> _selection_material;

  void handle_mouse_entered();
  void handle_mouse_exited();
  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx);

  friend Ref<HoneycombCell> make_honeycomb_cell(Hexagon hex, HoneycombCellMeshParams params);
};

Ref<HoneycombCell> make_honeycomb_cell(Hexagon hex, HoneycombCellMeshParams params);

}  // namespace sota
