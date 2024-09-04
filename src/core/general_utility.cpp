#include "core/general_utility.h"

#include <cstdlib>

#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "misc/types.h"

namespace sota {

using namespace gd;

void GeneralUtility::shift_compress(godot::PackedVector3Array& vertices, float shift, float compress, float offset) {
  for (auto& v : vertices) {
    v.y += shift;
    v.y *= compress;
    v.y += offset;
  }
}

void GeneralUtility::shift_compress_polyhedron(godot::PackedVector3Array& vertices,
                                               godot::PackedVector3Array initial_vertices, float shift, float compress,
                                               float offset) {
  int n = vertices.size();
  for (int i = 0; i < n; ++i) {
    Vector3 dir = initial_vertices[i].normalized();
    vertices[i] = initial_vertices[i] + dir * std::abs(vertices[i].length() - initial_vertices[i].length()) * compress;
  }
}

void GeneralUtility::make_smooth_normals(std::vector<GroupedHexagonMeshVertices>& vertex_groups) {
  GroupedHexagonMeshVertices all;
  for (auto& g : vertex_groups) {
    for (auto it = g.begin(); it != g.end(); ++it) {
      all[it->first].insert(all[it->first].end(), it->second.begin(), it->second.end());
    }
  }

  for (auto it = all.begin(); it != all.end(); ++it) {
    auto& series_of_normals = it->second;
    Vector3 normal(0.0, 0.0, 0.0);
    for (auto* n : series_of_normals) {
      normal += *n;
    }

    normal /= series_of_normals.size();
    normal.normalize();
    for (auto* n : series_of_normals) {
      *n = normal;
    }
  }
}

std::pair<std::vector<std::array<float, 3>>, std::vector<float>> GeneralUtility::get_border_line_coeffs(
    float R, float r, std::unordered_set<int> exclude_list) {
  auto get_coeffs = [R, r, exclude_list]() -> std::vector<std::array<float, 3>> {
    std::array<std::array<float, 3>, 6> all = HexBorderLineParams(R, r).get_coeffs();

    std::vector<std::array<float, 3>> coeffs;
    for (int i = 0; i < 6; ++i) {
      if (!exclude_list.empty() && exclude_list.contains(i)) {
        continue;
      }
      coeffs.push_back(all[i]);
    }
    return coeffs;
  };
  std::vector<std::array<float, 3>> coeffs = get_coeffs();
  unsigned int coeffs_size = coeffs.size();
  std::vector<float> coeffs_precalc(coeffs_size);
  for (unsigned int i = 0; i < coeffs_size; ++i) {
    coeffs_precalc[i] = Vector2(coeffs[i][0], coeffs[i][1]).length();
  }

  return {coeffs, coeffs_precalc};
}

}  // namespace sota
