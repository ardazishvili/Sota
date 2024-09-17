#pragma once

#include <array>
#include <map>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>

#include "tal/vector3.h"

namespace sota {

class HexMesh;

using HexagonNeighbours = std::array<HexMesh*, 6>;
using GroupedHexagonMeshVertices = std::map<std::pair<int, int>, std::vector<Vector3*>>;

enum class Biome { PLAIN = 0, HILL, MOUNTAIN, WATER };

struct ClipOptions {
  bool left{false};
  bool right{false};
  bool up{false};
  bool down{false};
};

}  // namespace sota
