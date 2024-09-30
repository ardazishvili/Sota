#include "ridge_set.h"

#include <array>
#include <random>

#include "mesh.h"
#include "ridge_impl/ridge_config.h"
#include "ridge_impl/ridge_connection.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set_maker.h"
#include "tal/godot_core.h"
#include "tal/vector2.h"
#include "tal/vector3.h"
#include "utilities.h"

namespace sota {

RidgeSet::RidgeSet(RidgeConfig config) : _config(config) {}

void RidgeSet::create_single(RidgeMesh* mesh, float offset) {
  SotaMesh* m = mesh->inner_mesh();

  Vector3 normal = m->base().center().normalized();
  Vector3 tangent = (m->base().points()[0] - m->base().center()).normalized();
  Vector3 c = mesh->get_center() + normal * offset;
  Vector3 start = c - tangent * 0.02;
  Vector3 end = c + tangent * 0.02;

  std::vector<std::pair<Vector3, Vector3>> bounds = {{start, end}};
  unsigned int pieces_num = 2;
  for (auto& [s, e] : bounds) {
    _ridges.emplace_back(s, e);
    Vector3 dist = e - s;
    std::vector<Vector3> points;

    for (unsigned int i = 0; i <= pieces_num; ++i) {
      points.push_back(s + static_cast<float>(i) * dist / pieces_num);
    }
    _ridges.back().set_points(points);
  }
}

void RidgeSet::create_dfs_random(std::vector<RidgeMesh*>& list, float offset, int divisions) {
  constexpr int seed = 0;
  std::mt19937 random_generator(seed);
  std::uniform_int_distribution<> int_dist(0, 1000);
  RidgeSetMaker maker(list);
  std::vector<RidgeConnection> connections = maker.construct(offset);

  unsigned int fracture_num = int_dist(random_generator) % 4;
  unsigned int connection_pieces_num = fracture_num + 1;

  auto tangents = [](const Vector3& lhs, const Vector3& rhs, const Vector3& lhs_normal) -> std::array<Vector3, 2> {
    Vector3 first = (rhs - lhs).cross(lhs_normal).normalized();
    Vector3 second = -first;
    return {first, second};
  };

  std::uniform_real_distribution<float> height_dist(_config.variation_min_bound, _config.variation_max_bound);
  std::vector<std::pair<Vector3, Vector3>> bounds;
  std::vector<Vector3> displacement_xyz(connection_pieces_num);
  unsigned int connection_num = connections.size();
  for (unsigned int k = 0; k < connection_num; ++k) {
    auto [lhs_vertex, rhs_vertex] = connections[k].get();
    Vector3 lhs_coord = lhs_vertex.coord;
    Vector3 rhs_coord = rhs_vertex.coord;
    auto ts = tangents(lhs_coord, rhs_coord, lhs_vertex.normal);
    Vector3 distance = rhs_coord - lhs_coord;
    for (unsigned int i = 0; i < connection_pieces_num; ++i) {
      Vector3 a = lhs_coord + static_cast<float>(i) * distance / connection_pieces_num;
      Vector3 b = lhs_coord + static_cast<float>(i + 1) * distance / connection_pieces_num;
      bounds.emplace_back(a, b);
      float randomness = height_dist(random_generator);
      Vector3 random_vector = randomness * ((i & 1) ? ts[1] : ts[0]);
      displacement_xyz[i] = random_vector;
    }
    for (unsigned int i = 1 + connection_pieces_num * k; i < connection_pieces_num * (k + 1); ++i) {
      bounds[i].first += displacement_xyz[(i - 1) % connection_pieces_num];
      bounds[i - 1].second += displacement_xyz[(i - 1) % connection_pieces_num];
    }
  }

  unsigned int pieces_num = divisions * 2;
  for (auto& [start, end] : bounds) {
    _ridges.emplace_back(start, end);
    Vector3 distance = end - start;
    std::vector<Vector3> p;

    for (unsigned int i = 0; i <= pieces_num; ++i) {
      p.push_back(start + static_cast<float>(i) * distance / pieces_num);
    }
    _ridges.back().set_points(p);
  }
}

}  // namespace sota
