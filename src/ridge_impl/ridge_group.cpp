#include "ridge_impl/ridge_group.h"

#include <algorithm>  // for transform
#include <cassert>
#include <cstddef>
#include <iterator>  // for back_insert_iterator, back_inserter
#include <memory>
#include <numeric>
#include <unordered_set>

#include "godot_core.h"
#include "misc/tile.h"
#include "ridge_config.h"
#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh
#include "ridge_impl/ridge_set.h"   // for RidgeSet
#include "tile_mesh.h"
#include "types.h"
#include "vector3i.h"

namespace sota {
class Ridge;

void RidgeGroup::init_ridges(DiscreteVertexToDistance& distance_map, float offset, int divisions) {
  if (!_ridge_set) {
    return;
  }
  if (_meshes.size() > 1) {
    _ridge_set.value()->create_dfs_random(_meshes, offset, divisions);
  } else {
    _ridge_set.value()->create_single(_meshes[0], offset);
  }

  assign_ridges();
  calculate_corner_points_distances_to_border(distance_map, divisions);
}

const GroupOfRidgeMeshes& RidgeGroup::meshes() { return _meshes; }

void RidgeGroup::fmap(std::function<void(const GroupOfRidgeMeshes&)> func) { func(_meshes); }
void RidgeGroup::fmap_mutable(std::function<void(GroupOfRidgeMeshes&)> func) { func(_meshes); }

void RidgeGroup::assign_ridges() {
  auto* ridges = _ridge_set.value()->ridges();
  std::vector<Ridge*> ridge_pointers;
  std::transform(ridges->begin(), ridges->end(), std::back_inserter(ridge_pointers),
                 [](Ridge& ridge) { return &ridge; });

  for (auto* mesh : _meshes) {
    mesh->set_ridges(ridge_pointers);
  }
}

void RidgeGroup::calculate_corner_points_distances_to_border(DiscreteVertexToDistance& distance_map, int divisions) {
  for (auto* m : _meshes) {
    m->calculate_corner_points_distances_to_border(distance_map, divisions);
  }
}

bool check_biomes(std::vector<RidgeGroup*>& groups) {
  if (groups.empty()) {
    return true;
  }
  Biome biome_of_first_group = groups[0]->biome();
  return std::all_of(groups.begin(), groups.end(),
                     [biome_of_first_group](RidgeGroup* group) { return group->biome() == biome_of_first_group; });
}

std::optional<RidgeGroup> combine(std::vector<RidgeGroup*> groups, RidgeConfig config) {
  if (groups.empty()) {
    return {};
  }
  if (!check_biomes(groups)) {
    return {};
  }
  size_t new_size = std::accumulate(groups.begin(), groups.end(), 0,
                                    [](size_t acc, const RidgeGroup* group) { return group->size(); });

  GroupOfRidgeMeshes res;
  res.reserve(new_size);
  for (auto* group : groups) {
    res.insert(res.cend(), group->_meshes.begin(), group->_meshes.end());
  }
  return (*groups.begin())->_ridge_set.has_value()
             ? RidgeGroup(res, std::make_unique<RidgeSet>(config), groups[0]->biome())
             : RidgeGroup(res, groups[0]->biome());
}

void subtract(std::vector<RidgeGroup>& base, std::vector<RidgeGroup*> removed) {
  for (RidgeGroup* group_to_be_removed : removed) {
    auto pred = [group_to_be_removed](const RidgeGroup& group) { return std::addressof(group) == group_to_be_removed; };

    size_t erased_num = std::erase_if(base, pred);
    if (erased_num != 1) {
      printerr("Can't found group to subtract");
    }
  }
}

static void dfs(RidgeMesh* current, GroupOfRidgeMeshes& ridge_meshed_to_add, std::unordered_set<RidgeMesh*>& visited) {
  if (visited.contains(current)) {
    return;
  }
  visited.insert(current);
  ridge_meshed_to_add.push_back(current);
  for (TileMesh* tile_mesh : current->tile()->neighbours()) {
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(tile_mesh);
    dfs(ridge_mesh, ridge_meshed_to_add, visited);
  }
}

std::vector<RidgeGroup> remove_mesh(RidgeGroup& ridge_group, RidgeMesh* mesh, RidgeConfig config) {
  Biome biome = ridge_group.biome();
  for (TileMesh* tile_mesh : mesh->tile()->neighbours()) {
    RidgeMesh* neighbour = dynamic_cast<RidgeMesh*>(tile_mesh);
    neighbour->tile()->remove_neighbour(mesh);
  }
  std::vector<RidgeGroup> res;
  std::unordered_set<RidgeMesh*> visited;
  for (RidgeMesh* ridge_mesh : ridge_group._meshes) {
    GroupOfRidgeMeshes ridge_meshed_to_add;
    if (ridge_mesh == mesh || visited.contains(ridge_mesh)) {
      continue;
    }
    dfs(ridge_mesh, ridge_meshed_to_add, visited);
    RidgeGroup new_group = ridge_group._ridge_set.has_value()
                               ? RidgeGroup(ridge_meshed_to_add, std::make_unique<RidgeSet>(config), biome)
                               : RidgeGroup(ridge_meshed_to_add, biome);
    res.emplace_back(std::move(new_group));
  }

  assert(1 <= res.size() && res.size() <= 3);

  return res;
}

}  // namespace sota
