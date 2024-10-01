#pragma once

#include <map>  // for map
#include <memory>
#include <utility>  // for pair
#include <vector>   // for vector

#include "core/tile_mesh.h"                        // for TileMesh
#include "misc/types.h"                            // for Biome
#include "polyhedron/polyhedron_mesh_processor.h"  // for PolyhedronProcessor
#include "ridge_impl/ridge.h"
#include "ridge_impl/ridge_based_object.h"  // for RidgeBased
#include "ridge_impl/ridge_group.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"
#include "tal/material.h"   // for ShaderMaterial
#include "tal/reference.h"  // for Ref
#include "tal/vector3i.h"

namespace sota {

class Hexagon;
class Pentagon;
class Polyhedron;
class RidgePolyhedron;

class PolyhedronRidgeProcessor : public PolyhedronProcessor, public RidgeBased {
 public:
  PolyhedronRidgeProcessor();
  PolyhedronRidgeProcessor(const PolyhedronRidgeProcessor &other) = delete;
  PolyhedronRidgeProcessor(PolyhedronRidgeProcessor &&other) = default;
  PolyhedronRidgeProcessor &operator=(const PolyhedronRidgeProcessor &other) = delete;
  PolyhedronRidgeProcessor &operator=(PolyhedronRidgeProcessor &&other) = default;

  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat, Polyhedron &polyhedron_mesh) override;
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat,
                      Polyhedron &polyhedron_mesh) override;
  void process(Polyhedron &polyhedron_mesh) override;

  void set_top_offset(float offset) { _ridge_config.top_ridge_offset = offset; }
  void set_bottom_offset(float offset) { _ridge_config.bottom_ridge_offset = offset; }
  float get_top_offset() const { return _ridge_config.top_ridge_offset; }
  float get_bottom_offset() const { return _ridge_config.bottom_ridge_offset; }

 private:
  // TODO fix possibility of zero
  RidgePolyhedron *_polyhedron_mesh;

  std::vector<Ref<TileMesh>> _meshes;

  DiscreteVertexToDistance _distance_map;

  // TODO fix possibility of zero
  float _R_average{0};

  void init(Polyhedron *polyhedron_mesh);

  void process_meshes();
  void set_neighbours();
  void set_group_neighbours();
  void init_biomes();
};

}  // namespace sota
