#pragma once

#include "core/hex_mesh.h"
#include "misc/types.h"
#include "tal/noise.h"
#include "tal/reference.h"
namespace sota {

struct HoneycombHoneyMeshParams {
  HexMeshParams hex_mesh_params;
  Ref<FastNoiseLite> noise{nullptr};
  int max_level{0};
  float fill_delta{0};
  float min_offset{0.0};
};

class HoneycombHoney : public HexMesh {
  GDCLASS(HoneycombHoney, HexMesh)

 public:
  HoneycombHoney() : HexMesh(make_unit_hexagon()) {}
  HoneycombHoney(Hexagon hex, HoneycombHoneyMeshParams params) : HexMesh(hex, params.hex_mesh_params) {
    _noise = params.noise;
    _max_level = params.max_level;
    _fill_delta = params.fill_delta;
    _min_offset = params.min_offset;
  }

  // getters
  GroupedHexagonMeshVertices get_grouped_vertices();
  std::pair<float, float> get_min_max_height() const { return {_min_y, _max_y}; }

  // setters
  void set_noise(Ref<FastNoiseLite> noise);
  void set_min_offset(float p_offset);
  void set_max_level(float p_max_level);

  void set_fill_delta(float d);
  void set_level(int p_level);
  void fill();
  void clear();
  int get_level() const;

  void calculate_initial_heights();
  void calculate_heights();

  void set_shift_compress(float y_shift, float y_compress);

  void lock() { _locked = true; }
  void unlock() { _locked = false; }
  bool is_locked() const { return _locked; }
  bool is_full() const;
  bool is_empty() const;

 protected:
  static void _bind_methods();

 private:
  Ref<FastNoiseLite> _noise;
  int _level{0};
  int _max_level{0};
  float _fill_delta{0};
  float _min_offset{0.0};
  bool _locked{false};

  void recalculate_vertices_update(float surplus);

  float _min_y = std::numeric_limits<float>::max();
  float _max_y = std::numeric_limits<float>::min();
  float _y_shift = 0.0f;     // no shift
  float _y_compress = 1.0f;  // no compress
};

Ref<HoneycombHoney> make_honeycomb_honey(Hexagon hex, HoneycombHoneyMeshParams params);

}  // namespace sota
