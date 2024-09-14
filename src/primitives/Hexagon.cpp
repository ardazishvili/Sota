#include "primitives/Hexagon.h"

#include "algo/constants.h"
#include "core/utils.h"
#include "tal/godot_core.h"

namespace sota {

void Hexagon::check() const {
  if (_points.size() != 6) {
    printerr("Hexagon has != 6 points");
  }
}

std::vector<Vector3> Hexagon::calculate_points(Vector3 center, float diameter) {
  std::vector<Vector3> result;
  float R = radius(diameter);
  float a = -PI / 6;
  for (int i = 0; i < 6; ++i) {
    result.push_back(center + Vector3(std::cos(a + i * PI / 3) * R, 0, std::sin(a + i * PI / 3) * R));
  }
  return result;
}

Hexagon make_hexagon_at_position(Vector3 position, float diameter) {
  return Hexagon(position, Hexagon::calculate_points(position, diameter), Vector3(0, 1, 0));
}

Hexagon make_hexagon_at_origin(float diameter) { return make_hexagon_at_position(Vector3(0, 0, 0), diameter); }

Hexagon make_unit_hexagon() { return make_hexagon_at_origin(1); }
}  // namespace sota
