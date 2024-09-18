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
 public:
  Tile() = delete;
  virtual ~Tile();

  Tile(Ref<TileMesh> mesh, Vector3 offset, Node3D* parent, OffsetCoordinates offset_coord);

  Ref<TileMesh> mesh() const;
  int id() const { return _mesh->get_id(); }
  bool is_shifted() const { return _shifted; }
  OffsetCoordinates get_offset_coords() const { return _offset_coord; }
  CubeCoordinates get_cube_coords() const { return offsetToCube(_offset_coord); }

 private:
  Ref<SphereShape3D> _sphere_shaped3d{nullptr};
  CollisionShape3D* _collision_shape3d{nullptr};
  StaticBody3D* _static_body{nullptr};
  MeshInstance3D* _main_mesh_instance{nullptr};

  Ref<TileMesh> _mesh;
  OffsetCoordinates _offset_coord;
  const bool _shifted;  // odd rows are shifted by half of small radius
};

class BiomeTile : public Tile {
 public:
  BiomeTile() = delete;
  BiomeTile(Ref<RidgeMesh> ridge_hex_mesh, Node3D* parent, Biome biome, OffsetCoordinates offset_coord)
      : Tile(ridge_hex_mesh, ridge_hex_mesh->get_center(), parent, offset_coord), _biome(biome) {}

  // getters
  Biome biome() const;
  Neighbours neighbours() const;

  // setters
  void set_neighbours(Neighbours neighbours);

 private:
  Biome _biome;
  Neighbours _neighbours;
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
