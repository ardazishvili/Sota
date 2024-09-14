#include "polyhedron/PolyhedronRidgeProcessor.h"

#include "misc/utilities.h"
#include "plain_hex_mesh.h"
#include "polyhedron/hex_polyhedron.h"
#include "ridge_hex_mesh.h"

namespace sota {

/////////////////////////////////////////////////// TEMPORARY BROKEN

void PolyhedronRidgeProcessor::process(PolyhedronMesh& polyhedron_mesh) {
  // initial heights calculation
  float global_min_y = std::numeric_limits<float>::max();
  float global_max_y = std::numeric_limits<float>::min();
  for (auto& mesh : polyhedron_mesh._hexagon_meshes) {
    PlainHexMesh* plain_mesh = dynamic_cast<PlainHexMesh*>(mesh->get_mesh().ptr());
    plain_mesh->calculate_initial_heights();
    auto [mesh_min_z, mesh_max_z] = plain_mesh->get_min_max_height();
    global_min_y = std::min(global_min_y, mesh_min_z);
    global_max_y = std::max(global_max_y, mesh_max_z);
  }

  float amplitude = global_max_y - global_min_y;
  float compress = polyhedron_mesh._compression_factor / amplitude;
  for (auto& mesh : polyhedron_mesh._hexagon_meshes) {
    PlainHexMesh* plain_mesh = dynamic_cast<PlainHexMesh*>(mesh->get_mesh().ptr());
    plain_mesh->set_shift_compress(-global_min_y, compress);
  }

  // final heights calculation
  for (auto& mesh : polyhedron_mesh._hexagon_meshes) {
    PlainHexMesh* plain_mesh = dynamic_cast<PlainHexMesh*>(mesh->get_mesh().ptr());
    plain_mesh->calculate_final_heights();
    plain_mesh->calculate_normals();
    plain_mesh->update();
  }
}

void PolyhedronRidgeProcessor::configure_cell(Hexagon hex, Biome biome, int& id, Ref<ShaderMaterial> mat,
                                              PolyhedronMesh& polyhedron_mesh) {
  RidgeHexMeshParams params{
      .hex_mesh_params = HexMeshParams{.id = id,
                                       .diameter = (hex.points()[0] - hex.center()).length() * 2,
                                       .frame_state = false,
                                       .frame_offset = 0.0,
                                       .material = mat,
                                       .divisions = 1,
                                       .clip_options = ClipOptions{}},
      .plain_noise = polyhedron_mesh._noise,
      .ridge_noise = nullptr,
  };
  Ref<RidgeHexMesh> m = make_impl<PlainHexMesh>(hex, params);
  m->set_tesselation_type(HexMesh::TesselationType::Polyhedron);
  // m->set_id(id);

  // m->set_divisions(divisions);
  // m->set_plain_noise(noise);  // for ridge based
  // m->set_material(mat);
  m->set_tesselation_mode(TesselationMode::Recursive);
  // m->init();

  auto* mi = memnew(MeshInstance3D());
  mi->set_mesh(m);

  polyhedron_mesh.add_child(mi);
  polyhedron_mesh._hexagon_meshes.push_back(mi);
  ++id;
}

}  // namespace sota
