#pragma once

#include <utility>
#include <vector>  // for vector

#include "ridge_impl/ridge.h"         // for Ridge
#include "ridge_impl/ridge_config.h"  // for RidgeConfig
#include "tal/vector3.h"

namespace sota {

// TODO remove dependency on RidgeHexMesh
class RidgeMesh;

class RidgeSet {
 public:
  RidgeSet(RidgeConfig config);
  RidgeSet(const RidgeSet& other) = default;
  RidgeSet(RidgeSet&& other) = default;
  RidgeSet& operator=(const RidgeSet& other) = default;
  RidgeSet& operator=(RidgeSet&& other) = default;

  void create_dfs_random(std::vector<RidgeMesh*>& list, float offset, int divisions);
  void create_single(RidgeMesh* mesh, float offset);
  std::vector<Ridge>* ridges() { return &_ridges; }

 private:
  std::vector<Ridge> _ridges;
  RidgeConfig _config;
};

}  // namespace sota
