#include "tile.h"

#include "core/utils.h"
#include "honeycomb/honeycomb_honey.h"
#include "misc/cube_coordinates.h"
#include "misc/types.h"
#include "ridge_mesh.h"
#include "tal/callable.h"
#include "tal/engine.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/vector3.h"

namespace sota {

// Tile definitions
Tile::~Tile() {}

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
  Node* root_scene = EditorInterface::get_singleton()->get_edited_scene_root();
  _main_mesh_instance->set_owner(root_scene);
  _static_body->set_owner(root_scene);
  _collision_shape3d->set_owner(root_scene);
#endif

  _static_body->connect("mouse_entered", Callable(mesh.ptr(), "handle_mouse_entered"));
  _static_body->connect("mouse_exited", Callable(mesh.ptr(), "handle_mouse_exited"));
  _static_body->connect("input_event", Callable(mesh.ptr(), "handle_input_event"));
}

Ref<TileMesh> Tile::mesh() const { return _mesh; }

// BiomeTile definitions
Biome BiomeTile::biome() const { return _biome; }

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
