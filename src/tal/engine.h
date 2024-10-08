#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/engine.hpp"

using Engine = godot::Engine;
using EditorInterface = godot::EditorInterface;

#else
#include "editor/editor_interface.h"
#include "core/config/engine.h"
#endif
