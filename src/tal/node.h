#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/node3d.hpp"

using Node3D = godot::Node3D;
using Node = godot::Node;
#else
#include "scene/3d/node_3d.h"
#include "scene/main/node.h"

using Node3D = Node3D;
using Node = Node;
#endif
