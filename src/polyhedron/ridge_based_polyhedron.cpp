#include "polyhedron/ridge_based_polyhedron.h"

#include "hex_polyhedron.h"
#include "misc/biome_calculator.h"
#include "tal/callable.h"
#include "tal/godot_core.h"

namespace sota {

void RidgeBasedPolyhedron::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_compression_factor"), &RidgeBasedPolyhedron::get_compression_factor);
  ClassDB::bind_method(D_METHOD("set_compression_factor", "p_compression_factor"),
                       &RidgeBasedPolyhedron::set_compression_factor);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "compression_factor"), "set_compression_factor", "get_compression_factor");

  ClassDB::bind_method(D_METHOD("get_noise"), &RidgeBasedPolyhedron::get_noise);
  ClassDB::bind_method(D_METHOD("set_noise", "p_noise"), &RidgeBasedPolyhedron::set_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_noise", "get_noise");
}

void RidgeBasedPolyhedron::set_compression_factor(const float p_compression_factor) {
  _compression_factor = p_compression_factor;
  init();
}

void RidgeBasedPolyhedron::set_noise(const Ref<FastNoiseLite> p_noise) {
  _noise = p_noise;
  if (_noise.ptr()) {
    _noise->connect("changed", Callable(this, "init"));
    init();
  }
}

float RidgeBasedPolyhedron::get_compression_factor() const { return _compression_factor; }
Ref<FastNoiseLite> RidgeBasedPolyhedron::get_noise() const { return _noise; }

void RidgeBasedPolyhedron::set_material_parameters(Ref<ShaderMaterial> mat) {}

}  // namespace sota
