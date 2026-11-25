#pragma once

#include <vector>

#include "core/general_utility.h"
#include "core/tile_mesh.h"
#include "misc/discretizer.h"
#include "misc/types.h"

namespace sota {

/**
 * @brief Applies flat or smooth shading to a collection of tile meshes.
 *
 * Flat shading recalculates face normals for each mesh. Smooth shading combines normals
 * at matching quantized positions across the supplied meshes, including tile boundaries,
 * so the input vertices must share a coordinate space. The processor borrows the tile
 * meshes and requests a refresh after updating their normals.
 */
class SmoothShadesProcessor {
 public:
  SmoothShadesProcessor(std::vector<TileMesh*> meshes) : _meshes(meshes) {}

  void calculate_normals(bool smooth_normals);

 private:
  std::vector<TileMesh*> _meshes;

  void meshes_update();
  void calculate_flat_normals();
  void calculate_smooth_normals();
};

}  // namespace sota
