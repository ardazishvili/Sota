#pragma once

#include <memory>
#include <vector>

#include "polyhedron/polyhedron_mesh_processor.h"
#include "ridge.h"
#include "ridge_impl/ridge_based_object.h"
#include "ridge_impl/ridge_group.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"
#include "tal/reference.h"
#include "tile_mesh.h"

namespace sota {

class RidgePolyhedron;

class PolyhedronRidgeProcessor : public PolyhedronProcessor, public RidgeBased {
 public:
  PolyhedronRidgeProcessor() = default;
  PolyhedronRidgeProcessor(const PolyhedronRidgeProcessor &other) = delete;
  PolyhedronRidgeProcessor(PolyhedronRidgeProcessor &&other) = default;
  PolyhedronRidgeProcessor &operator=(const PolyhedronRidgeProcessor &other) = delete;
  PolyhedronRidgeProcessor &operator=(PolyhedronRidgeProcessor &&other) = default;

  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat, Polyhedron &polyhedron_mesh) override;
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      Polyhedron &polyhedron_mesh) override;
  void process(Polyhedron &polyhedron_mesh) override;

 private:
  // TODO fix possibility of zero
  RidgePolyhedron *_polyhedron_mesh;

  std::vector<Ref<TileMesh>> _meshes;

  std::map<std::pair<int, int>, float> _distance_keeper;

  // TODO fix possibility of zero
  float _R_average{0};

  void init(Polyhedron *polyhedron_mesh);

  void process_meshes();
  void set_neighbours();
  void set_group_neighbours();
  void init_biomes();
};

}  // namespace sota
