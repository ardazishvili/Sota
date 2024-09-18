#pragma once

#include "polyhedron/polyhedron_mesh_processor.h"
#include "tile_mesh.h"

namespace sota {

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
