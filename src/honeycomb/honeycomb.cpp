#include "honeycomb.h"

#include <algorithm>      // for sort, max, min
#include <cmath>          // for pow
#include <limits>         // for numeric_limits
#include <memory>         // for allocator_traits<>:...
#include <random>         // for mt19937, uniform_in...
#include <unordered_map>  // for unordered_map, unor...
#include <utility>        // for pair
#include <vector>         // for vector

#include "core/general_utility.h"      // for GeneralUtility
#include "core/godot_utils.h"          // for clean_children
#include "core/hex_grid.h"             // for TilesLayout
#include "core/hex_mesh.h"             // for HexMesh, HexMeshParams
#include "core/hexagonal_utility.h"    // for HexagonalUtility
#include "core/mesh.h"                 // for SotaMesh
#include "core/rectangular_utility.h"  // for RectangularUtility
#include "core/smooth_shades_processor.h"
#include "core/tile_mesh.h"             // for TileMesh
#include "core/utils.h"                 // for pointy_top_x_offset
#include "honeycomb/honeycomb_cell.h"   // for HoneycombCell, Hone...
#include "honeycomb/honeycomb_honey.h"  // for HoneycombHoney, Hon...
#include "misc/cube_coordinates.h"      // for OffsetCoordinates
#include "misc/tile.h"                  // for HoneycombTile, Tile
#include "misc/types.h"                 // for GroupedMeshVertices
#include "primitives/hexagon.h"         // for make_hexagon_at_pos...
#include "tal/arrays.h"                 // for Array
#include "tal/callable.h"               // for Callable
#include "tal/godot_core.h"             // for D_METHOD, ClassDB
#include "tal/material.h"               // for ShaderMaterial
#include "tal/noise.h"                  // for FastNoiseLite
#include "tal/reference.h"              // for Ref
#include "tal/shader.h"                 // for Shader
#include "tal/texture.h"                // for Texture
#include "tal/vector2.h"                // for Vector2
#include "tal/vector3.h"                // for Vector3
#include "tal/vector3i.h"               // for Vector3i

namespace sota {

void Honeycomb::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_smooth_normals"), &Honeycomb::get_smooth_normals);
  ClassDB::bind_method(D_METHOD("set_smooth_normals", "p_smooth_normals"), &Honeycomb::set_smooth_normals);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "_smooth_normals"), "set_smooth_normals", "get_smooth_normals");

  ADD_GROUP("Honey", "honey");
  ClassDB::bind_method(D_METHOD("get_honey_random_level"), &Honeycomb::get_honey_random_level);
  ClassDB::bind_method(D_METHOD("set_honey_random_level", "p_honey_random_level"), &Honeycomb::set_honey_random_level);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "_honey_random_level"), "set_honey_random_level", "get_honey_random_level");

  ClassDB::bind_method(D_METHOD("get_honey_min_offset"), &Honeycomb::get_honey_min_offset);
  ClassDB::bind_method(D_METHOD("set_honey_min_offset", "p_honey_min_offset"), &Honeycomb::set_honey_min_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "_honey_min_offset"), "set_honey_min_offset", "get_honey_min_offset");

  ClassDB::bind_method(D_METHOD("get_honey_max_gain"), &Honeycomb::get_honey_max_gain);
  ClassDB::bind_method(D_METHOD("set_honey_max_gain", "p_honey_max_gain"), &Honeycomb::set_honey_max_gain);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "_honey_max_gain"), "set_honey_max_gain", "get_honey_max_gain");

  ClassDB::bind_method(D_METHOD("get_honey_fill_steps"), &Honeycomb::get_honey_fill_steps);
  ClassDB::bind_method(D_METHOD("set_honey_fill_steps", "p_honey_fill_steps"), &Honeycomb::set_honey_fill_steps);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_honey_fill_steps"), "set_honey_fill_steps", "get_honey_fill_steps");

  ClassDB::bind_method(D_METHOD("get_cell_texture"), &Honeycomb::get_cell_texture);
  ClassDB::bind_method(D_METHOD("set_cell_texture", "p_cell_texture"), &Honeycomb::set_cell_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_cell_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_cell_texture", "get_cell_texture");

  ClassDB::bind_method(D_METHOD("get_honey_texture"), &Honeycomb::get_honey_texture);
  ClassDB::bind_method(D_METHOD("set_honey_texture", "p_honey_texture"), &Honeycomb::set_honey_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_honey_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_honey_texture", "get_honey_texture");

  ClassDB::bind_method(D_METHOD("get_selection_texture"), &Honeycomb::get_selection_texture);
  ClassDB::bind_method(D_METHOD("set_selection_texture", "p_selection_texture"), &Honeycomb::set_selection_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_selection_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_selection_texture", "get_selection_texture");

  ClassDB::bind_method(D_METHOD("get_noise"), &Honeycomb::get_noise);
  ClassDB::bind_method(D_METHOD("set_noise", "p_noise"), &Honeycomb::set_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_noise_honey", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_noise",
               "get_noise");

  ClassDB::bind_method(D_METHOD("get_bottom_offset"), &Honeycomb::get_bottom_offset);
  ClassDB::bind_method(D_METHOD("set_bottom_offset", "p_bottom_offset"), &Honeycomb::set_bottom_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "_bottom_offset"), "set_bottom_offset", "get_bottom_offset");

  ClassDB::bind_method(D_METHOD("get_honey_shader"), &Honeycomb::get_honey_shader);
  ClassDB::bind_method(D_METHOD("set_honey_shader", "p_honey_shader"), &Honeycomb::set_honey_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_honey_shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"),
               "set_honey_shader", "get_honey_shader");

  ClassDB::bind_method(D_METHOD("get_selection_shader"), &Honeycomb::get_selection_shader);
  ClassDB::bind_method(D_METHOD("set_selection_shader", "p_selection_shader"), &Honeycomb::set_selection_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_selection_shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"),
               "set_selection_shader", "get_selection_shader");

  // Interface
  ClassDB::bind_method(D_METHOD("get_cells"), &Honeycomb::get_cells);
  ClassDB::bind_method(D_METHOD("get_min_cells"), &Honeycomb::get_min_cells);
  ClassDB::bind_method(D_METHOD("get_max_cells"), &Honeycomb::get_max_cells);
  ClassDB::bind_method(D_METHOD("all_cells_empty"), &Honeycomb::all_cells_empty);
}

void Honeycomb::init() {
  init_col_row_layout();
  if (_col_row_layout.empty()) {
    return;
  }
  init_hexmesh();

  calculate_cells();

  prepare_heights_calculation();
  calculate_final_heights();

  calculate_normals();
}

void Honeycomb::set_smooth_normals(const bool p_smooth_normals) {
  _smooth_normals = p_smooth_normals;
  calculate_normals();
}

void Honeycomb::set_honey_random_level(const bool p_honey_random_level) {
  _honey_random_level = p_honey_random_level;
  init();
}

void Honeycomb::set_honey_min_offset(float p_honey_min_offset) {
  _honey_min_offset = p_honey_min_offset;
  init();
}

void Honeycomb::set_honey_max_gain(float p_honey_max_gain) {
  _honey_max_gain = p_honey_max_gain;
  init();
}

void Honeycomb::set_honey_fill_steps(int p_honey_fill_steps) {
  _honey_fill_steps = p_honey_fill_steps;
  init();
}

void Honeycomb::set_noise(const Ref<FastNoiseLite> p_noise) {
  _noise = p_noise;
  if (_noise.ptr()) {
    _noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void Honeycomb::set_cell_texture(const Ref<Texture> p_cell_texture) {
  _cell_texture = p_cell_texture;
  init();
}

void Honeycomb::set_honey_texture(const Ref<Texture> p_honey_texture) {
  _honey_texture = p_honey_texture;
  init();
}

void Honeycomb::set_selection_texture(const Ref<Texture> p_selection_texture) {
  _selection_texture = p_selection_texture;
  init();
}

void Honeycomb::set_bottom_offset(float p_bottom_offset) {
  _bottom_offset = p_bottom_offset;
  init();
}

void Honeycomb::set_honey_shader(const Ref<Shader> p_honey_shader) {
  _honey_shader = p_honey_shader;
  init();
}

void Honeycomb::set_selection_shader(const Ref<Shader> p_selection_shader) {
  _selection_shader = p_selection_shader;
  init();
}

bool Honeycomb::get_smooth_normals() const { return _smooth_normals; }
Ref<FastNoiseLite> Honeycomb::get_noise() const { return _noise; }
Ref<Texture> Honeycomb::get_cell_texture() const { return _cell_texture; }
Ref<Texture> Honeycomb::get_honey_texture() const { return _honey_texture; }
Ref<Texture> Honeycomb::get_selection_texture() const { return _selection_texture; }
float Honeycomb::get_bottom_offset() const { return _bottom_offset; }
Ref<Shader> Honeycomb::get_honey_shader() const { return _honey_shader; }
Ref<Shader> Honeycomb::get_selection_shader() const { return _selection_shader; }
bool Honeycomb::get_honey_random_level() const { return _honey_random_level; }
float Honeycomb::get_honey_min_offset() const { return _honey_min_offset; }
float Honeycomb::get_honey_max_gain() const { return _honey_max_gain; }
int Honeycomb::get_honey_fill_steps() const { return _honey_fill_steps; }

// TODO use global seed
int seed = 1;
std::mt19937 gen(seed);

int Honeycomb::generate_min_honey_step() { return 0; }

int Honeycomb::generate_random_honey_step() {
  std::uniform_int_distribution<> dist(0, _honey_fill_steps);
  return dist(gen);
}

float Honeycomb::get_honey_step_value() {
  float max_y_offset = _honey_min_offset + _honey_max_gain;
  return (max_y_offset - _honey_min_offset) / _honey_fill_steps;
}

float Honeycomb::generate_offset(int step) { return _honey_min_offset + step * get_honey_step_value(); }

float Honeycomb::generate_random_honey_y_offset() { return generate_offset(generate_random_honey_step()); }

float Honeycomb::generate_min_honey_y_offset() { return generate_offset(generate_min_honey_step()); }

Array Honeycomb::get_cells_by_order(SortingOrder order, std::function<bool(HoneycombHoney*)> pred) const {
  std::vector<std::pair<int, HoneycombHoney*>> all_cells;
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* cell = tile->honey_mesh().ptr();
      if (pred(cell)) {
        all_cells.emplace_back(cell->get_level(), cell);
      }
    }
  }
  sort(order, all_cells);

  Array processed_cells;
  for (auto [level, cell] : all_cells) {
    if (level != all_cells.begin()->first) {
      break;
    }
    processed_cells.push_back(cell);
  }
  return processed_cells;
}

Array Honeycomb::get_cells() const {
  Array res;
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* cell = tile->honey_mesh().ptr();
      res.append(Ref<HoneycombHoney>(cell));
    }
  }
  return res;
}

Array Honeycomb::get_min_cells() const {
  return get_cells_by_order(SortingOrder::INCREASING, [](HoneycombHoney* cell) { return !cell->is_locked(); });
}

Array Honeycomb::get_max_cells() const {
  return get_cells_by_order(SortingOrder::DECREASING, [](HoneycombHoney* cell) {
    /* return !cell->is_empty(); */
    // commented above because filtering for non-empty cells lead to problem with return of nullable object. Nullable
    // objects are not yet supported in GDScript. Client code should check emptyness using HoneycombHoney::is_empty()
    // method
    return !cell->is_locked();
  });
}

bool Honeycomb::all_cells_empty() const {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* cell = tile->honey_mesh().ptr();
      if (!cell->is_empty()) {
        return false;
      }
    }
  }
  return true;
}

void Honeycomb::init_hexmesh() {
  std::unordered_map<int, Vector3> cells_offsets;
  std::unordered_map<int, Vector2> honey_offsets;
  for (auto row : _col_row_layout) {
    for (auto val : row) {
      auto x_offset = val.z * pointy_top_x_offset(_diameter);
      x_offset += is_odd(val.x) ? pointy_top_x_offset(_diameter) / 2 : 0;

      int id = calculate_id(val.x, val.z);
      auto z_offset = val.x * pointy_top_y_offset(_diameter);
      cells_offsets[id] = Vector3(x_offset, 0, z_offset);

      honey_offsets[id + calculate_honey_id_offset()] = Vector2(x_offset, z_offset);
    }
  }

  _tiles_layout.clear();
  clean_children(*this);

  for (auto row : _col_row_layout) {
    _tiles_layout.push_back({});
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);

      Ref<ShaderMaterial> cell_material;
      cell_material.instantiate();
      if (_shader.ptr()) {
        cell_material->set_shader(_shader);
      }
      if (_cell_texture.ptr()) {
        cell_material->set_shader_parameter("cell_texture", _cell_texture.ptr());
      }

      Ref<ShaderMaterial> cell_selection_material;
      cell_selection_material.instantiate();
      if (_selection_shader.ptr()) {
        cell_selection_material->set_shader(_selection_shader);
      }
      if (_selection_texture.ptr()) {
        cell_selection_material->set_shader_parameter("cell_texture", _selection_texture.ptr());
      }
      Hexagon cell_hex = make_hexagon_at_position(cells_offsets[id], _diameter);
      HoneycombCellMeshParams cell_params{.hex_mesh_params = HexMeshParams{.id = id,
                                                                           .diameter = _diameter,
                                                                           .frame_state = _frame_state,
                                                                           .frame_offset = _frame_offset,
                                                                           .material = cell_material,
                                                                           .divisions = _divisions,
                                                                           .clip_options = ClipOptions{}},
                                          .noise = _noise,
                                          .selection_material = cell_selection_material};

      Ref<ShaderMaterial> honey_material;
      honey_material.instantiate();
      if (_honey_shader.ptr()) {
        honey_material->set_shader(_honey_shader);
      }
      if (_honey_texture.ptr()) {
        honey_material->set_shader_parameter("honey_texture", _honey_texture.ptr());
      }

      int honey_level = _honey_random_level ? generate_random_honey_step() : generate_min_honey_step();
      Vector2 xz = honey_offsets[id + calculate_honey_id_offset()];
      float y = generate_offset(honey_level);
      Vector3 honey_offset(xz.x, y, xz.y);
      Hexagon honey_hex = make_hexagon_at_position(honey_offset, _diameter);

      HoneycombHoneyMeshParams honey_params{.hex_mesh_params = HexMeshParams{.id = id + calculate_honey_id_offset(),
                                                                             .diameter = _diameter,
                                                                             .frame_state = false,
                                                                             .frame_offset = 0.0,
                                                                             .material = honey_material,
                                                                             .divisions = _divisions,
                                                                             .clip_options = ClipOptions{}},
                                            .noise = _noise,
                                            .max_level = _honey_fill_steps,
                                            .fill_delta = get_honey_step_value(),
                                            .min_offset = _honey_min_offset};

      Ref<HoneycombCell> cell_tile = Ref<HoneycombCell>(memnew(HoneycombCell(cell_hex, cell_params)));
      Ref<HoneycombHoney> honey_tile = Ref<HoneycombHoney>(memnew(HoneycombHoney(honey_hex, honey_params)));
      HoneycombTile* t =
          memnew(HoneycombTile(cell_tile, honey_tile, this, OffsetCoordinates{.row = val.x, .col = val.z}));
      _tiles_layout.back().push_back(t);
    }
  }
}

void Honeycomb::calculate_cells() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombCell* cell_mesh = dynamic_cast<HoneycombCell*>(tile_ptr->mesh().ptr());

      cell_mesh->calculate_heights(_bottom_offset);
      cell_mesh->inner_mesh()->calculate_normals();
      cell_mesh->inner_mesh()->update();
    }
  }
}

std::pair<std::vector<TileMesh*>, std::vector<TileMesh*>> Honeycomb::meshes() {
  std::pair<std::vector<TileMesh*>, std::vector<TileMesh*>> res;
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      res.first.push_back(tile_ptr->mesh().ptr());
      res.second.push_back(dynamic_cast<HoneycombTile*>(tile_ptr)->honey_mesh().ptr());
    }
  }
  return res;
}

void Honeycomb::calculate_normals() {
  auto [cell_meshes, honey_meshes] = meshes();
  SmoothShadesProcessor(cell_meshes).calculate_normals(_smooth_normals);
  SmoothShadesProcessor(honey_meshes).calculate_normals(_smooth_normals);
}

void Honeycomb::prepare_heights_calculation() {
  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* honey_mesh = tile->honey_mesh().ptr();

      honey_mesh->calculate_initial_heights();
      auto [mesh_min_z, mesh_max_z] = honey_mesh->get_min_max_height();
      global_min_y = std::min(global_min_y, mesh_min_z);
      global_max_y = std::max(global_max_y, mesh_max_z);
    }
  }

  float amplitude = global_max_y - global_min_y;
  float compression_factor = 0.1 / amplitude;

  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* honey_mesh = tile->honey_mesh().ptr();

      honey_mesh->set_shift_compress(-global_min_y, compression_factor);
    }
  }
}

void Honeycomb::calculate_final_heights() {
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
      HoneycombHoney* honey_mesh = tile->honey_mesh().ptr();

      honey_mesh->calculate_heights();
      honey_mesh->inner_mesh()->calculate_normals();
    }
  }
}

// RectHoneycomb definitions
void RectHoneycomb::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &RectHoneycomb::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &RectHoneycomb::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &RectHoneycomb::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &RectHoneycomb::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_width"), "set_width", "get_width");
}

void RectHoneycomb::set_height(const int p_height) {
  _height = p_height > 1 ? p_height : 1;
  init();
}

void RectHoneycomb::set_width(const int p_width) {
  _width = p_width > 1 ? p_width : 1;
  init();
}

int RectHoneycomb::get_height() const { return _height; }
int RectHoneycomb::get_width() const { return _width; }

void RectHoneycomb::init_col_row_layout() {
  _col_row_layout = RectangularUtility::get_offset_coords_layout(_height, _width);
}

int RectHoneycomb::calculate_id(int row, int col) const { return RectangularUtility::calculate_id(row, col, _width); }

int RectHoneycomb::calculate_honey_id_offset() { return _height * _width; }

// HexagonalHoneycomb definitions
void HexagonalHoneycomb::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_size"), &HexagonalHoneycomb::get_size);
  ClassDB::bind_method(D_METHOD("set_size", "p_size"), &HexagonalHoneycomb::set_size);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_size"), "set_size", "get_size");

  ClassDB::bind_method(D_METHOD("get_center"), &HexagonalHoneycomb::get_center);
}

Vector3 HexagonalHoneycomb::get_center() const {
  int total = 0;
  for (auto& row : _tiles_layout) {
    total += row.size();
  }
  int middle = total / 2;
  int i = 0;
  for (auto& row : _tiles_layout) {
    for (auto& tile_ptr : row) {
      if (i == middle) {
        HoneycombTile* tile = dynamic_cast<HoneycombTile*>(tile_ptr);
        HoneycombHoney* cell = tile->honey_mesh().ptr();
        return cell->inner_mesh()->get_center() + Vector3(0, 0, radius(_diameter));  // + upper half of 0-row
      }
      ++i;
    }
  }
  printerr("Unreachable: error in HexagonalHoneycomb::get_center calculation");
  return Vector3(0, 0, 0);
}

void HexagonalHoneycomb::set_size(const int p_size) {
  _size = p_size > 1 ? p_size : 1;
  init();
}

int HexagonalHoneycomb::get_size() const { return _size; }

void HexagonalHoneycomb::init_col_row_layout() { _col_row_layout = HexagonalUtility::get_offset_coords_layout(_size); }

int HexagonalHoneycomb::calculate_id(int row, int col) const { return HexagonalUtility::calculate_id(row, col, _size); }

int HexagonalHoneycomb::calculate_honey_id_offset() { return std::pow(2 * _size + 1, 2); }

}  // namespace sota
