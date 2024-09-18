#pragma once

#include <map>
#include <memory>

#include "core/hex_mesh.h"
#include "misc/cube_coordinates.h"
#include "misc/tile.h"
#include "misc/types.h"
#include "tal/arrays.h"
#include "tal/node.h"
#include "tal/reference.h"
#include "tal/shader.h"
#include "tal/vector3i.h"
#include "tal/wrapped.h"

namespace sota {

using TilesLayout = std::vector<std::vector<Tile*>>;

class HexGrid : public Node3D {
  GDCLASS(HexGrid, Node3D)

 public:
  HexGrid() = default;

  void set_shader(const Ref<Shader> p_shader);
  Ref<Shader> get_shader() const;

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

  Array get_hex_meshes();

 protected:
  float _diameter{1};
  int _divisions{3};
  Ref<Shader> _shader;
  std::vector<std::vector<Vector3i>> _col_row_layout;
  TilesLayout _tiles_layout;

  static void _bind_methods();
  virtual void init();

  virtual void init_col_row_layout() = 0;
  virtual void init_hexmesh();
  std::map<CubeCoordinates, TileMesh*> _cube_to_hexagon;

  bool _frame_state{false};
  float _frame_offset{0.0};

 private:
};

class RectHexGrid : public HexGrid {
  GDCLASS(RectHexGrid, HexGrid)
 public:
  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;

  void set_height(const int p_height);
  int get_height() const;

  void set_width(const int p_width);
  int get_width() const;

 protected:
  int _height{0};
  int _width{0};
  static void _bind_methods();
};

class HexagonalHexGrid : public HexGrid {
  GDCLASS(HexagonalHexGrid, HexGrid)
 public:
  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;

  void set_size(const int p_size);
  int get_size() const;

 protected:
  int _size{0};
  static void _bind_methods();
};

}  // namespace sota
