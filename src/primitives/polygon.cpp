#include "primitives/polygon.h"

#include <algorithm>  // for sort

#include "tal/vector3.h"  // for Vector3

namespace sota {

void RegularPolygon::sort_points() {
  Vector3 v0 = _points[0] - _center;
  std::sort(_points.begin(), _points.end(), [this, v0](Vector3 a, Vector3 b) {
    Vector3 v1 = a - _center;
    Vector3 v2 = b - _center;
    return v1.signed_angle_to(v0, _center) < v2.signed_angle_to(v0, _center);
  });
}
}  // namespace sota
