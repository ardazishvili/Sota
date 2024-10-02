#include "ridge_impl/ridge_mesh.h"

#include <algorithm>  // for min, any_of
#include <iterator>   // for back_insert_it...
#include <set>        // for set

#include "core/general_utility.h"  // for MeshProcessor
#include "core/mesh.h"             // for SotaMesh
#include "misc/discretizer.h"      // for Dicretizer
#include "misc/types.h"            // for Neighbours
#include "misc/utilities.h"        // for to_point_divis...
#include "primitives/polygon.h"    // for RegularPolygon
#include "tal/arrays.h"            // for Vector3Array
#include "tal/callable.h"          // for Callable
#include "tal/godot_core.h"        // for print
#include "tal/noise.h"             // for FastNoiseLite
#include "tal/reference.h"         // for Ref
#include "tal/vector2.h"           // for Vector2
#include "tal/vector3.h"           // for Vector3
#include "tile_mesh.h"             // for TileMesh

namespace sota {
class Ridge;

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

void RidgeMesh::calculate_corner_points_distances_to_border(DiscreteVertexToDistance& distance_map, int divisions) {
  float diameter = _mesh->get_R() * 2;
  // TODO duplicated
  auto divisioned = [diameter, divisions](Vector3 point) {
    return VertexToNormalDiscretizer::get_discrete_vertex(point, diameter / (divisions * 2));
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

  PointToLineDistance_VectorMultBased calculator(get_exclude_border_set(), corner_points);
  float distance_to_border;
  for (auto& v : corner_points) {
    distance_to_border = calculator.calc(v);
    for (const auto& point : neighbours_corner_points) {
      // TODO length -> distance_to
      distance_to_border = std::min(distance_to_border, (Vector2(v.x, v.z) - Vector2(point.x, point.z)).length() +
                                                            distance_map[divisioned(point)]);
    }
    if (auto it = distance_map.find(divisioned(v)); it != distance_map.end()) {
      distance_to_border = std::min(distance_to_border, it->second);
    }
    distance_map[divisioned(v)] = distance_to_border;
  }
}

void RidgeMesh::shift_compress() {
  auto center = _mesh->base().center();
  if (!_processor) {
    print("processor of RidgeMesh object is nullptr");
  }
  _mesh->set_vertices(_processor->shift_compress(_mesh->get_vertices(), _y_shift, _y_compress, center.y));
}

std::set<int> RidgeMesh::get_exclude_border_set() const {
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
                                              float ridge_offset, DiscreteVertexToDistance& distance_map,
                                              int divisions) {
  shift_compress();

  std::vector<Vector3> neighbours_corner_points;
  for (TileMesh* n : _neighbours) {
    if (n) {
      std::vector<Vector3> points = dynamic_cast<RidgeMesh*>(n)->inner_mesh()->base().points();
      neighbours_corner_points.insert(neighbours_corner_points.end(), points.begin(), points.end());
    }
  }

  auto vertices = _mesh->get_vertices();

  float diameter = _mesh->get_R() * 2;
  vertices = _processor->calculate_ridge_based_heights(
      vertices, _mesh->base(), _ridges, neighbours_corner_points, _mesh->get_R(), get_exclude_border_set(), diameter,
      divisions, distance_map, _ridge_noise, ridge_offset, interpolation_func, _min_height, _max_height);

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
