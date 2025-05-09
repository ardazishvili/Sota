#include "biome_calculator.h"

#include <vector>

#include "tal/godot_core.h"

namespace sota {

BiomeCalculator::BiomeCalculator() {
  _BIOMES_HEIGHT_BOUND = {{Biome::WATER, _water_threshold},
                          {Biome::PLAIN, _plain_threshold},
                          {Biome::HILL, _hill_threshold},
                          {Biome::MOUNTAIN, 1.0f}};
}

Biome BiomeCalculator::calculate_biome(float min_z, float max_z, float cur_z) const {
  auto amplitude = max_z - min_z;

  auto water_max_z = min_z + amplitude * _BIOMES_HEIGHT_BOUND.find(Biome::WATER)->second;
  auto plain_max_z = min_z + amplitude * _BIOMES_HEIGHT_BOUND.find(Biome::PLAIN)->second;
  auto hill_max_z = min_z + amplitude * _BIOMES_HEIGHT_BOUND.find(Biome::HILL)->second;
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
    printerr("Non reachable: unknown biome");
  }
  return Biome::WATER;  // unreachable
}

std::vector<std::vector<Biome>> BiomeCalculator::calculate_biomes(float min_z, float max_z,
                                                                  std::vector<std::vector<float>> altitudes) {
  _BIOMES_HEIGHT_BOUND = {{Biome::WATER, _water_threshold},
                          {Biome::PLAIN, _plain_threshold},
                          {Biome::HILL, _hill_threshold},
                          {Biome::MOUNTAIN, 1.0f}};
  std::vector<std::vector<Biome>> biomes;
  BiomeCalculator calc;
  int row_num = altitudes.size();
  for (int row = 0; row < row_num; ++row) {
    biomes.push_back({});
    int col_num = altitudes[row].size();
    for (int col = 0; col < col_num; ++col) {
      biomes.back().push_back(calc.calculate_biome(min_z, max_z, altitudes[row][col]));
    }
  }
  return biomes;
}

}  // namespace sota
