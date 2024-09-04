#pragma once

#include <godot_cpp/classes/primitive_mesh.hpp>

#include "godot_cpp/classes/material.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "mesh.h"
#include "misc/types.h"
#include "primitives/Hexagon.h"

namespace sota {

enum class TesselationMode { Iterative = 0, Recursive };

struct HexMeshParams {
  int id{0};
  float diameter{1};

  // TODO frame behaviour for RidgeHexGridMap and sub-classes is not clear. Address it in follow-up issue/PR
  bool frame_state{false};
  float frame_offset{0.0};

  godot::Ref<godot::Material> material;
  int divisions{3};
  ClipOptions clip_options;
};

class HexMesh : public SotaMesh {
  GDCLASS(HexMesh, SotaMesh)

 public:
  enum class TesselationType { Plane = 0, Polyhedron };

  HexMesh();

  void set_diameter(const float p_diameter);
  float get_diameter() const;
  gd::Vector3 get_center() const { return _hex.center(); }

  void init_impl() override;
  void update();

  void set_frame_state(bool state) { frame_state = state; }
  void set_frame_value(float value) { frame_offset = value; }

  void set_tesselation_type(TesselationType p_tesselation_type) { tesselation_type = p_tesselation_type; }
  void set_clip_options(ClipOptions p_options) { clip_options = p_options; }
  void set_tesselation_mode(TesselationMode p_tesselation_mode) { tesselation_mode = p_tesselation_mode; }

 protected:
  HexMesh(Hexagon hex, HexMeshParams params);
  HexMesh(Hexagon hex);
  static void _bind_methods();

  void calculate_vertices_recursion();  // not tested e.g. for clips
  void calculate_vertices_iteration() const;

  float _R;
  float _r;
  float _diameter{1};
  Hexagon _hex = make_unit_hexagon();
  TesselationType tesselation_type{TesselationType::Plane};
  TesselationMode tesselation_mode{TesselationMode::Iterative};

  bool frame_state{false};
  float frame_offset{0.0};

  ClipOptions clip_options;

 private:
  void init_from_hex(Hexagon hex);
  void add_frame() const;
  void calculate_tex_uv1() const override;

  void z_clip(float boundary) const;

  friend godot::Ref<HexMesh> make_hex_mesh(Hexagon hex, HexMeshParams params);
};

godot::Ref<HexMesh> make_hex_mesh(Hexagon hex, HexMeshParams params);

}  // namespace sota
