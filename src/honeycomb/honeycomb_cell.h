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

class HoneycombCell : public TileMesh {
  GDCLASS(HoneycombCell, TileMesh)

 public:
  HoneycombCell() = default;  // existence is 'must' for Godot
  HoneycombCell(const HoneycombCell& other) = delete;
  HoneycombCell(HoneycombCell&& other) = delete;
  // copying operator= defined inside GDCLASS
  HoneycombCell& operator=(HoneycombCell&& rhs) = delete;

  // getters
  GroupedMeshVertices get_grouped_vertices();

  // setters
  void set_noise(Ref<FastNoiseLite> noise);
  void set_selection_material(Ref<ShaderMaterial> p_selection_material);

  void calculate_heights(float bottom_offset);
  HexMesh* inner_mesh() override { return _hex_mesh.ptr(); }
  int get_id() override { return _hex_mesh->get_id(); }

  HoneycombCell(Hexagon hex, HoneycombCellMeshParams params);

 protected:
  static void _bind_methods();

 private:
  Ref<FastNoiseLite> _noise;
  Ref<ShaderMaterial> _selection_material;
  Ref<HexMesh> _hex_mesh;

  void handle_mouse_entered();
  void handle_mouse_exited();
  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx);
};

}  // namespace sota
