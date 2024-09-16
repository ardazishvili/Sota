#pragma once

#include "core/mesh.h"
#include "misc/types.h"
#include "primitives/Edge.h"
#include "primitives/Face.h"
#include "primitives/Hexagon.h"
#include "primitives/Triangle.h"
#include "tal/arrays.h"
#include "tal/material.h"
#include "tal/reference.h"
#include "tal/vector3.h"
#include "tal/wrapped.h"

namespace sota {

enum class TesselationMode { Iterative = 0, Recursive };
enum class Orientation { Plane = 0, Polyhedron };

struct HexMeshParams {
  int id{0};
  float diameter{1};

  // TODO frame behaviour for RidgeHexGridMap and sub-classes is not clear. Address it in follow-up issue/PR
  bool frame_state{false};
  float frame_offset{0.0};

  Ref<Material> material;
  int divisions{3};
  ClipOptions clip_options;

  TesselationMode tesselation_mode{TesselationMode::Iterative};
  Orientation tesselation_type{Orientation::Plane};
};

class HexMesh : public SotaMesh {
  GDCLASS(HexMesh, SotaMesh)

 public:
  HexMesh();
  HexMesh(const HexMesh& other) = delete;
  HexMesh(HexMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  HexMesh& operator=(HexMesh&& rhs) = delete;

  void set_diameter(const float p_diameter);
  float get_diameter() const;
  Vector3 get_center() const { return _hex.center(); }
  Vector3 get_corner_point_normal(Vector3 point) {
    return _tesselation_type == Orientation::Plane ? _hex.normal() : point.normalized();
  }

  void update();

  void set_frame_state(bool state) { _frame_state = state; }
  void set_frame_value(float value) { _frame_offset = value; }

  void set_tesselation_type(Orientation p_tesselation_type) { _tesselation_type = p_tesselation_type; }
  void set_clip_options(ClipOptions p_options) { _clip_options = p_options; }
  void set_tesselation_mode(TesselationMode p_tesselation_mode) { _tesselation_mode = p_tesselation_mode; }

 protected:
  HexMesh(Hexagon hex, HexMeshParams params);
  HexMesh(Hexagon hex);
  static void _bind_methods();
  void init_impl() override;

  void calculate_vertices_recursion();  // not tested e.g. for clips
  void calculate_vertices_iteration();
  void add_face_to_base_hex(Edge e, float offset);  // negative offset means direction negative to base hex normal

  float _R;
  float _r;
  float _diameter{1};
  Hexagon _hex = make_unit_hexagon();
  Orientation _tesselation_type{Orientation::Plane};
  TesselationMode _tesselation_mode{TesselationMode::Iterative};

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
