#include "tile.h"

#include "core/utils.h"
#include "honeycomb/honeycomb_honey.h"
#include "misc/cube_coordinates.h"
#include "misc/types.h"
#include "ridge_mesh.h"
#include "ridge_impl/ridge_hex_grid.h"
#include "tal/callable.h"
#include "tal/engine.h"
#include "tal/event.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/vector3.h"

namespace sota {

// Tile definitions

Tile::Tile(Ref<TileMesh> mesh, Vector3 offset, Node3D* parent, OffsetCoordinates offset_coord)
    : _mesh(mesh), _offset_coord(offset_coord), _shifted(is_odd(offset_coord.row)) {
  _main_mesh_instance = memnew(MeshInstance3D());
  _sphere_shaped3d = Ref<SphereShape3D>(memnew(SphereShape3D()));

  auto points = mesh->inner_mesh()->base().points();
  auto center = mesh->inner_mesh()->base().center();
  _sphere_shaped3d->set_radius(center.distance_to(points[0]));

  _collision_shape3d = memnew(CollisionShape3D());
  _collision_shape3d->set_shape(_sphere_shaped3d);

  _static_body = memnew(StaticBody3D());
  _static_body->set_position(offset);

  _main_mesh_instance->set_mesh(mesh->inner_mesh());

  add_child(_main_mesh_instance);
  parent->add_child(this);

  _main_mesh_instance->add_child(_static_body);
  _static_body->add_child(_collision_shape3d);
#ifdef SOTA_ENGINE
  if (Engine::get_singleton()->is_editor_hint()) {
    Node* root_scene = EditorInterface::get_singleton()->get_edited_scene_root();
    this->set_owner(root_scene);
    _main_mesh_instance->set_owner(root_scene);
    _static_body->set_owner(root_scene);
    _collision_shape3d->set_owner(root_scene);
  }
#endif
}

Ref<TileMesh> Tile::mesh() const { return _mesh; }

// BiomeTile definitions

BiomeTile::BiomeTile(Ref<RidgeMesh> ridge_hex_mesh, Node3D* parent, Biome biome, OffsetCoordinates offset_coord,
                     int row, int col)
    : Tile(ridge_hex_mesh, ridge_hex_mesh->get_center(), parent, offset_coord), _biome(biome), _row(row), _col(col) {
  _static_body->connect("mouse_entered", Callable(this, "handle_mouse_entered"));
  _static_body->connect("mouse_exited", Callable(this, "handle_mouse_exited"));
  _static_body->connect("input_event", Callable(this, "handle_input_event"));
}

void BiomeTile::_bind_methods() {
  ClassDB::bind_method(D_METHOD("handle_mouse_entered"), &BiomeTile::handle_mouse_entered);
  ClassDB::bind_method(D_METHOD("handle_mouse_exited"), &BiomeTile::handle_mouse_exited);
  ClassDB::bind_method(D_METHOD("handle_input_event"), &BiomeTile::handle_input_event);
}

void BiomeTile::handle_mouse_entered() {
  // placeholder
}
void BiomeTile::handle_mouse_exited() {
  // placeholder
}

void BiomeTile::handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                                   const Vector3& p_normal, int32_t p_shape_idx) {
  if (auto* mouse_event = dynamic_cast<InputEventMouse*>(p_event.ptr()); mouse_event) {
    if (mouse_event->get_button_mask().has_flag(MOUSE_BUTTON_MASK_LEFT) && mouse_event->is_pressed()) {
      auto* ridge_hex_grid_parent = dynamic_cast<RidgeHexGrid*>(get_parent());
      ridge_hex_grid_parent->process_tile(_row, _col);
    }
  }
}

Biome BiomeTile::biome() const { return _biome; }
void BiomeTile::set_biome(Biome biome) { _biome = biome; }

Neighbours BiomeTile::neighbours() const { return _neighbours; }
void BiomeTile::set_neighbours(Neighbours neighbours) { _neighbours = neighbours; }

// HoneycombTile definitions
Ref<HoneycombHoney> HoneycombTile::honey_mesh() const { return _honey; }
HoneycombTile::HoneycombTile(Ref<HoneycombCell> walls, Ref<HoneycombHoney> honey, Node3D* parent,
                             OffsetCoordinates offset_coord)
    : Tile(walls, walls->inner_mesh()->get_center(), parent, offset_coord), _honey(honey) {
  _second_mesh_instance = memnew(MeshInstance3D());

  _second_mesh_instance->set_mesh(honey->inner_mesh());

  parent->add_child(_second_mesh_instance);
}

}  // namespace sota
