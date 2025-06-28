#pragma once

#include "core/mesh.h"
#include "misc/discretizer.h"
#include "misc/types.h"
#include "tal/reference.h"

namespace sota {

class SotaMesh;
class Tile;

class TileMesh : public RefCounted {
  GDCLASS(TileMesh, RefCounted)

 public:
  virtual int get_id() = 0;
  virtual SotaMesh* inner_mesh() const = 0;

  DiscreteVertexToNormals get_discrete_vertex_to_normals();
  Tile* tile() const { return _tile; }
  void set_tile(Tile* tile) { _tile = tile; }

 protected:
  static void _bind_methods() {}
  Tile* _tile = nullptr;
};

}  // namespace sota
