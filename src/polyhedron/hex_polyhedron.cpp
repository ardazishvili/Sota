#include "polyhedron/hex_polyhedron.h"

#include <algorithm>  // for transform, max, min
#include <cmath>      // for sqrt, round, cos, sin
#include <iterator>   // for back_insert_iterator
#include <limits>     // for numeric_limits
#include <vector>     // for vector

#include "algo/constants.h"         // for PI
#include "core/godot_utils.h"       // for clean_children
#include "core/utils.h"             // for map2d_to_3d, ico_in...
#include "misc/biome_calculator.h"  // for BiomeCalculator
#include "misc/types.h"             // for Biome, Biome::HILL
#include "primitives/hexagon.h"     // for Hexagon
#include "primitives/pentagon.h"    // for Pentagon
#include "tal/arrays.h"             // for Vector3Array, Array
#include "tal/callable.h"           // for Callable
#include "tal/godot_core.h"         // for D_METHOD, ClassDB
#include "tal/material.h"           // for ShaderMaterial
#include "tal/noise.h"              // for FastNoiseLite
#include "tal/shader.h"             // for Shader
#include "tal/texture.h"            // for Texture
#include "tal/vector2.h"            // for Vector2
#include "tal/vector3.h"            // for Vector3
#include "tal/vector3i.h"           // for Vector3i

namespace sota {

Polyhedron::Polyhedron() {
  _texture[Biome::PLAIN] = Ref<Texture>();
  _texture[Biome::HILL] = Ref<Texture>();
  _texture[Biome::WATER] = Ref<Texture>();
  _texture[Biome::MOUNTAIN] = Ref<Texture>();
}

void Polyhedron::_bind_methods() {
  ClassDB::bind_method(D_METHOD("init"), &Polyhedron::init);

  ClassDB::bind_method(D_METHOD("get_patch_resolution"), &Polyhedron::get_patch_resolution);
  ClassDB::bind_method(D_METHOD("set_patch_resolution", "p_patch_resolution"), &Polyhedron::set_patch_resolution);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "patch_resolution"), "set_patch_resolution", "get_patch_resolution");

  ClassDB::bind_method(D_METHOD("get_divisions"), &Polyhedron::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &Polyhedron::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_shader"), &Polyhedron::get_shader);
  ClassDB::bind_method(D_METHOD("set_shader", "p_shader"), &Polyhedron::set_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader",
               "get_shader");

  ClassDB::bind_method(D_METHOD("get_biomes_noise"), &Polyhedron::get_biomes_noise);
  ClassDB::bind_method(D_METHOD("set_biomes_noise", "p_biomes_noise"), &Polyhedron::set_biomes_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_biomes", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_biomes_noise",
               "get_biomes_noise");

  ADD_GROUP("Textures", "texture_");
  ClassDB::bind_method(D_METHOD("get_water_texture"), &Polyhedron::get_water_texture);
  ClassDB::bind_method(D_METHOD("set_water_texture", "p_texture"), &Polyhedron::set_water_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_water", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_water_texture", "get_water_texture");

  ClassDB::bind_method(D_METHOD("get_plain_texture"), &Polyhedron::get_plain_texture);
  ClassDB::bind_method(D_METHOD("set_plain_texture", "p_texture"), &Polyhedron::set_plain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_plain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_plain_texture", "get_plain_texture");

  ClassDB::bind_method(D_METHOD("get_hill_texture"), &Polyhedron::get_hill_texture);
  ClassDB::bind_method(D_METHOD("set_hill_texture", "p_texture"), &Polyhedron::set_hill_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_hill", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_hill_texture", "get_hill_texture");

  ClassDB::bind_method(D_METHOD("get_mountain_texture"), &Polyhedron::get_mountain_texture);
  ClassDB::bind_method(D_METHOD("set_mountain_texture", "p_texture"), &Polyhedron::set_mountain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_mountain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_mountain_texture", "get_mountain_texture");
}

void Polyhedron::set_divisions(const int p_divisions) {
  _divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

void Polyhedron::set_patch_resolution(const int p_patch_resolution) {
  _patch_resolution = p_patch_resolution;
  init();
}

void Polyhedron::set_shader(const Ref<Shader> p_shader) {
  _shader = p_shader;
  init();
}

void Polyhedron::set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise) {
  _biomes_noise = p_biomes_noise;
  if (_biomes_noise.ptr()) {
    _biomes_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

// TODO: textures code copypasted from RidgeHexGridMap
void Polyhedron::set_plain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::PLAIN] = p_texture;
  init();
}

void Polyhedron::set_hill_texture(const Ref<Texture> p_texture) {
  _texture[Biome::HILL] = p_texture;
  init();
}

void Polyhedron::set_water_texture(const Ref<Texture> p_texture) {
  _texture[Biome::WATER] = p_texture;
  init();
}

void Polyhedron::set_mountain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::MOUNTAIN] = p_texture;
  init();
}

int Polyhedron::get_divisions() const { return _divisions; }
int Polyhedron::get_patch_resolution() const { return _patch_resolution; }
Ref<Shader> Polyhedron::get_shader() const { return _shader; }
Ref<FastNoiseLite> Polyhedron::get_biomes_noise() const { return _biomes_noise; }
Ref<Texture> Polyhedron::get_plain_texture() const { return _texture.find(Biome::PLAIN)->second; }
Ref<Texture> Polyhedron::get_hill_texture() const { return _texture.find(Biome::HILL)->second; }
Ref<Texture> Polyhedron::get_water_texture() const { return _texture.find(Biome::WATER)->second; }
Ref<Texture> Polyhedron::get_mountain_texture() const { return _texture.find(Biome::MOUNTAIN)->second; }

template <typename TGON>
void Polyhedron::insert_to_polygons(Vector3 start_point, float diameter, float R, float r, int i, int j,
                                    Vector3Array icosahedron_points, Vector3i triangle,
                                    std::map<Vector3i, TGON>& polygons) const {
  float key_step = r / 3.0;
  auto f1 = [](float x) -> float { return sqrt(3) * x + sqrt(3) / 2; };
  auto f2 = [](float x) -> float { return -sqrt(3) * x + sqrt(3) / 2; };
  auto out_of_triangle = [f1, f2](float x, float z) -> bool { return z < 0 || z > f1(x) || z > f2(x); };

  Vector3 center(start_point.x + 2 * r * j, 0, start_point.z + diameter * 3.0 * i / 4.0);
  if (i & 1) {
    center.x -= r;  // offset of odd rows of pent-/hexagons;
  }
  if ((center.z < -r / 2) || center.z > (f1(center.x) + r / 2) || center.z > (f2(center.x) + r / 2)) {
    return;  // skip pent-/hexagons out of sample triangle
  }

  Vector3 mapped_center = map2d_to_3d(Vector2(center.x, center.z), icosahedron_points[triangle.x],
                                      icosahedron_points[triangle.y], icosahedron_points[triangle.z]);
  Vector3i key(std::round(mapped_center.x / key_step), std::round(mapped_center.y / key_step),
               std::round(mapped_center.z / key_step));  // to use dict. due to float fluctuations
  if (polygons.find(key) == polygons.end()) {
    polygons.emplace(key, TGON(mapped_center, mapped_center.normalized()));
  }
  TGON& polygon = polygons.find(key)->second;
  for (float angle = -PI / 6, k = 0; k < 6; k++, angle += PI / 3) {
    Vector3 point(center.x + cos(angle) * R, 0, center.z + sin(angle) * R);
    if (!out_of_triangle(point.x, point.z)) {
      polygon.add_point(map2d_to_3d(Vector2(point.x, point.z), icosahedron_points[triangle.x],
                                    icosahedron_points[triangle.y], icosahedron_points[triangle.z]));
    }
  }
}

std::pair<std::vector<Hexagon>, std::vector<Pentagon>> Polyhedron::calculate_shapes() const {
  float r = (1.0 / 2) / (_patch_resolution + 1);
  float R = r * 2 / sqrt(3);
  float diameter = 2 * R;
  Vector3 start_point(-0.5, 0, 0);

  Vector3Array icosahedron_points = ico_points();
  Array indices = ico_indices();
  auto is_pentagon_ij = [this](int i, int j) -> bool {
    return (i == (_patch_resolution + 1)) ||  // last row contains center of pentagon
           (i == 0 &&
            (j == 0 || j == (_patch_resolution + 1)));  // first and last points of first row are centers of pentagon
  };

  std::map<Vector3i, Hexagon> hexagon_map;
  std::map<Vector3i, Pentagon> pentagon_map;
  for (int t = 0; t < 20; ++t) {
    Vector3i triangle = indices[t];
    for (int i = 0; i < _patch_resolution + 2; ++i) {
      for (int j = 0; j < _patch_resolution + 2; ++j) {
        if (is_pentagon_ij(i, j)) {
          insert_to_polygons(start_point, diameter, R, r, i, j, icosahedron_points, triangle, pentagon_map);
        } else {
          insert_to_polygons(start_point, diameter, R, r, i, j, icosahedron_points, triangle, hexagon_map);
        }
      }
    }
  }

  std::vector<Hexagon> hexagons;
  std::transform(hexagon_map.begin(), hexagon_map.end(), std::back_inserter(hexagons),
                 [](std::pair<Vector3i, Hexagon> p) {
                   p.second.check();
                   p.second.sort_points();
                   return p.second;
                 });

  std::vector<Pentagon> pentagons;
  std::transform(pentagon_map.begin(), pentagon_map.end(), std::back_inserter(pentagons),
                 [](std::pair<Vector3i, Pentagon> p) {
                   p.second.check();
                   p.second.sort_points();
                   return p.second;
                 });

  return std::make_pair(hexagons, pentagons);
}

void Polyhedron::clear() {
  _hexagon_meshes.clear();
  _pentagon_meshes.clear();

  clean_children(*this);
}

template <typename T>
void Polyhedron::process_ngons(std::vector<T> ngons, float min_z, float max_z) {
  int id = 0;
  std::unordered_map<int, float> altitudes;
  for (const auto& ngon : ngons) {
    if (_biomes_noise.ptr()) {
      altitudes[id] = _biomes_noise->get_noise_3dv(ngon.center());
    } else {
      altitudes[id] = 0;
    }
    ++id;
  }

  for (auto [_, a] : altitudes) {
    min_z = std::min(min_z, a);
    max_z = std::max(max_z, a);
  }

  id = 0;
  BiomeCalculator biome_calculator;
  for (const auto& ngon : ngons) {
    Biome biome = biome_calculator.calculate_biome(min_z, max_z, altitudes[id]);

    Ref<ShaderMaterial> mat;
    mat.instantiate();
    if (_shader.ptr()) {
      mat->set_shader(_shader);
    }

    if (_texture.size() == 4) {
      mat->set_shader_parameter("water_texture", _texture[Biome::WATER].ptr());
      mat->set_shader_parameter("plain_texture", _texture[Biome::PLAIN].ptr());
      mat->set_shader_parameter("hill_texture", _texture[Biome::HILL].ptr());
      mat->set_shader_parameter("mountain_texture", _texture[Biome::MOUNTAIN].ptr());
    }

    set_material_parameters(mat);

    configure_cell(ngon, biome, id, mat);
  }
}

void Polyhedron::init() {
  clear();

  auto [hexagons, pentagons] = calculate_shapes();

  float min_z = std::numeric_limits<float>::max();
  float max_z = std::numeric_limits<float>::min();
  process_ngons(hexagons, min_z, max_z);
  process_ngons(pentagons, min_z, max_z);

  process_cells();
  calculate_normals();
}

}  // namespace sota
