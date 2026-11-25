#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/color.hpp"

using Color = godot::Color;
#else
#include "core/math/color.h"
#endif
