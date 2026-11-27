#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/callable.hpp"

using Callable = godot::Callable;
#else
#include "core/variant/callable.h"
#endif
