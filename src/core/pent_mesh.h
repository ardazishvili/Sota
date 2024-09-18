#pragma once

#include <memory>

#include "core/mesh.h"
#include "primitives/edge.h"
#include "primitives/pentagon.h"
#include "primitives/polygon.h"
#include "tal/material.h"
#include "tal/reference.h"
namespace sota {

struct PentagonMeshParams {
  int id{0};
  int divisions{1};
  Ref<Material> material;

  TesselationMode tesselation_mode{TesselationMode::Iterative};
  Orientation orientation{Orientation::Plane};
};

class PentMesh : public SotaMesh {
  GDCLASS(PentMesh, SotaMesh)

 public:
  PentMesh();
  PentMesh(const PentMesh& other) = delete;
  PentMesh(PentMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  PentMesh& operator=(PentMesh&& rhs) = delete;

  void init_impl() override;
  void update();

  PentMesh(Pentagon pentagon, PentagonMeshParams params);

 protected:
  static void _bind_methods() {}

 private:
  void calculate_tex_uv1() override;
  void calculate_vertices_recursion();  // not tested e.g. for clips
};

}  // namespace sota
