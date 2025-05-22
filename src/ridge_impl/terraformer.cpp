#include "ridge_impl/terraformer.h"

#include "ridge_impl/ridge_hex_grid.h"
#include "tile.h"

namespace sota {

void Terraformer::process(int row, int col) {
  BiomeTile *biome_tile = dynamic_cast<BiomeTile *>((*_tiles_layout)[row][col]);
  if (biome_tile->biome() == _biome_to_set) {
    return;
  }
  _ridge_hex_grid->update_biome(biome_tile, _biome_to_set);
}

}  // namespace sota
