#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <unordered_map>
#include <vector>

#include "tal/vector3.h"
#include "tal/vector3i.h"

namespace sota {

class TileMesh;
using Neighbours = std::vector<TileMesh*>;

enum class Biome { PLAIN = 0, HILL, MOUNTAIN, WATER };

struct ClipOptions {
  bool left{false};
  bool right{false};
  bool up{false};
  bool down{false};
};

}  // namespace sota
