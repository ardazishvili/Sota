#pragma once

#include <unordered_map>

#include "misc/types.h"

namespace sota {

class BiomeCalculator {
 public:
  BiomeCalculator();
  Biome calculate_biome(float min_z, float max_z, float cur_z) const;
  std::vector<std::vector<Biome>> calculate_biomes(float min_z, float max_z, std::vector<std::vector<float>> altitudes);

 private:
  std::unordered_map<Biome, float> _BIOMES_HEIGHT_BOUND;
  float _water_threshold = 0.2f;
  float _plain_threshold = 0.65f;
  float _hill_threshold = 0.75f;
};

}  // namespace sota
