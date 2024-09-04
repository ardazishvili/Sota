#pragma once

#include "godot_cpp/classes/wrapped.hpp"
#include "hex_mesh.h"
#include "types.h"

namespace sota {

class PrismHexMesh : public HexMesh {
  GDCLASS(PrismHexMesh, HexMesh)
 public:
  PrismHexMesh() : HexMesh() {}
  PrismHexMesh(Hexagon hex) : HexMesh(hex) {}

  void set_height(const float p_height);
  float get_height() const;

  void calculate_heights();

 protected:
  static void _bind_methods();

 private:
  float height{0.0};
};

}  // namespace sota
