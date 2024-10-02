#include "polyhedron/polyhedron_ridge_processor.h"

#include <algorithm>      // for copy_if, find, max
#include <functional>     // for reference_wrapper
#include <iterator>       // for back_insert_iterator
#include <limits>         // for numeric_limits
#include <memory>         // for make_unique, alloca...
#include <numeric>        // for accumulate
#include <optional>       // for optional
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector, vector<>::i...

#include "core/hex_mesh.h"   // for HexMeshParams
#include "core/mesh.h"       // for Orientation, Orient...
#include "core/pent_mesh.h"  // for PentagonMeshParams
#include "core/tile_mesh.h"  // for TileMesh
#include "misc/discretizer.h"
#include "misc/types.h"                   // for Biome, ClipOptions
#include "misc/utilities.h"               // for get_biome, create_h...
#include "polyhedron/hex_polyhedron.h"    // for Polyhedron
#include "polyhedron/ridge_polyhedron.h"  // for RidgePolyhedron
#include "primitives/hexagon.h"           // for Hexagon
#include "primitives/pentagon.h"          // for Pentagon
#include "ridge_impl/ridge_config.h"      // for RidgeConfig
#include "ridge_impl/ridge_group.h"       // for RidgeGroup, GroupOf...
#include "ridge_impl/ridge_mesh.h"        // for RidgeMesh, RidgeHex...
#include "ridge_impl/ridge_set.h"         // for RidgeSet
#include "tal/godot_core.h"               // for print, printerr
#include "tal/reference.h"                // for Ref
#include "tal/vector3.h"                  // for Vector3

namespace sota {

PolyhedronRidgeProcessor::PolyhedronRidgeProcessor() {
  _ridge_config = RidgeConfig{
      .variation_min_bound = 0.0,
      .variation_max_bound = 0.02,
      .top_ridge_offset = 0.1,
      .bottom_ridge_offset = -0.075,
  };
}

void PolyhedronRidgeProcessor::configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                                 Polyhedron& polyhedron) {
  auto& ridge_polyhedron = dynamic_cast<RidgePolyhedron&>(polyhedron);
  RidgeHexMeshParams params{
      .hex_mesh_params = HexMeshParams{.id = id,
                                       .material = mat,
                                       .divisions = polyhedron._divisions,
                                       .clip_options = ClipOptions{},
                                       .tesselation_mode = TesselationMode::Recursive,
                                       .orientation = Orientation::Polyhedron},
      .plain_noise = ridge_polyhedron._noise,
      .ridge_noise = nullptr,
  };

  auto* mi = memnew(MeshInstance3D());
  auto& hex = *dynamic_cast<Hexagon*>(wrapper.polygon());
  Ref<RidgeMesh> ridge_mesh = create_ridge_mesh(biome, hex, params);
  mi->set_mesh(ridge_mesh->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(ridge_mesh);
  ++id;
}

void PolyhedronRidgeProcessor::configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id,
                                                  Ref<ShaderMaterial> mat, Polyhedron& polyhedron) {
  auto& ridge_polyhedron = dynamic_cast<RidgePolyhedron&>(polyhedron);
  RidgePentagonMeshParams params{
      .pentagon_mesh_params = PentagonMeshParams{.id = id,
                                                 .divisions = polyhedron._divisions,
                                                 .material = mat,
                                                 .tesselation_mode = TesselationMode::Recursive,
                                                 .orientation = Orientation::Polyhedron},
      .plain_noise = ridge_polyhedron._noise,
      .ridge_noise = nullptr,
  };

  auto* mi = memnew(MeshInstance3D());
  auto& pentagon = *dynamic_cast<Pentagon*>(wrapper.polygon());
  Ref<RidgeMesh> ridge_mesh = create_ridge_mesh(biome, pentagon, params);
  mi->set_mesh(ridge_mesh->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(ridge_mesh);
  ++id;
}

void PolyhedronRidgeProcessor::set_neighbours() {
  int pentagon_cnt = 0;
  for (auto& p : _polyhedron_mesh->_neighbours_map) {
    int cur_id = p.first;
    std::set<int> neighbours_ids = p.second;
    auto cur_it = std::find_if(_meshes_wrapped.begin(), _meshes_wrapped.end(),
                               [cur_id](auto* wrapper) { return wrapper->id() == cur_id; });

    if (cur_it == _meshes_wrapped.end()) {
      printerr("Can't find PolygonWrapper object");
      continue;
    }
    PolygonWrapper* cur_wrapper = *cur_it;
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(cur_wrapper->mesh().ptr());
    Neighbours neighbours_meshes;
    for (auto n_id : neighbours_ids) {
      auto n_it = std::find_if(_meshes_wrapped.begin(), _meshes_wrapped.end(),
                               [n_id](auto* wrapper) { return wrapper->id() == n_id; });

      if (n_it == _meshes_wrapped.end()) {
        printerr("Can't find PolygonWrapper object");
        continue;
      }
      neighbours_meshes.push_back((*n_it)->mesh().ptr());
    }

    if (neighbours_meshes.size() == 5) {
      ++pentagon_cnt;
    } else if (neighbours_meshes.size() != 6) {
      printerr("Number of neighbours in PolyhedronRidgeProcessor is not 5 or 6");
    }

    ridge_mesh->set_neighbours(neighbours_meshes);
  }
  if (pentagon_cnt != 12) {
    printerr("Number of pentagons in PolyhedronRidgeProcessor is not 12");
  }
}

void dfs(TileMesh* cur, GroupOfRidgeMeshes& cur_group, std::unordered_set<TileMesh*>& visited, Biome biome) {
  auto opt_biome = get_biome(cur);
  if (!opt_biome) {
    print("Unknown biome while dfs'ing, return");
    return;
  }
  if (visited.contains(cur) || opt_biome.value() != biome) {
    return;
  }

  visited.insert(cur);
  RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(cur);
  cur_group.emplace_back(ridge_mesh);

  for (auto* n : ridge_mesh->get_neighbours()) {
    dfs(n, cur_group, visited, biome);
  }
}

void PolyhedronRidgeProcessor::init_biomes() {
  // Biome groups calculation
  _mountain_groups.clear();
  _water_groups.clear();
  _plain_groups.clear();
  _hill_groups.clear();
  std::unordered_set<TileMesh*> visited;
  for (PolygonWrapper* wrapper : _meshes_wrapped) {
    if (visited.contains(wrapper->mesh().ptr())) {
      continue;
    }
    GroupOfRidgeMeshes cur_group;
    auto opt_biome = get_biome(wrapper->mesh().ptr());
    if (!opt_biome) {
      print("Unknown biome, return");
      return;
    }
    Biome biome = opt_biome.value();
    dfs(wrapper->mesh().ptr(), cur_group, visited, biome);
    if (biome == Biome::PLAIN) {
      _plain_groups.emplace_back(cur_group);
    } else if (biome == Biome::HILL) {
      _hill_groups.emplace_back(cur_group);
    } else if (biome == Biome::WATER) {
      _water_groups.emplace_back(cur_group, std::make_unique<RidgeSet>(_ridge_config));
    } else if (biome == Biome::MOUNTAIN) {
      _mountain_groups.emplace_back(cur_group, std::make_unique<RidgeSet>(_ridge_config));
    }
  }
}

void PolyhedronRidgeProcessor::set_group_neighbours() {
  auto processor = [](const GroupOfRidgeMeshes& g) {
    for (RidgeMesh* ridge_mesh : g) {
      Neighbours group_neighbours;
      Neighbours all_neighbours = ridge_mesh->get_neighbours();
      std::copy_if(all_neighbours.begin(), all_neighbours.end(), std::back_inserter(group_neighbours),
                   [g](TileMesh* n) {
                     RidgeMesh* ridge_n = dynamic_cast<RidgeMesh*>(n);
                     return std::find(g.begin(), g.end(), ridge_n) != g.end();
                   });
      ridge_mesh->set_neighbours(group_neighbours);
    }
  };
  for (RidgeGroup& ridge_group : all_groups()) {
    ridge_group.fmap(processor);
  }
}

void PolyhedronRidgeProcessor::process_meshes() {
  set_neighbours();
  init_biomes();
  set_group_neighbours();

  // print_biomes();

  for (RidgeGroup& group : _mountain_groups) {
    group.init_ridges(_distance_map, _ridge_config.top_ridge_offset, _polyhedron_mesh->_divisions);
  }

  for (RidgeGroup& group : _water_groups) {
    group.init_ridges(_distance_map, _ridge_config.bottom_ridge_offset, _polyhedron_mesh->_divisions);
  }

  // initial heights calculation
  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  for (PolygonWrapper* wrapper : _meshes_wrapped) {
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(wrapper->mesh().ptr());
    ridge_mesh->calculate_initial_heights();
    ridge_mesh->calculate_normals();
    ridge_mesh->update();
    auto [mesh_min_z, mesh_max_z] = ridge_mesh->get_min_max_height();
    global_min_y = std::min(global_min_y, mesh_min_z);
    global_max_y = std::max(global_max_y, mesh_max_z);
  }

  float amplitude = global_max_y - global_min_y;
  float compress = _polyhedron_mesh->_compression_factor / amplitude;
  for (PolygonWrapper* wrapper : _meshes_wrapped) {
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(wrapper->mesh().ptr());
    ridge_mesh->set_shift_compress(-global_min_y, compress);
  }

  // final heights calculation
  for (PolygonWrapper* wrapper : _meshes_wrapped) {
    RidgeMesh* ridge_mesh = dynamic_cast<RidgeMesh*>(wrapper->mesh().ptr());

    float approx_diameter = _meshes_wrapped[0]->mesh()->inner_mesh()->get_R() * 2;
    ridge_mesh->calculate_final_heights(_distance_map, approx_diameter, _polyhedron_mesh->_divisions);
    ridge_mesh->recalculate_all_except_vertices();
    ridge_mesh->update();
  }
}

void PolyhedronRidgeProcessor::init(Polyhedron* polyhedron) {
  _meshes_wrapped.clear();
  _polyhedron_mesh = dynamic_cast<RidgePolyhedron*>(polyhedron);
  std::transform(polyhedron->_hexagons.begin(), polyhedron->_hexagons.end(), std::back_inserter(_meshes_wrapped),
                 [](PolygonWrapper& wrapper) { return &wrapper; });
  std::transform(polyhedron->_pentagons.begin(), polyhedron->_pentagons.end(), std::back_inserter(_meshes_wrapped),
                 [](PolygonWrapper& wrapper) { return &wrapper; });

  float all_r =
      std::accumulate(_meshes_wrapped.begin(), _meshes_wrapped.end(), 0.0f,
                      [](float acc, PolygonWrapper* wrapper) { return acc + wrapper->mesh()->inner_mesh()->get_R(); });
  _R_average = all_r / _meshes_wrapped.size();
}

void PolyhedronRidgeProcessor::process(Polyhedron& polyhedron_mesh) {
  init(&polyhedron_mesh);

  process_meshes();
}

}  // namespace sota
