#include "ridge_hex_grid_map.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <unordered_map>

#include "algo/dsu.h"
#include "biome_calculator.h"
#include "core/hex_mesh.h"
#include "core/utils.h"
#include "cube_coordinates.h"
#include "general_utility.h"
#include "godot_cpp/classes/grid_map.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/variant/plane.hpp"
#include "godot_cpp/variant/transform3d.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "hexagonal_utility.h"
#include "misc/tile.h"
#include "misc/types.h"
#include "misc/utilities.h"
#include "rectangular_utility.h"
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

  init_col_row_layout();
  if (col_row_layout.empty()) {
    return;
  }
  init_hexmesh();
  init_mesh_lib();
  init_grid();

  assign_cube_coordinates_map();
  init_biomes();

  /* print_biomes(); */

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
  std::unordered_map<int, Vector3> offsets;
  for (auto row : col_row_layout) {
    for (auto val : row) {
      auto x_offset = val.z * pointy_top_x_offset(diameter);
      x_offset += is_odd(val.x) ? pointy_top_x_offset(diameter) / 2 : 0;

      auto z_offset = val.x * pointy_top_y_offset(diameter);
      offsets[calculate_id(val.x, val.z)] = Vector3(x_offset, 0, z_offset);
    }
  }

  std::unordered_map<int, float> altitudes;
  for (auto row : col_row_layout) {
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);
      Vector3 o = offsets[id];
      if (biomes_noise.ptr()) {
        altitudes[id] = biomes_noise->get_noise_2d(o.x, o.z);
      } else {
        altitudes[id] = 0;
      }
    }
  }

  float min_z = std::numeric_limits<float>::max();
  float max_z = std::numeric_limits<float>::min();
  for (auto [id, a] : altitudes) {
    min_z = std::min(min_z, a);
    max_z = std::max(max_z, a);
  }

  _tiles_layout.clear();
  TypedArray<Node> children = get_children();
  for (int i = 0; i < children.size(); ++i) {
    Node* child = Object::cast_to<Node>(children[i].operator Object*());
    remove_child(child);
    child->queue_free();
  }
  BiomeCalculator biome_calculator;
  for (auto row : col_row_layout) {
    _tiles_layout.push_back({});
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);
      Biome biome = biome_calculator.calculate_biome(min_z, max_z, altitudes[id]);

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

      // TODO frame behaviour for RidgeHexGridMap and sub-classes is not clear. Address it in follow-up issue/PR
      m->set_frame_state(false);
      m->set_frame_value(0.0);

      m->set_divisions(divisions);
      m->set_offset(offsets[id]);
      m->set_material(mat);

      ClipOptions clip = get_clip_options(val.x, val.z);
      m->init(clip.left, clip.right, clip.up, clip.down);
      _tiles_layout.back().push_back(
          std::make_unique<BiomeTile>(m, this, biome, OffsetCoordinates{.row = val.x, .col = val.z}));
    }
  }
}

void RidgeHexGridMap::init_mesh_lib() {
  Ref<MeshLibrary> m = get_mesh_library();
  m.instantiate();
  _library = m;
  set_mesh_library(m);

  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      int id = tile_ptr->id();
      _library->create_item(id);
      _library->set_item_mesh(id, tile_ptr->mesh());
      if (tile_ptr->is_shifted()) {
        Transform3D t;
        t = t.translated(Vector3(pointy_top_x_offset(diameter) / 2, 0, 0));
        _library->set_item_mesh_transform(id, t);
      }
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
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      tile_ptr->mesh()->calculate_normals();
    }
  }
}

void RidgeHexGridMap::meshes_update() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      tile_ptr->mesh()->update();
    }
  }
}

void RidgeHexGridMap::calculate_smooth_normals() {
  std::vector<GroupedHexagonMeshVertices> vertex_groups;
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile_ptr->mesh().ptr());
      vertex_groups.push_back(mesh->get_grouped_vertices());
    }
  }

  GeneralUtility::make_smooth_normals(vertex_groups);
}

void RidgeHexGridMap::init_biomes() {
  _mountain_groups.clear();
  _water_groups.clear();
  _plain_groups.clear();
  _hill_groups.clear();
  BiomeGroups mountain_groups = collect_biome_groups(Biome::MOUNTAIN);
  for (auto group : mountain_groups) {
    _mountain_groups.emplace_back(group, std::make_unique<RidgeSet>(ridge_config));
  }

  _plain_groups = collect_biome_groups(Biome::PLAIN);
  _hill_groups = collect_biome_groups(Biome::HILL);

  BiomeGroups water_groups = collect_biome_groups(Biome::WATER);
  for (auto group : water_groups) {
    _water_groups.emplace_back(group, std::make_unique<RidgeSet>(ridge_config));
  }
}

void RidgeHexGridMap::calculate_neighbours(GroupOfHexagonMeshes& group) {
  auto member_of_group = [&group](HexMesh& mesh) {
    return std::find(group.begin(), group.end(), &mesh) != group.end();
  };
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr.get());
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
      if (std::find(group.begin(), group.end(), mesh) == group.end()) {
        continue;
      }
      CubeCoordinates cube_cur = tile->get_cube_coords();
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
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr.get());
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
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile_ptr->mesh().ptr());
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
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr.get());
      _cube_to_hexagon[tile->get_cube_coords()] = tile_ptr->mesh().ptr();
    }
  }
}

void RidgeHexGridMap::calculate_final_heights() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile_ptr->mesh().ptr());

      mesh->calculate_final_heights();
      mesh->calculate_normals();
      mesh->update();
    }
  }
}

void RidgeHexGridMap::print_biomes() {
  for (auto& [group, ridge_set] : _mountain_groups) {
    std::cout << "mountain group of size " << group.size() << std::endl;
  }
  for (auto& [group, ridge_set] : _water_groups) {
    std::cout << "water group of size " << group.size() << std::endl;
  }
  for (auto& group : _hill_groups) {
    std::cout << "hill group of size " << group.size() << std::endl;
  }
  for (auto& group : _plain_groups) {
    std::cout << "plain group of size " << group.size() << std::endl;
  }
}

// RectRidgeHexGridMap definitions
void RectRidgeHexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &RectRidgeHexGridMap::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &RectRidgeHexGridMap::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &RectRidgeHexGridMap::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &RectRidgeHexGridMap::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");

  ClassDB::bind_method(D_METHOD("get_clipped_option"), &RectRidgeHexGridMap::get_clipped_option);
  ClassDB::bind_method(D_METHOD("set_clipped_option", "p_clipped_option"), &RectRidgeHexGridMap::set_clipped_option);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "clipped"), "set_clipped_option", "get_clipped_option");
}

void RectRidgeHexGridMap::set_height(const int p_height) {
  height = p_height > 1 ? p_height : 1;
  init();
}

void RectRidgeHexGridMap::set_width(const int p_width) {
  width = p_width > 1 ? p_width : 1;
  init();
}

void RectRidgeHexGridMap::set_clipped_option(const bool p_clipped_option) {
  clipped = p_clipped_option;
  init();
}

int RectRidgeHexGridMap::get_height() const { return height; }
int RectRidgeHexGridMap::get_width() const { return width; }
bool RectRidgeHexGridMap::get_clipped_option() const { return clipped; }

void RectRidgeHexGridMap::init_col_row_layout() {
  col_row_layout = RectangularUtility::get_offset_coords_layout(height, width);
}

int RectRidgeHexGridMap::calculate_id(int row, int col) const {
  return RectangularUtility::calculate_id(row, col, width);
}

BiomeGroups RectRidgeHexGridMap::collect_biome_groups(Biome b) {
  int height = col_row_layout.size();
  int width = col_row_layout[0].size();
  algo::DSU<RidgeHexMesh*> u(height, width);

  auto flat = [width](int i, int j) { return i * width + j; };
  for (auto row : col_row_layout) {
    for (Vector3i v : row) {
      int i = v.x;
      int j = v.z;
      BiomeTile* tile = dynamic_cast<BiomeTile*>(_tiles_layout[i][j].get());
      Biome biome = tile->biome();
      if (biome != b) {
        continue;
      }
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
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

ClipOptions RectRidgeHexGridMap::get_clip_options(int row, int col) const {
  if (!clipped) {
    return {.left = false, .right = false, .up = false, .down = false};
  }
  return {.left = col == 0 && !is_odd(row),
          .right = col == (width - 1) && is_odd(row),
          .up = row == (height - 1),
          .down = row == 0};
}

// HexagonalRidgeHexGridMap definitions
void HexagonalRidgeHexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_size"), &HexagonalRidgeHexGridMap::get_size);
  ClassDB::bind_method(D_METHOD("set_size", "p_size"), &HexagonalRidgeHexGridMap::set_size);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "set_size", "get_size");
}

void HexagonalRidgeHexGridMap::set_size(const int p_size) {
  size = p_size > 1 ? p_size : 1;
  init();
}

int HexagonalRidgeHexGridMap::get_size() const { return size; }

void HexagonalRidgeHexGridMap::init_col_row_layout() {
  col_row_layout = HexagonalUtility::get_offset_coords_layout(size);
}

int HexagonalRidgeHexGridMap::calculate_id(int row, int col) const {
  return HexagonalUtility::calculate_id(row, col, size);
}

BiomeGroups HexagonalRidgeHexGridMap::collect_biome_groups(Biome b) {
  int width = size * 2 + 1;
  algo::DSU<RidgeHexMesh*> u(width, width);

  auto flat = [width](int i, int j) { return i * width + j; };
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      BiomeTile* tile = dynamic_cast<BiomeTile*>(tile_ptr.get());
      Biome biome = tile->biome();
      if (biome != b) {
        continue;
      }
      auto offset_coords = tile->get_offset_coords();
      int i = offset_coords.row;
      int j = offset_coords.col;
      RidgeHexMesh* mesh = dynamic_cast<RidgeHexMesh*>(tile->mesh().ptr());
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

ClipOptions HexagonalRidgeHexGridMap::get_clip_options(int row, int col) const {
  return {.left = false, .right = false, .up = false, .down = false};
}

}  // namespace sota
