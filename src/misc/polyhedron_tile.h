#pragma once

#include "misc/tile.h"
#include "tal/camera.h"
#include "tal/engine.h"
#include "tal/event.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/node.h"  // for Node3D
#include "tal/reference.h"
#include "tal/vector3.h"
#include "types.h"

namespace sota {

class SotaMesh;

enum class PolyhedronTileState { IDLE, DOWN, UP };

class PolyhedronTile : public Tile {
  GDCLASS(PolyhedronTile, Tile)
 public:
  PolyhedronTile() = default;
  PolyhedronTile(const PolyhedronTile& other) = delete;
  PolyhedronTile(PolyhedronTile&& other) = delete;
  // copying operator= defined inside GDCLASS
  PolyhedronTile& operator=(PolyhedronTile&& other) = delete;

  PolyhedronTile(Ref<TileMesh> mesh, Node3D* parent);

#ifdef SOTA_GDEXTENSION
  void _physics_process(double delta) override;
#elif SOTA_MODULE
  // not marked virtual since in original Godot code it's not 'virtual' it is 'GDVIRTUAL1()'
  // https://github.com/ardazishvili/Sota/issues/76
  void _physics_process(double delta);
#endif

 protected:
  static void _bind_methods();

 private:
  PolyhedronTileState _state{PolyhedronTileState::IDLE};
  Vector3 _min{0.9, 0.9, 0.9};
  Vector3 _max{1.0, 1.0, 1.0};
  Vector3 _scale_speed{0.0016, 0.0016, 0.0016};
  static constexpr float SCALE_TOL = 0.001;

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx) override;
  void handle_mouse_entered() override;
  void handle_mouse_exited() override;
};

}  // namespace sota
