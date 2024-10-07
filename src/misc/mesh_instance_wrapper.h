#pragma once

#include "tal/camera.h"
#include "tal/engine.h"
#include "tal/event.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/node.h"  // for Node3D
#include "tal/os.h"
#include "tal/reference.h"
// #include "tal/thread.h"
// #include "tal/time.h"
#include "tal/vector3.h"

namespace sota {

class SotaMesh;

class MeshInstanceWrapper : public Node3D {
  GDCLASS(MeshInstanceWrapper, Node3D)
 public:
  MeshInstanceWrapper() = default;
  MeshInstanceWrapper(const MeshInstanceWrapper& other) = default;
  MeshInstanceWrapper(MeshInstanceWrapper&& other) = default;
  /* MeshInstanceWrapper& operator=(const MeshInstanceWrapper& other) = default; */
  MeshInstanceWrapper& operator=(MeshInstanceWrapper&& other) = delete;

  MeshInstanceWrapper(SotaMesh* sota_mesh, Node3D* parent);

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx);
  void handle_mouse_entered();
  void handle_mouse_exited();

  //   void move_inward();
  //   void thread_tear_down();

 protected:
  static void _bind_methods();

 private:
  Ref<SphereShape3D> _sphere_shaped3d{nullptr};
  CollisionShape3D* _collision_shape3d{nullptr};
  StaticBody3D* _static_body{nullptr};
  MeshInstance3D* _mesh_instance{nullptr};

  //   // Ref<Thread> _thread;
};

}  // namespace sota
