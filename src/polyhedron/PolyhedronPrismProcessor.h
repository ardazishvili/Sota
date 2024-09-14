#pragma once

#include "tal/material.h"
#include "tal/reference.h"
#include "polyhedron/PolyhedronMeshProcessor.h"

namespace sota {

class PolyhedronPrismProcessor : public PolyhedronProcessor {
 public:
  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      PolyhedronMesh &polyhedron_mesh) override;
  void process(PolyhedronMesh &polyhedron_mesh) override;
};

}  // namespace sota
