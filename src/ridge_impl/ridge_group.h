#pragma once

#include <memory>
#include <optional>

#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"

namespace sota {

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
  void init_ridges(std::map<std::pair<int, int>, float>& distance_keeper, float offset, int divisions);

 private:
  GroupOfRidgeMeshes _meshes;
  std::optional<std::unique_ptr<RidgeSet>> _ridge_set{};

  void assign_ridges();
  void calculate_corner_points_distances_to_border(std::map<std::pair<int, int>, float>& distance_keeper,
                                                   int divisions);
};

}  // namespace sota
