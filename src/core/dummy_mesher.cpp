#include "core/dummy_mesher.h"

#include "tal/arrays.h"
#include "tal/color.h"
#include "tal/vector2.h"

namespace sota {

TangentsArray DummyMesher::calculate_tangents(int n) {
  TangentsArray result;
  result.resize(n * 4);
  result.fill(0);
  return result;
}

ColorsArray DummyMesher::calculate_colors(int n) {
  ColorsArray result;
  result.resize(n);
  result.fill(Color(0, 0, 0));
  return result;
}

Vector2Array DummyMesher::calculate_tex_uv2(int n) {
  Vector2Array result;
  result.resize(n);
  result.fill(Vector2(0, 0));
  return result;
}

ByteArray DummyMesher::calculate_color_custom0(int n) {
  ByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

ByteArray DummyMesher::calculate_color_custom1(int n) {
  ByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

ByteArray DummyMesher::calculate_color_custom2(int n) {
  ByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

ByteArray DummyMesher::calculate_color_custom3(int n) {
  ByteArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

IntArray DummyMesher::calculate_bones(int n) {
  IntArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

WeightsArray DummyMesher::calculate_weights(int n) {
  WeightsArray result;
  result.resize(4 * n);
  result.fill(0);
  return result;
}

}  // namespace sota
