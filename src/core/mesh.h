#pragma once

#include <vector>

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

  void set_vertices(Vector3Array vertices);

 protected:
  int _id{0};
  /**
   * @brief Opaque field to define tesselation level of particular shape. For different shapes may have different
   * meaning
   */
  int _divisions{1};

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

 private:
  Vector3Array normals_to_godot_fmt() const;
};

}  // namespace sota
