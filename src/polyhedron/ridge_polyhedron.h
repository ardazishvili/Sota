#pragma once

#include "polyhedron/ridge_based_polyhedron.h"
#include "polyhedron_ridge_processor.h"

namespace sota {

class PolygonWrapper;

class RidgePolyhedron : public RidgeBasedPolyhedron {
  GDCLASS(RidgePolyhedron, RidgeBasedPolyhedron)
 public:
 protected:
  RidgePolyhedron() : _ridge_processor(*this) {}
  RidgePolyhedron(const RidgePolyhedron& other) = delete;
  RidgePolyhedron(RidgePolyhedron&& other) = delete;
  // copying operator= defined inside GDCLASS
  RidgePolyhedron& operator=(RidgePolyhedron&& rhs) = delete;

  void set_ridge_top_offset(float p_ridge_top_offset);
  float get_ridge_top_offset() const;

  void set_ridge_bottom_offset(float p_ridge_bottom_offset);
  float get_ridge_bottom_offset() const;

  void set_biomes_hill_level_ratio(float p_biomes_hill_level_ratio);
  float get_biomes_hill_level_ratio() const;

  static void _bind_methods();
  void set_material_parameters(Ref<ShaderMaterial> mat) override;

  void configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _ridge_processor.configure_hexagon(wrapper, biome, id, mat, *this);
  }
  void configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _ridge_processor.configure_pentagon(wrapper, biome, id, mat, *this);
  }

  void process_cells() override { _ridge_processor.process(*this); }

 private:
  friend PolyhedronRidgeProcessor;
  float _biomes_hill_level_ratio{0.45};

  PolyhedronRidgeProcessor _ridge_processor;
};
}  // namespace sota
