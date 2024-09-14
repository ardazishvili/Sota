#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/vector3.hpp"

using Vector3 = godot::Vector3;

#else

#include "core/math/vector3.h"
using Vector3 = Vector3;

#endif
