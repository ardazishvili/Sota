#pragma once

#include "core/pent_mesh.h"  // for PentMesh, PentagonMeshParams
#include "core/tile_mesh.h"  // for TileMesh
#include "core/utils.h"
#include "misc/types.h"
#include "primitives/pentagon.h"  // for Pentagon
#include "tal/reference.h"        // for Ref
#include "tal/wrapped.h"

namespace sota {

struct PrismPentMeshParams {
  PentagonMeshParams pent_mesh_params;
  float height{0.0};
};

class PrismPentMesh : public PentMesh {
  GDCLASS(PrismPentMesh, PentMesh)
 public:
  PrismPentMesh() = default;  // existence is 'must' for Godot
  PrismPentMesh(const PrismPentMesh& other) = delete;
  PrismPentMesh(PrismPentMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  PrismPentMesh& operator=(PrismPentMesh&& rhs) = delete;

  void set_height(const float p_height);
  float get_height() const;

  PrismPentMesh(Pentagon pentagon, PrismPentMeshParams params);

 protected:
  static void _bind_methods();
  void init_impl() override;

 private:
  float _height{0.0};
};

class PrismPentTile : public TileMesh {
  GDCLASS(PrismPentTile, TileMesh)
 public:
  PrismPentTile() = default;  // required by godot
  int get_id() override { return _prism_pent_mesh->get_id(); }
  PentMesh* inner_mesh() override { return _prism_pent_mesh.ptr(); }
  PrismPentTile(Pentagon pentagon, PrismPentMeshParams params)
      : _prism_pent_mesh(Ref<PrismPentMesh>(memnew(PrismPentMesh(pentagon, params)))) {
    _prism_pent_mesh->init();
  }

 protected:
  static void _bind_methods() {}

 private:
  Ref<PrismPentMesh> _prism_pent_mesh;
};

}  // namespace sota
