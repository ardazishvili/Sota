#pragma once

#include "polyhedron/hex_polyhedron.h"
#include "tal/material.h"
#include "tal/reference.h"

namespace sota {

class RidgeBasedPolyhedron : public Polyhedron {
  GDCLASS(RidgeBasedPolyhedron, Polyhedron)
 public:
  RidgeBasedPolyhedron();

  void set_compression_factor(const float p_compression_factor);
  float get_compression_factor() const;

  void set_noise(const Ref<FastNoiseLite> p_noise);
  Ref<FastNoiseLite> get_noise() const;

 protected:
  float _compression_factor{0.1};
  Ref<FastNoiseLite> _noise;

  void set_material_parameters(Ref<ShaderMaterial> mat) override;

  static void _bind_methods();

 private:
  template <typename T>
  void process_ngons(std::vector<T> ngons, float min_z, float max_z);
};

}  // namespace sota
