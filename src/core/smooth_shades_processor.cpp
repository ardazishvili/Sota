#include "core/smooth_shades_processor.h"

namespace sota {

void SmoothShadesProcessor::calculate_normals(bool smooth_normals) {
  if (smooth_normals) {
    calculate_smooth_normals();
  } else {
    calculate_flat_normals();
  }
  meshes_update();
}

void SmoothShadesProcessor::meshes_update() {
  for (TileMesh* mesh : _meshes) {
    mesh->inner_mesh()->update();
  }
}

void SmoothShadesProcessor::calculate_flat_normals() {
  for (TileMesh* mesh : _meshes) {
    mesh->inner_mesh()->calculate_normals();
  }
}

void SmoothShadesProcessor::calculate_smooth_normals() {
  std::vector<DiscreteVertexToNormals> vertex_groups;
  for (TileMesh* mesh : _meshes) {
    vertex_groups.push_back(mesh->get_discrete_vertex_to_normals());
  }

  GeneralUtility::make_smooth_normals(vertex_groups);
}

}  // namespace sota
