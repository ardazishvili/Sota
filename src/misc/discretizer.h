#pragma once

#include <map>
#include <vector>

#include "tal/arrays.h"
#include "tal/vector3.h"
#include "tal/vector3i.h"

namespace sota {

using DiscreteVertex = Vector3i;
using Distance = float;
using DiscreteVertexToDistance = std::map<DiscreteVertex, Distance>;
using DiscreteVertexToNormals = std::map<DiscreteVertex, std::vector<Vector3*>>;

class VertexToNormalDiscretizer {
 public:
  VertexToNormalDiscretizer(float step) : _step(step) {}

  static DiscreteVertex get_discrete_vertex(Vector3 v, float step);

  void discretize(Vector3Array vertices, std::vector<Vector3>& normals) {
    int size = vertices.size();
    for (int i = 0; i < size; ++i) {
      Vector3 v = vertices[i];
      Vector3& n = normals[i];
      auto p = get_discrete_vertex(v, _step);
      _discretized[p].push_back(&n);
    }
  }

  DiscreteVertexToNormals get() { return _discretized; }

 private:
  DiscreteVertexToNormals _discretized;
  float _step;
};
}  // namespace sota
