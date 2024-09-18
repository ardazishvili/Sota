#pragma once

#include "core/hex_mesh.h"
#include "primitives/hexagon.h"
#include "tal/godot_core.h"
#include "tal/wrapped.h"
#include "types.h"

namespace sota {

struct PrismHexMeshParams {
  HexMeshParams hex_mesh_params;
  float height{0.0};
};

class PrismHexMesh : public HexMesh {
  GDCLASS(PrismHexMesh, HexMesh)
 public:
  PrismHexMesh() = default;  // existence is 'must' for Godot
  PrismHexMesh(const PrismHexMesh& other) = delete;
  PrismHexMesh(PrismHexMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  PrismHexMesh& operator=(PrismHexMesh&& rhs) = delete;

  void set_height(const float p_height);
  float get_height() const;

  PrismHexMesh(Hexagon hex, PrismHexMeshParams params);

 protected:
  static void _bind_methods();
  void init_impl() override;

 private:
  float _height{0.0};
};

class PrismHexTile : public TileMesh {
  GDCLASS(PrismHexTile, TileMesh)
 public:
  PrismHexTile() = default;  // required by godot

  int get_id() override { return _prism_hex_mesh->get_id(); }
  HexMesh* inner_mesh() override { return _prism_hex_mesh.ptr(); }
  PrismHexTile(Hexagon hex, PrismHexMeshParams params)
      : _prism_hex_mesh(Ref<PrismHexMesh>(memnew(PrismHexMesh(hex, params)))) {
    _prism_hex_mesh->init();
  }

 protected:
  static void _bind_methods() {}

 private:
  Ref<PrismHexMesh> _prism_hex_mesh;
};

Ref<PrismHexMesh> make_prism_mesh(Hexagon hex, PrismHexMeshParams params);

}  // namespace sota
