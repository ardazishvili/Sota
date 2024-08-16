#pragma once

#include <memory>
#include <unordered_map>

#include "core/hex_grid_map.h"
#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/classes/texture.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "ridge_impl/ridge_set.h"

namespace sota {

using GroupOfHexagonMeshes = std::vector<RidgeHexMesh*>;
using BiomeGroups = std::vector<GroupOfHexagonMeshes>;
using BiomeRidgeGroup = std::pair<GroupOfHexagonMeshes, std::unique_ptr<RidgeSet>>;

class RidgeHexGridMap : public HexGridMap {
  GDCLASS(RidgeHexGridMap, HexGridMap)

 public:
  RidgeHexGridMap();

  void set_biomes_noise(const gd::Ref<gd::FastNoiseLite> p_biomes_noise);
  gd::Ref<gd::FastNoiseLite> get_biomes_noise() const;

  void set_hex_noise(const gd::Ref<gd::FastNoiseLite> p_hex_noise);
  gd::Ref<gd::FastNoiseLite> get_hex_noise() const;

  void set_ridge_noise(const gd::Ref<gd::FastNoiseLite> p_ridge_noise);
  gd::Ref<gd::FastNoiseLite> get_ridge_noise() const;

  void set_plain_texture(const gd::Ref<gd::Texture> p_texture);
  gd::Ref<gd::Texture> get_plain_texture() const;

  void set_hill_texture(const gd::Ref<gd::Texture> p_texture);
  gd::Ref<gd::Texture> get_hill_texture() const;

  void set_water_texture(const gd::Ref<gd::Texture> p_texture);
  gd::Ref<gd::Texture> get_water_texture() const;

  void set_mountain_texture(const gd::Ref<gd::Texture> p_texture);
  gd::Ref<gd::Texture> get_mountain_texture() const;

  void set_ridge_variation_min_bound(float p_ridge_variation_min_bound);
  float get_ridge_variation_min_bound() const;

  void set_ridge_variation_max_bound(float p_ridge_variation_max_bound);
  float get_ridge_variation_max_bound() const;

  void set_ridge_top_offset(float p_ridge_top_offset);
  float get_ridge_top_offset() const;

  void set_ridge_bottom_offset(float p_ridge_bottom_offset);
  float get_ridge_bottom_offset() const;

  void set_biomes_hill_level_ratio(float p_biomes_hill_level_ratio);
  float get_biomes_hill_level_ratio() const;

  void set_biomes_plain_hill_gain(float p_biomes_plain_hill_gain);
  float get_biomes_plain_hill_gain() const;

  void set_smooth_normals(bool p_smooth_normals);
  bool get_smooth_normals() const;

 protected:
  static void _bind_methods();

  void init() override;
  void init_hexmesh() override;
  void init_mesh_lib() override;

 private:
  std::unordered_map<Biome, gd::Ref<gd::Texture>> texture;

  gd::Ref<gd::FastNoiseLite> biomes_noise;
  gd::Ref<gd::FastNoiseLite> plain_noise;
  gd::Ref<gd::FastNoiseLite> ridge_noise;

  std::vector<BiomeRidgeGroup> _mountain_groups;
  std::vector<BiomeRidgeGroup> _water_groups;
  BiomeGroups _plain_groups;
  BiomeGroups _hill_groups;

  bool smooth_normals{false};
  RidgeConfig ridge_config;
  float biomes_hill_level_ratio{0.7};
  float biomes_plain_hill_gain{0.1f};

  void assign_ridges(GroupOfHexagonMeshes& group, RidgeSet* ridge_set);
  void calculate_neighbours(GroupOfHexagonMeshes& group);
  void assign_neighbours(GroupOfHexagonMeshes& group);
  void create_biome_ridges(std::vector<BiomeRidgeGroup>& group, float ridge_offset);
  void calculate_corner_points_distances_to_border(GroupOfHexagonMeshes& group);

  BiomeGroups collect_biome_groups(Biome b, int row, int col);

  void init_biomes();
  void prepare_heights_calculation();
  void calculate_final_heights();

  void assign_cube_coordinates_map();
  void calculate_normals() override;
  void calculate_smooth_normals();
  void calculate_flat_normals();
  void meshes_update();
};

}  // namespace sota
