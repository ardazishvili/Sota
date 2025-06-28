#pragma once

#include <map>  // for map
#include <memory>
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "core/hex_grid.h"  // for HexGrid
#include "misc/cube_coordinates.h"
#include "misc/types.h"                     // for Biome, ClipOptions
#include "ridge_impl/ridge_based_object.h"  // for RidgeBased
#include "ridge_impl/ridge_group.h"         // for BiomeGroups, GroupOfRidge...
#include "ridge_impl/ridge_set.h"
#include "ridge_impl/terraformer.h"
#include "ridge_mesh.h"
#include "tal/godot_core.h"
#include "tal/noise.h"      // for FastNoiseLite
#include "tal/reference.h"  // for Ref
#include "tal/texture.h"    // for Texture
#include "tal/vector3.h"
#include "tal/vector3i.h"

#ifdef SOTA_GDEXTENSION
using String = godot::String;  // TODO: see https://github.com/ardazishvili/Sota/issues/89
#endif

namespace sota {

class BiomeTile;

class RidgeHexGrid : public HexGrid, public RidgeBased {
  GDCLASS(RidgeHexGrid, HexGrid)

 public:
  RidgeHexGrid();

  void set_terraformer(const Ref<Terraformer> p_terraformer);

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

  std::vector<Ref<MatrixProcessor>> _tile_processors;

  void update_biome(BiomeTile* biome_tile, Biome new_biome);

  Neighbours get_neighbours(BiomeTile* biome_tile, std::optional<const GroupOfRidgeMeshes*> group);

 protected:
  friend BiomeTile;  // It's OK for BiomeTiles to ping their parent, e.g. in 'on_click' handler

  void process_tile(int row, int col) {
    for (Ref<MatrixProcessor> processor : _tile_processors) {
      processor->process(row, col);
    }
  }

  DiscreteVertexToDistance _distance_map;

  static void _bind_methods();

  void init() override;
  void calculate_geometry();
  void make_tiles() override;

  BiomeTile* make_biome_tile(Biome biome, int row, int col);

 private:
  std::unordered_map<Biome, Ref<Texture>> _texture;

  Ref<FastNoiseLite> _biomes_noise;
  Ref<FastNoiseLite> _plain_noise;
  Ref<FastNoiseLite> _ridge_noise;

  bool _smooth_normals{false};
  float _biomes_hill_level_ratio{0.7};
  float _biomes_plain_hill_gain{0.1f};
  std::unordered_map<int, Vector3> _id_to_offset_coordinate;

  float _global_min_y = std::numeric_limits<float>::max();
  float _global_max_y = std::numeric_limits<float>::min();

  void assign_neighbours(const GroupOfRidgeMeshes& group);
  void init_ridges(std::vector<RidgeGroup>& group, float ridge_offset);

  virtual BiomeGroups collect_biome_groups(Biome b) = 0;
  virtual ClipOptions get_clip_options(int row, int col) const = 0;

  void init_biomes();
  void prepare_heights_calculation(RidgeGroup& group);
  void calculate_final_heights(RidgeGroup& group);

  void assign_cube_coordinates_map();
  void calculate_normals() override;
  void calculate_offsets();
  std::vector<std::vector<Biome>> calculate_biomes();

  std::vector<TileMesh*> meshes();

  Ref<RidgeMesh> make_biome_mesh(Biome biome, int id, Vector3i layout);
  RidgeGroup& get_group(BiomeTile* biome_tile);
  BiomeTile* get_biome_tile(TileMesh* target);
  void update_biome_groups(std::vector<RidgeGroup*> groups_to_be_removed, std::vector<RidgeGroup>& to_be_added,
                           Biome biome);
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
  void set_biomes(String str, int row_num, int col_num);

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
  void set_biomes(String str, int size);

 protected:
  int _size{0};
  static void _bind_methods();

 private:
};
}  // namespace sota
