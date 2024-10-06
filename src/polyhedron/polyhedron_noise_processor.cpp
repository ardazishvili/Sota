#include "polyhedron/polyhedron_noise_processor.h"

#include <algorithm>  // for max, min
#include <limits>     // for numeric_limits

#include "core/hex_mesh.h"                // for HexMeshParams
#include "core/mesh.h"                    // for Orientation, Orient...
#include "core/pent_mesh.h"               // for PentagonMeshParams
#include "core/tile_mesh.h"               // for TileMesh
#include "misc/types.h"                   // for ClipOptions, Biome
#include "polyhedron/hex_polyhedron.h"    // for Polyhedron
#include "polyhedron/noise_polyhedron.h"  // for NoisePolyhedron
#include "primitives/hexagon.h"           // for Hexagon
#include "primitives/pentagon.h"          // for Pentagon
#include "ridge_impl/plain_mesh.h"        // for PlainMesh
#include "ridge_impl/ridge_mesh.h"        // for make_ridge_hex_mesh

namespace sota {

void PolyhedronNoiseProcessor::configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                                 Polyhedron& polyhedron) {
  auto noise_polyhedron = dynamic_cast<NoisePolyhedron*>(&polyhedron);
  RidgeHexMeshParams params{
      .hex_mesh_params = HexMeshParams{.id = id,
                                       .material = mat,
                                       .divisions = polyhedron._divisions,
                                       .clip_options = ClipOptions{},
                                       .tesselation_mode = TesselationMode::Recursive,
                                       .orientation = Orientation::Polyhedron},
      .plain_noise = noise_polyhedron->_plain_noise,
      .ridge_noise = nullptr,
  };

  auto* mi = memnew(MeshInstance3D());
  auto& hex = *dynamic_cast<Hexagon*>(wrapper.polygon());
  Ref<PlainMesh> plain_mesh = make_ridge_hex_mesh<PlainMesh>(hex, params);
  mi->set_mesh(plain_mesh->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(plain_mesh, &polyhedron);
  ++id;
}

void PolyhedronNoiseProcessor::configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id,
                                                  Ref<ShaderMaterial> mat, Polyhedron& polyhedron) {
  auto noise_polyhedron = dynamic_cast<NoisePolyhedron*>(&polyhedron);
  RidgePentagonMeshParams params{
      .pentagon_mesh_params = PentagonMeshParams{.id = id,
                                                 .divisions = polyhedron._divisions,
                                                 .material = mat,
                                                 .tesselation_mode = TesselationMode::Recursive,
                                                 .orientation = Orientation::Polyhedron},
      .plain_noise = noise_polyhedron->_plain_noise,
      .ridge_noise = nullptr,
  };

  auto* mi = memnew(MeshInstance3D());
  auto& pentagon = *dynamic_cast<Pentagon*>(wrapper.polygon());
  Ref<PlainMesh> plain_mesh = make_ridge_pentagon_mesh<PlainMesh>(pentagon, params);
  mi->set_mesh(plain_mesh->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(plain_mesh, &polyhedron);
  ++id;
}

void PolyhedronNoiseProcessor::process_meshes(Polyhedron& polyhedron, std::vector<Ref<TileMesh>>& meshes) {
  auto noise_polyhedron = dynamic_cast<NoisePolyhedron*>(&polyhedron);

  // initial heights calculation
  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  for (Ref<TileMesh>& mesh : meshes) {
    PlainMesh* plain_mesh = dynamic_cast<PlainMesh*>(mesh.ptr());
    plain_mesh->calculate_initial_heights();
    plain_mesh->calculate_normals();
    plain_mesh->update();
    auto [mesh_min_z, mesh_max_z] = plain_mesh->get_min_max_height();
    global_min_y = std::min(global_min_y, mesh_min_z);
    global_max_y = std::max(global_max_y, mesh_max_z);
  }

  float amplitude = global_max_y - global_min_y;
  float compress = noise_polyhedron->_compression_factor / amplitude;
  for (auto& mesh : meshes) {
    PlainMesh* plain_mesh = dynamic_cast<PlainMesh*>(mesh.ptr());
    plain_mesh->set_shift_compress(-global_min_y, compress);
  }

  // final heights calculation
  for (auto& mesh : meshes) {
    PlainMesh* plain_mesh = dynamic_cast<PlainMesh*>(mesh.ptr());
    float approx_diameter = meshes[0]->inner_mesh()->get_R() * 2;
    plain_mesh->calculate_final_heights(_distance_map, approx_diameter, polyhedron._divisions);
    plain_mesh->recalculate_all_except_vertices();
    plain_mesh->update();
  }
}

void PolyhedronNoiseProcessor::process(Polyhedron& polyhedron) {
  std::vector<Ref<TileMesh>> all_meshes;

  std::transform(polyhedron._hexagons.begin(), polyhedron._hexagons.end(), std::back_inserter(all_meshes),
                 [](PolygonWrapper& wrapper) { return wrapper.mesh(); });
  std::transform(polyhedron._pentagons.begin(), polyhedron._pentagons.end(), std::back_inserter(all_meshes),
                 [](PolygonWrapper& wrapper) { return wrapper.mesh(); });

  process_meshes(polyhedron, all_meshes);
}

}  // namespace sota
