#pragma once

#include <map>  // for map
#include <memory>
#include <optional>
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "core/tile_mesh.h"  // for TileMesh
#include "discretizer.h"
#include "godot_cpp/core/memory.hpp"
#include "misc/mesh_instance_wrapper.h"
#include "misc/types.h"  // for Biome
#include "polygon.h"
#include "polyhedron/polyhedron_mesh_processor.h"
#include "polyhedron/polyhedron_noise_processor.h"
#include "polyhedron/polyhedron_prism_processor.h"
#include "polyhedron/polyhedron_ridge_processor.h"
#include "primitives/hexagon.h"
#include "primitives/pentagon.h"
#include "tal/arrays.h"  // for Vector3Array
#include "tal/engine.h"
#include "tal/material.h"  // for ShaderMaterial
#include "tal/mesh.h"
#include "tal/node.h"       // for Node3D
#include "tal/noise.h"      // for FastNoiseLite
#include "tal/reference.h"  // for Ref
#include "tal/shader.h"     // for Shader
#include "tal/texture.h"    // for Texture
#include "tal/vector3.h"    // for Vector3
#include "tal/vector3i.h"   // for Vector3i

namespace sota {
class Hexagon;
class Pentagon;

class PolygonWrapper : public Node3D {
 public:
  explicit PolygonWrapper(std::unique_ptr<RegularPolygon> polygon) : _id(CNT++), _polygon(std::move(polygon)) {}
  ~PolygonWrapper() = default;
  PolygonWrapper(const PolygonWrapper& other) = delete;
  PolygonWrapper(PolygonWrapper&& other) = default;
  PolygonWrapper& operator=(const PolygonWrapper& other) = delete;
  PolygonWrapper& operator=(PolygonWrapper&& other) = delete;

  // getters
  RegularPolygon* polygon() { return _polygon.get(); }
  Ref<TileMesh> mesh() { return _tile_mesh; }
  int id() const { return _id; }

  // setters
  void set_mesh(Ref<TileMesh> tile_mesh, Node3D* parent) {
    parent->add_child(this);
    _tile_mesh = tile_mesh;
    _mesh_instance_wrapper = memnew(MeshInstanceWrapper(_tile_mesh->inner_mesh(), this));

    // _mesh_instance = memnew(MeshInstance3D());
    // _mesh_instance->set_mesh(_tile_mesh->inner_mesh());
    // add_child(_mesh_instance);

    // add_child(_mesh_instance_wrapper);
    // #ifdef SOTA_ENGINE
    //     if (Engine::get_singleton()->is_editor_hint()) {
    //       Node* root_scene = EditorInterface::get_singleton()->get_edited_scene_root();
    //       _mesh_instance->set_owner(root_scene);
    //     }
    // #endif
  }

 private:
  static int CNT;
  int _id;
  std::unique_ptr<RegularPolygon> _polygon;

  MeshInstance3D* _mesh_instance{nullptr};

  Ref<TileMesh> _tile_mesh;
  MeshInstanceWrapper* _mesh_instance_wrapper;
};

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

  std::vector<PolygonWrapper*> _hexagons;
  std::vector<PolygonWrapper*> _pentagons;

  static void _bind_methods();

  virtual void configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) = 0;
  virtual void configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat) = 0;
  virtual void process_cells() = 0;
  virtual void set_material_parameters(Ref<ShaderMaterial> mat) = 0;
  virtual void calculate_normals() = 0;
  void init();

  template <typename T>
  void process_ngons(std::vector<PolygonWrapper*>& ngons, float min_z, float max_z);

 private:
  friend class PolyhedronRidgeProcessor;
  friend class PolyhedronNoiseProcessor;
  friend class PolyhedronPrismProcessor;

  int _divisions{1};
  int _patch_resolution{1};
  mutable std::map<int, std::set<int>> _neighbours_map;

  std::pair<std::vector<PolygonWrapper*>, std::vector<PolygonWrapper*>> calculate_shapes() const;

  template <typename TGON>
  std::optional<PolygonWrapper*> insert_to_polygons(Vector3 start_point, float diameter, float R, float r, int i, int j,
                                                    Vector3Array icosahedron_points, Vector3i triangle,
                                                    std::map<Vector3i, PolygonWrapper*>& polygons) const;

  void clear();
};

}  // namespace sota
