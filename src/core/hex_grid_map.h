#pragma once

#include <map>
#include <memory>

#include "core/hex_mesh.h"
#include "godot_cpp/classes/grid_map.hpp"
#include "godot_cpp/classes/mesh_library.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "misc/cube_coordinates.h"
#include "misc/tile.h"
#include "misc/types.h"

namespace sota {

using TilesLayout = std::vector<std::vector<std::unique_ptr<Tile>>>;

class HexGridMap : public gd::Node3D {
  GDCLASS(HexGridMap, gd::Node3D)

 public:
  HexGridMap() = default;

  void _ready() override;

  void set_shader(const gd::Ref<gd::Shader> p_shader);
  gd::Ref<gd::Shader> get_shader() const;

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_diameter(const float p_diameter);
  float get_diameter() const;

  void set_frame_state(const bool p_state);
  bool get_frame_state() const;

  void set_frame_offset(const float p_offset);
  float get_frame_offset() const;

  virtual int calculate_id(int row, int col) const = 0;

  virtual void calculate_normals() {}

 protected:
  float diameter{1};
  int divisions{3};
  gd::Ref<gd::Shader> shader;
  std::vector<std::vector<gd::Vector3i>> col_row_layout;
  TilesLayout _tiles_layout;

  static void _bind_methods();
  virtual void init();

  virtual void init_col_row_layout() = 0;
  virtual void init_hexmesh();
  std::map<CubeCoordinates, HexMesh*> _cube_to_hexagon;

  gd::Ref<gd::MeshLibrary> _library;

  bool frame_state{false};
  float frame_offset{0.0};

 private:
};

class RectHexGridMap : public HexGridMap {
  GDCLASS(RectHexGridMap, HexGridMap)
 public:
  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;

  void set_height(const int p_height);
  int get_height() const;

  void set_width(const int p_width);
  int get_width() const;

 protected:
  int height{0};
  int width{0};
  static void _bind_methods();
};

class HexagonalHexGridMap : public HexGridMap {
  GDCLASS(HexagonalHexGridMap, HexGridMap)
 public:
  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;

  void set_size(const int p_size);
  int get_size() const;

 protected:
  int size{0};
  static void _bind_methods();
};

}  // namespace sota
