#include "hill_hex_mesh.h"

#include "core/utils.h"

namespace sota {
using namespace gd;

void HillHexMesh::calculate_final_heights() {
  shift_compress();
  float r = small_radius(diameter);
  auto t = [this, r](float x, float z) { return (r - std::min(r, Vector2(0, 0).distance_to(Vector2(x, z)))) / r; };
  for (auto& v : vertices_) {
    v.y *= std::lerp(1, 3, t(v.x, v.z));
  }

  _min_y += _y_shift;
  _min_y *= _y_compress;
  _min_y += offset.y;

  _max_y += _y_shift;
  _max_y *= _y_compress;
  _max_y += offset.y;
}

}  // namespace sota
