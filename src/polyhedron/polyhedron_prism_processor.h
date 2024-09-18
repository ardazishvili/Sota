#pragma once

#include "polyhedron/polyhedron_mesh_processor.h"
#include "tal/material.h"
#include "tal/reference.h"

namespace sota {

class PolyhedronPrismProcessor : public PolyhedronProcessor {
 public:
  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat, Polyhedron &polyhedron_mesh) override;
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      Polyhedron &polyhedron_mesh) override;
  void process(Polyhedron &polyhedron_mesh) override;
};

}  // namespace sota
