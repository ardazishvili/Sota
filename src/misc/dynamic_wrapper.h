#pragma once

#include "tal/camera.h"
#include "tal/engine.h"
#include "tal/event.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/node.h"  // for Node3D
#include "tal/reference.h"
#include "tal/vector3.h"

namespace sota {

class SotaMesh;

enum class DynamicWrapperState { IDLE, DOWN, UP };

class DynamicWrapper : public Node3D {
  GDCLASS(DynamicWrapper, Node3D)
 public:
  DynamicWrapper() = default;
  DynamicWrapper(const DynamicWrapper& other) = default;
  DynamicWrapper(DynamicWrapper&& other) = default;
  // copying operator= defined inside GDCLASS
  DynamicWrapper& operator=(DynamicWrapper&& other) = delete;

  DynamicWrapper(SotaMesh* sota_mesh, Node3D* parent);

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
  Ref<SphereShape3D> _sphere_shaped3d;
  CollisionShape3D* _collision_shape3d{nullptr};
  StaticBody3D* _static_body{nullptr};
  MeshInstance3D* _mesh_instance{nullptr};

  DynamicWrapperState _state{DynamicWrapperState::IDLE};
  Vector3 _min{0.9, 0.9, 0.9};
  Vector3 _max{1.0, 1.0, 1.0};
  Vector3 _scale_speed{0.0016, 0.0016, 0.0016};
  static constexpr float SCALE_TOL = 0.001;

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx);
  void handle_mouse_entered();
  void handle_mouse_exited();
};

}  // namespace sota
