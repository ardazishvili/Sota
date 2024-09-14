#include "hill_hex_mesh.h"

#include "tal/vector2.h"

namespace sota {

void HillHexMesh::calculate_final_heights() {
  shift_compress();
  auto t = [this](float x, float z) { return (_r - std::min(_r, Vector2(0, 0).distance_to(Vector2(x, z)))) / _r; };
  for (auto& v : vertices_) {
    v.y *= std::lerp(1, 3, t(v.x, v.z));
  }

  _min_y += _y_shift;
  _min_y *= _y_compress;
  _min_y += _hex.center().y;

  _max_y += _y_shift;
  _max_y *= _y_compress;
  _max_y += _hex.center().y;
}

}  // namespace sota
