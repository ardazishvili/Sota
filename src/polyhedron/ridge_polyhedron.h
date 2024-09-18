#pragma once

#include "polyhedron/ridge_based_polyhedron.h"
#include "polyhedron_ridge_processor.h"

namespace sota {

class RidgePolyhedron : public RidgeBasedPolyhedron {
  GDCLASS(RidgePolyhedron, RidgeBasedPolyhedron)
 public:
 protected:
  static void _bind_methods() {}

  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat) override {
    _ridge_processor.configure_cell(hex, biome, id, mat, *this);
  }
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat) override {
    _ridge_processor.configure_cell(pentagon, biome, id, mat, *this);
  }

  void process_cells() override { _ridge_processor.process(*this); }

 private:
  friend PolyhedronRidgeProcessor;

  PolyhedronRidgeProcessor _ridge_processor;
};
}  // namespace sota
