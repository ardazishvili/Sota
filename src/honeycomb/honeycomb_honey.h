#pragma once

#include <limits>   // for numeric_limits
#include <memory>   // for unique_ptr
#include <utility>  // for pair

#include "core/general_utility.h"  // for MeshProcessor
#include "core/hex_mesh.h"         // for HexMesh, HexMeshParams
#include "core/tile_mesh.h"        // for TileMesh
#include "misc/types.h"            // for GroupedMeshVertices
#include "tal/noise.h"             // for FastNoiseLite
#include "tal/reference.h"         // for Ref

namespace sota {
class Hexagon;

struct HoneycombHoneyMeshParams {
  HexMeshParams hex_mesh_params;
  Ref<FastNoiseLite> noise{nullptr};
  int max_level{0};
  float fill_delta{0};
  float min_offset{0.0};
};

class HoneycombHoney : public TileMesh {
  GDCLASS(HoneycombHoney, TileMesh)

 public:
  HoneycombHoney() = default;  // existence is 'must' for Godot
  HoneycombHoney(const HoneycombHoney& other) = delete;
  HoneycombHoney(HoneycombHoney&& other) = delete;
  // copying operator= defined inside GDCLASS
  HoneycombHoney& operator=(HoneycombHoney&& rhs) = delete;

  // getters
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

  HexMesh* inner_mesh() const override { return _hex_mesh.ptr(); }
  int get_id() override { return _hex_mesh->get_id(); }

  HoneycombHoney(Hexagon hex, HoneycombHoneyMeshParams params);

 protected:
  static void _bind_methods();

 private:
  Ref<FastNoiseLite> _noise;
  int _level{0};
  int _max_level{0};
  float _fill_delta{0};
  float _min_offset{0.0};
  bool _locked{false};
  std::unique_ptr<MeshProcessor> _processor;

  Ref<HexMesh> _hex_mesh;

  void recalculate_vertices_update(float surplus);

  float _min_y = std::numeric_limits<float>::max();
  float _max_y = std::numeric_limits<float>::min();
  float _y_shift = 0.0f;     // no shift
  float _y_compress = 1.0f;  // no compress
};

}  // namespace sota
