#pragma once

#include "core/mesh.h"
#include "misc/discretizer.h"
#include "misc/types.h"
#include "tal/reference.h"
#include "tal/wrapped.h"  // for GDCLASS

namespace sota {

class SotaMesh;
class Tile;

/**
 * @brief Reference-counted interface connecting a tile to its generated mesh.
 *
 * Implementations supply a tile ID and the underlying SotaMesh. The associated Tile pointer
 * is non-owning.
 */
class TileMesh : public RefCounted {
  GDCLASS(TileMesh, RefCounted)

 public:
  virtual int get_id() = 0;
  virtual SotaMesh* inner_mesh() const = 0;

  /// Groups pointers to the mesh normals by quantized vertex position for smoothing.
  DiscreteVertexToNormals get_discrete_vertex_to_normals();
  Tile* tile() const { return _tile; }
  void set_tile(Tile* tile) { _tile = tile; }

 protected:
  static void _bind_methods() {}
  Tile* _tile = nullptr;
};

}  // namespace sota
