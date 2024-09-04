#include "primitives/Polygon.h"

namespace sota {

using namespace godot;

void RegularPolygon::sort_points() {
  godot::Vector3 v0 = _points[0] - _center;
  std::sort(_points.begin(), _points.end(), [this, v0](godot::Vector3 a, godot::Vector3 b) {
    godot::Vector3 v1 = a - _center;
    godot::Vector3 v2 = b - _center;
    return v1.signed_angle_to(v0, _center) < v2.signed_angle_to(v0, _center);
  });
}
}  // namespace sota
