#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/string.hpp"

using String = godot::String;
#else
#include "core/string/ustring.h"
#endif
