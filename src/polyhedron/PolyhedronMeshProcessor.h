#pragma once

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "misc/types.h"
#include "primitives/Hexagon.h"

namespace sota {

class PolyhedronMesh;

class PolyhedronProcessor {
 public:
  virtual ~PolyhedronProcessor() = default;

  virtual void configure_cell(Hexagon hex, Biome biome, int &id, godot::Ref<godot::ShaderMaterial> mat,
                              PolyhedronMesh &polyhedron_mesh) = 0;
  virtual void process(PolyhedronMesh &polyhedron_mesh) = 0;
};

}  // namespace sota
