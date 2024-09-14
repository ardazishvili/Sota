#pragma once

#include <unordered_set>

#include "misc/types.h"
#include "tal/arrays.h"

namespace sota {
class GeneralUtility {
 public:
  static void shift_compress(Vector3Array& vertices, float shift, float compress, float offset);
  static Vector3Array shift_compress_polyhedron(Vector3Array vertices, Vector3Array initial_vertices, float shift,
                                                float compress, float offset);
  static void make_smooth_normals(std::vector<GroupedHexagonMeshVertices>& vertex_groups);
  static std::pair<std::vector<std::array<float, 3>>, std::vector<float>> get_border_line_coeffs(
      float R, float r, std::unordered_set<int> exclude_list);
};

struct HexBorderLineParams {
  HexBorderLineParams(float R, float r) {
    _coeffs = {std::array<float, 3>{-R / (2 * r), 1.0f, R}, std::array<float, 3>{1.0f, 0.0f, -r},
               std::array<float, 3>{R / (2 * r), 1.0f, -R}, std::array<float, 3>{-R / (2 * r), 1.0f, -R},
               std::array<float, 3>{1.0f, 0.0f, r},         std::array<float, 3>{R / (2 * r), 1.0f, R}};
  }
  std::array<std::array<float, 3>, 6> get_coeffs() { return _coeffs; }

 private:
  std::array<std::array<float, 3>, 6> _coeffs;
};

}  // namespace sota
