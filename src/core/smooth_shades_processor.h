#pragma once

#include <vector>

#include "core/general_utility.h"
#include "core/tile_mesh.h"
#include "misc/discretizer.h"
#include "misc/types.h"

namespace sota {

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
