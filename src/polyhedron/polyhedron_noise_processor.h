#pragma once

#include <map>      // for map
#include <utility>  // for pair
#include <vector>   // for vector

#include "core/tile_mesh.h"
#include "misc/types.h"                            // for Biome
#include "polyhedron/polyhedron_mesh_processor.h"  // for PolyhedronProcessor
#include "tal/material.h"                          // for ShaderMaterial
#include "tal/reference.h"                         // for Ref

namespace sota {
class Hexagon;
class Pentagon;
class Polyhedron;
class TileMesh;

class PolyhedronNoiseProcessor : public PolyhedronProcessor {
 public:
  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat, Polyhedron &polyhedron_mesh) override;
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      Polyhedron &polyhedron_mesh) override;
  void process(Polyhedron &polyhedron_mesh) override;

 private:
  void process_meshes(Polyhedron &polyhedron_mesh, std::vector<Ref<TileMesh>> &meshes);

  std::map<std::pair<int, int>, float> _distance_keeper;
};
}  // namespace sota
