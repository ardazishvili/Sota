#include "ridge_set_maker.h"

#include <algorithm>
#include <random>
#include <utility>

#include "mesh.h"
#include "misc/types.h"
#include "ridge_impl/ridge_mesh.h"
#include "tal/godot_core.h"
#include "vector3.h"

namespace sota {

// TODO add globals
const int seed = 0;
const int variable_int = 35;

unsigned int RidgeSetMaker::unvisited_neighbours_count(const RidgeMesh* mesh) const {
  return unvisited_neighbours(mesh).size();
}

RidgeMeshPointerVector RidgeSetMaker::unvisited_neighbours(const RidgeMesh* mesh) const {
  auto all_neighbours = mesh->get_neighbours();
  RidgeMeshPointerVector unvisited;
  for (TileMesh* tile_mesh : all_neighbours) {
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(tile_mesh);
    if (!_visited.contains(ridge_mesh) && std::find(_meshes.begin(), _meshes.end(), ridge_mesh) != _meshes.end()) {
      unvisited.push_back(ridge_mesh);
    }
  }
  return unvisited;
}

std::vector<std::pair<std::pair<Vector3, Vector3>, std::pair<Vector3, Vector3>>> RidgeSetMaker::construct(
    float offset) {
  std::mt19937 random_generator(seed);
  std::uniform_int_distribution<> int_dist(0, 1000);
  auto compare_increasing = [this](const RidgeMesh* lhs, const RidgeMesh* rhs) {
    return unvisited_neighbours_count(lhs) < unvisited_neighbours_count(rhs);
  };
  std::sort(_meshes.begin(), _meshes.end(), compare_increasing);

  int iter = variable_int;
  std::vector<std::pair<std::pair<Vector3, Vector3>, std::pair<Vector3, Vector3>>> res;
  int l = 0;
  int r = _meshes.size() - 1;
  while (iter && l < r) {
    RidgeMesh* least_n_mesh = _meshes[l];
    RidgeMesh* most_n_mesh = _meshes[r];

    RidgeMesh* cur = least_n_mesh;
    while (cur != most_n_mesh) {
      RidgeMeshPointerVector neighbours = unvisited_neighbours(cur);
      if (neighbours.empty()) {
        break;
      }
      unsigned int next_idx = int_dist(random_generator) % neighbours.size();
      RidgeMesh* next = neighbours[next_idx];

      Vector3 cur_center = cur->get_center();
      Vector3 cur_position = cur_center + cur->inner_mesh()->get_base_normal_direction(cur_center) * offset;
      Vector3 next_center = next->get_center();
      Vector3 next_position = next_center + next->inner_mesh()->get_base_normal_direction(next_center) * offset;

      res.emplace_back(std::make_pair(cur_position, cur->inner_mesh()->get_base_normal_direction(cur_position)),
                       std::make_pair(next_position, next->inner_mesh()->get_base_normal_direction(next_position)));
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
