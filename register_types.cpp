#include "register_types.h"

#include "core/hex_grid.h"
#include "core/hex_mesh.h"
#include "core/mesh.h"
#include "core/pent_mesh.h"
#include "honeycomb/honeycomb.h"
#include "honeycomb/honeycomb_cell.h"
#include "honeycomb/honeycomb_honey.h"
#include "polyhedron/hex_polyhedron.h"
#include "prism_impl/prism_hex_mesh.h"
#include "ridge_impl/ridge_hex_grid.h"
#include "ridge_impl/ridge_hex_mesh.h"
#include "src/tal/godot_core.h"

/**
 * @brief register classes used in editor
 *
 * @param p_level
 *
 * Some classes registered as abstract while they are not abstract in terms of C++. The reason to do it is to prohibit
 * their instantiation outside of designed scope. For example, RidgeHexMesh class has meaning only if being used in
 * RidgeHexGrid (and it's subclasses). If RidgeHexMesh is registered via GDREGISTER_CLASS it's open to instantiation
 * e.g. in MeshInstance3D which makes no sence by design of RidgeHexGrid
 */
void initialize_Sota_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  // base types
  GDREGISTER_ABSTRACT_CLASS(sota::SotaMesh);
  GDREGISTER_CLASS(sota::HexMesh);
  GDREGISTER_CLASS(sota::PentMesh);

  // Grids made of hexes
  GDREGISTER_ABSTRACT_CLASS(sota::HexGrid);
  GDREGISTER_CLASS(sota::RectHexGrid);
  GDREGISTER_CLASS(sota::HexagonalHexGrid);

  // Grids made of ridge hexes - hexes based on global graph of ridges
  GDREGISTER_ABSTRACT_CLASS(sota::RidgeHexMesh);  // NOT ABSTRACT, see comment to `initialize_Sota_module`
  GDREGISTER_ABSTRACT_CLASS(sota::RidgeHexGrid);
  GDREGISTER_CLASS(sota::RectRidgeHexGrid);
  GDREGISTER_CLASS(sota::HexagonalRidgeHexGrid);

  // Grids made of pairs of hexes
  GDREGISTER_ABSTRACT_CLASS(sota::HoneycombCell);   // NOT ABSTRACT, see comment to `initialize_Sota_module`
  GDREGISTER_ABSTRACT_CLASS(sota::HoneycombHoney);  // NOT ABSTRACT, see comment to `initialize_Sota_module`
  GDREGISTER_ABSTRACT_CLASS(sota::Honeycomb);
  GDREGISTER_CLASS(sota::RectHoneycomb);
  GDREGISTER_CLASS(sota::HexagonalHoneycomb);

  // Volumetric grids of hexes
  GDREGISTER_CLASS(sota::PrismHexMesh);
  GDREGISTER_CLASS(sota::PolyhedronMesh);
}

void uninitialize_Sota_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

#ifdef SOTA_GDEXTENSION
extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT sota_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                     const GDExtensionClassLibraryPtr p_library,
                                     GDExtensionInitialization *r_initialization) {
  GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

  init_obj.register_initializer(initialize_Sota_module);
  init_obj.register_terminator(uninitialize_Sota_module);
  init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
#endif
