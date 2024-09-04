#pragma once

#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/shader.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/classes/texture.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "misc/types.h"
#include "polyhedron/PolyhedronPrismProcessor.h"
#include "polyhedron/PolyhedronRidgeProcessor.h"
#include "primitives/Hexagon.h"
#include "primitives/Pentagon.h"

namespace sota {

class PolyhedronMesh : public godot::Node3D {
  GDCLASS(PolyhedronMesh, godot::Node3D)

 public:
  PolyhedronMesh();

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_patch_resolution(const int p_patch_resolution);
  int get_patch_resolution() const;

  void set_compression_factor(const float p_compression_factor);
  float get_compression_factor() const;

  void set_shader(const godot::Ref<godot::Shader> p_shader);
  godot::Ref<godot::Shader> get_shader() const;

  // noise
  void set_biomes_noise(const godot::Ref<godot::FastNoiseLite> p_biomes_noise);
  godot::Ref<godot::FastNoiseLite> get_biomes_noise() const;

  void set_noise(const godot::Ref<godot::FastNoiseLite> p_noise);
  godot::Ref<godot::FastNoiseLite> get_noise() const;

  // textures
  void set_plain_texture(const godot::Ref<godot::Texture> p_texture);
  godot::Ref<godot::Texture> get_plain_texture() const;

  void set_hill_texture(const godot::Ref<godot::Texture> p_texture);
  godot::Ref<godot::Texture> get_hill_texture() const;

  void set_water_texture(const godot::Ref<godot::Texture> p_texture);
  godot::Ref<godot::Texture> get_water_texture() const;

  void set_mountain_texture(const godot::Ref<godot::Texture> p_texture);
  godot::Ref<godot::Texture> get_mountain_texture() const;

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

 private:
  friend class PolyhedronRidgeProcessor;
  friend class PolyhedronPrismProcessor;

  int divisions{1};
  int patch_resolution{1};
  float compression_factor{0.1};

  godot::Ref<godot::FastNoiseLite> noise;
  godot::Ref<godot::FastNoiseLite> biomes_noise;

  std::vector<godot::MeshInstance3D *> hexagon_meshes;
  std::vector<godot::MeshInstance3D *> pentagon_meshes;
  godot::Ref<godot::Shader> shader;
  std::unordered_map<Biome, godot::Ref<godot::Texture>> texture;
  std::unordered_map<Biome, float> prism_heights;

  std::pair<std::vector<Hexagon>, std::vector<Pentagon>> calculate_shapes() const;

  template <typename TGON>
  void insert_to_polygons(godot::Vector3 start_point, float diameter, float R, float r, int i, int j,
                          godot::PackedVector3Array icosahedron_points, godot::Vector3i triangle,
                          std::map<godot::Vector3i, TGON> &polygons) const;

  void init();
  void clear();

  void process_hexagons(std::vector<Hexagon> hexagons);
  void process_pentagons(std::vector<Pentagon> pentatons);

  void set_ridge_based_heights();
  void set_cylinder_based_heights();
  void set_pentagons();

  PolyhedronPrismProcessor prism_processor;
  PolyhedronRidgeProcessor ridge_processor;
};

}  // namespace sota
