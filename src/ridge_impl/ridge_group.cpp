#include "ridge_impl/ridge_group.h"

namespace sota {

void RidgeGroup::create(std::map<std::pair<int, int>, float>& distance_keeper, float offset, int divisions) {
  if (_meshes.size() > 1) {
    _ridge_set->create_dfs_random(_meshes, offset, divisions);
  } else {
    _ridge_set->create_single(_meshes[0], offset);
  }

  assign_ridges();
  calculate_corner_points_distances_to_border(distance_keeper, divisions);
}

const GroupOfRidgeMeshes& RidgeGroup::meshes() { return _meshes; }

void RidgeGroup::fmap(std::function<void(const GroupOfRidgeMeshes&)> func) { func(_meshes); }

void RidgeGroup::assign_ridges() {
  auto* ridges = _ridge_set->ridges();
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
