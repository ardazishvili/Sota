#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/material.hpp"
#include "godot_cpp/classes/shader_material.hpp"

using Material = godot::Material;
using ShaderMaterial = godot::ShaderMaterial;
#else
#endif
