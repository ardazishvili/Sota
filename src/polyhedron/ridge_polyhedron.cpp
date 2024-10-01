#include "polyhedron/ridge_polyhedron.h"

namespace sota {

void RidgePolyhedron::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_ridge_top_offset"), &RidgePolyhedron::get_ridge_top_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_top_offset", "p_ridge_top_offset"), &RidgePolyhedron::set_ridge_top_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_top_offset"), "set_ridge_top_offset", "get_ridge_top_offset");
  ClassDB::bind_method(D_METHOD("get_ridge_bottom_offset"), &RidgePolyhedron::get_ridge_bottom_offset);
  ClassDB::bind_method(D_METHOD("set_ridge_bottom_offset", "p_ridge_bottom_offset"),
                       &RidgePolyhedron::set_ridge_bottom_offset);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ridge_bottom_offset"), "set_ridge_bottom_offset",
               "get_ridge_bottom_offset");
  ClassDB::bind_method(D_METHOD("get_biomes_hill_level_ratio"), &RidgePolyhedron::get_biomes_hill_level_ratio);
  ClassDB::bind_method(D_METHOD("set_biomes_hill_level_ratio", "p_biomes_hill_level_ratio"),
                       &RidgePolyhedron::set_biomes_hill_level_ratio);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "biomes_hill_level_ratio"), "set_biomes_hill_level_ratio",
               "get_biomes_hill_level_ratio");
}

void RidgePolyhedron::set_ridge_top_offset(float p_ridge_top_offset) {
  _ridge_processor.set_top_offset(p_ridge_top_offset);
  init();
}

void RidgePolyhedron::set_ridge_bottom_offset(float p_ridge_bottom_offset) {
  _ridge_processor.set_bottom_offset(p_ridge_bottom_offset);
  init();
}

void RidgePolyhedron::set_biomes_hill_level_ratio(float p_biomes_hill_level_ratio) {
  _biomes_hill_level_ratio = p_biomes_hill_level_ratio;
  init();
}

float RidgePolyhedron::get_ridge_top_offset() const { return _ridge_processor.get_top_offset(); }
float RidgePolyhedron::get_ridge_bottom_offset() const { return _ridge_processor.get_bottom_offset(); }
float RidgePolyhedron::get_biomes_hill_level_ratio() const { return _biomes_hill_level_ratio; }

void RidgePolyhedron::set_material_parameters(Ref<ShaderMaterial> mat) {
  mat->set_shader_parameter("top_offset", _ridge_processor.get_top_offset());
  mat->set_shader_parameter("bottom_offset", _ridge_processor.get_bottom_offset());
  mat->set_shader_parameter("hill_level_ratio", _biomes_hill_level_ratio);
}

}  // namespace sota
