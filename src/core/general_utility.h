#pragma once

#include <unordered_set>

#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "misc/types.h"

namespace sota {
class GeneralUtility {
 public:
  static void shift_compress(godot::PackedVector3Array& vertices, float shift, float compress, float offset);
  static void shift_compress_polyhedron(godot::PackedVector3Array& vertices, godot::PackedVector3Array initial_vertices,
                                        float shift, float compress, float offset);
  static void make_smooth_normals(std::vector<GroupedHexagonMeshVertices>& vertex_groups);
  static std::pair<std::vector<std::array<float, 3>>, std::vector<float>> get_border_line_coeffs(
      float R, float r, std::unordered_set<int> exclude_list);
};

struct HexBorderLineParams {
  HexBorderLineParams(float R, float r) {
    coeffs = {std::array<float, 3>{-R / (2 * r), 1.0f, R}, std::array<float, 3>{1.0f, 0.0f, -r},
              std::array<float, 3>{R / (2 * r), 1.0f, -R}, std::array<float, 3>{-R / (2 * r), 1.0f, -R},
              std::array<float, 3>{1.0f, 0.0f, r},         std::array<float, 3>{R / (2 * r), 1.0f, R}};
  }
  std::array<std::array<float, 3>, 6> get_coeffs() { return coeffs; }

 private:
  std::array<std::array<float, 3>, 6> coeffs;
};

}  // namespace sota
