#pragma once

#include "core/hex_mesh.h"
#include "misc/types.h"
#include "ridge_impl/ridge_hex_mesh.h"

namespace sota {

class Tile {
 public:
  virtual ~Tile() = default;

  Tile(gd::Ref<HexMesh> mesh) : _mesh(mesh) {}
  gd::Ref<HexMesh> mesh() const;

 private:
  gd::Ref<HexMesh> _mesh;
};

class BiomeTile : public Tile {
 public:
  BiomeTile() = delete;
  BiomeTile(gd::Ref<RidgeHexMesh> mesh, Biome biome) : Tile(mesh), _biome(biome) {}

  // getters
  Biome biome() const;
  HexagonNeighbours neighbours() const;

  // setters
  void set_neighbours(HexagonNeighbours neighbours);

 private:
  Biome _biome;
  HexagonNeighbours _neighbours;
};

}  // namespace sota
