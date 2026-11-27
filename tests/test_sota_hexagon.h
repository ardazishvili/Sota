#pragma once

#include <cmath>
#include <cstddef>

#include "primitives/hexagon.h"
#include "tests/test_macros.h"

namespace sota::tests {

TEST_CASE("[Sota] Hexagon vertices lie on a circle around a translated center") {
  const Vector3 center(3, 2, -4);
  constexpr float diameter = 4.0f;
  constexpr real_t tolerance = 0.00001;
  const auto points = Hexagon::calculate_points(center, diameter);

  REQUIRE(points.size() == 6);
  for (std::size_t i = 0; i < points.size(); ++i) {
    const Vector3 offset = points[i] - center;
    CHECK(std::abs(offset.y) <= tolerance);
    CHECK(std::abs(offset.length() - diameter / 2.0f) <= tolerance);
    for (std::size_t j = 0; j < i; ++j) {
      CHECK(points[i].distance_squared_to(points[j]) > tolerance * tolerance);
    }
  }
}

}  // namespace sota::tests
