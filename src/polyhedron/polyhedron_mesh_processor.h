#pragma once

#include "misc/types.h"
#include "primitives/hexagon.h"
#include "primitives/pentagon.h"
#include "tal/material.h"
#include "tal/reference.h"

namespace sota {

class Polyhedron;

class PolyhedronProcessor {
 public:
  virtual ~PolyhedronProcessor() = default;

  virtual void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat,
                              Polyhedron &polyhedron_mesh) = 0;
  virtual void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat,
                              Polyhedron &polyhedron_mesh) = 0;
  virtual void process(Polyhedron &polyhedron_mesh) = 0;
};

}  // namespace sota