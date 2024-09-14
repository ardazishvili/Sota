#pragma once

#include "primitives/Polygon.h"
#include "tal/vector3.h"

namespace sota {

class Hexagon : public RegularPolygon {
 public:
  Hexagon(Vector3 center, std::vector<Vector3> points, Vector3 normal) : RegularPolygon(center, points, normal) {}
  Hexagon(Vector3 center, Vector3 normal) : RegularPolygon(center, normal) {}
  Hexagon(const Hexagon& other) = default;
  Hexagon(Hexagon&& other) = default;
  Hexagon& operator=(const Hexagon& rhs) = default;
  Hexagon& operator=(Hexagon&& rhs) = default;

  void check() const override;
  static std::vector<Vector3> calculate_points(Vector3 center, float diameter);
};

Hexagon make_hexagon_at_position(Vector3 position, float diameter);
Hexagon make_hexagon_at_origin(float diameter);
Hexagon make_unit_hexagon();

}  // namespace sota
