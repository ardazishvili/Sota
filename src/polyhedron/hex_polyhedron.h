#pragma once

#include "tal/arrays.h"
#include "tal/material.h"
#include "tal/mesh.h"
#include "tal/node.h"
#include "tal/noise.h"
#include "tal/shader.h"
#include "tal/texture.h"
#include "tal/vector3.h"
#include "tal/vector3i.h"
#include "misc/types.h"
#include "polyhedron/PolyhedronPrismProcessor.h"
#include "polyhedron/PolyhedronRidgeProcessor.h"
#include "primitives/Hexagon.h"
#include "primitives/Pentagon.h"

namespace sota {

class PolyhedronMesh : public Node3D {
  GDCLASS(PolyhedronMesh, Node3D)

 public:
  PolyhedronMesh();

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_patch_resolution(const int p_patch_resolution);
  int get_patch_resolution() const;

  void set_compression_factor(const float p_compression_factor);
  float get_compression_factor() const;

  void set_shader(const Ref<Shader> p_shader);
  Ref<Shader> get_shader() const;

  // noise
  void set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise);
  Ref<FastNoiseLite> get_biomes_noise() const;

  void set_noise(const Ref<FastNoiseLite> p_noise);
  Ref<FastNoiseLite> get_noise() const;

  // textures
  void set_plain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_plain_texture() const;

  void set_hill_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_hill_texture() const;

  void set_water_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_water_texture() const;

  void set_mountain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_mountain_texture() const;

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

  int _divisions{1};
  int _patch_resolution{1};
  float _compression_factor{0.1};

  Ref<FastNoiseLite> _noise;
  Ref<FastNoiseLite> _biomes_noise;

  std::vector<MeshInstance3D *> _hexagon_meshes;
  std::vector<MeshInstance3D *> _pentagon_meshes;
  Ref<Shader> shader;
  std::unordered_map<Biome, Ref<Texture>> _texture;
  std::unordered_map<Biome, float> _prism_heights;

  std::pair<std::vector<Hexagon>, std::vector<Pentagon>> calculate_shapes() const;

  template <typename TGON>
  void insert_to_polygons(Vector3 start_point, float diameter, float R, float r, int i, int j,
                          Vector3Array icosahedron_points, Vector3i triangle, std::map<Vector3i, TGON> &polygons) const;

  void init();
  void clear();

  void process_hexagons(std::vector<Hexagon> hexagons);
  void process_pentagons(std::vector<Pentagon> pentatons);

  void set_ridge_based_heights();
  void set_cylinder_based_heights();
  void set_pentagons();

  PolyhedronPrismProcessor _prism_processor;
  PolyhedronRidgeProcessor _ridge_processor;
};

}  // namespace sota
