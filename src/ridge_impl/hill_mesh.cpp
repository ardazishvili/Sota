#include "hill_mesh.h"

#include <memory>  // for unique_ptr

#include "core/mesh.h"           // for SotaMesh
#include "general_utility.h"     // for MeshProcessor
#include "primitives/polygon.h"  // for RegularPolygon
#include "tal/vector3.h"         // for Vector3

namespace sota {

void HillMesh::calculate_final_heights(std::map<std::pair<int, int>, float>& distance_keeper, float diameter,
                                       int divisions) {
  shift_compress();

  // TODO add piping for inputs/outputs
  auto vertices = _mesh->get_vertices();
  _processor->calculate_hill_heights(vertices, _mesh->get_r(), _mesh->get_R(), _mesh->get_center());
  _mesh->set_vertices(vertices);

  _min_height += _y_shift;
  _min_height *= _y_compress;
  _min_height += _mesh->base().center().y;

  _max_height += _y_shift;
  _max_height *= _y_compress;
  _max_height += _mesh->base().center().y;
}

}  // namespace sota
