#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/ref.hpp"

template <typename T>
using Ref = godot::Ref<T>;

using RefCounted = godot::RefCounted;
#else
#endif
