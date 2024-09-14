#pragma once

#include <memory>
#include <unordered_map>

#include "core/hex_grid.h"
#include "cube_coordinates.h"
#include "ridge_impl/ridge_set.h"
#include "tal/noise.h"
#include "tal/texture.h"

namespace sota {

using GroupOfHexagonMeshes = std::vector<RidgeHexMesh*>;
using BiomeGroups = std::vector<GroupOfHexagonMeshes>;
using BiomeRidgeGroup = std::pair<GroupOfHexagonMeshes, std::unique_ptr<RidgeSet>>;

class RidgeHexGrid : public HexGrid {
  GDCLASS(RidgeHexGrid, HexGrid)

 public:
  RidgeHexGrid();

  void set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise);
  Ref<FastNoiseLite> get_biomes_noise() const;

  void set_hex_noise(const Ref<FastNoiseLite> p_hex_noise);
  Ref<FastNoiseLite> get_hex_noise() const;

  void set_ridge_noise(const Ref<FastNoiseLite> p_ridge_noise);
  Ref<FastNoiseLite> get_ridge_noise() const;

  void set_plain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_plain_texture() const;

  void set_hill_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_hill_texture() const;

  void set_water_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_water_texture() const;

  void set_mountain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_mountain_texture() const;

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
  std::vector<BiomeRidgeGroup> _mountain_groups;
  std::vector<BiomeRidgeGroup> _water_groups;
  BiomeGroups _plain_groups;
  BiomeGroups _hill_groups;

  static void _bind_methods();

  void init() override;
  void init_hexmesh() override;

 private:
  std::unordered_map<Biome, Ref<Texture>> _texture;

  Ref<FastNoiseLite> _biomes_noise;
  Ref<FastNoiseLite> _plain_noise;
  Ref<FastNoiseLite> _ridge_noise;

  bool _smooth_normals{false};
  RidgeConfig _ridge_config;
  float _biomes_hill_level_ratio{0.7};
  float _biomes_plain_hill_gain{0.1f};

  void assign_ridges(GroupOfHexagonMeshes& group, RidgeSet* ridge_set);
  void calculate_neighbours(GroupOfHexagonMeshes& group);
  void assign_neighbours(GroupOfHexagonMeshes& group);
  void create_biome_ridges(std::vector<BiomeRidgeGroup>& group, float ridge_offset);
  void calculate_corner_points_distances_to_border(GroupOfHexagonMeshes& group);

  virtual BiomeGroups collect_biome_groups(Biome b) = 0;
  virtual ClipOptions get_clip_options(int row, int col) const = 0;

  void init_biomes();
  void prepare_heights_calculation();
  void calculate_final_heights();

  void assign_cube_coordinates_map();
  void calculate_normals() override;
  void calculate_smooth_normals();
  void calculate_flat_normals();
  void meshes_update();
  void print_biomes();
};

class RectRidgeHexGrid : public RidgeHexGrid {
  GDCLASS(RectRidgeHexGrid, RidgeHexGrid)
 public:
  void set_height(const int p_height);
  int get_height() const;

  void set_width(const int p_width);
  int get_width() const;

  void set_clipped_option(bool p_clipped_option);
  bool get_clipped_option() const;

  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;
  BiomeGroups collect_biome_groups(Biome b) override;
  ClipOptions get_clip_options(int row, int col) const override;

 protected:
  int _height{0};
  int _width{0};
  bool _clipped{false};
  static void _bind_methods();
};

class HexagonalRidgeHexGrid : public RidgeHexGrid {
  GDCLASS(HexagonalRidgeHexGrid, RidgeHexGrid)
 public:
  void set_size(const int p_size);
  int get_size() const;

  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;
  BiomeGroups collect_biome_groups(Biome b) override;
  ClipOptions get_clip_options(int row, int col) const override;

 protected:
  int _size{0};
  static void _bind_methods();

 private:
};
}  // namespace sota
