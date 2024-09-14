#pragma once

#include "tal/arrays.h"
#include "tal/mesh.h"
#include "tal/vector3.h"

namespace sota {

/**
 * @brief Base class for all meshes
 *
 * Since PrimitiveMesh is used as super-class there are mandatory parts like vertices, normals, indices, etc.
 */
class SotaMesh : public PrimitiveMesh {
  GDCLASS(SotaMesh, PrimitiveMesh)
 public:
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

  /**
   * @brief Mandatory method to define after subclassing from PrimitiveMesh
   *
   * @return Array of vertices, normals, indices, etc. See Godot docs
   */
  Array _create_mesh_array() const override;
  /**
   * @brief Method is useful e.g. if vertices are (re)calculated from scratch or their order is changed
   */
  void recalculate_all_except_vertices() const;
  /**
   * @brief Calculate normals. Usually used when positions of vertices are changed but order remains the same
   */
  void calculate_normals() const;

  Vector3Array get_vertices() const;

  void set_vertices(Vector3Array vertices);

 protected:
  int _id{0};
  /**
   * @brief Opaque field to define tesselation level of particular shape. For different shapes may have different
   * meaning
   */
  int _divisions{1};

  mutable Vector3Array vertices_;
  mutable Vector3Array normals_;
  mutable TangentsArray tangents_;
  mutable ColorsArray colors_;
  mutable Vector2Array tex_uv1_;
  mutable Vector2Array tex_uv2_;
  mutable IntArray indices_;
  mutable ByteArray color_custom0_;
  mutable ByteArray color_custom1_;
  mutable ByteArray color_custom2_;
  mutable ByteArray color_custom3_;
  mutable IntArray bones_;
  mutable WeightsArray weights_;

  static void _bind_methods();
  virtual void init_impl() = 0;

  void calculate_tangents() const;
  void calculate_colors() const;
  /**
   * @brief Mapping of texture is a mandatory method to define by subclass.
   */
  virtual void calculate_tex_uv1() const = 0;
  virtual void calculate_tex_uv2() const;
  void calculate_indices() const;
  void calculate_color_custom() const;
  void calculate_bones_weights() const;

  /**
   * @brief Recursively splits triangle into 4 smaller triangles and appends to vertices.
   *
   * @param a - vertice of triangle
   * @param b - vertice of triangle
   * @param c - vertice of triangle
   * @param level - required levels of recursion. Level 1 means no recursion, i.e. 1 triangle will be added. On level 2
   * - 4 triangles. Level 3 - 16 triangles and so on
   */
  void tesselate_into_triangles(Vector3 a, Vector3 b, Vector3 c, int level) const;
};

}  // namespace sota
