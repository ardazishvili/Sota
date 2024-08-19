#pragma once

#include "godot_cpp/classes/mesh_library.hpp"
#include "godot_cpp/classes/texture.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "hex_grid_map.h"
#include "honeycomb/honeycomb_honey.h"
namespace sota {

class Honeycomb : public HexGridMap {
  GDCLASS(Honeycomb, HexGridMap)

 public:
  Honeycomb() = default;
  void set_cell_texture(const gd::Ref<gd::Texture> p_cell_texture);
  gd::Ref<gd::Texture> get_cell_texture() const;

  void set_honey_texture(const gd::Ref<gd::Texture> p_honey_texture);
  gd::Ref<gd::Texture> get_honey_texture() const;

  void set_selection_texture(const gd::Ref<gd::Texture> p_selection_texture);
  gd::Ref<gd::Texture> get_selection_texture() const;

  void set_noise(const gd::Ref<gd::FastNoiseLite> p_noise);
  gd::Ref<gd::FastNoiseLite> get_noise() const;

  void set_bottom_offset(float p_ridge_bottom_offset);
  float get_bottom_offset() const;

  void set_smooth_normals(bool p_smooth_normals);
  bool get_smooth_normals() const;

  void set_honey_random_level(bool p_honey_random_level);
  bool get_honey_random_level() const;

  void set_honey_min_offset(float p_honey_min_offset);
  float get_honey_min_offset() const;

  void set_honey_max_gain(float p_honey_max_gain);
  float get_honey_max_gain() const;

  void set_honey_fill_steps(int p_honey_fill_steps);
  int get_honey_fill_steps() const;

  void set_honey_shader(const gd::Ref<gd::Shader> p_honey_shader);
  gd::Ref<gd::Shader> get_honey_shader() const;

  void set_selection_shader(const gd::Ref<gd::Shader> p_selection_shader);
  gd::Ref<gd::Shader> get_selection_shader() const;

  gd::Array get_cells() const;      // return all honey cells
  gd::Array get_min_cells() const;  // find minimum honey level and return all cells which has that level
  gd::Array get_max_cells() const;  // find maximum honey level and return all cells which has that level
  bool all_cells_empty() const;

 protected:
  static void _bind_methods();

  void init() override;
  void init_hexmesh() override;
  void init_mesh_lib() override;

  virtual void init_grid_honey() = 0;
  virtual int calculate_honey_id_offset() = 0;

 private:
  gd::Ref<gd::MeshLibrary> honey_library;
  gd::Ref<gd::Texture> cell_texture;
  gd::Ref<gd::Texture> honey_texture;
  gd::Ref<gd::Texture> selection_texture;
  gd::Ref<gd::Shader> selection_shader;
  gd::Ref<gd::FastNoiseLite> noise;
  gd::Ref<gd::Shader> honey_shader;
  float bottom_offset{-0.5};
  bool smooth_normals{false};
  bool honey_random_level{false};

  float honey_min_offset{-0.4};
  float honey_max_gain{0.3};
  int honey_fill_steps{8};

  float generate_min_honey_y_offset();
  float generate_random_honey_y_offset();
  float generate_offset(int step);
  float get_honey_step_value();
  int generate_min_honey_step();
  int generate_random_honey_step();

  void prepare_heights_calculation();
  void calculate_final_heights();

  void calculate_cells();
  void calculate_normals() override;
  void calculate_flat_normals();
  void calculate_smooth_normals();
  void meshes_update();

  enum class SortingOrder { INCREASING, DECREASING };

  template <typename T>
  void sort(Honeycomb::SortingOrder order, T& container) const {
    if (order == SortingOrder::INCREASING) {
      std::sort(container.begin(), container.end());
    } else if (order == SortingOrder::DECREASING) {
      std::sort(container.rbegin(), container.rend());
    }
  }

  gd::Array get_cells_by_order(SortingOrder order, std::function<bool(HoneycombHoney*)> pred) const;
};

class RectHoneycomb : public Honeycomb {
  GDCLASS(RectHoneycomb, Honeycomb)
 public:
  void set_height(const int p_height);
  int get_height() const;

  void set_width(const int p_width);
  int get_width() const;

  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;
  void init_grid_honey() override;
  int calculate_honey_id_offset() override;

 protected:
  int height{0};
  int width{0};
  static void _bind_methods();
};

class HexagonalHoneycomb : public Honeycomb {
  GDCLASS(HexagonalHoneycomb, Honeycomb)
 public:
  void set_size(const int p_size);
  int get_size() const;

  void init_col_row_layout() override;
  int calculate_id(int row, int col) const override;
  void init_grid_honey() override;
  int calculate_honey_id_offset() override;

  gd::Vector3 get_center() const;

 protected:
  int size{0};
  static void _bind_methods();
};

}  // namespace sota
