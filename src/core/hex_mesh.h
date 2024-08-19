#pragma once

#include <godot_cpp/classes/primitive_mesh.hpp>

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "misc/types.h"

namespace sota {

class HexMesh : public gd::PrimitiveMesh {
  GDCLASS(HexMesh, gd::PrimitiveMesh)

 public:
  HexMesh();

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_diameter(const float p_diameter);
  float get_diameter() const;

  void set_id(int p_id) { id = p_id; }
  int get_id() const { return id; }

  gd::Array _create_mesh_array() const override;

  void calculate_normals() const;
  void init(bool clip_left = false, bool clip_right = false, bool clip_up = false, bool clip_bottom = false);
  void update();

  void set_frame_state(bool state) { frame_state = state; }
  void set_frame_value(float value) { frame_offset = value; }

 protected:
  static void _bind_methods();

  void calculate_vertices(bool clip_left = false, bool clip_right = false, bool clip_up = false,
                          bool clip_bottom = false) const;

  mutable gd::PackedVector3Array vertices_;
  mutable gd::PackedVector3Array normals_;

  int id;
  float R;
  float r;
  float diameter{1};
  int divisions{3};
  std::array<gd::Vector3, 6> _corner_points;

  bool frame_state{false};
  float frame_offset{0.0};

 private:
  mutable gd::PackedFloat32Array tangents_;
  mutable gd::PackedColorArray colors_;
  mutable gd::PackedVector2Array tex_uv1_;
  mutable gd::PackedVector2Array tex_uv2_;
  mutable gd::PackedInt32Array indices_;
  mutable gd::PackedByteArray color_custom0_;
  mutable gd::PackedByteArray color_custom1_;
  mutable gd::PackedByteArray color_custom2_;
  mutable gd::PackedByteArray color_custom3_;
  mutable gd::PackedInt32Array bones_;
  mutable gd::PackedFloat32Array weights_;

  void calculate_tangents() const;
  void add_frame() const;
  void calculate_colors() const;
  void calculate_tex_uv1() const;
  void calculate_tex_uv2() const;
  void calculate_indices() const;
  void calculate_color_custom() const;
  void calculate_bones_weights() const;

  void z_clip(float boundary) const;
};

}  // namespace sota
