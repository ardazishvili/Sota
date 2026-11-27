#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>

#include "misc/cube_coordinates.h"

using sota::CubeCoordinates;
using sota::OffsetCoordinates;

TEST_CASE("Cube and odd-row offset coordinates agree across row parity and sign") {
  struct Conversion {
    CubeCoordinates cube;
    OffsetCoordinates offset;
  };
  const Conversion cases[] = {
      {{2, 3, -5}, {-3, 3}},
      {{2, 2, -4}, {-2, 3}},
      {{-2, 0, 2}, {0, -2}},
      {{2, -2, 0}, {2, 1}},
      {{2, -3, 1}, {3, 0}},
  };
  for (const auto& [cube, offset] : cases) {
    CAPTURE(offset.row);
    const auto converted = sota::cubeToOffset(cube);
    CHECK(converted.row == offset.row);
    CHECK(converted.col == offset.col);
    CHECK(sota::offsetToCube(offset) == cube);
    CHECK(sota::offsetToCube(converted) == cube);
  }
}

TEST_CASE("Cube rounding corrects the component with the largest rounding error") {
  const CubeCoordinates q_adjusted = {1, 0, -1};
  const CubeCoordinates r_adjusted = {-1, 1, 0};
  const CubeCoordinates s_adjusted = {0, -1, 1};
  CHECK(sota::cubeRound(0.49f, 0.25f, -0.74f) == q_adjusted);
  CHECK(sota::cubeRound(-0.74f, 0.49f, 0.25f) == r_adjusted);
  CHECK(sota::cubeRound(0.25f, -0.74f, 0.49f) == s_adjusted);
}

TEST_CASE("Pixel conversion maps known hex centers to their cube coordinates") {
  constexpr float radius = 2.0f;
  const float half_horizontal_spacing = std::sqrt(3.0f);
  const CubeCoordinates origin = {0, 0, 0};
  const CubeCoordinates right = {1, 0, -1};
  const CubeCoordinates lower_right = {1, -1, 0};
  CHECK(sota::pixelToCube(0.0f, 0.0f, radius) == origin);
  CHECK(sota::pixelToCube(2.0f * half_horizontal_spacing, 0.0f, radius) == right);
  CHECK(sota::pixelToCube(half_horizontal_spacing, 3.0f, radius) == lower_right);
}

TEST_CASE("A translated hex has six distinct adjacent cube coordinates") {
  const CubeCoordinates center = {2, -1, -1};
  const CubeCoordinates expected[] = {
      {2, 0, -2}, {3, -1, -2}, {3, -2, -1}, {2, -2, 0}, {1, -1, 0}, {1, 0, -1},
  };
  const auto actual = sota::neighbours(center);
  CHECK(actual.size() == 6);
  for (const auto& neighbour : expected) {
    CAPTURE(neighbour);
    CHECK(std::count(actual.begin(), actual.end(), neighbour) == 1);
  }
}
