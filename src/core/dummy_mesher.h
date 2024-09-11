#pragma once

#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"

namespace sota {

class DummyMesher {
 public:
  static godot::PackedFloat32Array calculate_tangents(int n);
  static godot::PackedColorArray calculate_colors(int n);
  static godot::PackedVector2Array calculate_tex_uv2(int n);
  static godot::PackedByteArray calculate_color_custom0(int n);
  static godot::PackedByteArray calculate_color_custom1(int n);
  static godot::PackedByteArray calculate_color_custom2(int n);
  static godot::PackedByteArray calculate_color_custom3(int n);
  static godot::PackedInt32Array calculate_bones(int n);
  static godot::PackedFloat32Array calculate_weights(int n);
};

}  // namespace sota
