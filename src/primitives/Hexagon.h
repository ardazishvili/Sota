#pragma once

#include "godot_cpp/variant/vector3.hpp"
#include "primitives/Polygon.h"

namespace sota {

class Hexagon : public RegularPolygon {
 public:
  Hexagon(godot::Vector3 center, std::vector<godot::Vector3> points, godot::Vector3 normal)
      : RegularPolygon(center, points, normal) {}
  Hexagon(godot::Vector3 center, godot::Vector3 normal) : RegularPolygon(center, normal) {}
  Hexagon(const Hexagon& other) = default;
  Hexagon(Hexagon&& other) = default;
  Hexagon& operator=(const Hexagon& rhs) = default;
  Hexagon& operator=(Hexagon&& rhs) = default;

  void check() const override;
  static std::vector<godot::Vector3> calculate_points(godot::Vector3 center, float diameter);
};

Hexagon make_hexagon_at_position(godot::Vector3 position, float diameter);
Hexagon make_hexagon_at_origin(float diameter);
Hexagon make_unit_hexagon();

}  // namespace sota
