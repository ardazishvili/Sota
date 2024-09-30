#pragma once

#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "ridge_impl/ridge.h"  // for Ridge
#include "ridge_impl/ridge_connection.h"
#include "ridge_impl/ridge_mesh.h"
#include "tal/vector3.h"

namespace sota {

class RidgeConnection;
class RidgeMesh;

using RidgeMeshPointerVector = std::vector<RidgeMesh*>;
using RidgeVector = std::vector<Ridge>;

class RidgeSetMaker {
 public:
  RidgeSetMaker(RidgeMeshPointerVector meshes) : _meshes(meshes) {}
  std::vector<RidgeConnection> construct(float offset);

 private:
  RidgeMeshPointerVector _meshes;
  std::unordered_set<RidgeMesh*> _visited;

  unsigned int unvisited_neighbours_count(const RidgeMesh* mesh) const;
  RidgeMeshPointerVector unvisited_neighbours(const RidgeMesh* mesh) const;
};

}  // namespace sota
