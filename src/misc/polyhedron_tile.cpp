#include "misc/polyhedron_tile.h"

#include "core/tile_mesh.h"
#include "mesh.h"
#include "tal/callable.h"
#include "tal/tmath.h"
#include "types.h"
#include "vector3.h"

namespace sota {

PolyhedronTile::PolyhedronTile(Ref<TileMesh> mesh, Node3D* parent) : Tile(mesh, parent) {
  _static_body->set_position(mesh->inner_mesh()->get_center());
  _sphere_shaped3d->set_radius(mesh->inner_mesh()->get_R());

  _static_body->connect("input_event", Callable(this, "handle_input_event"));
  _static_body->connect("mouse_entered", Callable(this, "handle_mouse_entered"));
  _static_body->connect("mouse_exited", Callable(this, "handle_mouse_exited"));
}

void PolyhedronTile::_physics_process(double delta) {
  Vector3 scale = _main_mesh_instance->get_scale();
  if (_state == PolyhedronTileState::DOWN) {
    if (Math::is_equal_approx(scale.x, _min.x, SCALE_TOL)) {
      _state = PolyhedronTileState::UP;
      return;
    }
    _main_mesh_instance->set_scale(scale - _scale_speed);
  } else if (_state == PolyhedronTileState::UP) {
    if (Math::is_equal_approx(scale.x, _max.x, SCALE_TOL)) {
      _state = PolyhedronTileState::IDLE;
      return;
    }
    _main_mesh_instance->set_scale(scale + _scale_speed);
  }
}

void PolyhedronTile::handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event,
                                        const Vector3& p_event_position, const Vector3& p_normal, int32_t p_shape_idx) {
  if (auto* mouse_event = dynamic_cast<InputEventMouse*>(p_event.ptr()); mouse_event) {
    if (mouse_event->get_button_mask().has_flag(MOUSE_BUTTON_MASK_LEFT) && mouse_event->is_pressed()) {
      _state = PolyhedronTileState::DOWN;
    }
  }
}

void PolyhedronTile::handle_mouse_entered() {
  // placeholder
}
void PolyhedronTile::handle_mouse_exited() {
  // placeholder
}

void PolyhedronTile::_bind_methods() {
  ClassDB::bind_method(D_METHOD("handle_mouse_entered"), &PolyhedronTile::handle_mouse_entered);
  ClassDB::bind_method(D_METHOD("handle_mouse_exited"), &PolyhedronTile::handle_mouse_exited);
  ClassDB::bind_method(D_METHOD("handle_input_event"), &PolyhedronTile::handle_input_event);
}

}  // namespace sota
