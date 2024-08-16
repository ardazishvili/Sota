#include "core/hex_grid_map.h"

#include <memory>

#include "core/hex_mesh.h"
#include "core/utils.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"

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

  ClassDB::bind_method(D_METHOD("get_height"), &HexGridMap::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &HexGridMap::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");

  ClassDB::bind_method(D_METHOD("get_width"), &HexGridMap::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &HexGridMap::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");

  ClassDB::bind_method(D_METHOD("get_shader"), &HexGridMap::get_shader);
  ClassDB::bind_method(D_METHOD("set_shader", "p_shader"), &HexGridMap::set_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader",
               "get_shader");
}

void HexGridMap::init() {
  set_cell_size(Vector3(pointy_top_x_offset(diameter), 1.0, pointy_top_y_offset(diameter)));

  init_hexmesh();
  init_mesh_lib();
  init_grid();

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

void HexGridMap::set_height(const int p_height) {
  height = p_height > 1 ? p_height : 1;
  init();
}

void HexGridMap::set_width(const int p_width) {
  width = p_width > 1 ? p_width : 1;
  init();
}

void HexGridMap::set_shader(const Ref<Shader> p_shader) {
  shader = p_shader;
  init();
}

float HexGridMap::get_diameter() const { return diameter; }
int HexGridMap::get_divisions() const { return divisions; }
int HexGridMap::get_height() const { return height; }
int HexGridMap::get_width() const { return width; }
Ref<Shader> HexGridMap::get_shader() const { return shader; }

int HexGridMap::calculate_id(int row, int col) const { return row * width + col; }

void HexGridMap::init_hexmesh() {
  _tiles_layout.clear();
  for (int row = 0; row < height; ++row) {
    _tiles_layout.push_back({});
    for (int col = 0; col < width; ++col) {
      int id = calculate_id(row, col);

      Ref<ShaderMaterial> mat;
      mat.instantiate();
      if (shader.ptr()) {
        mat->set_shader(shader);
      }

      Ref<HexMesh> m = Ref(memnew(HexMesh()));
      m->set_id(id);
      m->set_diameter(diameter);
      m->set_divisions(divisions);
      m->set_material(mat);
      m->init();
      _tiles_layout.back().push_back(std::make_unique<Tile>(m));
    }
  }
}

void HexGridMap::init_mesh_lib() {
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

void HexGridMap::init_grid() {
  clear();
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int id = calculate_id(row, col);
      set_cell_item(Vector3i(col, 0, row), id);
    }
  }
}

void HexGridMap::_ready() { init(); }

}  // namespace sota
