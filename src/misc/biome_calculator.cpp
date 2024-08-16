#include "biome_calculator.h"

#include "godot_cpp/variant/utility_functions.hpp"

namespace sota {

BiomeCalculator::BiomeCalculator() {
  BIOMES_HEIGHT_BOUND = {{Biome::WATER, water_threshold},
                         {Biome::PLAIN, plain_threshold},
                         {Biome::HILL, hill_threshold},
                         {Biome::MOUNTAIN, 1.0f}};
}

Biome BiomeCalculator::calculate_biome(float min_z, float max_z, float cur_z) const {
  auto amplitude = max_z - min_z;
  auto water_max_z = min_z + amplitude * BIOMES_HEIGHT_BOUND.find(Biome::WATER)->second;
  auto plain_max_z = min_z + amplitude * BIOMES_HEIGHT_BOUND.find(Biome::PLAIN)->second;
  auto hill_max_z = min_z + amplitude * BIOMES_HEIGHT_BOUND.find(Biome::HILL)->second;
  auto mountain_max_z = max_z;

  if (min_z <= cur_z && cur_z <= water_max_z) {
    return Biome::WATER;
  } else if (water_max_z < cur_z && cur_z <= plain_max_z) {
    return Biome::PLAIN;
  } else if (plain_max_z < cur_z && cur_z <= hill_max_z) {
    return Biome::HILL;
  } else if (hill_max_z < cur_z && cur_z <= mountain_max_z) {
    return Biome::MOUNTAIN;
  } else {
    godot::UtilityFunctions::printerr("Non reachable: unknown biome");
  }
  return Biome::WATER;  // unreachable
}

}  // namespace sota
