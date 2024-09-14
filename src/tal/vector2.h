#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/vector2.hpp"

using Vector2 = godot::Vector2;
#else
#include "core/math/vector2.h"

using Vector2 = Vector2;
#endif
