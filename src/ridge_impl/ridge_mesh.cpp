#include "ridge_impl/ridge_mesh.h"

#include <algorithm>
#include <limits>
#include <set>
#include <unordered_set>

#include "core/general_utility.h"
#include "core/utils.h"
#include "mesh.h"
#include "misc/utilities.h"
#include "tal/callable.h"
#include "tal/godot_core.h"
#include "tal/vector2.h"
#include "tal/vector3.h"
#include "tile_mesh.h"
#include "types.h"

namespace sota {

void RidgeMesh::_bind_methods() {}

void RidgeMesh::set_plain_noise(Ref<FastNoiseLite> p_noise) {
  _plain_noise = p_noise;
  if (_plain_noise.ptr()) {
    _plain_noise->connect("changed", Callable(_mesh.ptr(), "request_update"));
    _mesh->update();
  }
}

void RidgeMesh::set_ridge_noise(Ref<FastNoiseLite> p_ridge_noise) {
  _ridge_noise = p_ridge_noise;
  if (_ridge_noise.ptr()) {
    _ridge_noise->connect("changed", Callable(_mesh.ptr(), "request_update"));
    _mesh->update();
  }
}

void RidgeMesh::calculate_corner_points_distances_to_border(std::map<std::pair<int, int>, float>& distance_keeper,
                                                            int divisions) {
  float diameter = _mesh->get_R() * 2;
  // TODO duplicated
  auto divisioned = [diameter, divisions](Vector3 point) {
    return to_point_divisioned_position(point, diameter, divisions);
  };

  std::vector<Vector3> neighbours_corner_points;
  for (const auto& n : _neighbours) {
    if (n) {
      RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(n);
      auto points = ridge_mesh->inner_mesh()->base().points();
      neighbours_corner_points.insert(neighbours_corner_points.end(), points.begin(), points.end());
    }
  }

  auto corner_points = _mesh->base().points();
  auto center = _mesh->base().center();

  std::set<int> exclude_list = get_exclude_list();
  PointToLineDistance_VectorMultBased calculator(exclude_list, corner_points);
  float distance_to_border;
  for (auto& v : corner_points) {
    distance_to_border = calculator.calc(v);
    for (const auto& point : neighbours_corner_points) {
      // TODO length -> distance_to
      distance_to_border = std::min(distance_to_border, (Vector2(v.x, v.z) - Vector2(point.x, point.z)).length() +
                                                            distance_keeper[divisioned(point)]);
    }
    if (auto it = distance_keeper.find(divisioned(v)); it != distance_keeper.end()) {
      distance_to_border = std::min(distance_to_border, it->second);
    }
    distance_keeper[divisioned(v)] = distance_to_border;
  }
}

void RidgeMesh::shift_compress() {
  auto center = _mesh->base().center();
  if (!_processor) {
    print("processor of RidgeMesh object is nullptr");
  }
  _mesh->set_vertices(_processor->shift_compress(_mesh->get_vertices(), _y_shift, _y_compress, center.y));
}

std::set<int> RidgeMesh::get_exclude_list() {
  std::set<int> result;
  std::vector<Vector3> corner_points = _mesh->base().points();
  int size = corner_points.size();
  float approx_R = inner_mesh()->get_R();
  for (int i = 0; i < size; ++i) {
    Vector3 p1 = corner_points[i];
    Vector3 p2 = corner_points[(i + 1) % size];
    Vector3 mid = (p1 + p2) / 2;
    if (std::any_of(_neighbours.begin(), _neighbours.end(), [approx_R, mid](TileMesh* tile_mesh) {
          if (!tile_mesh) {
            return false;
          }
          return mid.distance_to(tile_mesh->inner_mesh()->get_center()) < (approx_R * 1.1);
        })) {
      result.insert(i);
    }
  }
  return result;
}

void RidgeMesh::calculate_ridge_based_heights(std::function<double(double, double, double)> interpolation_func,
                                              float ridge_offset, std::map<std::pair<int, int>, float>& distance_keeper,
                                              int divisions) {
  float diameter = _mesh->get_R() * 2;
  shift_compress();

  std::vector<Vector3> neighbours_corner_points;
  for (TileMesh* n : _neighbours) {
    if (n) {
      std::vector<Vector3> points = dynamic_cast<RidgeMesh*>(n)->inner_mesh()->base().points();
      neighbours_corner_points.insert(neighbours_corner_points.end(), points.begin(), points.end());
    }
  }

  // TODO duplicated
  auto divisioned = [diameter, divisions](Vector3 point) {
    return to_point_divisioned_position(point, diameter, divisions);
  };

  std::set<int> exclude_list = get_exclude_list();
  auto vertices = _mesh->get_vertices();

  vertices = _processor->calculate_ridge_based_heights(
      vertices, _mesh->base(), _ridges, neighbours_corner_points, _mesh->get_R(), exclude_list, diameter, divisions,
      distance_keeper, _ridge_noise, ridge_offset, interpolation_func, _min_height, _max_height);
  _mesh->set_vertices(vertices);
}

void RidgeMesh::calculate_initial_heights() {
  auto normal = _mesh->base().normal();
  _initial_vertices = _mesh->get_vertices();

  // TODO Pipe inputs/outputs
  auto vertices = _mesh->get_vertices();
  _processor->calculate_initial_heights(vertices, _plain_noise, _min_height, _max_height, normal);
  _mesh->set_vertices(vertices);

  _mesh->update();
}

GroupedMeshVertices RidgeMesh::get_grouped_vertices() {
  GroupedMeshVertices vertex_groups;
  auto vertices = _mesh->get_vertices();
  auto& normals = _mesh->get_normals();
  int size = vertices.size();
  for (int i = 0; i < size; ++i) {
    Vector3 v = vertices[i];
    Vector3& n = normals[i];
    auto p = to_point_divisioned_position(v, _mesh->get_R() * 2, _mesh->get_divisions());
    vertex_groups[p].push_back(&n);
  }
  return vertex_groups;
}

std::vector<TileMesh*> RidgeMesh::get_neighbours() const {
  std::vector<TileMesh*> res;
  std::copy_if(_neighbours.begin(), _neighbours.end(), std::back_inserter(res),
               [](TileMesh* n) { return n != nullptr; });
  return res;
}

void RidgeMesh::set_shift_compress(float y_shift, float y_compress) {
  _y_shift = y_shift;
  _y_compress = y_compress;
}

}  // namespace sota
