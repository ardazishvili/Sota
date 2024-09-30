#pragma once

#include <memory>   // for unique_ptr
#include <utility>  // for move, swap
#include <vector>   // for vector

#include "mesh.h"
#include "primitives/edge.h"      // for Edge
#include "primitives/face.h"      // for Face
#include "primitives/polygon.h"   // for RegularPolygon
#include "primitives/triangle.h"  // for Triangle
#include "tal/arrays.h"           // for Vector3Array
#include "tal/mesh.h"             // for PrimitiveMesh
#include "tal/vector3.h"          // for Vector3

namespace sota {

enum class TesselationMode { Iterative = 0, Recursive };
enum class Orientation { Plane = 0, Polyhedron };

/**
 * @brief Base class for all meshes
 *
 * Since PrimitiveMesh is used as super-class there are mandatory parts like vertices, normals, indices, etc.
 */
class SotaMesh : public PrimitiveMesh {
  GDCLASS(SotaMesh, PrimitiveMesh)
 public:
  SotaMesh(std::unique_ptr<RegularPolygon> base_polygon) : _base_ngon(std::move(base_polygon)) { _base_ngon->check(); }
  /**
   * @brief Property shared with Godot inspector
   *
   * @param p_divisions
   */
  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_id(int p_id) { _id = p_id; }
  int get_id() const { return _id; }

  void init();
  void update();

  const RegularPolygon& base() const { return *_base_ngon.get(); }
  void set_base(std::unique_ptr<RegularPolygon> base) { _base_ngon = std::move(base); }

  /**
   * @brief Mandatory method to define after subclassing from PrimitiveMesh
   *
   * @return Array of vertices, normals, indices, etc. See Godot docs
   */
#ifdef SOTA_GDEXTENSION
  Array _create_mesh_array() const override;
#else
  void _create_mesh_array(Array& result) const override;
#endif

  /**
   * @brief Method is useful e.g. if vertices are (re)calculated from scratch or their order is changed
   */
  void recalculate_all_except_vertices();
  /**
   * @brief Calculate normals. Usually used when positions of vertices are changed but order remains the same
   */
  void calculate_normals();

  Vector3Array get_vertices() const;
  std::vector<Vector3>& get_normals() { return normals_; }

  void set_vertices(Vector3Array vertices);

  void set_orientation(Orientation p_orientation) { _orientation = p_orientation; }
  void set_tesselation_mode(TesselationMode p_tesselation_mode) { _tesselation_mode = p_tesselation_mode; }

  Orientation get_orientation() const { return _orientation; }
  TesselationMode get_tesselation_mode() const { return _tesselation_mode; }

  Vector3 get_center() const { return _base_ngon->center(); }
  float get_R() const { return _base_ngon->center().distance_to(_base_ngon->points()[0]); }
  float get_r() const {
    auto points = _base_ngon->points();
    Vector3 side_center = (points[0] + points[1]) / 2;
    return _base_ngon->center().distance_to(side_center);
  }

  Vector3 get_base_normal_direction(Vector3 point) const {
    return _orientation == Orientation::Plane ? _base_ngon->normal() : point.normalized();
  }

 protected:
  int _id{0};
  /**
   * @brief Opaque field to define tesselation level of particular shape. For different shapes may have different
   * meaning
   */
  int _divisions{1};

  /**
   * @brief Object to store base polygon
   */
  std::unique_ptr<RegularPolygon> _base_ngon;

  Orientation _orientation{Orientation::Plane};
  TesselationMode _tesselation_mode{TesselationMode::Iterative};

  Vector3Array vertices_;
  std::vector<Vector3> normals_;
  TangentsArray tangents_;
  ColorsArray colors_;
  Vector2Array tex_uv1_;
  Vector2Array tex_uv2_;
  IntArray indices_;
  ByteArray color_custom0_;
  ByteArray color_custom1_;
  ByteArray color_custom2_;
  ByteArray color_custom3_;
  IntArray bones_;
  WeightsArray weights_;

  static void _bind_methods();
  virtual void init_impl() = 0;

  void calculate_tangents();
  void calculate_colors();
  /**
   * @brief Mapping of texture is a mandatory method to define by subclass.
   */
  virtual void calculate_tex_uv1() = 0;
  virtual void calculate_tex_uv2();
  void calculate_indices();
  void calculate_color_custom();
  void calculate_bones_weights();

  /**
   * @brief Recursively splits triangle into 4 smaller triangles and appends to vertices.
   *
   * @param a - vertice of triangle
   * @param b - vertice of triangle
   * @param c - vertice of triangle
   * @param level - required levels of recursion. Level 1 means no recursion, i.e. 1 triangle will be added. On level 2
   * - 4 triangles. Level 3 - 16 triangles and so on
   */
  void tesselate_into_triangles(Vector3 a, Vector3 b, Vector3 c, int level);

  // negative offset means direction negative to base hex normal
  void add_face_to_base_edge(Edge e, float offset) {
    auto c = e.a + get_base_normal_direction(e.a) * offset;
    auto d = e.b + get_base_normal_direction(e.b) * offset;

    Edge edge1 = Edge{.a = e.a, .b = e.b};
    Edge edge2 = Edge{.a = c, .b = d};
    if (offset < 0) {
      std::swap(edge1, edge2);
    }
    auto [first_triangle, second_triangle] = Face(edge1, edge2).get_triangles();

    vertices_.append_array(first_triangle.to_godot_array());
    vertices_.append_array(second_triangle.to_godot_array());
  }

  void add_faces(float offset) {
    for (auto& v : vertices_) {
      if (_orientation == Orientation::Polyhedron) {
        v += v.normalized() * offset;
      } else if (_orientation == Orientation::Plane) {
        v += _base_ngon->normal() * offset;
      }
    }

    const int n = _base_ngon->points().size();
    for (int i = 0; i < n; ++i) {
      auto corner_points = _base_ngon->points();

      add_face_to_base_edge(Edge{.a = corner_points[i], .b = corner_points[(i + 1) % n]}, offset);
    }
  }

 private:
  Vector3Array normals_to_godot_fmt() const;
};

}  // namespace sota
