#pragma once

#include <unordered_set>
#include <vector>

#include "ridge.h"
#include "ridge_mesh.h"
#include "tal/vector3.h"

namespace sota {

class RidgeMesh;
using RidgeMeshPointerVector = std::vector<RidgeMesh*>;
using RidgeVector = std::vector<Ridge>;

class RidgeSetMaker {
 public:
  RidgeSetMaker(RidgeMeshPointerVector meshes) : _meshes(meshes) {}
  std::vector<std::pair<std::pair<Vector3, Vector3>, std::pair<Vector3, Vector3>>> construct(float offset);

 private:
  RidgeMeshPointerVector _meshes;
  std::unordered_set<RidgeMesh*> _visited;

  unsigned int unvisited_neighbours_count(const RidgeMesh* mesh) const;
  RidgeMeshPointerVector unvisited_neighbours(const RidgeMesh* mesh) const;
};

}  // namespace sota
