#pragma once

#include <memory>
#include <vector>

#include "polyhedron/polyhedron_mesh_processor.h"
#include "ridge.h"
#include "ridge_impl/ridge_mesh.h"
#include "ridge_impl/ridge_set.h"
#include "tal/reference.h"
#include "tile_mesh.h"

namespace sota {

class RidgePolyhedron;

// TODO copypaste from ridge_hex_grid.h
using GroupOfRidgeMeshes = std::vector<RidgeMesh *>;
using BiomeGroups = std::vector<GroupOfRidgeMeshes>;
struct RidgeGroup {
  GroupOfRidgeMeshes meshes;
  std::unique_ptr<RidgeSet> ridge_set;

  // TODO make class
  //  BiomeRG() = default;
  //  BiomeRG(const BiomeRG &other) = delete;
  //  BiomeRG(BiomeRG &&other) = default;
  //  BiomeRG &operator=(const BiomeRG &other) = delete;
  //  BiomeRG &operator=(BiomeRG &&other) = default;
};

class PolyhedronRidgeProcessor : public PolyhedronProcessor {
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
  // TODO copypaste from ridge_hex_grid.h
  std::vector<RidgeGroup> _mountain_groups;
  std::vector<RidgeGroup> _water_groups;
  BiomeGroups _plain_groups;
  BiomeGroups _hill_groups;
  RidgeConfig _ridge_config;

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
  void set_biomes();

  // TODO copypaste from ridge_hex_grid.h
  void print_biomes();
  void assign_ridges(GroupOfRidgeMeshes &group, RidgeSet *ridge_set);
  void calculate_corner_points_distances_to_border(GroupOfRidgeMeshes &group);
};

}  // namespace sota
