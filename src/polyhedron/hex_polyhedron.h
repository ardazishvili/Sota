#pragma once

#include <memory>

#include "misc/types.h"
#include "polyhedron/polyhedron_mesh_processor.h"
#include "polyhedron/polyhedron_noise_processor.h"
#include "polyhedron/polyhedron_prism_processor.h"
#include "polyhedron/polyhedron_ridge_processor.h"
#include "primitives/hexagon.h"
#include "primitives/pentagon.h"
#include "tal/arrays.h"
#include "tal/material.h"
#include "tal/mesh.h"
#include "tal/node.h"
#include "tal/noise.h"
#include "tal/shader.h"
#include "tal/texture.h"
#include "tal/vector3.h"
#include "tal/vector3i.h"

namespace sota {

class Polyhedron : public Node3D {
  GDCLASS(Polyhedron, Node3D)

 public:
  Polyhedron();
  Polyhedron(const Polyhedron& other) = delete;
  Polyhedron(Polyhedron&& other) = delete;
  // copying operator= defined inside GDCLASS
  Polyhedron& operator=(Polyhedron&& rhs) = delete;

  void set_divisions(const int p_divisions);
  int get_divisions() const;

  void set_patch_resolution(const int p_patch_resolution);
  int get_patch_resolution() const;

  void set_shader(const Ref<Shader> p_shader);
  Ref<Shader> get_shader() const;

  void set_biomes_noise(const Ref<FastNoiseLite> p_biomes_noise);
  Ref<FastNoiseLite> get_biomes_noise() const;

  // textures
  void set_plain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_plain_texture() const;

  void set_hill_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_hill_texture() const;

  void set_water_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_water_texture() const;

  void set_mountain_texture(const Ref<Texture> p_texture);
  Ref<Texture> get_mountain_texture() const;

 protected:
  Ref<Shader> _shader;
  Ref<FastNoiseLite> _biomes_noise;
  std::unordered_map<Biome, Ref<Texture>> _texture;

  static void _bind_methods();

  virtual void configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat) = 0;
  virtual void configure_cell(Pentagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat) = 0;
  virtual void process_cells() = 0;
  virtual void set_material_parameters(Ref<ShaderMaterial> mat) = 0;
  void init();

  template <typename T>
  void process_ngons(std::vector<T> ngons, float min_z, float max_z);

 private:
  friend class PolyhedronRidgeProcessor;
  friend class PolyhedronNoiseProcessor;
  friend class PolyhedronPrismProcessor;

  int _divisions{1};
  int _patch_resolution{1};

  std::vector<Ref<TileMesh>> _hexagon_meshes;
  std::vector<Ref<TileMesh>> _pentagon_meshes;

  std::pair<std::vector<Hexagon>, std::vector<Pentagon>> calculate_shapes() const;

  template <typename TGON>
  void insert_to_polygons(Vector3 start_point, float diameter, float R, float r, int i, int j,
                          Vector3Array icosahedron_points, Vector3i triangle, std::map<Vector3i, TGON>& polygons) const;

  void clear();
};

}  // namespace sota
