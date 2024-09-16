#pragma once

#include "hex_mesh.h"
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

  void calculate_heights();

 protected:
  PrismHexMesh(Hexagon hex, PrismHexMeshParams params);
  static void _bind_methods();

 private:
  float _height{0.0};

  friend Ref<PrismHexMesh> make_prism_mesh(Hexagon hex, PrismHexMeshParams params);
};

Ref<PrismHexMesh> make_prism_mesh(Hexagon hex, PrismHexMeshParams params);

}  // namespace sota
