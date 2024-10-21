#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/thread.hpp"

using Thread = godot::Thread;
#else
// TODO ADD

// using Thread = Thread;
#endif
