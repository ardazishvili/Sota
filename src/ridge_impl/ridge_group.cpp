#include "ridge_impl/ridge_group.h"

#include <algorithm>  // for transform
#include <iterator>   // for back_insert_iterator, back_inserter

#include "ridge_impl/ridge_mesh.h"  // for RidgeMesh
#include "ridge_impl/ridge_set.h"   // for RidgeSet

namespace sota {
class Ridge;

void RidgeGroup::init_ridges(std::map<std::pair<int, int>, float>& distance_keeper, float offset, int divisions) {
  if (!_ridge_set) {
    return;
  }
  if (_meshes.size() > 1) {
    _ridge_set.value()->create_dfs_random(_meshes, offset, divisions);
  } else {
    _ridge_set.value()->create_single(_meshes[0], offset);
  }

  assign_ridges();
  calculate_corner_points_distances_to_border(distance_keeper, divisions);
}

const GroupOfRidgeMeshes& RidgeGroup::meshes() { return _meshes; }

void RidgeGroup::fmap(std::function<void(const GroupOfRidgeMeshes&)> func) { func(_meshes); }

void RidgeGroup::assign_ridges() {
  auto* ridges = _ridge_set.value()->ridges();
  std::vector<Ridge*> ridge_pointers;
  std::transform(ridges->begin(), ridges->end(), std::back_inserter(ridge_pointers),
                 [](Ridge& ridge) { return &ridge; });

  for (auto* mesh : _meshes) {
    mesh->set_ridges(ridge_pointers);
  }
}

void RidgeGroup::calculate_corner_points_distances_to_border(std::map<std::pair<int, int>, float>& distance_keeper,
                                                             int divisions) {
  for (auto* m : _meshes) {
    m->calculate_corner_points_distances_to_border(distance_keeper, divisions);
  }
}
}  // namespace sota
