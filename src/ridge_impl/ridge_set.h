#pragma once

#include <utility>
#include <vector>

#include "ridge.h"
#include "ridge_config.h"
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

  // TODO make class/struct for connection and use in RidgeSetMaker
  using Connection = std::pair<std::pair<Vector3, Vector3>, std::pair<Vector3, Vector3>>;
};

}  // namespace sota
