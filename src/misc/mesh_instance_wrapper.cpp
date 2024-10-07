#include "misc/mesh_instance_wrapper.h"

#include "core/mesh.h"
#include "tal/callable.h"

namespace sota {

MeshInstanceWrapper::MeshInstanceWrapper(SotaMesh* sota_mesh, Node3D* parent) {
  _mesh_instance = memnew(MeshInstance3D());
  _sphere_shaped3d = Ref<SphereShape3D>(memnew(SphereShape3D()));
  _collision_shape3d = memnew(CollisionShape3D());
  _static_body = memnew(StaticBody3D());

  _static_body->set_position(sota_mesh->get_center());

  _sphere_shaped3d->set_radius(sota_mesh->get_R());
  // std::cout << "R: " << sota_mesh->get_R() << std::endl;
  _collision_shape3d->set_shape(_sphere_shaped3d);
  _mesh_instance->set_mesh(sota_mesh);

  add_child(_mesh_instance);
  parent->add_child(this);

  _mesh_instance->add_child(_static_body);
  _static_body->add_child(_collision_shape3d);

#ifdef SOTA_ENGINE
  if (Engine::get_singleton()->is_editor_hint()) {
    Node* root_scene = EditorInterface::get_singleton()->get_edited_scene_root();
    this->set_owner(root_scene);
    _mesh_instance->set_owner(root_scene);
    _static_body->set_owner(root_scene);
    _collision_shape3d->set_owner(root_scene);
  }
#endif

  _static_body->connect("input_event", Callable(this, "handle_input_event"));
  _static_body->connect("mouse_entered", Callable(this, "handle_mouse_entered"));
  _static_body->connect("mouse_exited", Callable(this, "handle_mouse_exited"));
}

void MeshInstanceWrapper::handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event,
                                             const Vector3& p_event_position, const Vector3& p_normal,
                                             int32_t p_shape_idx) {
  if (auto* mouse_event = dynamic_cast<InputEventMouse*>(p_event.ptr()); mouse_event) {
    if (mouse_event->get_button_mask().has_flag(MOUSE_BUTTON_MASK_LEFT) && mouse_event->is_pressed()) {
      std::cout << "clicked" << std::endl;
    }
  }
}

void MeshInstanceWrapper::handle_mouse_entered() {
  // placeholder
}
void MeshInstanceWrapper::handle_mouse_exited() {
  // placeholder
}

// void MeshInstanceWrapper::move_inward() {
// #ifdef SOTA_ENGINE
//   if (Engine::get_singleton()->is_editor_hint()) {
//     return;
//   }
// #endif
//   print("move_inward started");
//   int i = 0;
//   auto* time = Time::get_singleton();
//   auto* os = OS::get_singleton();
//   uint64_t start = time->get_ticks_msec();
//   while (time->get_ticks_msec() - start < 200) {
//     os->delay_msec(10);
//     i++;
//   }
//   print("let's call tear down");

//   thread_tear_down();
// }

// void MeshInstanceWrapper::thread_tear_down() {
//   _thread->call_deferred("wait_to_finish");
//   _thread.unref();
// }

void MeshInstanceWrapper::_bind_methods() {
  ClassDB::bind_method(D_METHOD("handle_mouse_entered"), &MeshInstanceWrapper::handle_mouse_entered);
  ClassDB::bind_method(D_METHOD("handle_mouse_exited"), &MeshInstanceWrapper::handle_mouse_exited);
  ClassDB::bind_method(D_METHOD("handle_input_event"), &MeshInstanceWrapper::handle_input_event);
}

}  // namespace sota
