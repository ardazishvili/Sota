#include "misc/cube_coordinates.h"

#include <cmath>

namespace sota {

// cube to odd-r conversion
OffsetCoordinates cubeToOffset(CubeCoordinates coord) {
  int col = coord.q + (coord.r - (coord.r & 1)) / 2;
  int row = -coord.r;
  return OffsetCoordinates{.row = row, .col = col};
}

// odd-r to cube conversion
CubeCoordinates offsetToCube(OffsetCoordinates coord) {
  int q = coord.col + ((coord.row & 1) + coord.row) / 2;
  int r = -coord.row;
  return CubeCoordinates{.q = q, .r = r, .s = -q - r};
}

bool operator==(const CubeCoordinates& lhs, const CubeCoordinates& rhs) {
  return lhs.q == rhs.q && lhs.r == rhs.r && lhs.s == rhs.s;
}

bool operator<(const CubeCoordinates& lhs, const CubeCoordinates& rhs) {
  return std::vector<int>{lhs.q, lhs.r, lhs.q} < std::vector<int>{rhs.q, rhs.r, rhs.q};
}

CubeCoordinates& operator+=(CubeCoordinates& lhs, const CubeCoordinates& rhs) {
  lhs.q += rhs.q;
  lhs.r += rhs.r;
  lhs.s += rhs.s;
  return lhs;
}

CubeCoordinates operator+(CubeCoordinates lhs, const CubeCoordinates& rhs) {
  lhs += rhs;
  return lhs;
}

CubeCoordinates& operator-=(CubeCoordinates& lhs, const CubeCoordinates& rhs) {
  lhs.q -= rhs.q;
  lhs.r -= rhs.r;
  lhs.s -= rhs.s;
  return lhs;
}

CubeCoordinates operator-(CubeCoordinates lhs, const CubeCoordinates& rhs) {
  lhs -= rhs;
  return lhs;
}

CubeCoordinates operator-(const CubeCoordinates& a) { return CubeCoordinates{.q = -a.q, .r = -a.r, .s = -a.s}; }

std::ostream& operator<<(std::ostream& oss, const CubeCoordinates& c) {
  oss << "(Q: " << c.q << ", R: " << c.r << ", S: " << c.s << ")";
  return oss;
}

CubeCoordinates unitCubeQ() { return CubeCoordinates{.q = 1, .r = 0, .s = 0}; }
CubeCoordinates unitCubeR() { return CubeCoordinates{.q = 0, .r = 1, .s = 0}; }
CubeCoordinates unitCubeS() { return CubeCoordinates{.q = 0, .r = 0, .s = 1}; }

CubeCoordinates cubeRound(float frac_q, float frac_r, float frac_s) {
  int q = std::round(frac_q);
  int r = std::round(frac_r);
  int s = std::round(frac_s);

  float q_diff = std::abs(q - frac_q);
  float r_diff = std::abs(r - frac_r);
  float s_diff = std::abs(s - frac_s);

  if (q_diff > r_diff && q_diff > s_diff) {
    q = -r - s;
  } else if (r_diff > s_diff) {
    r = -q - s;
  } else {
    s = -q - r;
  }

  return CubeCoordinates{.q = q, .r = r, .s = s};
}

CubeCoordinates pixelToCube(float x, float y, float R) {
  float q = (std::sqrt(3) / 3 * x + 1.0 / 3 * y) / R;
  float r = (-2.0 / 3 * y) / R;
  return cubeRound(q, r, -q - r);
}

std::vector<CubeCoordinates> neighbours(CubeCoordinates current) {
  return {
      {current + unitCubeR() - unitCubeS()}, {current + unitCubeQ() - unitCubeS()},
      {current + unitCubeQ() - unitCubeR()}, {current - unitCubeR() + unitCubeS()},
      {current - unitCubeQ() + unitCubeS()}, {current - unitCubeQ() + unitCubeR()},
  };
}

}  // namespace sota
