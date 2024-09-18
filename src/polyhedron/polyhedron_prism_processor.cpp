#include "polyhedron/polyhedron_prism_processor.h"

#include "core/hex_mesh.h"
#include "polyhedron/hex_polyhedron.h"
#include "primitives/pentagon.h"
#include "prism_hex_mesh.h"
#include "prism_pent_mesh.h"
#include "prism_polyhedron.h"
#include "tal/godot_core.h"
#include "tal/mesh.h"
#include "tal/reference.h"
#include "types.h"
#include "utils.h"

namespace sota {

void PolyhedronPrismProcessor::configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                              Polyhedron& polyhedron) {
  auto& prism_polyhedron = dynamic_cast<PrismPolyhedron&>(polyhedron);
  PrismHexMeshParams params{.hex_mesh_params = HexMeshParams{.id = id,
                                                             .material = mat,
                                                             .divisions = polyhedron._divisions,
                                                             .clip_options = ClipOptions(),
                                                             .tesselation_mode = TesselationMode::Recursive,
                                                             .orientation = Orientation::Polyhedron},
                            .height = prism_polyhedron._prism_heights[biome]};

  auto* mi = memnew(MeshInstance3D());
  Ref<PrismHexTile> prism_tile = Ref<PrismHexTile>(memnew(PrismHexTile(hex, params)));
  mi->set_mesh(prism_tile->inner_mesh());

  polyhedron.add_child(mi);
  polyhedron._hexagon_meshes.push_back(prism_tile);
  ++id;
}

void PolyhedronPrismProcessor::configure_cell(Pentagon pentagon, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                              Polyhedron& polyhedron) {
  auto& prism_polyhedron = dynamic_cast<PrismPolyhedron&>(polyhedron);
  PrismPentMeshParams params{.pent_mesh_params = PentagonMeshParams{.id = id,
                                                                    .divisions = polyhedron._divisions,
                                                                    .material = mat,
                                                                    .tesselation_mode = TesselationMode::Recursive,
                                                                    .orientation = Orientation::Polyhedron},
                             .height = prism_polyhedron._prism_heights[biome]};

  auto* mi = memnew(MeshInstance3D());
  Ref<PrismPentTile> prism_tile = Ref<PrismPentTile>(memnew(PrismPentTile(pentagon, params)));
  mi->set_mesh(prism_tile->inner_mesh());

  polyhedron.add_child(mi);
  polyhedron._pentagon_meshes.push_back(prism_tile);
  ++id;
}

void PolyhedronPrismProcessor::process(Polyhedron& polyhedron_mesh) {}

}  // namespace sota
