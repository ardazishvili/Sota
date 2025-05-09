#include "ridge_impl/terraformer.h"

#include "ridge_impl/ridge_hex_grid.h"

namespace sota {

void Terraformer::process(int row, int col) {
  (*_tiles_layout)[row][col]->destroy();
  (*_tiles_layout)[row][col] = _ridge_hex_grid->make_biome_tile(_biome_to_set, row, col);
}

}  // namespace sota
