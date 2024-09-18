#pragma once

#include "core/mesh.h"
#include "tal/reference.h"

namespace sota {

class SotaMesh;

class TileMesh : public RefCounted {
  GDCLASS(TileMesh, RefCounted)

 public:
  virtual int get_id() = 0;
  virtual SotaMesh* inner_mesh() = 0;

 protected:
  static void _bind_methods() {}
};

}  // namespace sota
