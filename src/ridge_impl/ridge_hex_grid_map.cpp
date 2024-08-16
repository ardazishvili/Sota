#include "ridge_hex_grid_map.h"

#include <algorithm>
#include <memory>

#include "algo/dsu.h"
#include "biome_calculator.h"
#include "core/hex_mesh.h"
#include "core/utils.h"
#include "godot_cpp/classes/grid_map.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/variant/plane.hpp"
#include "godot_cpp/variant/transform3d.hpp"
#include "misc/tile.h"
#include "misc/types.h"
#include "misc/utilities.h"
#include "ridge_impl/ridge.h"
#include "ridge_impl/ridge_config.h"
#include "ridge_impl/ridge_hex_mesh.h"

namespace sota {

using namespace gd;

RidgeHexGridMap::RidgeHexGridMap() {
  texture[Biome::PLAIN] = Ref<Texture>();
  texture[Biome::HILL] = Ref<Texture>();
  texture[Biome::WATER] = Ref<Texture>();
  texture[Biome::MOUNTAIN] = Ref<Texture>();
}

void RidgeHexGridMap::init() {
  set_cell_size(Vector3(pointy_top_x_offset(diameter), 1.0, pointy_top_y_offset(diameter)));

  init_hexmesh();
  init_mesh_lib();
  init_grid();

  assign_cube_coordinates_map();
  init_biomes();

  prepare_heights_calculation();
  calculate_final_heights();
  calculate_normals();
}

void RidgeHexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_smooth_normals"), &RidgeHexGridMap::get_smooth_normals);
  ClassDB::bind_method(D_METHOD("set_smooth_normals", "p_smooth_normals"), &RidgeHexGridMap::set_smooth_normals);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_normals"), "set_smooth_normals", "get_smooth_normals");

  ADD_GROUP("Ridge params", "ridge_");
  ClassDB::bind_method(D_METHOD("get_ridge_variation_min_bound"), &RidgeHexGridMap::get_ridge_variation_min_bound);
  ClassDB::bind_method(D_METHOD("set_ridge_variation_min_bound", "p_ridge_variation_min_bound"),
                       &RidgeHexGridMap::set_ridge_variation_min_bound);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_variation_min_bound"), "set_ridge_variation_min_bound",
               "get_ridge_variation_min_bound");
  ClassDB::bind_method(D_METHOD("get_ridge_variation_max_bound"), &RidgeHexGridMap::get_ridge_variation_max_bound);
  ClassDB::bind_method(D_METHOD("set_ridge_variation_max_bound", "p_ridge_variation_max_bound"),
                       &RidgeHexGridMap::set_ridge_variation_max_bound);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_variation_max_bound"), "set_ridge_variation_max_bound",
               "get_ridge_variation_max_bound");
  ClassDB::bind_method(D_METHOD("get_ridge_top_offset"), &RidgeHexGridMap::get_ridge_top_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_top_offset", "p_ridge_top_offset"), &RidgeHexGridMap::set_ridge_top_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_top_offset"), "set_ridge_top_offset", "get_ridge_top_offset");
  ClassDB::bind_method(D_METHOD("get_ridge_bottom_offset"), &RidgeHexGridMap::get_ridge_bottom_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_bottom_offset", "p_ridge_bottom_offset"),
                       &RidgeHexGridMap::set_ridge_bottom_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_bottom_offset"), "set_ridge_bottom_offset",
               "get_ridge_bottom_offset");

  ADD_GROUP("Biomes params", "biomes_");
  ClassDB::bind_method(D_METHOD("get_biomes_hill_level_ratio"), &RidgeHexGridMap::get_biomes_hill_level_ratio);
  ClassDB::bind_method(D_METHOD("set_biomes_hill_level_ratio", "p_biomes_hill_level_ratio"),
                       &RidgeHexGridMap::set_biomes_hill_level_ratio);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "biomes_hill_level_ratio"), "set_biomes_hill_level_ratio",
               "get_biomes_hill_level_ratio");
  ClassDB::bind_method(D_METHOD("get_biomes_plain_hill_gain"), &RidgeHexGridMap::get_biomes_plain_hill_gain);
  ClassDB::bind_method(D_METHOD("set_biomes_plain_hill_gain", "p_biomes_plain_hill_gain"),
                       &RidgeHexGridMap::set_biomes_plain_hill_gain);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "biomes_plain_hill_gain"), "set_biomes_plain_hill_gain",
               "get_biomes_plain_hill_gain");

  ADD_GROUP("Noise", "noise_");
  ClassDB::bind_method(D_METHOD("get_biomes_noise"), &RidgeHexGridMap::get_biomes_noise);
  ClassDB::bind_method(D_METHOD("set_biomes_noise", "p_biomes_noise"), &RidgeHexGridMap::set_biomes_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_biomes", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_biomes_noise",
               "get_biomes_noise");

  ClassDB::bind_method(D_METHOD("get_hex_noise"), &RidgeHexGridMap::get_hex_noise);
  ClassDB::bind_method(D_METHOD("set_hex_noise", "p_hex_noise"), &RidgeHexGridMap::set_hex_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_plain_hill", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_hex_noise",
               "get_hex_noise");

  ClassDB::bind_method(D_METHOD("get_ridge_noise"), &RidgeHexGridMap::get_ridge_noise);
  ClassDB::bind_method(D_METHOD("set_ridge_noise", "p_ridge_noise"), &RidgeHexGridMap::set_ridge_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_mountain_water", PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
               "set_ridge_noise", "get_ridge_noise");

  ADD_GROUP("Textures", "texture_");
  ClassDB::bind_method(D_METHOD("get_plain_texture"), &RidgeHexGridMap::get_plain_texture);
  ClassDB::bind_method(D_METHOD("set_plain_texture", "p_texture"), &RidgeHexGridMap::set_plain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_plain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_plain_texture", "get_plain_texture");

  ClassDB::bind_method(D_METHOD("get_hill_texture"), &RidgeHexGridMap::get_hill_texture);
  ClassDB::bind_method(D_METHOD("set_hill_texture", "p_texture"), &RidgeHexGridMap::set_hill_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_hill", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_hill_texture", "get_hill_texture");

  ClassDB::bind_method(D_METHOD("get_water_texture"), &RidgeHexGridMap::get_water_texture);
  ClassDB::bind_method(D_METHOD("set_water_texture", "p_texture"), &RidgeHexGridMap::set_water_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_water", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_water_texture", "get_water_texture");

  ClassDB::bind_method(D_METHOD("get_mountain_texture"), &RidgeHexGridMap::get_mountain_texture);
  ClassDB::bind_method(D_METHOD("set_mountain_texture", "p_texture"), &RidgeHexGridMap::set_mountain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_mountain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_mountain_texture", "get_mountain_texture");
}

void RidgeHexGridMap::set_smooth_normals(const bool p_smooth_normals) {
  smooth_normals = p_smooth_normals;
  calculate_normals();
}

void RidgeHexGridMap::set_ridge_variation_min_bound(const float p_ridge_variation_min_bound) {
  ridge_config.variation_min_bound = p_ridge_variation_min_bound;
  init();
}

void RidgeHexGridMap::set_ridge_variation_max_bound(const float p_ridge_variation_max_bound) {
  ridge_config.variation_max_bound = p_ridge_variation_max_bound;
  init();
}
void RidgeHexGridMap::set_ridge_top_offset(float p_ridge_top_offset) {
  ridge_config.top_ridge_offset = p_ridge_top_offset;
  init();
}

void RidgeHexGridMap::set_ridge_bottom_offset(float p_ridge_bottom_offset) {
  ridge_config.bottom_ridge_offset = p_ridge_bottom_offset;
  init();
}

void RidgeHexGridMap::set_biomes_hill_level_ratio(float p_biomes_hill_level_ratio) {
  biomes_hill_level_ratio = p_biomes_hill_level_ratio;
  init();
}

void RidgeHexGridMap::set_biomes_plain_hill_gain(float p_biomes_plain_hill_gain) {
  biomes_plain_hill_gain = p_biomes_plain_hill_gain;
  init();
}

void RidgeHexGridMap::set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise) {
  biomes_noise = p_biomes_noise;
  if (biomes_noise.ptr()) {
    biomes_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGridMap::set_hex_noise(const Ref<FastNoiseLite> p_hex_noise) {
  plain_noise = p_hex_noise;
  if (plain_noise.ptr()) {
    plain_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGridMap::set_ridge_noise(const Ref<FastNoiseLite> p_ridge_noise) {
  ridge_noise = p_ridge_noise;
  if (ridge_noise.ptr()) {
    ridge_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeHexGridMap::set_plain_texture(const Ref<Texture> p_texture) {
  texture[Biome::PLAIN] = p_texture;
  init();
}

void RidgeHexGridMap::set_hill_texture(const Ref<Texture> p_texture) {
  texture[Biome::HILL] = p_texture;
  init();
}

void RidgeHexGridMap::set_water_texture(const Ref<Texture> p_texture) {
  texture[Biome::WATER] = p_texture;
  init();
}

void RidgeHexGridMap::set_mountain_texture(const Ref<Texture> p_texture) {
  texture[Biome::MOUNTAIN] = p_texture;
  init();
}

bool RidgeHexGridMap::get_smooth_normals() const { return smooth_normals; }
Ref<FastNoiseLite> RidgeHexGridMap::get_biomes_noise() const { return biomes_noise; }
Ref<FastNoiseLite> RidgeHexGridMap::get_hex_noise() const { return plain_noise; }
Ref<FastNoiseLite> RidgeHexGridMap::get_ridge_noise() const { return ridge_noise; }
float RidgeHexGridMap::get_ridge_variation_min_bound() const { return ridge_config.variation_min_bound; }
float RidgeHexGridMap::get_ridge_variation_max_bound() const { return ridge_config.variation_max_bound; }
float RidgeHexGridMap::get_ridge_top_offset() const { return ridge_config.top_ridge_offset; }
float RidgeHexGridMap::get_ridge_bottom_offset() const { return ridge_config.bottom_ridge_offset; }
float RidgeHexGridMap::get_biomes_hill_level_ratio() const { return biomes_hill_level_ratio; }
float RidgeHexGridMap::get_biomes_plain_hill_gain() const { return biomes_plain_hill_gain; }
Ref<Texture> RidgeHexGridMap::get_plain_texture() const { return texture.find(Biome::PLAIN)->second; }
Ref<Texture> RidgeHexGridMap::get_hill_texture() const { return texture.find(Biome::HILL)->second; }
Ref<Texture> RidgeHexGridMap::get_water_texture() const { return texture.find(Biome::WATER)->second; }
Ref<Texture> RidgeHexGridMap::get_mountain_texture() const { return texture.find(Biome::MOUNTAIN)->second; }

void RidgeHexGridMap::init_hexmesh() {
  std::vector<Vector3> offsets;
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      auto x_offset = col * pointy_top_x_offset(diameter);
      x_offset += is_odd(row) ? pointy_top_x_offset(diameter) / 2 : 0;

      auto z_offset = row * pointy_top_y_offset(diameter);
      offsets.push_back(Vector3(x_offset, 0, z_offset));
    }
  }

  std::vector<float> altitudes;
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int id = calculate_id(row, col);
      Vector3 o = offsets[id];
      if (biomes_noise.ptr()) {
        altitudes.push_back(biomes_noise->get_noise_2d(o.x, o.z));
      } else {
        altitudes.push_back(0);
      }
    }
  }

  auto [min_z, max_z] = std::minmax_element(altitudes.begin(), altitudes.end());

  _tiles_layout.clear();
  BiomeCalculator biome_calculator;
  for (int row = 0; row < height; ++row) {
    _tiles_layout.push_back({});
    for (int col = 0; col < width; ++col) {
      int id = calculate_id(row, col);
      Biome biome = biome_calculator.calculate_biome(*min_z, *max_z, altitudes[id]);

      Ref<ShaderMaterial> mat;
      mat.instantiate();
      if (shader.ptr()) {
        mat->set_shader(shader);
      }
      if (texture[biome].ptr()) {
        mat->set_shader_parameter("water_texture", texture[Biome::WATER].ptr());
        mat->set_shader_parameter("plain_texture", texture[Biome::PLAIN].ptr());
        mat->set_shader_parameter("hill_texture", texture[Biome::HILL].ptr());
        mat->set_shader_parameter("mountain_texture", texture[Biome::MOUNTAIN].ptr());

        mat->set_shader_parameter("top_offset", ridge_config.top_ridge_offset);
        mat->set_shader_parameter("bottom_offset", ridge_config.bottom_ridge_offset);
        mat->set_shader_parameter("hill_level_ratio", biomes_hill_level_ratio);
      }

      Ref<RidgeHexMesh> m = create_hex_mesh(biome);
      m->set_id(id);
      m->set_plain_noise(plain_noise);
      m->set_ridge_noise(ridge_noise);
      m->set_diameter(diameter);
      m->set_divisions(divisions);
      m->set_offset(offsets[id]);
      m->set_material(mat);
      m->init();
      _tiles_layout.back().push_back(std::make_unique<BiomeTile>(m, biome));
    }
  }
}

void RidgeHexGridMap::calculate_normals() {
  if (smooth_normals) {
    calculate_smooth_normals();
  } else {
    calculate_flat_normals();
  }
  meshes_update();
}

void RidgeHexGridMap::calculate_flat_normals() {
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      _tiles_layout[row][col]->mesh()->calculate_normals();
    }
  }
}

void RidgeHexGridMap::meshes_update() {
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      _tiles_layout[row][col]->mesh()->update();
    }
  }
}

void RidgeHexGridMap::calculate_smooth_normals() {
  std::vector<GroupedHexagonMeshVertices> vertex_groups(height * width);
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(_tiles_layout[row][col]->mesh().ptr());
      vertex_groups[calculate_id(row, col)] = mesh->get_grouped_vertices();
    }
  }

  // TODO:Additional calculations for duplicated part of terrain

  GroupedHexagonMeshVertices all;
  for (auto& g : vertex_groups) {
    for (auto it = g.begin(); it != g.end(); ++it) {
      all[it->first].insert(all[it->first].end(), it->second.begin(), it->second.end());
    }
  }

  for (auto it = all.begin(); it != all.end(); ++it) {
    auto& series_of_normals = it->second;
    Vector3 normal(0.0, 0.0, 0.0);
    for (auto* n : series_of_normals) {
      normal += *n;
    }

    normal /= series_of_normals.size();
    normal.normalize();
    for (auto* n : series_of_normals) {
      *n = normal;
    }
  }
}

BiomeGroups RidgeHexGridMap::collect_biome_groups(Biome b, int row, int col) {
  algo::DSU<RidgeHexMesh*> u(row, col);

  auto flat = [col, row](int i, int j) { return i * col + j; };
  int rows = _tiles_layout.size();
  for (int i = 0; i < rows; ++i) {
    int col = _tiles_layout[i].size();
    for (int j = 0; j < col; ++j) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(_tiles_layout[i][j].get());
      Biome biome = tile->biome();
      if (biome != b) {
        continue;
      }
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
      u.push(flat(i, j), mesh);
      if (i & 1) {
        u.make_union(flat(i, j), flat(i - 1, j));
        if (j != col - 1) {
          u.make_union(flat(i, j), flat(i - 1, j + 1));
        }
      } else {
        u.make_union(flat(i, j), flat(i - 1, j));
        if (j != 0) {
          u.make_union(flat(i, j), flat(i - 1, j - 1));
        }
      }

      if (j != 0) {
        u.make_union(flat(i, j), flat(i, j - 1));
      }
    }
  }
  return u.groups();
}

void RidgeHexGridMap::init_biomes() {
  _mountain_groups.clear();
  _water_groups.clear();
  _plain_groups.clear();
  _hill_groups.clear();
  BiomeGroups mountain_groups = collect_biome_groups(Biome::MOUNTAIN, height, width);
  for (auto group : mountain_groups) {
    _mountain_groups.emplace_back(group, std::make_unique<RidgeSet>(ridge_config));
  }

  _plain_groups = collect_biome_groups(Biome::PLAIN, height, width);
  _hill_groups = collect_biome_groups(Biome::HILL, height, width);

  BiomeGroups water_groups = collect_biome_groups(Biome::WATER, height, width);
  for (auto group : water_groups) {
    _water_groups.emplace_back(group, std::make_unique<RidgeSet>(ridge_config));
  }
}

void RidgeHexGridMap::calculate_neighbours(GroupOfHexagonMeshes& group) {
  auto member_of_group = [&group](HexMesh& mesh) {
    return std::find(group.begin(), group.end(), &mesh) != group.end();
  };
  int rows = _tiles_layout.size();
  for (int i = 0; i < rows; ++i) {
    int col = _tiles_layout[i].size();
    for (int j = 0; j < col; ++j) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(_tiles_layout[i][j].get());
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
      if (std::find(group.begin(), group.end(), mesh) == group.end()) {
        continue;
      }
      CubeCoordinates cube_cur = offsetToCube(OffsetCoordinates{.row = i, .col = j});
      HexagonNeighbours hexagon_neighbours = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
      std::vector<CubeCoordinates> neighbours_hexes_coord = neighbours(cube_cur);

      for (unsigned int i = 0; i < neighbours_hexes_coord.size(); ++i) {
        CubeCoordinates n = neighbours_hexes_coord[i];
        if (_cube_to_hexagon.contains(n) && member_of_group(*_cube_to_hexagon[n]) && member_of_group(*mesh)) {
          hexagon_neighbours[i] = _cube_to_hexagon[n];
        }
      }

      tile->set_neighbours(hexagon_neighbours);
    }
  }
}

void RidgeHexGridMap::assign_neighbours(GroupOfHexagonMeshes& group) {
  int rows = _tiles_layout.size();
  for (int i = 0; i < rows; ++i) {
    int col = _tiles_layout[i].size();
    for (int j = 0; j < col; ++j) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(_tiles_layout[i][j].get());
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
      if (std::find(group.begin(), group.end(), mesh) == group.end()) {
        continue;
      }
      mesh->set_neighbours(tile->neighbours());
    }
  }
}

void RidgeHexGridMap::assign_ridges(GroupOfHexagonMeshes& group, RidgeSet* ridge_set) {
  auto* ridges = ridge_set->ridges();
  std::vector<Ridge*> ridge_pointers;
  std::transform(ridges->begin(), ridges->end(), std::back_inserter(ridge_pointers),
                 [](Ridge& ridge) { return &ridge; });

  for (auto* mesh : group) {
    mesh->set_ridges(ridge_pointers);
  }
}

void RidgeHexGridMap::calculate_corner_points_distances_to_border(GroupOfHexagonMeshes& group) {
  std::vector<RidgeHexMesh*> meshes;
  int rows = _tiles_layout.size();
  for (int i = 0; i < rows; ++i) {
    int col = _tiles_layout[i].size();
    for (int j = 0; j < col; ++j) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(_tiles_layout[i][j]->mesh().ptr());
      if (std::find(group.begin(), group.end(), mesh) == group.end()) {
        continue;
      }
      meshes.push_back(mesh);
    }
  }
  auto compare_increasing = [this](const RidgeHexMesh* lhs, const RidgeHexMesh* rhs) {
    return lhs->get_neighbours().size() < rhs->get_neighbours().size();
  };
  for (auto* m : meshes) {
    m->calculate_corner_points_distances_to_border();
  }
  std::sort(meshes.begin(), meshes.end(), compare_increasing);
}

void RidgeHexGridMap::create_biome_ridges(std::vector<BiomeRidgeGroup>& group, float ridge_offset) {
  for (auto& [group, ridge_set] : group) {
    calculate_neighbours(group);
    assign_neighbours(group);
    if (group.size() > 1) {
      ridge_set->create_dfs_random(group, ridge_offset, divisions);
    } else {
      ridge_set->create_single(group[0], ridge_offset);
    }
    auto ridges = *ridge_set->ridges();

    assign_ridges(group, ridge_set.get());
    calculate_corner_points_distances_to_border(group);
  }
}

void RidgeHexGridMap::prepare_heights_calculation() {
  create_biome_ridges(_mountain_groups, ridge_config.top_ridge_offset);
  create_biome_ridges(_water_groups, ridge_config.bottom_ridge_offset);

  for (auto& group : _plain_groups) {
    calculate_neighbours(group);
    assign_neighbours(group);
  }

  for (auto& group : _hill_groups) {
    calculate_neighbours(group);
    assign_neighbours(group);
  }

  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  auto calculate_initial = [&global_max_y, &global_min_y](GroupOfHexagonMeshes& group) {
    for (auto* mesh : group) {
      mesh->calculate_initial_heights();
      auto [mesh_min_z, mesh_max_z] = mesh->get_min_max_height();
      global_min_y = std::min(global_min_y, mesh_min_z);
      global_max_y = std::max(global_max_y, mesh_max_z);
    }
  };
  for (auto& [group, ridge_set] : _mountain_groups) {
    calculate_initial(group);
  }
  for (auto& [group, ridge_set] : _water_groups) {
    calculate_initial(group);
  }
  for (auto& group : _hill_groups) {
    calculate_initial(group);
  }
  for (auto& group : _plain_groups) {
    calculate_initial(group);
  }

  float amplitude = global_max_y - global_min_y;
  float compression_factor = biomes_plain_hill_gain / amplitude;

  for (auto& [group, ridge_set] : _mountain_groups) {
    for (auto* mesh : group) {
      mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  }

  for (auto& [group, ridge_set] : _water_groups) {
    for (auto* mesh : group) {
      mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  }

  for (auto& group : _hill_groups) {
    for (auto* mesh : group) {
      mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  }

  for (auto& group : _plain_groups) {
    for (auto* mesh : group) {
      mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  }
}

void RidgeHexGridMap::assign_cube_coordinates_map() {
  int rows = _tiles_layout.size();
  for (int i = 0; i < rows; ++i) {
    int col = _tiles_layout[i].size();
    for (int j = 0; j < col; ++j) {
      CubeCoordinates cube_coord = offsetToCube(OffsetCoordinates{.row = i, .col = j});

      _cube_to_hexagon[cube_coord] = _tiles_layout[i][j]->mesh().ptr();
    }
  }
}

void RidgeHexGridMap::init_mesh_lib() {
  Ref<MeshLibrary> m = get_mesh_library();
  m.instantiate();
  _library = m;
  set_mesh_library(m);

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int id = calculate_id(row, col);
      _library->create_item(id);
      _library->set_item_mesh(id, _tiles_layout[row][col]->mesh());
      if (is_odd(row)) {
        Transform3D t;
        t = t.translated(Vector3(pointy_top_x_offset(diameter) / 2, 0, 0));
        _library->set_item_mesh_transform(id, t);
      }
    }
  }
}

void RidgeHexGridMap::calculate_final_heights() {
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(_tiles_layout[row][col]->mesh().ptr());

      mesh->calculate_final_heights();
    }
  }
}

}  // namespace sota
