#include "tile.h"

#include <memory>

#include "cube_coordinates.h"
#include "godot_cpp/classes/collision_shape2d.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/grid_map.hpp"
#include "godot_cpp/classes/sphere_shape3d.hpp"
#include "godot_cpp/classes/static_body3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "honeycomb/honeycomb_honey.h"
#include "types.h"
#include "utils.h"

namespace sota {

using namespace gd;

Tile::~Tile() {}

Tile::Tile(gd::Ref<HexMesh> mesh, OffsetCoordinates offset_coord)
    : _mesh(mesh), offset_coord(offset_coord), shifted(is_odd(offset_coord.row)) {}

Tile::Tile(gd::Ref<HexMesh> mesh, Vector3 offset, gd::GridMap* parent, OffsetCoordinates offset_coord)
    : _mesh(mesh), offset_coord(offset_coord), shifted(is_odd(offset_coord.row)) {
  _mesh_instance3d = memnew(MeshInstance3D());
  _sphere_shaped3d = Ref(memnew(SphereShape3D()));

  _sphere_shaped3d->set_radius(small_radius(mesh->get_diameter()));

  _collision_shape3d = memnew(CollisionShape3D());
  _collision_shape3d->set_shape(_sphere_shaped3d);

  _static_body = memnew(StaticBody3D());

  _mesh_instance3d->set_position(offset);
  _mesh_instance3d->set_mesh(mesh);

  parent->add_child(_mesh_instance3d);
  _mesh_instance3d->add_child(_static_body);
  _static_body->add_child(_collision_shape3d);
  if (Engine::get_singleton()->is_editor_hint()) {
    Node* root_scene = EditorInterface::get_singleton()->get_edited_scene_root();
    _mesh_instance3d->set_owner(root_scene);
    _static_body->set_owner(root_scene);
    _collision_shape3d->set_owner(root_scene);
  }

  _static_body->connect("mouse_entered", Callable(mesh.ptr(), "handle_mouse_entered"));
  _static_body->connect("mouse_exited", Callable(mesh.ptr(), "handle_mouse_exited"));
  _static_body->connect("input_event", Callable(mesh.ptr(), "handle_input_event"));
}

Ref<HexMesh> Tile::mesh() const { return _mesh; }

Biome BiomeTile::biome() const { return _biome; }

HexagonNeighbours BiomeTile::neighbours() const { return _neighbours; }
void BiomeTile::set_neighbours(HexagonNeighbours neighbours) { _neighbours = neighbours; }

Ref<HoneycombHoney> HoneycombTile::honey_mesh() const { return _honey; }

}  // namespace sota
