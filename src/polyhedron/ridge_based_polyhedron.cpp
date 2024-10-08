#include "polyhedron/ridge_based_polyhedron.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "core/smooth_shades_processor.h"
#include "core/tile_mesh.h"
#include "misc/discretizer.h"
#include "polyhedron_mesh_processor.h"
#include "tal/callable.h"    // for Callable
#include "tal/godot_core.h"  // for D_METHOD, ClassDB
#include "tal/material.h"    // for ShaderMaterial
#include "tal/reference.h"   // for Ref

namespace sota {

void RidgeBasedPolyhedron::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_smooth_normals"), &RidgeBasedPolyhedron::get_smooth_normals);
  ClassDB::bind_method(D_METHOD("set_smooth_normals", "p_smooth_normals"), &RidgeBasedPolyhedron::set_smooth_normals);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "_smooth_normals"), "set_smooth_normals", "get_smooth_normals");

  ClassDB::bind_method(D_METHOD("get_compression_factor"), &RidgeBasedPolyhedron::get_compression_factor);
  ClassDB::bind_method(D_METHOD("set_compression_factor", "p_compression_factor"),
                       &RidgeBasedPolyhedron::set_compression_factor);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "compression_factor"), "set_compression_factor", "get_compression_factor");

  ClassDB::bind_method(D_METHOD("get_plain_noise"), &RidgeBasedPolyhedron::get_plain_noise);
  ClassDB::bind_method(D_METHOD("set_plain_noise", "p_noise"), &RidgeBasedPolyhedron::set_plain_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "plain_noise", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_plain_noise",
               "get_plain_noise");

  ClassDB::bind_method(D_METHOD("get_ridge_noise"), &RidgeBasedPolyhedron::get_ridge_noise);
  ClassDB::bind_method(D_METHOD("set_ridge_noise", "p_noise"), &RidgeBasedPolyhedron::set_ridge_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ridge_noise", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_ridge_noise",
               "get_ridge_noise");
}

void RidgeBasedPolyhedron::set_smooth_normals(const bool p_smooth_normals) {
  _smooth_normals = p_smooth_normals;
  calculate_normals();
}

void RidgeBasedPolyhedron::set_compression_factor(const float p_compression_factor) {
  _compression_factor = p_compression_factor;
  init();
}

void RidgeBasedPolyhedron::set_plain_noise(const Ref<FastNoiseLite> p_noise) {
  _plain_noise = p_noise;
  if (_plain_noise.ptr()) {
    _plain_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void RidgeBasedPolyhedron::set_ridge_noise(const Ref<FastNoiseLite> p_noise) {
  _ridge_noise = p_noise;
  if (_ridge_noise.ptr()) {
    _ridge_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

bool RidgeBasedPolyhedron::get_smooth_normals() const { return _smooth_normals; }
float RidgeBasedPolyhedron::get_compression_factor() const { return _compression_factor; }
Ref<FastNoiseLite> RidgeBasedPolyhedron::get_plain_noise() const { return _plain_noise; }
Ref<FastNoiseLite> RidgeBasedPolyhedron::get_ridge_noise() const { return _ridge_noise; }

void RidgeBasedPolyhedron::calculate_normals() { SmoothShadesProcessor(meshes()).calculate_normals(_smooth_normals); }

std::vector<TileMesh*> RidgeBasedPolyhedron::meshes() {
  std::vector<TileMesh*> res;
  std::transform(_hexagons.begin(), _hexagons.end(), std::back_inserter(res),
                 [](PolygonWrapper& wrapper) { return wrapper.mesh().ptr(); });
  std::transform(_pentagons.begin(), _pentagons.end(), std::back_inserter(res),
                 [](PolygonWrapper& wrapper) { return wrapper.mesh().ptr(); });
  return res;
}

}  // namespace sota
