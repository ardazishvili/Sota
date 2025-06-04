#include "ridge_hex_grid.h"

#include <algorithm>   // for find, max, min
#include <functional>  // for reference_wrapper
#include <limits>      // for numeric_limits
#include <memory>      // for make_unique, alloca...
#include <numeric>
#include <unordered_map>  // for unordered_map, unor...

#include "algo/dsu.h"                  // for DSU
#include "core/general_utility.h"      // for GeneralUtility
#include "core/godot_utils.h"          // for clean_children
#include "core/hex_grid.h"             // for TilesLayout
#include "core/hex_mesh.h"             // for HexMeshParams
#include "core/hexagonal_utility.h"    // for HexagonalUtility
#include "core/mesh.h"                 // for SotaMesh
#include "core/rectangular_utility.h"  // for RectangularUtility
#include "core/smooth_shades_processor.h"
#include "core/tile_mesh.h"           // for TileMesh
#include "core/utils.h"               // for is_odd, pointy_top_...
#include "misc/biome_calculator.h"    // for BiomeCalculator
#include "misc/cube_coordinates.h"    // for CubeCoordinates
#include "misc/tile.h"                // for BiomeTile, Tile
#include "misc/types.h"               // for Biome, GroupedMeshV...
#include "misc/utilities.h"           // for create_ridge_mesh
#include "primitives/hexagon.h"       // for make_hexagon_at_pos...
#include "ridge_impl/ridge_config.h"  // for RidgeConfig
#include "ridge_impl/ridge_group.h"   // for RidgeGroup, GroupOf...
#include "ridge_impl/ridge_mesh.h"    // for RidgeMesh, RidgeHex...
#include "ridge_impl/ridge_set.h"     // for RidgeSet
#include "ridge_impl/terraformer.h"
#include "tal/callable.h"    // for Callable
#include "tal/godot_core.h"  // for D_METHOD, ClassDB
#include "tal/material.h"    // for ShaderMaterial
#include "tal/noise.h"       // for FastNoiseLite
#include "tal/texture.h"     // for Texture
#include "tal/vector3.h"     // for Vector3
#include "tal/vector3i.h"    // for Vector3i

namespace sota {

RidgeHexGrid::RidgeHexGrid() {
  _texture[Biome::PLAIN] = Ref<Texture>();
  _texture[Biome::HILL] = Ref<Texture>();
  _texture[Biome::WATER] = Ref<Texture>();
  _texture[Biome::MOUNTAIN] = Ref<Texture>();
}

void RidgeHexGrid::init() {
  init_col_row_layout();
  if (_col_row_layout.empty()) {
    return;
  }

  calculate_offsets();

  make_tiles();

  calculate_geometry();
}

void RidgeHexGrid::calculate_geometry() {
  assign_cube_coordinates_map();
  init_biomes();

  // print_biomes();

  prepare_heights_calculation();
  calculate_final_heights();
  calculate_normals();
}

void RidgeHexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_terraformer", "p_terraformer"), &RidgeHexGrid::set_terraformer);

  ClassDB::bind_method(D_METHOD("get_smooth_normals"), &RidgeHexGrid::get_smooth_normals);
  ClassDB::bind_method(D_METHOD("set_smooth_normals", "p_smooth_normals"), &RidgeHexGrid::set_smooth_normals);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "_smooth_normals"), "set_smooth_normals", "get_smooth_normals");

  ADD_GROUP("Ridge params", "ridge_");
  ClassDB::bind_method(D_METHOD("get_ridge_variation_min_bound"), &RidgeHexGrid::get_ridge_variation_min_bound);
  ClassDB::bind_method(D_METHOD("set_ridge_variation_min_bound", "p_ridge_variation_min_bound"),
                       &RidgeHexGrid::set_ridge_variation_min_bound);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_variation_min_bound"), "set_ridge_variation_min_bound",
               "get_ridge_variation_min_bound");
  ClassDB::bind_method(D_METHOD("get_ridge_variation_max_bound"), &RidgeHexGrid::get_ridge_variation_max_bound);
  ClassDB::bind_method(D_METHOD("set_ridge_variation_max_bound", "p_ridge_variation_max_bound"),
                       &RidgeHexGrid::set_ridge_variation_max_bound);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_variation_max_bound"), "set_ridge_variation_max_bound",
               "get_ridge_variation_max_bound");
  ClassDB::bind_method(D_METHOD("get_ridge_top_offset"), &RidgeHexGrid::get_ridge_top_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_top_offset", "p_ridge_top_offset"), &RidgeHexGrid::set_ridge_top_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_top_offset"), "set_ridge_top_offset", "get_ridge_top_offset");
  ClassDB::bind_method(D_METHOD("get_ridge_bottom_offset"), &RidgeHexGrid::get_ridge_bottom_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_bottom_offset", "p_ridge_bottom_offset"),
                       &RidgeHexGrid::set_ridge_bottom_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_bottom_offset"), "set_ridge_bottom_offset",
               "get_ridge_bottom_offset");

  ADD_GROUP("Biomes params", "biomes_");
  ClassDB::bind_method(D_METHOD("get_biomes_hill_level_ratio"), &RidgeHexGrid::get_biomes_hill_level_ratio);
  ClassDB::bind_method(D_METHOD("set_biomes_hill_level_ratio", "p_biomes_hill_level_ratio"),
                       &RidgeHexGrid::set_biomes_hill_level_ratio);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "biomes_hill_level_ratio"), "set_biomes_hill_level_ratio",
               "get_biomes_hill_level_ratio");
  ClassDB::bind_method(D_METHOD("get_biomes_plain_hill_gain"), &RidgeHexGrid::get_biomes_plain_hill_gain);
  ClassDB::bind_method(D_METHOD("set_biomes_plain_hill_gain", "p_biomes_plain_hill_gain"),
                       &RidgeHexGrid::set_biomes_plain_hill_gain);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "biomes_plain_hill_gain"), "set_biomes_plain_hill_gain",
               "get_biomes_plain_hill_gain");

  ADD_GROUP("Noise", "noise_");
  ClassDB::bind_method(D_METHOD("get_biomes_noise"), &RidgeHexGrid::get_biomes_noise);
  ClassDB::bind_method(D_METHOD("set_biomes_noise", "p_biomes_noise"), &RidgeHexGrid::set_biomes_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_biomes", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_biomes_noise",
               "get_biomes_noise");

  ClassDB::bind_method(D_METHOD("get_hex_noise"), &RidgeHexGrid::get_hex_noise);
  ClassDB::bind_method(D_METHOD("set_hex_noise", "p_hex_noise"), &RidgeHexGrid::set_hex_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_plain_hill", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_hex_noise",
               "get_hex_noise");

  ClassDB::bind_method(D_METHOD("get_ridge_noise"), &RidgeHexGrid::get_ridge_noise);
  ClassDB::bind_method(D_METHOD("set_ridge_noise", "p_ridge_noise"), &RidgeHexGrid::set_ridge_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_mountain_water", PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
               "set_ridge_noise", "get_ridge_noise");

  ADD_GROUP("Textures", "texture_");
  ClassDB::bind_method(D_METHOD("get_plain_texture"), &RidgeHexGrid::get_plain_texture);
  ClassDB::bind_method(D_METHOD("set_plain_texture", "p_texture"), &RidgeHexGrid::set_plain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_plain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_plain_texture", "get_plain_texture");

  ClassDB::bind_method(D_METHOD("get_hill_texture"), &RidgeHexGrid::get_hill_texture);
  ClassDB::bind_method(D_METHOD("set_hill_texture", "p_texture"), &RidgeHexGrid::set_hill_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_hill", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_hill_texture", "get_hill_texture");

  ClassDB::bind_method(D_METHOD("get_water_texture"), &RidgeHexGrid::get_water_texture);
  ClassDB::bind_method(D_METHOD("set_water_texture", "p_texture"), &RidgeHexGrid::set_water_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_water", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_water_texture", "get_water_texture");

  ClassDB::bind_method(D_METHOD("get_mountain_texture"), &RidgeHexGrid::get_mountain_texture);
  ClassDB::bind_method(D_METHOD("set_mountain_texture", "p_texture"), &RidgeHexGrid::set_mountain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_mountain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_mountain_texture", "get_mountain_texture");
}

void RidgeHexGrid::set_terraformer(const Ref<Terraformer> p_terraformer) {
  p_terraformer->set_ridge_hex_grid(this);
  p_terraformer->set_tiles(&_tiles_layout);

  _tile_processors.push_back(p_terraformer);
}

void RidgeHexGrid::set_smooth_normals(const bool p_smooth_normals) {
  _smooth_normals = p_smooth_normals;
  calculate_normals();
}

void RidgeHexGrid::set_ridge_variation_min_bound(const float p_ridge_variation_min_bound) {
  _ridge_config.variation_min_bound = p_ridge_variation_min_bound;
  init();
}

void RidgeHexGrid::set_ridge_variation_max_bound(const float p_ridge_variation_max_bound) {
  _ridge_config.variation_max_bound = p_ridge_variation_max_bound;
  init();
}

void RidgeHexGrid::set_ridge_top_offset(float p_ridge_top_offset) {
  _ridge_config.top_ridge_offset = p_ridge_top_offset;
  init();
}

void RidgeHexGrid::set_ridge_bottom_offset(float p_ridge_bottom_offset) {
  _ridge_config.bottom_ridge_offset = p_ridge_bottom_offset;
  init();
}

void RidgeHexGrid::set_biomes_hill_level_ratio(float p_biomes_hill_level_ratio) {
  _biomes_hill_level_ratio = p_biomes_hill_level_ratio;
  init();
}

void RidgeHexGrid::set_biomes_plain_hill_gain(float p_biomes_plain_hill_gain) {
  _biomes_plain_hill_gain = p_biomes_plain_hill_gain;
  init();
}

void RidgeHexGrid::set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise) {
  _biomes_noise = p_biomes_noise;
  if (_biomes_noise.ptr()) {
    _biomes_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGrid::set_hex_noise(const Ref<FastNoiseLite> p_hex_noise) {
  _plain_noise = p_hex_noise;
  if (_plain_noise.ptr()) {
    _plain_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGrid::set_ridge_noise(const Ref<FastNoiseLite> p_ridge_noise) {
  _ridge_noise = p_ridge_noise;
  if (_ridge_noise.ptr()) {
    _ridge_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGrid::set_plain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::PLAIN] = p_texture;
  init();
}

void RidgeHexGrid::set_hill_texture(const Ref<Texture> p_texture) {
  _texture[Biome::HILL] = p_texture;
  init();
}

void RidgeHexGrid::set_water_texture(const Ref<Texture> p_texture) {
  _texture[Biome::WATER] = p_texture;
  init();
}

void RidgeHexGrid::set_mountain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::MOUNTAIN] = p_texture;
  init();
}

bool RidgeHexGrid::get_smooth_normals() const { return _smooth_normals; }
Ref<FastNoiseLite> RidgeHexGrid::get_biomes_noise() const { return _biomes_noise; }
Ref<FastNoiseLite> RidgeHexGrid::get_hex_noise() const { return _plain_noise; }
Ref<FastNoiseLite> RidgeHexGrid::get_ridge_noise() const { return _ridge_noise; }
float RidgeHexGrid::get_ridge_variation_min_bound() const { return _ridge_config.variation_min_bound; }
float RidgeHexGrid::get_ridge_variation_max_bound() const { return _ridge_config.variation_max_bound; }
float RidgeHexGrid::get_ridge_top_offset() const { return _ridge_config.top_ridge_offset; }
float RidgeHexGrid::get_ridge_bottom_offset() const { return _ridge_config.bottom_ridge_offset; }
float RidgeHexGrid::get_biomes_hill_level_ratio() const { return _biomes_hill_level_ratio; }
float RidgeHexGrid::get_biomes_plain_hill_gain() const { return _biomes_plain_hill_gain; }
Ref<Texture> RidgeHexGrid::get_plain_texture() const { return _texture.find(Biome::PLAIN)->second; }
Ref<Texture> RidgeHexGrid::get_hill_texture() const { return _texture.find(Biome::HILL)->second; }
Ref<Texture> RidgeHexGrid::get_water_texture() const { return _texture.find(Biome::WATER)->second; }
Ref<Texture> RidgeHexGrid::get_mountain_texture() const { return _texture.find(Biome::MOUNTAIN)->second; }

void RidgeHexGrid::calculate_offsets() {
  for (auto row : _col_row_layout) {
    for (auto val : row) {
      auto x_offset = val.z * pointy_top_x_offset(_diameter);
      x_offset += is_odd(val.x) ? pointy_top_x_offset(_diameter) / 2 : 0;

      auto z_offset = val.x * pointy_top_y_offset(_diameter);
      _offsets[calculate_id(val.x, val.z)] = Vector3(x_offset, 0, z_offset);
    }
  }
}

std::vector<std::vector<Biome>> RidgeHexGrid::calculate_biomes() {
  std::vector<std::vector<float>> altitudes;
  for (auto row : _col_row_layout) {
    altitudes.push_back({});
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);
      Vector3 o = _offsets[id];
      if (_biomes_noise.ptr()) {
        altitudes.back().push_back(_biomes_noise->get_noise_2d(o.x, o.z));
      } else {
        altitudes.back().push_back(0.0f);
      }
    }
  }

  float min_z = std::numeric_limits<float>::max();
  float max_z = std::numeric_limits<float>::min();

  int row_num = _col_row_layout.size();

  for (int row = 0; row < row_num; ++row) {
    int col_num = _col_row_layout[row].size();
    for (int col = 0; col < col_num; ++col) {
      min_z = std::min(min_z, altitudes[row][col]);
      max_z = std::max(max_z, altitudes[row][col]);
    }
  }

  return BiomeCalculator().calculate_biomes(min_z, max_z, altitudes);
}

BiomeTile* RidgeHexGrid::make_biome_tile(Biome biome, int row, int col) {
  Vector3i val = _col_row_layout[row][col];
  int id = calculate_id(val.x, val.z);

  Ref<ShaderMaterial> mat;
  mat.instantiate();
  if (_shader.ptr()) {
    mat->set_shader(_shader);
  }
  if (_texture[biome].ptr()) {
    mat->set_shader_parameter("water_texture", _texture[Biome::WATER].ptr());
    mat->set_shader_parameter("plain_texture", _texture[Biome::PLAIN].ptr());
    mat->set_shader_parameter("hill_texture", _texture[Biome::HILL].ptr());
    mat->set_shader_parameter("mountain_texture", _texture[Biome::MOUNTAIN].ptr());

    mat->set_shader_parameter("top_offset", _ridge_config.top_ridge_offset);
    mat->set_shader_parameter("bottom_offset", _ridge_config.bottom_ridge_offset);
    mat->set_shader_parameter("hill_level_ratio", _biomes_hill_level_ratio);
  }

  Hexagon hex = make_hexagon_at_position(_offsets[id], _diameter);

  ClipOptions clip_options = get_clip_options(val.x, val.z);
  RidgeHexMeshParams params{
      .hex_mesh_params = HexMeshParams{.id = id,
                                       .diameter = _diameter,
                                       .frame_state = _frame_state,
                                       .frame_offset = _frame_offset,
                                       .material = mat,
                                       .divisions = _divisions,
                                       .clip_options = clip_options},
      .plain_noise = _plain_noise,
      .ridge_noise = _ridge_noise,
  };

  Ref<RidgeMesh> m = create_ridge_mesh(biome, hex, params);
  return make_non_ref<BiomeTile>(m, this, biome, OffsetCoordinates{.row = val.x, .col = val.z}, row, col);
}

void RidgeHexGrid::make_tiles() {
  _tiles_layout.clear();
  clean_children(*this);

  std::vector<std::vector<Biome>> biomes = calculate_biomes();

  int row_num = _col_row_layout.size();
  for (int row = 0; row < row_num; ++row) {
    int col_num = _col_row_layout[row].size();
    _tiles_layout.push_back({});
    for (int col = 0; col < col_num; ++col) {
      Biome biome = biomes[row][col];

      _tiles_layout.back().push_back(make_biome_tile(biome, row, col));
    }
  }
}

std::vector<TileMesh*> RidgeHexGrid::meshes() {
  std::vector<TileMesh*> res;

  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      res.push_back(tile_ptr->mesh().ptr());
    }
  }

  return res;
}

void RidgeHexGrid::calculate_normals() { SmoothShadesProcessor(meshes()).calculate_normals(_smooth_normals); }

void RidgeHexGrid::init_biomes() {
  _mountain_groups.clear();
  _water_groups.clear();
  _plain_groups.clear();
  _hill_groups.clear();

  BiomeGroups mountain_groups = collect_biome_groups(Biome::MOUNTAIN);
  for (auto group : mountain_groups) {
    _mountain_groups.emplace_back(group, std::make_unique<RidgeSet>(_ridge_config));
  }

  BiomeGroups plain_groups = collect_biome_groups(Biome::PLAIN);
  for (auto group : plain_groups) {
    _plain_groups.emplace_back(group);
  }

  BiomeGroups hill_groups = collect_biome_groups(Biome::HILL);
  for (auto group : hill_groups) {
    _hill_groups.emplace_back(group);
  }

  BiomeGroups water_groups = collect_biome_groups(Biome::WATER);
  for (auto group : water_groups) {
    _water_groups.emplace_back(group, std::make_unique<RidgeSet>(_ridge_config));
  }
}

void RidgeHexGrid::calculate_neighbours(const GroupOfRidgeMeshes& group) {
  auto member_of_group = [&group](RidgeMesh& ridge_hex_mesh) {
    return std::find(group.begin(), group.end(), &ridge_hex_mesh) != group.end();
  };
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr);
      RidgeMesh* ridge_hex_mesh = dynamic_cast<RidgeMesh*>(tile->mesh().ptr());
      if (std::find(group.begin(), group.end(), ridge_hex_mesh) == group.end()) {
        continue;
      }
      CubeCoordinates cube_cur = tile->get_cube_coords();
      Neighbours hexagon_neighbours = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
      std::vector<CubeCoordinates> neighbours_hexes_coord = neighbours(cube_cur);

      for (unsigned int i = 0; i < neighbours_hexes_coord.size(); ++i) {
        CubeCoordinates n = neighbours_hexes_coord[i];
        RidgeMesh* casted = dynamic_cast<RidgeMesh*>(_cube_to_hexagon[n]);
        if (_cube_to_hexagon.contains(n) && member_of_group(*casted) && member_of_group(*ridge_hex_mesh)) {
          hexagon_neighbours[i] = _cube_to_hexagon[n];
        }
      }

      tile->set_neighbours(hexagon_neighbours);
    }
  }
}

void RidgeHexGrid::assign_neighbours(const GroupOfRidgeMeshes& group) {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr);
      RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(tile->mesh().ptr());
      if (std::find(group.begin(), group.end(), ridge_mesh) == group.end()) {
        continue;
      }
      ridge_mesh->set_neighbours(tile->neighbours());
    }
  }
}

void RidgeHexGrid::init_ridges(std::vector<RidgeGroup>& group, float ridge_offset) {
  for (RidgeGroup& group : group) {
    group.init_ridges(_distance_map, ridge_offset, _divisions);
  }
}

void RidgeHexGrid::prepare_heights_calculation() {
  for (RidgeGroup& group : all_groups()) {
    calculate_neighbours(group.meshes());
    assign_neighbours(group.meshes());
  }
  init_ridges(_mountain_groups, _ridge_config.top_ridge_offset);
  init_ridges(_water_groups, _ridge_config.bottom_ridge_offset);

  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  auto calculate_initial = [&global_max_y, &global_min_y](const GroupOfRidgeMeshes& group) {
    for (auto* mesh : group) {
      mesh->calculate_initial_heights();
      auto [mesh_min_z, mesh_max_z] = mesh->get_min_max_height();
      global_min_y = std::min(global_min_y, mesh_min_z);
      global_max_y = std::max(global_max_y, mesh_max_z);
    }
  };
  for (RidgeGroup& group : all_groups()) {
    group.fmap(calculate_initial);
  }

  float amplitude = global_max_y - global_min_y;
  float compression_factor = _biomes_plain_hill_gain / amplitude;

  auto shift_compress = [global_min_y, compression_factor](const GroupOfRidgeMeshes& group) {
    for (auto* mesh : group) {
      mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  };

  for (RidgeGroup& group : all_groups()) {
    group.fmap(shift_compress);
  }
}

void RidgeHexGrid::assign_cube_coordinates_map() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr);
      _cube_to_hexagon[tile->get_cube_coords()] = tile_ptr->mesh().ptr();
    }
  }
}

void RidgeHexGrid::calculate_final_heights() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      RidgeMesh* mesh = dynamic_cast<RidgeMesh*>(tile_ptr->mesh().ptr());

      mesh->calculate_final_heights(_distance_map, _diameter, _divisions);
      mesh->calculate_normals();
      mesh->update();
    }
  }
}

// RectRidgeHexGrid definitions
void RectRidgeHexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &RectRidgeHexGrid::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &RectRidgeHexGrid::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &RectRidgeHexGrid::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &RectRidgeHexGrid::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_width"), "set_width", "get_width");

  ClassDB::bind_method(D_METHOD("get_clipped_option"), &RectRidgeHexGrid::get_clipped_option);
  ClassDB::bind_method(D_METHOD("set_clipped_option", "p_clipped_option"), &RectRidgeHexGrid::set_clipped_option);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "_clipped"), "set_clipped_option", "get_clipped_option");

  ClassDB::bind_method(D_METHOD("set_biomes"), &RectRidgeHexGrid::set_biomes);
}

void RectRidgeHexGrid::set_height(const int p_height) {
  _height = p_height > 1 ? p_height : 1;
  init();
}

void RectRidgeHexGrid::set_width(const int p_width) {
  _width = p_width > 1 ? p_width : 1;
  init();
}

void RectRidgeHexGrid::set_clipped_option(const bool p_clipped_option) {
  _clipped = p_clipped_option;
  init();
}

int RectRidgeHexGrid::get_height() const { return _height; }
int RectRidgeHexGrid::get_width() const { return _width; }
bool RectRidgeHexGrid::get_clipped_option() const { return _clipped; }

void RectRidgeHexGrid::init_col_row_layout() {
  _col_row_layout = RectangularUtility::get_offset_coords_layout(_height, _width);
}

int RectRidgeHexGrid::calculate_id(int row, int col) const {
  return RectangularUtility::calculate_id(row, col, _width);
}

BiomeGroups RectRidgeHexGrid::collect_biome_groups(Biome b) {
  int height = _col_row_layout.size();
  int width = _col_row_layout[0].size();
  algo::DSU<RidgeMesh*> u(height, width);

  auto flat = [width](int i, int j) { return i * width + j; };
  for (auto row : _col_row_layout) {
    for (Vector3i v : row) {
      int i = v.x;
      int j = v.z;
      BiomeTile* tile = dynamic_cast<BiomeTile*>(_tiles_layout[i][j]);
      Biome biome = tile->biome();
      if (biome != b) {
        continue;
      }
      RidgeMesh* mesh = dynamic_cast<RidgeMesh*>(tile->mesh().ptr());
      u.push(flat(i, j), mesh);
      u.make_union(flat(i, j), flat(i - 1, j));
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i - 1, j + 1}))) {
        u.make_union(flat(i, j), flat(i - 1, j + 1));
      }
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i - 1, j - 1}))) {
        u.make_union(flat(i, j), flat(i - 1, j - 1));
      }
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i, j - 1}))) {
        u.make_union(flat(i, j), flat(i, j - 1));
      }
    }
  }
  return u.groups();
}

ClipOptions RectRidgeHexGrid::get_clip_options(int row, int col) const {
  if (!_clipped) {
    return {.left = false, .right = false, .up = false, .down = false};
  }
  return {.left = col == 0 && !is_odd(row),
          .right = col == (_width - 1) && is_odd(row),
          .up = row == (_height - 1),
          .down = row == 0};
}
#ifdef SOTA_GDEXTENSION
void RectRidgeHexGrid::set_biomes(godot::String str, int row_num, int col_num) {
#else
void RectRidgeHexGrid::set_biomes(String str, int row_num, int col_num) {
#endif
  int n = str.length();
  if (n != (row_num * col_num)) {
    print("Wrong number of biomes in set_biomes method ");
    return;
  }
  for (int i = 0; i < n; ++i) {
    char c = str[i];
    if (c != 'P' && c != 'H' && c != 'M' && c != 'W') {
      print("Unknown type of terrain in set_biomes method");
    }
  }

  _tiles_layout.clear();
  clean_children(*this);
  _tiles_layout = std::vector<std::vector<Tile*>>(row_num, std::vector<Tile*>(col_num, nullptr));
  for (int i = 0; i < n; ++i) {
    int row = i / row_num;
    int col = i % col_num;
    char c = str[i];
    switch (c) {
      case 'P':
        _tiles_layout[row][col] = make_biome_tile(Biome::PLAIN, row, col);
        break;
      case 'H':
        _tiles_layout[row][col] = make_biome_tile(Biome::HILL, row, col);
        break;
      case 'M':
        _tiles_layout[row][col] = make_biome_tile(Biome::MOUNTAIN, row, col);
        break;
      case 'W':
        _tiles_layout[row][col] = make_biome_tile(Biome::WATER, row, col);
        break;
    }
  }

  calculate_geometry();
}

// HexagonalRidgeHexGrid definitions
void HexagonalRidgeHexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_size"), &HexagonalRidgeHexGrid::get_size);
  ClassDB::bind_method(D_METHOD("set_size", "p_size"), &HexagonalRidgeHexGrid::set_size);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_size"), "set_size", "get_size");

  ClassDB::bind_method(D_METHOD("set_biomes"), &HexagonalRidgeHexGrid::set_biomes);
}

void HexagonalRidgeHexGrid::set_size(const int p_size) {
  _size = p_size > 1 ? p_size : 1;
  init();
}

int HexagonalRidgeHexGrid::get_size() const { return _size; }

void HexagonalRidgeHexGrid::init_col_row_layout() {
  _col_row_layout = HexagonalUtility::get_offset_coords_layout(_size);
}

int HexagonalRidgeHexGrid::calculate_id(int row, int col) const {
  return HexagonalUtility::calculate_id(row, col, _size);
}

BiomeGroups HexagonalRidgeHexGrid::collect_biome_groups(Biome b) {
  int width = _size * 2 + 1;
  algo::DSU<RidgeMesh*> u(width, width);

  auto flat = [width](int i, int j) { return i * width + j; };
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr);
      Biome biome = tile->biome();
      if (biome != b) {
        continue;
      }
      auto offset_coords = tile->get_offset_coords();
      int i = offset_coords.row;
      int j = offset_coords.col;
      RidgeMesh* mesh = dynamic_cast<RidgeMesh*>(tile->mesh().ptr());
      u.push(flat(i, j), mesh);
      u.make_union(flat(i, j), flat(i - 1, j));
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i - 1, j + 1}))) {
        u.make_union(flat(i, j), flat(i - 1, j + 1));
      }
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i - 1, j - 1}))) {
        u.make_union(flat(i, j), flat(i - 1, j - 1));
      }
      if (_cube_to_hexagon.contains(offsetToCube(OffsetCoordinates{i, j - 1}))) {
        u.make_union(flat(i, j), flat(i, j - 1));
      }
    }
  }
  return u.groups();
}

ClipOptions HexagonalRidgeHexGrid::get_clip_options(int row, int col) const {
  return {.left = false, .right = false, .up = false, .down = false};
}
#ifdef SOTA_GDEXTENSION
void HexagonalRidgeHexGrid::set_biomes(godot::String str, int size) {
#else
void HexagonalRidgeHexGrid::set_biomes(String str, int size) {
#endif
  int n = std::accumulate(_tiles_layout.begin(), _tiles_layout.end(), 0,
                          [](int acc, const std::vector<Tile*>& row_of_tiles) { return acc + row_of_tiles.size(); });
  if (n != str.length()) {
    print("Wrong number of biomes in set_biomes method ");
    return;
  }
  for (int i = 0; i < n; ++i) {
    char c = str[i];
    if (c != 'P' && c != 'H' && c != 'M' && c != 'W') {
      print("Unknown type of terrain in set_biomes method");
    }
  }

  int str_index = 0;
  for (std::vector<Tile*>& row_of_tiles : _tiles_layout) {
    for (Tile* tile : row_of_tiles) {
      auto* biome_tile = dynamic_cast<BiomeTile*>(tile);
      int row = biome_tile->row();
      int col = biome_tile->col();
      _tiles_layout[row][col]->destroy();

      char c = str[str_index];
      switch (c) {
        case 'P':
          _tiles_layout[row][col] = make_biome_tile(Biome::PLAIN, row, col);
          break;
        case 'H':
          _tiles_layout[row][col] = make_biome_tile(Biome::HILL, row, col);
          break;
        case 'M':
          _tiles_layout[row][col] = make_biome_tile(Biome::MOUNTAIN, row, col);
          break;
        case 'W':
          _tiles_layout[row][col] = make_biome_tile(Biome::WATER, row, col);
          break;
      }
      ++str_index;
    }
  }

  calculate_geometry();
}

}  // namespace sota
