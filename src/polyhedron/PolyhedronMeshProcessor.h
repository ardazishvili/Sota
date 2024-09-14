#pragma once

#include "tal/material.h"
#include "tal/reference.h"
#include "misc/types.h"
#include "primitives/Hexagon.h"

namespace sota {

class PolyhedronMesh;

class PolyhedronProcessor {
 public:
  virtual ~PolyhedronProcessor() = default;

  virtual void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat,
                              PolyhedronMesh &polyhedron_mesh) = 0;
  virtual void process(PolyhedronMesh &polyhedron_mesh) = 0;
};

}  // namespace sota
