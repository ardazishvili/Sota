#pragma once

#include "mesh.h"
#include "primitives/Pentagon.h"
#include "tal/material.h"
#include "tal/reference.h"
namespace sota {

struct PentagonMeshParams {
  int id{0};
  int divisions{1};
  Ref<Material> material;
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
  void calculate_tex_uv1() override;
  void calculate_vertices_recursion();  // not tested e.g. for clips
  friend Ref<PentMesh> make_pent_mesh(Pentagon pentagon, PentagonMeshParams params);
};

Ref<PentMesh> make_pent_mesh(Pentagon pentagon, PentagonMeshParams params);

}  // namespace sota
