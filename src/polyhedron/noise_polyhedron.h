#pragma once

#include "polyhedron/ridge_based_polyhedron.h"
#include "polyhedron_noise_processor.h"

namespace sota {

class NoisePolyhedron : public RidgeBasedPolyhedron {
  GDCLASS(NoisePolyhedron, RidgeBasedPolyhedron)
 public:
  NoisePolyhedron() = default;
  NoisePolyhedron(const NoisePolyhedron& other) = delete;
  NoisePolyhedron(NoisePolyhedron&& other) = delete;
  // copying operator= defined inside GDCLASS
  NoisePolyhedron& operator=(NoisePolyhedron&& rhs) = delete;

 protected:
  static void _bind_methods() {}
  void set_material_parameters(Ref<ShaderMaterial> mat) override {}

  void configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _noise_processor.configure_cell(hex, biome, id, mat, *this);
  }
  void configure_cell(Pentagon pentagon, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _noise_processor.configure_cell(pentagon, biome, id, mat, *this);
  }

  void process_cells() override { _noise_processor.process(*this); }

 private:
  friend PolyhedronNoiseProcessor;

  PolyhedronNoiseProcessor _noise_processor;
};

}  // namespace sota
