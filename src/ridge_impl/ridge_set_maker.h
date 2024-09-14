#pragma once

#include <unordered_set>
#include <vector>

#include "tal/vector3.h"
#include "ridge.h"
#include "ridge_hex_mesh.h"

namespace sota {

class RidgeHexMesh;
using HexagonMeshPointerVector = std::vector<RidgeHexMesh*>;
using RidgeVector = std::vector<Ridge>;

class RidgeSetMaker {
 public:
  RidgeSetMaker(HexagonMeshPointerVector hexes) : _hexes(hexes) {}
  std::vector<std::pair<Vector3, Vector3>> construct(float y_coord);

 private:
  HexagonMeshPointerVector _hexes;
  std::unordered_set<RidgeHexMesh*> _visited;

  unsigned int unvisited_neighbours_count(const RidgeHexMesh* mesh) const;
  HexagonMeshPointerVector unvisited_neighbours(const RidgeHexMesh* mesh) const;
};

}  // namespace sota
