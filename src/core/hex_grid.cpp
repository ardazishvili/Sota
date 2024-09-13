#include "core/hex_grid.h"

#include "core/hex_mesh.h"
#include "core/utils.h"
#include "cube_coordinates.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "godot_utils.h"
#include "hexagonal_utility.h"
#include "primitives/Hexagon.h"
#include "rectangular_utility.h"
#include "types.h"

namespace sota {

using namespace gd;

void HexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("init"), &HexGrid::init);

  // Properties
  ClassDB::bind_method(D_METHOD("get_divisions"), &HexGrid::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &HexGrid::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_diameter"), &HexGrid::get_diameter);
  ClassDB::bind_method(D_METHOD("set_diameter", "p_diameter"), &HexGrid::set_diameter);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "_diameter"), "set_diameter", "get_diameter");

  ClassDB::bind_method(D_METHOD("get_shader"), &HexGrid::get_shader);
  ClassDB::bind_method(D_METHOD("set_shader", "p_shader"), &HexGrid::set_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "_shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader",
               "get_shader");

  ClassDB::bind_method(D_METHOD("get_frame_state"), &HexGrid::get_frame_state);
  ClassDB::bind_method(D_METHOD("set_frame_state", "p_frame_state"), &HexGrid::set_frame_state);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_frame"), "set_frame_state", "get_frame_state");

  ClassDB::bind_method(D_METHOD("get_frame_offset"), &HexGrid::get_frame_offset);
  ClassDB::bind_method(D_METHOD("set_frame_offset", "p_frame_offset"), &HexGrid::set_frame_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "_frame_offset"), "set_frame_offset", "get_frame_offset");

  // API
  ClassDB::bind_method(D_METHOD("get_hex_meshes"), &HexGrid::get_hex_meshes);
}

void HexGrid::init() {
  init_col_row_layout();
  init_hexmesh();

  calculate_normals();
}

void HexGrid::set_divisions(const int p_divisions) {
  _divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

void HexGrid::set_diameter(const float p_diameter) {
  _diameter = p_diameter > 0 ? p_diameter : 0;
  init();
}

void HexGrid::set_shader(const Ref<Shader> p_shader) {
  _shader = p_shader;
  init();
}
void HexGrid::set_frame_state(const bool p_state) {
  _frame_state = p_state;
  init();
}

void HexGrid::set_frame_offset(const float p_offset) {
  _frame_offset = p_offset;
  init();
}

float HexGrid::get_diameter() const { return _diameter; }
int HexGrid::get_divisions() const { return _divisions; }
Ref<Shader> HexGrid::get_shader() const { return _shader; }
bool HexGrid::get_frame_state() const { return _frame_state; }
float HexGrid::get_frame_offset() const { return _frame_offset; }

void HexGrid::init_hexmesh() {
  clean_children(*this);
  _tiles_layout.clear();

  for (auto row : _col_row_layout) {
    _tiles_layout.push_back({});
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);

      Ref<ShaderMaterial> mat;
      mat.instantiate();
      if (_shader.ptr()) {
        mat->set_shader(_shader);
      }

      Vector3 offset = Vector3(0, 0, 0);
      offset.x = val.z * pointy_top_x_offset(_diameter);
      offset.x += is_odd(val.x) ? pointy_top_x_offset(_diameter) / 2 : 0;
      offset.z = val.x * pointy_top_y_offset(_diameter);

      HexMeshParams params{.id = id,
                           .diameter = _diameter,
                           .frame_state = _frame_state,
                           .frame_offset = _frame_offset,
                           .material = mat,
                           .divisions = _divisions,
                           .clip_options = ClipOptions{}};
      Hexagon hex = make_hexagon_at_position(offset, _diameter);
      Ref<HexMesh> m = make_hex_mesh(hex, params);

      _tiles_layout.back().push_back(
          make_non_ref<Tile>(m, offset, this, OffsetCoordinates{.row = val.x, .col = val.z}));
    }
  }
}

Array HexGrid::get_hex_meshes() {
  Array result;
  for (std::vector<Tile*>& row : _tiles_layout) {
    for (Tile* tile : row) {
      result.append(tile->mesh());
    }
  }
  return result;
}

// RectHexGrid definitions
void RectHexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &RectHexGrid::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &RectHexGrid::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &RectHexGrid::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &RectHexGrid::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");
}

void RectHexGrid::set_height(const int p_height) {
  _height = p_height > 1 ? p_height : 1;
  init();
}

void RectHexGrid::set_width(const int p_width) {
  _width = p_width > 1 ? p_width : 1;
  init();
}

int RectHexGrid::get_height() const { return _height; }
int RectHexGrid::get_width() const { return _width; }

void RectHexGrid::init_col_row_layout() {
  _col_row_layout = RectangularUtility::get_offset_coords_layout(_height, _width);
}

int RectHexGrid::calculate_id(int row, int col) const { return RectangularUtility::calculate_id(row, col, _width); }

// HexagonalHexGrid definitions
void HexagonalHexGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_size"), &HexagonalHexGrid::get_size);
  ClassDB::bind_method(D_METHOD("set_size", "p_size"), &HexagonalHexGrid::set_size);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "_size"), "set_size", "get_size");
}

void HexagonalHexGrid::set_size(const int p_size) {
  _size = p_size > 1 ? p_size : 1;
  init();
}

int HexagonalHexGrid::get_size() const { return _size; }

void HexagonalHexGrid::init_col_row_layout() { _col_row_layout = HexagonalUtility::get_offset_coords_layout(_size); }

int HexagonalHexGrid::calculate_id(int row, int col) const { return HexagonalUtility::calculate_id(row, col, _size); }

}  // namespace sota
