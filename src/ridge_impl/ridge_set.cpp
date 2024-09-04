#include "ridge_set.h"

#include <array>
#include <random>

#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "ridge_impl/ridge_config.h"
#include "ridge_impl/ridge_hex_mesh.h"
#include "ridge_impl/ridge_set_maker.h"

namespace sota {

using namespace gd;

RidgeSet::RidgeSet(RidgeConfig config) : _config(config) {}

void RidgeSet::create_single(RidgeHexMesh* mesh, float y_coord) {
  Vector3 center = mesh->get_center();
  Vector3 start = center;
  start.x -= 0.02;
  start.y = y_coord;
  Vector3 end = center;
  end.x += 0.02;
  end.y = y_coord;

  std::vector<std::pair<Vector3, Vector3>> bounds = {{start, end}};
  unsigned int pieces_num = 2;
  for (auto& [start, end] : bounds) {
    _ridges.emplace_back(start, end);
    float dist_x = end.x - start.x;
    float dist_y = end.y - start.y;
    float dist_z = end.z - start.z;
    std::vector<Vector3> p;

    for (unsigned int i = 0; i <= pieces_num; ++i) {
      p.push_back(
          {start.x + i * dist_x / pieces_num, start.y + i * dist_y / pieces_num, start.z + i * dist_z / pieces_num});
    }
    _ridges.back().set_points(p);
  }
}

void RidgeSet::create_dfs_random(std::vector<RidgeHexMesh*>& list, float y_coord, int divisions) {
  constexpr int seed = 0;
  std::mt19937 random_generator(seed);
  std::uniform_int_distribution<> int_dist(0, 1000);
  RidgeSetMaker maker(list);
  std::vector<Connection> connections = maker.construct(y_coord);

  unsigned int pieces_num = divisions * 2;
  unsigned int fracture_num = int_dist(random_generator) % 4;
  unsigned int connection_pieces_num = fracture_num + 1;

  auto normals = [](const Vector3& lhs, const Vector3& rhs) -> std::array<Vector2, 2> {
    float dx = rhs.x - lhs.x;
    float dz = rhs.z - lhs.z;

    return {Vector2(-dz, dx).normalized(), Vector2(dz, -dx).normalized()};
  };

  std::uniform_real_distribution<float> height_dist(_config.variation_min_bound, _config.variation_max_bound);
  std::vector<std::pair<Vector3, Vector3>> bounds;
  std::vector<Vector3> displacement_xyz(connection_pieces_num);
  unsigned int connection_num = connections.size();
  for (unsigned int k = 0; k < connection_num; ++k) {
    auto& [lhs, rhs] = connections[k];
    auto ns = normals(lhs, rhs);
    float dist_x = rhs.x - lhs.x;
    float dist_y = rhs.y - lhs.y;
    float dist_z = rhs.z - lhs.z;
    for (unsigned int i = 0; i < connection_pieces_num; ++i) {
      Vector3 a{lhs.x + i * dist_x / connection_pieces_num, lhs.y + i * dist_y / connection_pieces_num,
                lhs.z + i * dist_z / connection_pieces_num};
      Vector3 b{lhs.x + (i + 1) * dist_x / connection_pieces_num, lhs.y + (i + 1) * dist_y / connection_pieces_num,
                lhs.z + (i + 1) * dist_z / connection_pieces_num};
      bounds.emplace_back(a, b);
      float randomness = height_dist(random_generator);
      Vector2 random_normals = randomness * ((i & 1) ? ns[1] : ns[0]);
      displacement_xyz[i] = Vector3(random_normals.x, -randomness, random_normals.y);
    }
    for (unsigned int i = 1 + connection_pieces_num * k; i < connection_pieces_num * (k + 1); ++i) {
      bounds[i].first += displacement_xyz[(i - 1) % connection_pieces_num];
      bounds[i - 1].second += displacement_xyz[(i - 1) % connection_pieces_num];
    }
  }

  for (auto& [start, end] : bounds) {
    _ridges.emplace_back(start, end);
    float dist_x = end.x - start.x;
    float dist_y = end.y - start.y;
    float dist_z = end.z - start.z;
    std::vector<Vector3> p;

    for (unsigned int i = 0; i <= pieces_num; ++i) {
      p.push_back(
          {start.x + i * dist_x / pieces_num, start.y + i * dist_y / pieces_num, start.z + i * dist_z / pieces_num});
    }
    _ridges.back().set_points(p);
  }
}

}  // namespace sota
