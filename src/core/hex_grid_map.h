#pragma once

#include <map>
#include <memory>

#include "core/hex_mesh.h"
#include "godot_cpp/classes/grid_map.hpp"
#include "godot_cpp/classes/mesh_library.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "misc/cube_coordinates.h"
#include "misc/tile.h"
#include "misc/types.h"

namespace sota {

using TilesLayout = std::vector<std::vector<std::unique_ptr<Tile>>>;

class HexGridMap : public gd::GridMap {
  GDCLASS(HexGridMap, gd::GridMap)

 public:
  HexGridMap() = default;
  ~HexGridMap() = default;

  void _ready() override;

  void set_height(const int p_height);
  int get_height() const;

  void set_width(const int p_width);
  int get_width() const;

  void set_shader(const gd::Ref<gd::Shader> p_shader);
  gd::Ref<gd::Shader> get_shader() const;

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_diameter(const float p_diameter);
  float get_diameter() const;

  int calculate_id(int row, int col) const;

  virtual void calculate_normals() {}

 protected:
  int height{1};
  int width{1};
  float diameter{1};
  int divisions{3};
  gd::Ref<gd::Shader> shader;
  TilesLayout _tiles_layout;

  static void _bind_methods();
  virtual void init();

  void init_grid();
  virtual void init_hexmesh();
  virtual void init_mesh_lib();
  // TODO use HexMesh here
  std::map<CubeCoordinates, HexMesh*> _cube_to_hexagon;

  gd::Ref<gd::MeshLibrary> _library;

 private:
};

}  // namespace sota
