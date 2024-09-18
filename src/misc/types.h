#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <unordered_map>
#include <vector>

#include "tal/vector3.h"

namespace sota {

class TileMesh;

using Neighbours = std::vector<TileMesh*>;
using GroupedMeshVertices = std::map<std::pair<int, int>, std::vector<Vector3*>>;

enum class Biome { PLAIN = 0, HILL, MOUNTAIN, WATER };

struct ClipOptions {
  bool left{false};
  bool right{false};
  bool up{false};
  bool down{false};
};

}  // namespace sota
