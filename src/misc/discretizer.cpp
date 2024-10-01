#include "misc/discretizer.h"

#include <cmath>

#include "misc/utilities.h"
#include "tal/vector3i.h"

namespace sota {

DiscreteVertex VertexToNormalDiscretizer::get_discrete_vertex(Vector3 v, float step) {
  return DiscreteVertex(std::round(v.x / step), std::round(v.y / step), std::round(v.z / step));
};

}  // namespace sota
