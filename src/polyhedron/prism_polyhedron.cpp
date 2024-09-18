#include "polyhedron/prism_polyhedron.h"

#include "tal/godot_core.h"

namespace sota {

void PrismPolyhedron::_bind_methods() {
  ADD_GROUP("Prism heights", "prism_heights_");
  ClassDB::bind_method(D_METHOD("get_water_height"), &PrismPolyhedron::get_water_height);
  ClassDB::bind_method(D_METHOD("set_water_height", "p_height"), &PrismPolyhedron::set_water_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_water"), "set_water_height", "get_water_height");

  ClassDB::bind_method(D_METHOD("get_plain_height"), &PrismPolyhedron::get_plain_height);
  ClassDB::bind_method(D_METHOD("set_plain_height", "p_height"), &PrismPolyhedron::set_plain_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_plain"), "set_plain_height", "get_plain_height");

  ClassDB::bind_method(D_METHOD("get_hill_height"), &PrismPolyhedron::get_hill_height);
  ClassDB::bind_method(D_METHOD("set_hill_height", "p_height"), &PrismPolyhedron::set_hill_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_hill"), "set_hill_height", "get_hill_height");

  ClassDB::bind_method(D_METHOD("get_mountain_height"), &PrismPolyhedron::get_mountain_height);
  ClassDB::bind_method(D_METHOD("set_mountain_height", "p_height"), &PrismPolyhedron::set_mountain_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_mountain"), "set_mountain_height", "get_mountain_height");
}

// Heights
void PrismPolyhedron::set_plain_height(const float p_height) {
  _prism_heights[Biome::PLAIN] = p_height;
  init();
}

void PrismPolyhedron::set_hill_height(const float p_height) {
  _prism_heights[Biome::HILL] = p_height;
  init();
}

void PrismPolyhedron::set_water_height(const float p_height) {
  _prism_heights[Biome::WATER] = p_height;
  init();
}

void PrismPolyhedron::set_mountain_height(const float p_height) {
  _prism_heights[Biome::MOUNTAIN] = p_height;
  init();
}

float PrismPolyhedron::get_plain_height() const { return _prism_heights.find(Biome::PLAIN)->second; }
float PrismPolyhedron::get_hill_height() const { return _prism_heights.find(Biome::HILL)->second; }
float PrismPolyhedron::get_water_height() const { return _prism_heights.find(Biome::WATER)->second; }
float PrismPolyhedron::get_mountain_height() const { return _prism_heights.find(Biome::MOUNTAIN)->second; }

void PrismPolyhedron::set_material_parameters(Ref<ShaderMaterial> mat) {
  mat->set_shader_parameter("water_height", _prism_heights[Biome::WATER]);
  mat->set_shader_parameter("plain_height", _prism_heights[Biome::PLAIN]);
  mat->set_shader_parameter("hill_height", _prism_heights[Biome::HILL]);
  mat->set_shader_parameter("mountain_height", _prism_heights[Biome::MOUNTAIN]);
}

}  // namespace sota
