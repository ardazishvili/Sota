#pragma once

#include <godot_cpp/classes/primitive_mesh.hpp>

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector3.hpp"

namespace sota {

/**
 * @brief Base class for all meshes
 *
 * Since PrimitiveMesh is used as super-class there are mandatory parts like vertices, normals, indices, etc.
 */
class SotaMesh : public godot::PrimitiveMesh {
  GDCLASS(SotaMesh, godot::PrimitiveMesh)
 public:
  /**
   * @brief Property shared with Godot inspector
   *
   * @param p_divisions
   */
  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_id(int p_id) { id = p_id; }
  int get_id() const { return id; }

  void init();

  /**
   * @brief Mandatory method to define after subclassing from PrimitiveMesh
   *
   * @return Array of vertices, normals, indices, etc. See Godot docs
   */
  godot::Array _create_mesh_array() const override;
  /**
   * @brief Method is useful e.g. if vertices are (re)calculated from scratch or their order is changed
   */
  void recalculate_all_except_vertices() const;
  /**
   * @brief Calculate normals. Usually used when positions of vertices are changed but order remains the same
   */
  void calculate_normals() const;

  godot::PackedVector3Array get_vertices() const;

  void set_vertices(godot::PackedVector3Array vertices);

 protected:
  int id{0};
  /**
   * @brief Opaque field to define tesselation level of particular shape. For different shapes may have different
   * meaning
   */
  int divisions{1};

  mutable godot::PackedVector3Array vertices_;
  mutable godot::PackedVector3Array normals_;
  mutable godot::PackedFloat32Array tangents_;
  mutable godot::PackedColorArray colors_;
  mutable godot::PackedVector2Array tex_uv1_;
  mutable godot::PackedVector2Array tex_uv2_;
  mutable godot::PackedInt32Array indices_;
  mutable godot::PackedByteArray color_custom0_;
  mutable godot::PackedByteArray color_custom1_;
  mutable godot::PackedByteArray color_custom2_;
  mutable godot::PackedByteArray color_custom3_;
  mutable godot::PackedInt32Array bones_;
  mutable godot::PackedFloat32Array weights_;

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
  void tesselate_into_triangles(godot::Vector3 a, godot::Vector3 b, godot::Vector3 c, int level) const;
};

}  // namespace sota
