#include "core/pent_mesh.h"

#include <cmath>
#include <memory>

#include "algo/constants.h"
#include "mesh.h"
#include "primitives/edge.h"
#include "primitives/face.h"
#include "primitives/pentagon.h"
#include "primitives/triangle.h"
#include "tal/reference.h"
#include "tal/vector2.h"
#include "tal/vector3.h"

namespace sota {

PentMesh::PentMesh() : SotaMesh(std::make_unique<Pentagon>(make_unit_pentagon())) { init(); }

PentMesh::PentMesh(Pentagon pentagon, PentagonMeshParams params) : SotaMesh(std::make_unique<Pentagon>(pentagon)) {
  _id = params.id;
  _divisions = params.divisions;
  set_material(params.material);
  _tesselation_mode = params.tesselation_mode;
  _orientation = params.orientation;
}

void PentMesh::init_impl() {
  calculate_vertices_recursion();
  recalculate_all_except_vertices();
}

void PentMesh::calculate_vertices_recursion() {
  vertices_.clear();
  auto corner_points = _base_ngon->points();
  auto center = _base_ngon->center();

  for (int i = 0; i < 5; ++i) {
    // for simplicity use "convex" center(e.g. in case of polyhedron)
    tesselate_into_triangles(corner_points[i], corner_points[(i + 1) % 5], center, _divisions);
  }
  return;
}

void PentMesh::calculate_tex_uv1() {
  tex_uv1_.clear();
  auto corner_points = _base_ngon->points();

  float side = (corner_points[0] - corner_points[1]).length();

  Vector3 apothem_start_point = (corner_points[0] + corner_points[1]) / 2;
  Vector3 apothem_direction = (corner_points[3] - apothem_start_point).normalized();
  float ri = side / (2 * std::tan(PI / 5));  // inradius
  float rc = side / (2 * std::sin(PI / 5));  // circumradius
  Vector3 c = apothem_start_point + ri * apothem_direction;
  Vector3 cross_direction = apothem_direction.cross(c.normalized());

  float width = (corner_points[0] - corner_points[2]).length();
  float height = ri + rc;

  Vector3 p0 = c - apothem_direction * ri - cross_direction * width / 2;
  Vector3 p1 = c + apothem_direction * rc - cross_direction * width / 2;
  Vector3 p2 = c - apothem_direction * ri + cross_direction * width / 2;

  Vector3 height_line = (p1 - p0).normalized();
  Vector3 width_line = (p2 - p0).normalized();

  auto tex_coord_mapper = [height_line, width_line, width, height, p0](Vector3 v) -> Vector2 {
    Vector3 v_rel_p0 = v - p0;
    return {height_line.dot(v_rel_p0) / height, width_line.dot(v_rel_p0) / width};
  };
  for (const auto& v : vertices_) {
    tex_uv1_.push_back(tex_coord_mapper(v));
  }
}

void PentMesh::update() { request_update(); }

}  // namespace sota
