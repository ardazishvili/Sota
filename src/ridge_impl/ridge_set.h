#pragma once

#include <vector>

#include "ridge.h"
#include "ridge_config.h"

namespace sota {

// TODO remove dependency on RidgeHexMesh
class RidgeHexMesh;
class RidgeSet {
 public:
  RidgeSet(RidgeConfig config);
  void create_dfs_random(std::vector<RidgeHexMesh*>& list, float y_coord, int divisions);
  void create_single(RidgeHexMesh* mesh, float y_coord);
  std::vector<Ridge>* ridges() { return &_ridges; }

 private:
  std::vector<Ridge> _ridges;
  RidgeConfig _config;

  using Connection = std::pair<gd::Vector3, gd::Vector3>;
};

}  // namespace sota
