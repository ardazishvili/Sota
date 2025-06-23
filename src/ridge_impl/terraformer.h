#pragma once

#include "core/hex_grid.h"
#include "misc/types.h"
#include "tal/godot_core.h"
#include "tal/reference.h"
#include "tal/wrapped.h"

namespace sota {

// TODO: remove backward dependency - deep refactor required to factor out creation of BiomeTile from RidgeHexGrid
// class, i.e. separate issue https://github.com/ardazishvili/Sota/issues/82
class RidgeHexGrid;

class MatrixProcessor : public RefCounted {
  GDCLASS(MatrixProcessor, RefCounted)
 public:
  virtual void process(int row, int col) = 0;

 protected:
  static void _bind_methods() {}
};

class Terraformer : public MatrixProcessor {
  GDCLASS(Terraformer, RefCounted)
 public:
  void set_biome_to_set(int biome) { _biome_to_set = static_cast<Biome>(biome); }
  Biome get_biome_to_set() const { return _biome_to_set; }

  void process(int row, int col) override;

  void set_tiles(TilesLayout* tiles) { _tiles_layout = tiles; }
  void set_ridge_hex_grid(RidgeHexGrid* ridge_hex_grid) { _ridge_hex_grid = ridge_hex_grid; }

 protected:
  static void _bind_methods() { ClassDB::bind_method(D_METHOD("set_biome_to_set"), &Terraformer::set_biome_to_set); }

 private:
  Biome _biome_to_set = Biome::PLAIN;
  TilesLayout* _tiles_layout;
  RidgeHexGrid* _ridge_hex_grid;
};

}  // namespace sota
