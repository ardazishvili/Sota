#pragma once

#include <ostream>
#include <vector>

namespace sota {

struct CubeCoordinates {
  int q;
  int r;
  int s;
};

struct OffsetCoordinates {
  int row;
  int col;
};

OffsetCoordinates cubeToOffset(CubeCoordinates coord);
CubeCoordinates offsetToCube(OffsetCoordinates coord);

bool operator<(const CubeCoordinates& lhs, const CubeCoordinates& rhs);
bool operator==(const CubeCoordinates& lhs, const CubeCoordinates& rhs);

CubeCoordinates& operator+=(CubeCoordinates& lhs, const CubeCoordinates& rhs);
CubeCoordinates operator+(CubeCoordinates lhs, const CubeCoordinates& rhs);
CubeCoordinates& operator-=(CubeCoordinates& lhs, const CubeCoordinates& rhs);
CubeCoordinates operator-(CubeCoordinates lhs, const CubeCoordinates& rhs);
CubeCoordinates operator-(const CubeCoordinates& rhs);
std::ostream& operator<<(std::ostream& oss, const CubeCoordinates& c);

CubeCoordinates unitCubeQ();
CubeCoordinates unitCubeR();
CubeCoordinates unitCubeS();
CubeCoordinates cubeRound(float frac_q, float frac_r, float frac_s);
CubeCoordinates pixelToCube(float x, float y, float R);
std::vector<CubeCoordinates> neighbours(CubeCoordinates current);

}  // namespace sota
