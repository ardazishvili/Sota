#include "tile.h"

#include "types.h"

namespace sota {
using namespace gd;

Ref<HexMesh> Tile::mesh() const { return _mesh; }

Biome BiomeTile::biome() const { return _biome; }
HexagonNeighbours BiomeTile::neighbours() const { return _neighbours; }
void BiomeTile::set_neighbours(HexagonNeighbours neighbours) { _neighbours = neighbours; }

}  // namespace sota
