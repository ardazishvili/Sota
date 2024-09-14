#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

template <typename T>
using Ref = godot::Ref<T>;

using RefCounted = godot::RefCounted;
#else

#include "core/object/ref_counted.h"

#endif
