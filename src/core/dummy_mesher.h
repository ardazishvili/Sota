#pragma once

#include "tal/arrays.h"

namespace sota {

/**
 * @brief Class to provide dummy information for PrimitiveMesh
 */
class DummyMesher {
 public:
  static TangentsArray calculate_tangents(int n);
  static ColorsArray calculate_colors(int n);
  static Vector2Array calculate_tex_uv2(int n);
  static ByteArray calculate_color_custom0(int n);
  static ByteArray calculate_color_custom1(int n);
  static ByteArray calculate_color_custom2(int n);
  static ByteArray calculate_color_custom3(int n);
  static IntArray calculate_bones(int n);
  static WeightsArray calculate_weights(int n);
};

}  // namespace sota
