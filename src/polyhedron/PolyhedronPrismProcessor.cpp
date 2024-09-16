#include "polyhedron/PolyhedronPrismProcessor.h"

#include "hex_mesh.h"
#include "polyhedron/hex_polyhedron.h"
#include "prism_hex_mesh.h"
#include "tal/mesh.h"
#include "tal/reference.h"
#include "types.h"

namespace sota {

void PolyhedronPrismProcessor::configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                              PolyhedronMesh& polyhedron_mesh) {
  PrismHexMeshParams params{.hex_mesh_params = HexMeshParams{.id = id,
                                                             .material = mat,
                                                             .divisions = polyhedron_mesh._divisions,
                                                             .clip_options = ClipOptions(),
                                                             .tesselation_mode = TesselationMode::Recursive,
                                                             .tesselation_type = TesselationType::Polyhedron},
                            .height = polyhedron_mesh._prism_heights[biome]};
  Ref<PrismHexMesh> m = make_prism_mesh(hex, params);

  auto* mi = memnew(MeshInstance3D());
  mi->set_mesh(m);

  polyhedron_mesh.add_child(mi);
  polyhedron_mesh._hexagon_meshes.push_back(mi);
  ++id;
}

void PolyhedronPrismProcessor::process(PolyhedronMesh& polyhedron_mesh) {
  for (auto& mesh : polyhedron_mesh._hexagon_meshes) {
    PrismHexMesh* prism_mesh = dynamic_cast<PrismHexMesh*>(mesh->get_mesh().ptr());
    prism_mesh->calculate_heights();
    prism_mesh->recalculate_all_except_vertices();
    prism_mesh->update();
  }
}

}  // namespace sota
