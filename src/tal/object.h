#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/object.hpp"

using Object = godot::Object;
#else
#include "core/object/object.h"

using Object = Object;
#endif
