#pragma once

#include "polyhedron/PolyhedronMeshProcessor.h"

namespace sota {

///////////////////////////////////////////////////// TEMPORARY BROKEN

class PolyhedronRidgeProcessor : public PolyhedronProcessor {
 public:
  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      PolyhedronMesh &polyhedron_mesh) override;
  void process(PolyhedronMesh &polyhedron_mesh) override;
};
}  // namespace sota
