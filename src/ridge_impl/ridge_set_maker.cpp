#include "ridge_set_maker.h"

#include <algorithm>
#include <random>

#include "misc/types.h"
#include "ridge_impl/ridge_hex_mesh.h"

namespace sota {

// TODO add globals
const int seed = 0;
const int variable_int = 35;

unsigned int RidgeSetMaker::unvisited_neighbours_count(const RidgeHexMesh* mesh) const {
  return unvisited_neighbours(mesh).size();
}

HexagonMeshPointerVector RidgeSetMaker::unvisited_neighbours(const RidgeHexMesh* mesh) const {
  auto all_neighbours = mesh->get_neighbours();
  HexagonMeshPointerVector unvisited;
  for (HexMesh* m : all_neighbours) {
    RidgeHexMesh* ridge_mesh = dynamic_cast<RidgeHexMesh*>(m);
    if (!_visited.contains(ridge_mesh) && std::find(_hexes.begin(), _hexes.end(), ridge_mesh) != _hexes.end()) {
      unvisited.push_back(ridge_mesh);
    }
  }
  return unvisited;
}

std::vector<std::pair<Vector3, Vector3>> RidgeSetMaker::construct(float y_coord) {
  std::mt19937 random_generator(seed);
  std::uniform_int_distribution<> int_dist(0, 1000);
  auto compare_increasing = [this](const RidgeHexMesh* lhs, const RidgeHexMesh* rhs) {
    return unvisited_neighbours_count(lhs) < unvisited_neighbours_count(rhs);
  };
  std::sort(_hexes.begin(), _hexes.end(), compare_increasing);

  int iter = variable_int;
  std::vector<std::pair<Vector3, Vector3>> res;
  int l = 0;
  int r = _hexes.size() - 1;
  while (iter && l < r) {
    RidgeHexMesh* least_n_mesh = _hexes[l];
    RidgeHexMesh* most_n_mesh = _hexes[r];

    RidgeHexMesh* cur = least_n_mesh;
    while (cur != most_n_mesh) {
      HexagonMeshPointerVector neighbours = unvisited_neighbours(cur);
      if (neighbours.empty()) {
        break;
      }
      unsigned int next_idx = int_dist(random_generator) % neighbours.size();

      Vector3 cur_position = cur->get_center();
      cur_position.y = y_coord;
      Vector3 next_position = neighbours[next_idx]->get_center();
      next_position.y = y_coord;

      res.emplace_back(cur_position, next_position);
      _visited.insert(cur);
      cur = neighbours[next_idx];
    }

    --iter;
    ++l;
    --r;
  }

  return res;
}

}  // namespace sota
