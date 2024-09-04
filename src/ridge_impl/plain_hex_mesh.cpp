#include "plain_hex_mesh.h"

namespace sota {

void PlainHexMesh::calculate_final_heights() {
  shift_compress();
  _min_y += _y_shift;
  _min_y *= _y_compress;
  _min_y += _hex.center().y;

  _max_y += _y_shift;
  _max_y *= _y_compress;
  _max_y += _hex.center().y;
}

}  // namespace sota
