#pragma once

#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "misc/types.h"                  // for Biome, Biome::HILL, Biome::...
#include "polyhedron/hex_polyhedron.h"   // for Polyhedron
#include "polyhedron_prism_processor.h"  // for PolyhedronPrismProcessor
#include "primitives/hexagon.h"          // for Hexagon
#include "primitives/pentagon.h"         // for Pentagon
#include "tal/material.h"                // for ShaderMaterial
#include "tal/reference.h"               // for Ref

namespace sota {

class PrismPolyhedron : public Polyhedron {
  GDCLASS(PrismPolyhedron, Polyhedron)
 public:
  PrismPolyhedron(const PrismPolyhedron& other) = delete;
  PrismPolyhedron(PrismPolyhedron&& other) = delete;
  // copying operator= defined inside GDCLASS
  PrismPolyhedron& operator=(PrismPolyhedron&& other) = delete;

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
  void calculate_normals() override { /* no need for this for prisms at the moment*/
  }

  void configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _prism_processor.configure_hexagon(wrapper, biome, id, mat, *this);
  }
  void configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) override {
    _prism_processor.configure_pentagon(wrapper, biome, id, mat, *this);
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
