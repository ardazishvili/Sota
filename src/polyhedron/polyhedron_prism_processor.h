#pragma once

#include "misc/types.h"                            // for Biome
#include "polyhedron/polyhedron_mesh_processor.h"  // for PolyhedronProcessor
#include "tal/material.h"                          // for ShaderMaterial
#include "tal/reference.h"                         // for Ref

namespace sota {
class Hexagon;
class Pentagon;
class Polyhedron;

class PolyhedronPrismProcessor : public PolyhedronProcessor {
 public:
  void configure_hexagon(PolygonWrapper &wrapper, Biome biome, int &id, Ref<ShaderMaterial> mat,
                         Polyhedron &polyhedron_mesh) override;
  void configure_pentagon(PolygonWrapper &wrapper, Biome biome, int &id, Ref<ShaderMaterial> mat,
                          Polyhedron &polyhedron_mesh) override;
  void process(Polyhedron &polyhedron_mesh) override;
};

}  // namespace sota
