#include "core/hex_grid_map.h"

#include <memory>

#include "core/hex_mesh.h"
#include "core/utils.h"
#include "cube_coordinates.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "hexagonal_utility.h"
#include "primitives/Hexagon.h"
#include "rectangular_utility.h"
#include "types.h"

namespace sota {

using namespace gd;

void HexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("init"), &HexGridMap::init);

  ClassDB::bind_method(D_METHOD("get_divisions"), &HexGridMap::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &HexGridMap::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_diameter"), &HexGridMap::get_diameter);
  ClassDB::bind_method(D_METHOD("set_diameter", "p_diameter"), &HexGridMap::set_diameter);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "diameter"), "set_diameter", "get_diameter");

  ClassDB::bind_method(D_METHOD("get_shader"), &HexGridMap::get_shader);
  ClassDB::bind_method(D_METHOD("set_shader", "p_shader"), &HexGridMap::set_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader",
               "get_shader");

  ClassDB::bind_method(D_METHOD("get_frame_state"), &HexGridMap::get_frame_state);
  ClassDB::bind_method(D_METHOD("set_frame_state", "p_frame_state"), &HexGridMap::set_frame_state);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_frame"), "set_frame_state", "get_frame_state");

  ClassDB::bind_method(D_METHOD("get_frame_offset"), &HexGridMap::get_frame_offset);
  ClassDB::bind_method(D_METHOD("set_frame_offset", "p_frame_offset"), &HexGridMap::set_frame_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frame_offset"), "set_frame_offset", "get_frame_offset");
}

void HexGridMap::init() {
  init_col_row_layout();
  init_hexmesh();

  calculate_normals();
}

void HexGridMap::set_divisions(const int p_divisions) {
  divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

void HexGridMap::set_diameter(const float p_diameter) {
  diameter = p_diameter > 0 ? p_diameter : 0;
  init();
}

void HexGridMap::set_shader(const Ref<Shader> p_shader) {
  shader = p_shader;
  init();
}
void HexGridMap::set_frame_state(const bool p_state) {
  frame_state = p_state;
  init();
}

void HexGridMap::set_frame_offset(const float p_offset) {
  frame_offset = p_offset;
  init();
}

float HexGridMap::get_diameter() const { return diameter; }
int HexGridMap::get_divisions() const { return divisions; }
Ref<Shader> HexGridMap::get_shader() const { return shader; }
bool HexGridMap::get_frame_state() const { return frame_state; }
float HexGridMap::get_frame_offset() const { return frame_offset; }

void HexGridMap::init_hexmesh() {
  _tiles_layout.clear();
  TypedArray<Node> children = get_children();
  for (int i = 0; i < children.size(); ++i) {
    Node* child = Object::cast_to<Node>(children[i].operator Object*());
    remove_child(child);
    child->queue_free();
  }

  for (auto row : col_row_layout) {
    _tiles_layout.push_back({});
    for (auto val : row) {
      int id = calculate_id(val.x, val.z);

      Ref<ShaderMaterial> mat;
      mat.instantiate();
      if (shader.ptr()) {
        mat->set_shader(shader);
      }

      Vector3 offset = Vector3(0, 0, 0);
      offset.x = val.z * pointy_top_x_offset(diameter);
      offset.x += is_odd(val.x) ? pointy_top_x_offset(diameter) / 2 : 0;
      offset.z = val.x * pointy_top_y_offset(diameter);

      HexMeshParams params{.id = id,
                           .diameter = diameter,
                           .frame_state = frame_state,
                           .frame_offset = frame_offset,
                           .material = mat,
                           .divisions = divisions,
                           .clip_options = ClipOptions{}};
      Hexagon hex = make_hexagon_at_position(offset, diameter);
      Ref<HexMesh> m = make_hex_mesh(hex, params);

      _tiles_layout.back().push_back(
          std::make_unique<Tile>(m, offset, this, OffsetCoordinates{.row = val.x, .col = val.z}));
    }
  }
}

void HexGridMap::_ready() { init(); }

// RectHexGridMap definitions
void RectHexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &RectHexGridMap::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &RectHexGridMap::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &RectHexGridMap::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &RectHexGridMap::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");
}

void RectHexGridMap::set_height(const int p_height) {
  height = p_height > 1 ? p_height : 1;
  init();
}

void RectHexGridMap::set_width(const int p_width) {
  width = p_width > 1 ? p_width : 1;
  init();
}

int RectHexGridMap::get_height() const { return height; }
int RectHexGridMap::get_width() const { return width; }

void RectHexGridMap::init_col_row_layout() {
  col_row_layout = RectangularUtility::get_offset_coords_layout(height, width);
}

int RectHexGridMap::calculate_id(int row, int col) const { return RectangularUtility::calculate_id(row, col, width); }

// HexagonalHexGridMap definitions
void HexagonalHexGridMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_size"), &HexagonalHexGridMap::get_size);
  ClassDB::bind_method(D_METHOD("set_size", "p_size"), &HexagonalHexGridMap::set_size);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "set_size", "get_size");
}

void HexagonalHexGridMap::set_size(const int p_size) {
  size = p_size > 1 ? p_size : 1;
  init();
}

int HexagonalHexGridMap::get_size() const { return size; }

void HexagonalHexGridMap::init_col_row_layout() { col_row_layout = HexagonalUtility::get_offset_coords_layout(size); }

int HexagonalHexGridMap::calculate_id(int row, int col) const { return HexagonalUtility::calculate_id(row, col, size); }

}  // namespace sota
