#pragma once

#include <functional>
#include <iterator>
#include <vector>

#include "ridge.h"
#include "ridge_impl/ridge_group.h"
#include "tal/godot_core.h"

namespace sota {

class RidgeBased {
 public:
  void print_biomes() {
    print("\n");
    for (RidgeGroup& group : _mountain_groups) {
      print("Mountain group of size ", group.meshes().size());
    }
    for (RidgeGroup& group : _water_groups) {
      print("Water group of size ", group.meshes().size());
    }
    for (RidgeGroup& group : _hill_groups) {
      print("Hill group of size ", group.meshes().size());
    }
    for (RidgeGroup& group : _plain_groups) {
      print("Plain group of size ", group.meshes().size());
    }
  }

  /**
   * @brief Helper method to get all groups at once
   *
   * Useful e.g. to fmap function in one loop for all meshes in all groups
   *
   * @return container with groups of all biomes
   */
  std::vector<std::reference_wrapper<RidgeGroup>> all_groups() {
    std::vector<std::reference_wrapper<RidgeGroup>> res;
    res.insert(res.end(), _mountain_groups.begin(), _mountain_groups.end());
    res.insert(res.end(), _water_groups.begin(), _water_groups.end());
    res.insert(res.end(), _plain_groups.begin(), _plain_groups.end());
    res.insert(res.end(), _hill_groups.begin(), _hill_groups.end());
    return res;
  }

 protected:
  std::vector<RidgeGroup> _mountain_groups;
  std::vector<RidgeGroup> _water_groups;
  std::vector<RidgeGroup> _plain_groups;
  std::vector<RidgeGroup> _hill_groups;
  RidgeConfig _ridge_config;
};

}  // namespace sota
