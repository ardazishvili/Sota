#include "polyhedron/PolyhedronPrismProcessor.h"

#include "polyhedron/hex_polyhedron.h"
#include "prism_hex_mesh.h"
#include "tal/mesh.h"
#include "tal/reference.h"

namespace sota {

void PolyhedronPrismProcessor::configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                              PolyhedronMesh& polyhedron_mesh) {
  Ref<PrismHexMesh> m(memnew(PrismHexMesh(hex)));
  m->set_height(polyhedron_mesh._prism_heights[biome]);
  m->set_tesselation_type(HexMesh::TesselationType::Polyhedron);
  m->set_id(id);

  m->set_divisions(polyhedron_mesh._divisions);
  m->set_material(mat);
  m->set_tesselation_mode(TesselationMode::Recursive);
  m->init();

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
