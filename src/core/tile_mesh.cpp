#include "core/tile_mesh.h"

namespace sota {

DiscreteVertexToNormals TileMesh::get_discrete_vertex_to_normals() {
  auto d = VertexToNormalDiscretizer(0.0001);
  d.discretize(inner_mesh()->get_vertices(), inner_mesh()->get_normals());
  return d.get();
}

}  // namespace sota
