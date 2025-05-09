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

namespace sota {

class Tile : public Node3D {
  GDCLASS(Tile, Node3D);

 public:
  Tile() = default;
  Tile(const Tile& other) = default;
  Tile(Tile&& other) = default;
  // copying operator= defined inside GDCLASS
  Tile& operator=(Tile&& other) = delete;

  Tile(Ref<TileMesh> mesh, Vector3 offset, Node3D* parent, OffsetCoordinates offset_coord);

  Ref<TileMesh> mesh() const;
  int id() const { return _mesh->get_id(); }
  bool is_shifted() const { return _shifted; }
  OffsetCoordinates get_offset_coords() const { return _offset_coord; }
  CubeCoordinates get_cube_coords() const { return offsetToCube(_offset_coord); }

  void destroy() {
    get_parent()->remove_child(this);
    this->queue_free();
  }

 protected:
  StaticBody3D* _static_body{nullptr};
  static void _bind_methods() {}

 private:
  Ref<SphereShape3D> _sphere_shaped3d{nullptr};
  CollisionShape3D* _collision_shape3d{nullptr};
  MeshInstance3D* _main_mesh_instance{nullptr};

  Ref<TileMesh> _mesh;
  OffsetCoordinates _offset_coord;
  const bool _shifted = false;  // odd rows are shifted by half of small radius
};

class BiomeTile : public Tile {
  GDCLASS(BiomeTile, Tile)
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
                          const Vector3& p_normal, int32_t p_shape_idx);
  void handle_mouse_entered();
  void handle_mouse_exited();

  int _row = -1;  // INVALID BY DEFAULT
  int _col = -1;  // INVALID BY DEFAULT
};

class HoneycombTile : public Tile {
 public:
  HoneycombTile() = delete;
  HoneycombTile(Ref<HoneycombCell> walls, Ref<HoneycombHoney> honey, Node3D* parent, OffsetCoordinates offset_coord);

  // getters
  Ref<HoneycombHoney> honey_mesh() const;

 private:
  Ref<HoneycombHoney> _honey;
  MeshInstance3D* _second_mesh_instance{nullptr};
};

}  // namespace sota
