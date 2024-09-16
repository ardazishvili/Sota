#pragma once

#include "core/mesh.h"
#include "misc/types.h"
#include "primitives/Hexagon.h"
#include "tal/arrays.h"
#include "tal/material.h"
#include "tal/reference.h"
#include "tal/vector3.h"
#include "tal/wrapped.h"

namespace sota {

enum class TesselationMode { Iterative = 0, Recursive };

struct HexMeshParams {
  int id{0};
  float diameter{1};

  // TODO frame behaviour for RidgeHexGridMap and sub-classes is not clear. Address it in follow-up issue/PR
  bool frame_state{false};
  float frame_offset{0.0};

  Ref<Material> material;
  int divisions{3};
  ClipOptions clip_options;
};

class HexMesh : public SotaMesh {
  GDCLASS(HexMesh, SotaMesh)

 public:
  enum class TesselationType { Plane = 0, Polyhedron };

  HexMesh();
  HexMesh(const HexMesh& other) = delete;
  HexMesh(HexMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  HexMesh& operator=(HexMesh&& rhs) = delete;

  void set_diameter(const float p_diameter);
  float get_diameter() const;
  Vector3 get_center() const { return _hex.center(); }

  void update();

  void set_frame_state(bool state) { _frame_state = state; }
  void set_frame_value(float value) { _frame_offset = value; }

  void set_tesselation_type(TesselationType p_tesselation_type) { tesselation_type = p_tesselation_type; }
  void set_clip_options(ClipOptions p_options) { _clip_options = p_options; }
  void set_tesselation_mode(TesselationMode p_tesselation_mode) { tesselation_mode = p_tesselation_mode; }

 protected:
  HexMesh(Hexagon hex, HexMeshParams params);
  HexMesh(Hexagon hex);
  static void _bind_methods();
  void init_impl() override;

  void calculate_vertices_recursion();  // not tested e.g. for clips
  void calculate_vertices_iteration();

  float _R;
  float _r;
  float _diameter{1};
  Hexagon _hex = make_unit_hexagon();
  TesselationType tesselation_type{TesselationType::Plane};
  TesselationMode tesselation_mode{TesselationMode::Iterative};

  bool _frame_state{false};
  float _frame_offset{0.0};

  ClipOptions _clip_options;

 private:
  void init_from_hex(Hexagon hex);
  void add_frame();
  void calculate_tex_uv1() override;

  void z_clip(float boundary);

  friend Ref<HexMesh> make_hex_mesh(Hexagon hex, HexMeshParams params);
};

Ref<HexMesh> make_hex_mesh(Hexagon hex, HexMeshParams params);

}  // namespace sota
