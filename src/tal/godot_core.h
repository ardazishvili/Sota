#pragma once

#ifdef SOTA_GDEXTENSION
#include <utility>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

using ClassDB = godot::ClassDB;
using UtilityFunctions = godot::UtilityFunctions;
using Variant = godot::Variant;
using PropertyInfo = godot::PropertyInfo;
using ModuleInitializationLevel = godot::ModuleInitializationLevel;
using GDExtensionBinding = godot::GDExtensionBinding;
constexpr godot::PropertyHint PROPERTY_HINT_RESOURCE_TYPE = godot::PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
constexpr godot::ModuleInitializationLevel MODULE_INITIALIZATION_LEVEL_SCENE =
    godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE;

template <typename... Args>
auto D_METHOD(Args&&... args) -> decltype(godot::D_METHOD(std::forward<Args>(args)...)) {
  return godot::D_METHOD(std::forward<Args>(args)...);
}

#else
#endif
