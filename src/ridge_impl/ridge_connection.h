#pragma once

#include <utility>

#include "tal/vector3.h"

namespace sota {

struct RidgeVertex {
  RidgeVertex(Vector3 p_coord, Vector3 p_normal) : coord(p_coord), normal(p_normal) {}

  Vector3 coord;
  Vector3 normal;  // normal vector to base polygon at 'coord'
};

class RidgeConnection {
 public:
  RidgeConnection(RidgeVertex first, RidgeVertex second) : _first(first), _second(second) {}

  std::pair<RidgeVertex, RidgeVertex> get() const { return std::make_pair(_first, _second); }

 private:
  RidgeVertex _first;
  RidgeVertex _second;
};
}  // namespace sota
