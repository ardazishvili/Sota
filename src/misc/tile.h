#pragma once

#include <cstdint>

#include "core/hex_mesh.h"
#include "honeycomb/honeycomb_cell.h"
#include "honeycomb/honeycomb_honey.h"
#include "misc/cube_coordinates.h"
#include "misc/types.h"
#include "ridge_impl/ridge_mesh.h"
#include "tal/mesh.h"
#include "tal/node.h"
#include "tal/reference.h"
#include "tile_mesh.h"

namespace sota {

class Tile : public Node3D {
  GDCLASS(Tile, Node3D);

 public:
  Tile() = default;
  Tile(const Tile& other) = default;
  Tile(Tile&& other) = default;
  // copying operator= defined inside GDCLASS
  Tile& operator=(Tile&& other) = delete;

  Tile(Ref<TileMesh> mesh, Node3D* parent);

  Ref<TileMesh> mesh() const;
  int id() const { return _mesh->get_id(); }

  void destroy() {
    get_parent()->remove_child(this);
    this->queue_free();
  }

  void replace_mesh(Ref<TileMesh> new_mesh) {
    _mesh = new_mesh;
    auto points = new_mesh->inner_mesh()->base().points();
    auto center = new_mesh->inner_mesh()->base().center();
    _sphere_shaped3d->set_radius(center.distance_to(points[0]));
    _main_mesh_instance->set_mesh(new_mesh->inner_mesh());
  }

 protected:
  StaticBody3D* _static_body{nullptr};
  Ref<SphereShape3D> _sphere_shaped3d{nullptr};
  Ref<TileMesh> _mesh;
  MeshInstance3D* _main_mesh_instance{nullptr};

  static void _bind_methods() {}
  virtual void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                                  const Vector3& p_normal, int32_t p_shape_idx) = 0;
  virtual void handle_mouse_entered() = 0;
  virtual void handle_mouse_exited() = 0;

 private:
  CollisionShape3D* _collision_shape3d{nullptr};
};

class OffsetTile : public Tile {
  GDCLASS(OffsetTile, Tile);

 public:
  OffsetTile() = default;
  OffsetTile(const OffsetTile& other) = default;
  OffsetTile(OffsetTile&& other) = default;
  // copying operator= defined inside GDCLASS
  OffsetTile& operator=(OffsetTile&& other) = delete;

  OffsetTile(Ref<TileMesh> mesh, Vector3 offset, Node3D* parent, OffsetCoordinates offset_coord)
      : Tile(mesh, parent), _offset_coord(offset_coord), _shifted(is_odd(offset_coord.row)) {
    auto points = _mesh->inner_mesh()->base().points();
    auto center = _mesh->inner_mesh()->base().center();
    _sphere_shaped3d->set_radius(center.distance_to(points[0]));

    _static_body->set_position(offset);
  }
  bool is_shifted() const { return _shifted; }
  OffsetCoordinates get_offset_coords() const { return _offset_coord; }
  CubeCoordinates get_cube_coords() const { return offsetToCube(_offset_coord); }

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx) override {}
  void handle_mouse_entered() override {}
  void handle_mouse_exited() override {}

 protected:
  static void _bind_methods() {}

 private:
  OffsetCoordinates _offset_coord;
  const bool _shifted = false;  // odd rows are shifted by half of small radius
};

class BiomeTile : public OffsetTile {
  GDCLASS(BiomeTile, OffsetTile)
 public:
  BiomeTile() = default;
  BiomeTile(const BiomeTile& other) = default;
  BiomeTile(BiomeTile&& other) = default;
  // copying operator= defined inside GDCLASS
  BiomeTile& operator=(BiomeTile&& other) = delete;

  BiomeTile(Ref<RidgeMesh> ridge_hex_mesh, Node3D* parent, Biome biome, OffsetCoordinates offset_coord, int row,
            int col);

  // getters
  Biome biome() const;
  void set_biome(Biome biome);
  Neighbours neighbours() const;
  int row() const { return _row; }
  int col() const { return _col; }

  // setters
  void set_neighbours(Neighbours neighbours);

 protected:
  static void _bind_methods();

 private:
  Biome _biome;
  Neighbours _neighbours;

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx) override;
  void handle_mouse_entered() override;
  void handle_mouse_exited() override;

  int _row = -1;  // INVALID BY DEFAULT
  int _col = -1;  // INVALID BY DEFAULT
};

class HoneycombTile : public OffsetTile {
 public:
  HoneycombTile() = delete;
  HoneycombTile(Ref<HoneycombCell> walls, Ref<HoneycombHoney> honey, Node3D* parent, OffsetCoordinates offset_coord);

  // getters
  Ref<HoneycombHoney> honey_mesh() const;

  void handle_input_event(Camera3D* p_camera, const Ref<InputEvent>& p_event, const Vector3& p_event_position,
                          const Vector3& p_normal, int32_t p_shape_idx) override {}
  void handle_mouse_entered() override {}
  void handle_mouse_exited() override {}

 private:
  Ref<HoneycombHoney> _honey;
  MeshInstance3D* _second_mesh_instance{nullptr};
};

}  // namespace sota
