#pragma once

#include "primitives/edge.h"
#include "primitives/triangle.h"

namespace sota {

class Face {
 public:
  Face(Edge f, Edge s) {
    first = Triangle{.a = f.a, .b = f.b, .c = s.a};
    second = Triangle{.a = s.a, .b = f.b, .c = s.b};
  }

  std::pair<Triangle, Triangle> get_triangles() { return {first, second}; }

 private:
  Triangle first;
  Triangle second;
};

}  // namespace sota
