#pragma once

#include <cstddef>
#include <functional>  // for function
#include <map>         // for map
#include <memory>      // for unique_ptr
#include <optional>    // for optional
#include <utility>     // for move, pair
#include <vector>      // for vector

#include "ridge.h"
#include "ridge_config.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"  // for RidgeSet
#include "types.h"

namespace sota {
class RidgeMesh;

using GroupOfRidgeMeshes = std::vector<RidgeMesh*>;
using BiomeGroups = std::vector<GroupOfRidgeMeshes>;

class RidgeGroup {
 public:
  RidgeGroup(GroupOfRidgeMeshes meshes, std::unique_ptr<RidgeSet> ridge_set, Biome biome)
      : _meshes(meshes), _ridge_set(std::move(ridge_set)), _biome(biome) {}
  explicit RidgeGroup(GroupOfRidgeMeshes meshes, Biome biome) : _meshes(meshes), _ridge_set({}), _biome(biome) {}

  RidgeGroup() = default;
  RidgeGroup(const RidgeGroup& other) = delete;
  RidgeGroup(RidgeGroup&& other) = default;
  RidgeGroup& operator=(const RidgeGroup& other) = delete;
  RidgeGroup& operator=(RidgeGroup&& other) = default;

  // getters
  const GroupOfRidgeMeshes& meshes();

  void fmap(std::function<void(const GroupOfRidgeMeshes&)> func);
  void fmap_mutable(std::function<void(GroupOfRidgeMeshes&)> func);
  void init_ridges(DiscreteVertexToDistance& distance_map, float offset, int divisions);
  bool has(RidgeMesh* mesh) { return std::find(_meshes.begin(), _meshes.end(), mesh) != _meshes.end(); }
  bool has_ridge_set() const { return _ridge_set.has_value(); };
  size_t size() const { return _meshes.size(); }
  Biome biome() { return _biome; }

  void add(RidgeMesh* mesh) { _meshes.push_back(mesh); }

 private:
  friend std::optional<RidgeGroup> combine(std::vector<RidgeGroup*> groups, RidgeConfig config);
  friend std::vector<RidgeGroup> remove_mesh(RidgeGroup& ridge_group, RidgeMesh* mesh, RidgeConfig config);

  GroupOfRidgeMeshes _meshes;
  Biome _biome;
  std::optional<std::unique_ptr<RidgeSet>> _ridge_set{};

  void assign_ridges();
  void calculate_corner_points_distances_to_border(DiscreteVertexToDistance& distance_map, int divisions);
};

bool check_biomes(std::vector<RidgeGroup>& groups);

// Return new RidgeGroup based on multiple RidgeGroup's.
std::optional<RidgeGroup> combine(std::vector<RidgeGroup*> groups, RidgeConfig config);

// If vector of RidgeGroup's is given remove specific RidgeGroups from it if that pointer to it is present in
// 'groups_to_be_removed' vector
void subtract(std::vector<RidgeGroup>& base, std::vector<RidgeGroup*> removed);

// If RidgeGroup is given return vector of RidgeGroups which are formed by removal of specific RidgeMesh from original
// RidgeGroup. Result must contain 1, 2 or 3 RidgeGroups - impossible to form more groups by removal of only element
std::vector<RidgeGroup> remove_mesh(RidgeGroup& ridge_group, RidgeMesh* mesh, RidgeConfig config);

}  // namespace sota
