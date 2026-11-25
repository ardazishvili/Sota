#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/camera3d.hpp"

using Camera3D = godot::Camera3D;
#else
#include "scene/3d/camera_3d.h"
#endif
