#include "polyhedron/hex_polyhedron.h"

#include <algorithm>
#include <vector>

#include "Hexagon.h"
#include "algo/constants.h"
#include "biome_calculator.h"
#include "core/godot_utils.h"
#include "core/utils.h"
#include "tal/arrays.h"
#include "tal/callable.h"
#include "tal/godot_core.h"
#include "tal/material.h"
#include "tal/mesh.h"
#include "tal/noise.h"
#include "tal/shader.h"
#include "tal/vector2.h"
#include "tal/vector3.h"
#include "tal/vector3i.h"
#include "pent_mesh.h"
#include "types.h"

namespace sota {

PolyhedronMesh::PolyhedronMesh() {
  _texture[Biome::PLAIN] = Ref<Texture>();
  _texture[Biome::HILL] = Ref<Texture>();
  _texture[Biome::WATER] = Ref<Texture>();
  _texture[Biome::MOUNTAIN] = Ref<Texture>();

  _prism_heights[Biome::WATER] = -0.02;
  _prism_heights[Biome::PLAIN] = 0.0;
  _prism_heights[Biome::HILL] = 0.04;
  _prism_heights[Biome::MOUNTAIN] = 0.08;
}

void PolyhedronMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("init"), &PolyhedronMesh::init);

  ClassDB::bind_method(D_METHOD("get_patch_resolution"), &PolyhedronMesh::get_patch_resolution);
  ClassDB::bind_method(D_METHOD("set_patch_resolution", "p_patch_resolution"), &PolyhedronMesh::set_patch_resolution);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "patch_resolution"), "set_patch_resolution", "get_patch_resolution");

  ClassDB::bind_method(D_METHOD("get_compression_factor"), &PolyhedronMesh::get_compression_factor);
  ClassDB::bind_method(D_METHOD("set_compression_factor", "p_compression_factor"),
                       &PolyhedronMesh::set_compression_factor);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "compression_factor"), "set_compression_factor", "get_compression_factor");

  ClassDB::bind_method(D_METHOD("get_divisions"), &PolyhedronMesh::get_divisions);
  ClassDB::bind_method(D_METHOD("set_divisions", "p_divisions"), &PolyhedronMesh::set_divisions);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "divisions"), "set_divisions", "get_divisions");

  ClassDB::bind_method(D_METHOD("get_shader"), &PolyhedronMesh::get_shader);
  ClassDB::bind_method(D_METHOD("set_shader", "p_shader"), &PolyhedronMesh::set_shader);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader",
               "get_shader");

  ADD_GROUP("Noise", "noise_");
  ClassDB::bind_method(D_METHOD("get_biomes_noise"), &PolyhedronMesh::get_biomes_noise);
  ClassDB::bind_method(D_METHOD("set_biomes_noise", "p_biomes_noise"), &PolyhedronMesh::set_biomes_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_biomes", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_biomes_noise",
               "get_biomes_noise");
  ClassDB::bind_method(D_METHOD("get_noise"), &PolyhedronMesh::get_noise);
  ClassDB::bind_method(D_METHOD("set_noise", "p_noise"), &PolyhedronMesh::set_noise);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_noise", "get_noise");

  ADD_GROUP("Textures", "texture_");
  ClassDB::bind_method(D_METHOD("get_water_texture"), &PolyhedronMesh::get_water_texture);
  ClassDB::bind_method(D_METHOD("set_water_texture", "p_texture"), &PolyhedronMesh::set_water_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_water", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_water_texture", "get_water_texture");

  ClassDB::bind_method(D_METHOD("get_plain_texture"), &PolyhedronMesh::get_plain_texture);
  ClassDB::bind_method(D_METHOD("set_plain_texture", "p_texture"), &PolyhedronMesh::set_plain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_plain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_plain_texture", "get_plain_texture");

  ClassDB::bind_method(D_METHOD("get_hill_texture"), &PolyhedronMesh::get_hill_texture);
  ClassDB::bind_method(D_METHOD("set_hill_texture", "p_texture"), &PolyhedronMesh::set_hill_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_hill", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_hill_texture", "get_hill_texture");

  ClassDB::bind_method(D_METHOD("get_mountain_texture"), &PolyhedronMesh::get_mountain_texture);
  ClassDB::bind_method(D_METHOD("set_mountain_texture", "p_texture"), &PolyhedronMesh::set_mountain_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_mountain", PROPERTY_HINT_RESOURCE_TYPE, "Texture"),
               "set_mountain_texture", "get_mountain_texture");

  ADD_GROUP("Prism heights", "prism_heights_");
  ClassDB::bind_method(D_METHOD("get_water_height"), &PolyhedronMesh::get_water_height);
  ClassDB::bind_method(D_METHOD("set_water_height", "p_height"), &PolyhedronMesh::set_water_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_water"), "set_water_height", "get_water_height");

  ClassDB::bind_method(D_METHOD("get_plain_height"), &PolyhedronMesh::get_plain_height);
  ClassDB::bind_method(D_METHOD("set_plain_height", "p_height"), &PolyhedronMesh::set_plain_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_plain"), "set_plain_height", "get_plain_height");

  ClassDB::bind_method(D_METHOD("get_hill_height"), &PolyhedronMesh::get_hill_height);
  ClassDB::bind_method(D_METHOD("set_hill_height", "p_height"), &PolyhedronMesh::set_hill_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_hill"), "set_hill_height", "get_hill_height");

  ClassDB::bind_method(D_METHOD("get_mountain_height"), &PolyhedronMesh::get_mountain_height);
  ClassDB::bind_method(D_METHOD("set_mountain_height", "p_height"), &PolyhedronMesh::set_mountain_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height_mountain"), "set_mountain_height", "get_mountain_height");
}

void PolyhedronMesh::set_divisions(const int p_divisions) {
  _divisions = p_divisions > 1 ? p_divisions : 1;
  init();
}

void PolyhedronMesh::set_patch_resolution(const int p_patch_resolution) {
  _patch_resolution = p_patch_resolution;
  init();
}

void PolyhedronMesh::set_compression_factor(const float p_compression_factor) {
  _compression_factor = p_compression_factor;
  init();
}

void PolyhedronMesh::set_shader(const Ref<Shader> p_shader) {
  shader = p_shader;
  init();
}

// TODO: noise code copypasted from RidgeHexGridMap
void PolyhedronMesh::set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise) {
  _biomes_noise = p_biomes_noise;
  if (_biomes_noise.ptr()) {
    _biomes_noise->connect("changed", Callable(this, "init"));
    init();
  }
}

void PolyhedronMesh::set_noise(const Ref<FastNoiseLite> p_noise) {
  _noise = p_noise;
  if (_noise.ptr()) {
    _noise->connect("changed", Callable(this, "init"));
    init();
  }
}

// TODO: textures code copypasted from RidgeHexGridMap
void PolyhedronMesh::set_plain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::PLAIN] = p_texture;
  init();
}

void PolyhedronMesh::set_hill_texture(const Ref<Texture> p_texture) {
  _texture[Biome::HILL] = p_texture;
  init();
}

void PolyhedronMesh::set_water_texture(const Ref<Texture> p_texture) {
  _texture[Biome::WATER] = p_texture;
  init();
}

void PolyhedronMesh::set_mountain_texture(const Ref<Texture> p_texture) {
  _texture[Biome::MOUNTAIN] = p_texture;
  init();
}

// Heights
void PolyhedronMesh::set_plain_height(const float p_height) {
  _prism_heights[Biome::PLAIN] = p_height;
  init();
}

void PolyhedronMesh::set_hill_height(const float p_height) {
  _prism_heights[Biome::HILL] = p_height;
  init();
}

void PolyhedronMesh::set_water_height(const float p_height) {
  _prism_heights[Biome::WATER] = p_height;
  init();
}

void PolyhedronMesh::set_mountain_height(const float p_height) {
  _prism_heights[Biome::MOUNTAIN] = p_height;
  init();
}

int PolyhedronMesh::get_divisions() const { return _divisions; }
int PolyhedronMesh::get_patch_resolution() const { return _patch_resolution; }
float PolyhedronMesh::get_compression_factor() const { return _compression_factor; }
Ref<Shader> PolyhedronMesh::get_shader() const { return shader; }
Ref<FastNoiseLite> PolyhedronMesh::get_biomes_noise() const { return _biomes_noise; }
Ref<FastNoiseLite> PolyhedronMesh::get_noise() const { return _noise; }
Ref<Texture> PolyhedronMesh::get_plain_texture() const { return _texture.find(Biome::PLAIN)->second; }
Ref<Texture> PolyhedronMesh::get_hill_texture() const { return _texture.find(Biome::HILL)->second; }
Ref<Texture> PolyhedronMesh::get_water_texture() const { return _texture.find(Biome::WATER)->second; }
Ref<Texture> PolyhedronMesh::get_mountain_texture() const { return _texture.find(Biome::MOUNTAIN)->second; }
float PolyhedronMesh::get_plain_height() const { return _prism_heights.find(Biome::PLAIN)->second; }
float PolyhedronMesh::get_hill_height() const { return _prism_heights.find(Biome::HILL)->second; }
float PolyhedronMesh::get_water_height() const { return _prism_heights.find(Biome::WATER)->second; }
float PolyhedronMesh::get_mountain_height() const { return _prism_heights.find(Biome::MOUNTAIN)->second; }

template <typename TGON>
void PolyhedronMesh::insert_to_polygons(Vector3 start_point, float diameter, float R, float r, int i, int j,
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
  for (float angle = -PI / 6, i = 0; i < 6; i++, angle += PI / 3) {
    Vector3 point(center.x + cos(angle) * R, 0, center.z + sin(angle) * R);
    if (!out_of_triangle(point.x, point.z)) {
      polygon.add_point(map2d_to_3d(Vector2(point.x, point.z), icosahedron_points[triangle.x],
                                    icosahedron_points[triangle.y], icosahedron_points[triangle.z]));
    }
  }
}

std::pair<std::vector<Hexagon>, std::vector<Pentagon>> PolyhedronMesh::calculate_shapes() const {
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

void PolyhedronMesh::clear() {
  _hexagon_meshes.clear();
  _pentagon_meshes.clear();

  clean_children(*this);
}

void PolyhedronMesh::process_hexagons(std::vector<Hexagon> hexagons) {
  int id = 0;
  std::unordered_map<int, float> altitudes;
  for (const auto& hex : hexagons) {
    if (_biomes_noise.ptr()) {
      altitudes[id] = _biomes_noise->get_noise_3dv(hex.center());
    } else {
      altitudes[id] = 0;
    }
    ++id;
  }

  float min_z = std::numeric_limits<float>::max();
  float max_z = std::numeric_limits<float>::min();
  for (auto [id, a] : altitudes) {
    min_z = std::min(min_z, a);
    max_z = std::max(max_z, a);
  }

  id = 0;
  BiomeCalculator biome_calculator;
  for (const auto& hex : hexagons) {
    Biome biome = biome_calculator.calculate_biome(min_z, max_z, altitudes[id]);

    Ref<ShaderMaterial> mat;
    mat.instantiate();
    if (shader.ptr()) {
      mat->set_shader(shader);
    }
    if (_texture[biome].ptr()) {
      mat->set_shader_parameter("water_texture", _texture[Biome::WATER].ptr());
      mat->set_shader_parameter("plain_texture", _texture[Biome::PLAIN].ptr());
      mat->set_shader_parameter("hill_texture", _texture[Biome::HILL].ptr());
      mat->set_shader_parameter("mountain_texture", _texture[Biome::MOUNTAIN].ptr());

      mat->set_shader_parameter("water_height", _prism_heights[Biome::WATER]);
      mat->set_shader_parameter("plain_height", _prism_heights[Biome::PLAIN]);
      mat->set_shader_parameter("hill_height", _prism_heights[Biome::HILL]);
      mat->set_shader_parameter("mountain_height", _prism_heights[Biome::MOUNTAIN]);
    }

    // ridge_processor.configure_cell(hex, biome, id, mat, *this);
    _prism_processor.configure_cell(hex, biome, id, mat, *this);
  }

  // ridge_processor.process(*this);
  _prism_processor.process(*this);
}

void PolyhedronMesh::process_pentagons(std::vector<Pentagon> pentatons) {
  int id = 0;
  for (const auto& pentagon : pentatons) {
    Ref<ShaderMaterial> mat;
    mat.instantiate();
    if (shader.ptr()) {
      mat->set_shader(shader);
    }
    if (_texture.size() == 4) {
      mat->set_shader_parameter("water_texture", _texture[Biome::WATER].ptr());
      mat->set_shader_parameter("plain_texture", _texture[Biome::PLAIN].ptr());
      mat->set_shader_parameter("hill_texture", _texture[Biome::HILL].ptr());
      mat->set_shader_parameter("mountain_texture", _texture[Biome::MOUNTAIN].ptr());

      mat->set_shader_parameter("water_height", _prism_heights[Biome::WATER]);
      mat->set_shader_parameter("plain_height", _prism_heights[Biome::PLAIN]);
      mat->set_shader_parameter("hill_height", _prism_heights[Biome::HILL]);
      mat->set_shader_parameter("mountain_height", _prism_heights[Biome::MOUNTAIN]);
    }

    PentagonMeshParams params{.id = id, .divisions = _divisions, .material = mat};
    Ref<PentMesh> m = make_pent_mesh(pentagon, params);

    auto* mi = memnew(MeshInstance3D());
    mi->set_mesh(m);

    add_child(mi);
    _pentagon_meshes.push_back(mi);
    ++id;
  }

  set_pentagons();
}

void PolyhedronMesh::init() {
  clear();

  auto [hexagons, pentagons] = calculate_shapes();

  process_hexagons(hexagons);
  process_pentagons(pentagons);
}

void PolyhedronMesh::set_pentagons() {
  for (auto& mesh : _pentagon_meshes) {
    PentMesh* pent_mesh = dynamic_cast<PentMesh*>(mesh->get_mesh().ptr());
    pent_mesh->recalculate_all_except_vertices();
    pent_mesh->update();
  }
}

}  // namespace sota
