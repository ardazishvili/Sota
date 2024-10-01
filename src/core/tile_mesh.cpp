#include "core/tile_mesh.h"

namespace sota {

DiscreteVertexToNormals TileMesh::get_discrete_vertex_to_normals() {
  auto d = VertexToNormalDiscretizer(inner_mesh()->get_r() / (inner_mesh()->get_divisions() * 2));
  d.discretize(inner_mesh()->get_vertices(), inner_mesh()->get_normals());
  return d.get();
}

}  // namespace sota
