#include "core/dummy_mesher.h"

#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/vector2.hpp"

namespace sota {

using namespace godot;

PackedFloat32Array DummyMesher::calculate_tangents(int n) {
  PackedFloat32Array result;
  result.resize(n * 4);
  result.fill(0);
  return result;
}

PackedColorArray DummyMesher::calculate_colors(int n) {
  PackedColorArray result;
  result.resize(n);
  result.fill(Color(0, 0, 0));
  return result;
}

PackedVector2Array DummyMesher::calculate_tex_uv2(int n) {
  PackedVector2Array result;
  result.resize(n);
  result.fill(Vector2(0, 0));
  return result;
}

PackedByteArray DummyMesher::calculate_color_custom0(int n) {
  PackedByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

PackedByteArray DummyMesher::calculate_color_custom1(int n) {
  PackedByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

PackedByteArray DummyMesher::calculate_color_custom2(int n) {
  PackedByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

PackedByteArray DummyMesher::calculate_color_custom3(int n) {
  PackedByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

PackedInt32Array DummyMesher::calculate_bones(int n) {
  PackedInt32Array result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

PackedFloat32Array DummyMesher::calculate_weights(int n) {
  PackedFloat32Array result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

}  // namespace sota
