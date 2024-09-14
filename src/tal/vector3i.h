#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/vector3i.hpp"

using Vector3i = godot::Vector3i;
#else
#include "core/math/vector3i.h"

using Vector3i = Vector3i;
#endif
