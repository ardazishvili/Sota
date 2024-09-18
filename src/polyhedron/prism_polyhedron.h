#pragma once

#include "polyhedron/hex_polyhedron.h"

namespace sota {

class PrismPolyhedron : public Polyhedron {
  GDCLASS(PrismPolyhedron, Polyhedron)
 public:
  PrismPolyhedron() : Polyhedron() {
    _prism_heights[Biome::WATER] = 0.0;
    _prism_heights[Biome::PLAIN] = 0.02;
    _prism_heights[Biome::HILL] = 0.07;
    _prism_heights[Biome::MOUNTAIN] = 0.15;
  }

  // heights
  void set_plain_height(const float p_height);
  float get_plain_height() const;

  void set_hill_height(const float p_height);
  float get_hill_height() const;

  void set_water_height(const float p_height);
  float get_water_height() const;

  void set_mountain_height(const float p_height);
  float get_mountain_height() const;

 protected:
  static void _bind_methods();
  void set_material_parameters(Ref<ShaderMaterial> mat) override;

  void configure_cell(Hexagon hex, Biome biome, int &id, Ref<ShaderMaterial> mat) override {
    _prism_processor.configure_cell(hex, biome, id, mat, *this);
  }
  void configure_cell(Pentagon pentagon, Biome biome, int &id, Ref<ShaderMaterial> mat) override {
    _prism_processor.configure_cell(pentagon, biome, id, mat, *this);
  }

  void process_cells() override { _prism_processor.process(*this); }

 private:
  friend class PolyhedronPrismProcessor;

  std::unordered_map<Biome, float> _prism_heights;
  PolyhedronPrismProcessor _prism_processor;

  template <typename T>
  void process_ngons(std::vector<T> hexagons);
};

}  // namespace sota
