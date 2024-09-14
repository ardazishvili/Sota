#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/vector2i.hpp"

using Vector2i = godot::Vector2i;
#else
#include "core/math/vector2i.h"

using Vector2i = Vector2i;
#endif
