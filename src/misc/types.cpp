#include "misc/types.h"

#include "tal/godot_core.h"

namespace sota {

std::string biome_to_string(Biome biome) {
  switch (biome) {
    case Biome::PLAIN:
      return "plain";
    case Biome::HILL:
      return "hill";
    case Biome::MOUNTAIN:
      return "mountain";
    case Biome::WATER:
      return "water";
  }

  printerr("can't convert Biome to string");  // should be never reached
  return "unreachable code";
}

}  // namespace sota
