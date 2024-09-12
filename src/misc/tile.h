#pragma once

#include <cstdint>

#include "core/hex_mesh.h"
#include "cube_coordinates.h"
#include "godot_cpp/classes/collision_shape3d.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/sphere_shape3d.hpp"
#include "godot_cpp/classes/static_body3d.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "honeycomb/honeycomb_cell.h"
#include "honeycomb/honeycomb_honey.h"
#include "misc/types.h"
#include "ridge_impl/ridge_hex_mesh.h"

namespace sota {

class Tile : public godot::Node3D {
 public:
  Tile() = delete;
  virtual ~Tile();

  Tile(gd::Ref<HexMesh> mesh, gd::Vector3 offset, gd::Node3D* parent, OffsetCoordinates offset_coord);

  gd::Ref<HexMesh> mesh() const;
  int id() const { return _mesh->get_id(); }
  bool is_shifted() const { return shifted; }
  OffsetCoordinates get_offset_coords() const { return offset_coord; }
  CubeCoordinates get_cube_coords() const { return offsetToCube(offset_coord); }

 private:
  gd::Ref<gd::SphereShape3D> _sphere_shaped3d{nullptr};
  gd::CollisionShape3D* _collision_shape3d{nullptr};
  gd::StaticBody3D* _static_body{nullptr};
  gd::MeshInstance3D* _main_mesh_instance{nullptr};

  gd::Ref<HexMesh> _mesh;
  OffsetCoordinates offset_coord;
  const bool shifted;  // odd rows are shifted by half of small radius
};

class BiomeTile : public Tile {
 public:
  BiomeTile() = delete;
  BiomeTile(gd::Ref<RidgeHexMesh> mesh, gd::Node3D* parent, Biome biome, OffsetCoordinates offset_coord)
      : Tile(mesh, mesh->get_center(), parent, offset_coord), _biome(biome) {}

  // getters
  Biome biome() const;
  HexagonNeighbours neighbours() const;

  // setters
  void set_neighbours(HexagonNeighbours neighbours);

 private:
  Biome _biome;
  HexagonNeighbours _neighbours;
};

class HoneycombTile : public Tile {
 public:
  HoneycombTile() = delete;
  HoneycombTile(gd::Ref<HoneycombCell> walls, gd::Ref<HoneycombHoney> honey, gd::Node3D* parent,
                OffsetCoordinates offset_coord);

  // getters
  gd::Ref<HoneycombHoney> honey_mesh() const;

 private:
  gd::Ref<HoneycombHoney> _honey;
  gd::MeshInstance3D* _second_mesh_instance{nullptr};
};

}  // namespace sota
