#pragma once

#include "tal/arrays.h"
#include "tal/vector3.h"

namespace sota {

struct Triangle {
  Vector3 a;
  Vector3 b;
  Vector3 c;

  Vector3Array to_godot_array() const { return Vector3Array{a, b, c}; }
};

}  // namespace sota
