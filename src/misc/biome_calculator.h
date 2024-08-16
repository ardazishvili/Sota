#pragma once

#include <unordered_map>

#include "misc/types.h"

namespace sota {

class BiomeCalculator {
 public:
  BiomeCalculator();
  Biome calculate_biome(float min_z, float max_z, float cur_z) const;

 private:
  std::unordered_map<Biome, float> BIOMES_HEIGHT_BOUND;
  float water_threshold = 0.2f;
  float plain_threshold = 0.65f;
  float hill_threshold = 0.75f;
};

}  // namespace sota
