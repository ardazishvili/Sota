#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "core/hex_grid.h"
#include "core/hex_mesh.h"
#include "godot_cpp/core/class_db.hpp"
#include "hex_polyhedron.h"
#include "honeycomb/honeycomb.h"
#include "honeycomb/honeycomb_cell.h"
#include "honeycomb/honeycomb_honey.h"
#include "mesh.h"
#include "pent_mesh.h"
#include "prism_impl/prism_hex_mesh.h"
#include "ridge_impl/ridge_hex_grid.h"
#include "ridge_impl/ridge_hex_mesh.h"

void initialize_sota_module(gd::ModuleInitializationLevel p_level) {
  if (p_level != gd::MODULE_INITIALIZATION_LEVEL_SCENE) {
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
  GDREGISTER_CLASS(sota::RidgeHexMesh);
  GDREGISTER_ABSTRACT_CLASS(sota::RidgeHexGrid);
  GDREGISTER_CLASS(sota::RectRidgeHexGrid);
  GDREGISTER_CLASS(sota::HexagonalRidgeHexGrid);

  // Grids made of pairs of hexes
  GDREGISTER_CLASS(sota::HoneycombCell);
  GDREGISTER_CLASS(sota::HoneycombHoney);
  GDREGISTER_ABSTRACT_CLASS(sota::Honeycomb);
  GDREGISTER_CLASS(sota::RectHoneycomb);
  GDREGISTER_CLASS(sota::HexagonalHoneycomb);

  // Volumetric grids of hexes
  GDREGISTER_CLASS(sota::PrismHexMesh);
  GDREGISTER_CLASS(sota::PolyhedronMesh);
}

void uninitialize_sota_module(gd::ModuleInitializationLevel p_level) {
  if (p_level != gd::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT sota_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                     const GDExtensionClassLibraryPtr p_library,
                                     GDExtensionInitialization *r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

  init_obj.register_initializer(initialize_sota_module);
  init_obj.register_terminator(uninitialize_sota_module);
  init_obj.set_minimum_library_initialization_level(gd::MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
