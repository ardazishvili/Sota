#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/node3d.hpp"

using Node3D = godot::Node3D;
using Node = godot::Node;
#else
#endif
