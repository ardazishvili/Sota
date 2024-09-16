#pragma once

#include "hex_mesh.h"
#include "tal/wrapped.h"
#include "types.h"

namespace sota {

class PrismHexMesh : public HexMesh {
  GDCLASS(PrismHexMesh, HexMesh)
 public:
  PrismHexMesh() = default;  // existence is 'must' for Godot
  PrismHexMesh(const PrismHexMesh& other) = delete;
  PrismHexMesh(PrismHexMesh&& other) = delete;
  // copying operator= defined inside GDCLASS
  PrismHexMesh& operator=(PrismHexMesh&& rhs) = delete;

  PrismHexMesh(Hexagon hex) : HexMesh(hex) {}

  void set_height(const float p_height);
  float get_height() const;

  void calculate_heights();

 protected:
  static void _bind_methods();

 private:
  float _height{0.0};
};

}  // namespace sota
