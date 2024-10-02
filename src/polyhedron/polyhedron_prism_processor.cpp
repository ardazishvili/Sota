#include "polyhedron/polyhedron_prism_processor.h"

#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "core/hex_mesh.h"               // for HexMeshParams, HexMesh
#include "core/mesh.h"                   // for Orientation, Orient...
#include "core/pent_mesh.h"              // for PentagonMeshParams
#include "core/tile_mesh.h"              // for TileMesh
#include "misc/types.h"                  // for Biome, ClipOptions
#include "polyhedron/hex_polyhedron.h"   // for Polyhedron
#include "primitives/hexagon.h"          // for Hexagon
#include "primitives/pentagon.h"         // for Pentagon
#include "prism_impl/prism_hex_mesh.h"   // for PrismHexTile, Prism...
#include "prism_impl/prism_pent_mesh.h"  // for PrismPentTile, Pris...
#include "prism_polyhedron.h"            // for PrismPolyhedron
#include "tal/material.h"                // for ShaderMaterial
#include "tal/mesh.h"                    // for MeshInstance3D
#include "tal/reference.h"               // for Ref

namespace sota {

void PolyhedronPrismProcessor::configure_hexagon(PolygonWrapper& wrapper, Biome biome, int& id, Ref<ShaderMaterial> mat,
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
  auto& hex = *dynamic_cast<Hexagon*>(wrapper.polygon());
  Ref<PrismHexTile> prism_tile = Ref<PrismHexTile>(memnew(PrismHexTile(hex, params)));
  mi->set_mesh(prism_tile->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(prism_tile);
  ++id;
}

void PolyhedronPrismProcessor::configure_pentagon(PolygonWrapper& wrapper, Biome biome, int& id,
                                                  Ref<ShaderMaterial> mat, Polyhedron& polyhedron) {
  auto& prism_polyhedron = dynamic_cast<PrismPolyhedron&>(polyhedron);
  PrismPentMeshParams params{.pent_mesh_params = PentagonMeshParams{.id = id,
                                                                    .divisions = polyhedron._divisions,
                                                                    .material = mat,
                                                                    .tesselation_mode = TesselationMode::Recursive,
                                                                    .orientation = Orientation::Polyhedron},
                             .height = prism_polyhedron._prism_heights[biome]};

  auto* mi = memnew(MeshInstance3D());
  auto& pentagon = *dynamic_cast<Pentagon*>(wrapper.polygon());
  Ref<PrismPentTile> prism_tile = Ref<PrismPentTile>(memnew(PrismPentTile(pentagon, params)));
  mi->set_mesh(prism_tile->inner_mesh());

  polyhedron.add_child(mi);
  wrapper.set_mesh(prism_tile);
  ++id;
}

void PolyhedronPrismProcessor::process(Polyhedron& polyhedron_mesh) {}

}  // namespace sota
