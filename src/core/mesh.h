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

class SotaMesh : public godot::PrimitiveMesh {
  GDCLASS(SotaMesh, godot::PrimitiveMesh)
 public:
  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_id(int p_id) { id = p_id; }
  int get_id() const { return id; }

  void init() {
    init_impl();
    request_update();
  }

  godot::Array _create_mesh_array() const override;
  void recalculate_all_except_vertices() const;
  void calculate_normals() const;

 protected:
  int id;
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
  virtual void calculate_tex_uv1() const = 0;
  virtual void calculate_tex_uv2() const;
  void calculate_indices() const;
  void calculate_color_custom() const;
  void calculate_bones_weights() const;

  void tesselate_into_triangles(godot::Vector3 a, godot::Vector3 b, godot::Vector3 c, int level) const;
};

}  // namespace sota
