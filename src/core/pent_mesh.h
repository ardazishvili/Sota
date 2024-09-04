#pragma once

#include "godot_cpp/classes/material.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "mesh.h"
#include "primitives/Pentagon.h"
namespace sota {

struct PentagonMeshParams {
  int id{0};
  int divisions{1};
  godot::Ref<godot::Material> material;
};

class PentMesh : public SotaMesh {
  GDCLASS(PentMesh, SotaMesh)

 public:
  PentMesh();

  void init_impl() override;
  void update();

 protected:
  PentMesh(Pentagon pentagon, PentagonMeshParams params);
  static void _bind_methods() {}
  Pentagon _pentagon{make_unit_pentagon()};

 private:
  void init_from_pentagon(Pentagon pentagon);
  void calculate_tex_uv1() const override;
  void calculate_vertices_recursion();  // not tested e.g. for clips
  friend godot::Ref<PentMesh> make_pent_mesh(Pentagon pentagon, PentagonMeshParams params);
};

godot::Ref<PentMesh> make_pent_mesh(Pentagon pentagon, PentagonMeshParams params);

}  // namespace sota
