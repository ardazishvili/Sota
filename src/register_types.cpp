#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "core/hex_grid_map.h"
#include "core/hex_mesh.h"
#include "godot_cpp/core/class_db.hpp"
#include "honeycomb/honeycomb.h"
#include "honeycomb/honeycomb_cell.h"
#include "honeycomb/honeycomb_honey.h"
#include "ridge_impl/ridge_hex_grid_map.h"
#include "ridge_impl/ridge_hex_mesh.h"

void initialize_sota_module(gd::ModuleInitializationLevel p_level) {
  if (p_level != gd::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  GDREGISTER_CLASS(sota::HexMesh);
  GDREGISTER_CLASS(sota::RidgeHexMesh);
  GDREGISTER_ABSTRACT_CLASS(sota::HexGridMap);
  GDREGISTER_CLASS(sota::RectHexGridMap);
  GDREGISTER_CLASS(sota::HexagonalHexGridMap);

  GDREGISTER_ABSTRACT_CLASS(sota::RidgeHexGridMap);
  GDREGISTER_CLASS(sota::RectRidgeHexGridMap);
  GDREGISTER_CLASS(sota::HexagonalRidgeHexGridMap);

  GDREGISTER_CLASS(sota::HoneycombCell);
  GDREGISTER_CLASS(sota::HoneycombHoney);

  GDREGISTER_ABSTRACT_CLASS(sota::Honeycomb);
  GDREGISTER_CLASS(sota::RectHoneycomb);
  GDREGISTER_CLASS(sota::HexagonalHoneycomb);
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
