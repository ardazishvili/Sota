#pragma once

#include <memory>

#include "core/mesh.h"       // for SotaMesh, Orientation, Tess...
#include "core/tile_mesh.h"  // for TileMesh
#include "misc/types.h"      // for ClipOptions
#include "primitives/edge.h"
#include "primitives/face.h"
#include "primitives/hexagon.h"
#include "primitives/polygon.h"
#include "primitives/triangle.h"
#include "tal/arrays.h"
#include "tal/material.h"   // for Material
#include "tal/reference.h"  // for Ref
#include "tal/vector3.h"
#include "tal/wrapped.h"

namespace sota {
class Hexagon;

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
  // TODO remove default arg field and make ctor instead
  Orientation orientation{Orientation::Plane};
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

  void set_frame_state(bool state) { _frame_state = state; }
  void set_frame_value(float value) { _frame_offset = value; }

  void set_clip_options(ClipOptions p_options) { _clip_options = p_options; }

  HexMesh(Hexagon hex, HexMeshParams params);

 protected:
  HexMesh(Hexagon hex);

  static void _bind_methods();
  void init_impl() override;

  void calculate_vertices_recursion();  // not tested e.g. for clips
  void calculate_vertices_iteration();

  float _R;
  float _r;
  float _diameter{1};

  bool _frame_state{false};
  float _frame_offset{0.0};

  ClipOptions _clip_options;

 private:
  void add_frame();
  void calculate_tex_uv1() override;

  void z_clip(float boundary);
};

class SimpleMesh : public TileMesh {
  GDCLASS(SimpleMesh, TileMesh)
 public:
  SimpleMesh() = default;  // required by godot
  int get_id() override;
  HexMesh* inner_mesh() override { return _hex_mesh.ptr(); }
  SimpleMesh(Hexagon hex, HexMeshParams params);

 protected:
  static void _bind_methods() {}

 private:
  Ref<HexMesh> _hex_mesh;
};

}  // namespace sota
