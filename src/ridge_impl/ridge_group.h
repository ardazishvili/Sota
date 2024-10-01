#pragma once

#include <functional>  // for function
#include <map>         // for map
#include <memory>      // for unique_ptr
#include <optional>    // for optional
#include <utility>     // for move, pair
#include <vector>      // for vector

#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"  // for RidgeSet

namespace sota {
class RidgeMesh;

using GroupOfRidgeMeshes = std::vector<RidgeMesh*>;
using BiomeGroups = std::vector<GroupOfRidgeMeshes>;

class RidgeGroup {
 public:
  RidgeGroup(GroupOfRidgeMeshes meshes, std::unique_ptr<RidgeSet> ridge_set)
      : _meshes(meshes), _ridge_set(std::move(ridge_set)) {}
  RidgeGroup(GroupOfRidgeMeshes meshes) : _meshes(meshes), _ridge_set({}) {}

  RidgeGroup() = default;
  RidgeGroup(const RidgeGroup& other) = delete;
  RidgeGroup(RidgeGroup&& other) = default;
  RidgeGroup& operator=(const RidgeGroup& other) = delete;
  RidgeGroup& operator=(RidgeGroup&& other) = default;

  // getters
  const GroupOfRidgeMeshes& meshes();

  void fmap(std::function<void(const GroupOfRidgeMeshes&)> func);
  void init_ridges(DiscreteVertexToDistance& distance_map, float offset, int divisions);

 private:
  GroupOfRidgeMeshes _meshes;
  std::optional<std::unique_ptr<RidgeSet>> _ridge_set{};

  void assign_ridges();
  void calculate_corner_points_distances_to_border(DiscreteVertexToDistance& distance_map, int divisions);
};

}  // namespace sota
