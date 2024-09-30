#include "primitives/pentagon.h"

#include <cmath>  // for cos, sin

#include "algo/constants.h"  // for PI
#include "core/utils.h"      // for radius
#include "tal/godot_core.h"  // for printerr
#include "tal/vector3.h"     // for Vector3

namespace sota {

void Pentagon::check() const {
  if (_points.size() != 5) {
    printerr("Pentagon has != 5 points");
  }
}

std::vector<Vector3> Pentagon::calculate_points(Vector3 center, float diameter) {
  std::vector<Vector3> result;
  float R = radius(diameter);

  for (int i = 0; i < 5; ++i) {
    result.push_back(center + Vector3(std::cos(i * 2 * PI / 5) * R, 0, std::sin(i * 2 * PI / 5) * R));
  }
  return result;
}

Pentagon make_unit_pentagon() {
  Vector3 center = Vector3(0, 0, 0);
  return Pentagon(center, Pentagon::calculate_points(center, 1), Vector3(0, 1, 0));
}
}  // namespace sota
