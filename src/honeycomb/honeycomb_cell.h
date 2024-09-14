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
  HoneycombCell() : HexMesh() {}
  HoneycombCell(Hexagon hex, HoneycombCellMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
    _noise = params.noise;
    _selection_material = params.selection_material;
  }

  // getters
  GroupedHexagonMeshVertices get_grouped_vertices();

  // setters
  void set_noise(Ref<FastNoiseLite> noise);
  void set_selection_material(Ref<ShaderMaterial> p_selection_material);

  void calculate_heights(float bottom_offset);

 protected:
  static void _bind_methods();

 private:
  Ref<FastNoiseLite> _noise;
  Ref<ShaderMaterial> _selection_material;

  void handle_mouse_entered();
  void handle_mouse_exited();
  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx);
};

Ref<HoneycombCell> make_honeycomb_cell(Hexagon hex, HoneycombCellMeshParams params);

}  // namespace sota
