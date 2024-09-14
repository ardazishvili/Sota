#pragma once

#include "primitives/Polygon.h"
#include "tal/vector3.h"

namespace sota {

class Pentagon : public RegularPolygon {
 public:
  Pentagon(Vector3 center, std::vector<Vector3> points, Vector3 normal) : RegularPolygon(center, points, normal) {}
  Pentagon(Vector3 center, Vector3 normal) : RegularPolygon(center, normal) {}
  Pentagon(const Pentagon& other) = default;
  Pentagon(Pentagon&& other) = default;
  Pentagon& operator=(const Pentagon& rhs) = default;
  Pentagon& operator=(Pentagon&& rhs) = default;

  void check() const override;
  static std::vector<Vector3> calculate_points(Vector3 center, float diameter);
};

Pentagon make_unit_pentagon();
}  // namespace sota
