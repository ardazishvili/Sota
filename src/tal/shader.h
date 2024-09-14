#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/shader.hpp"

using Shader = godot::Shader;
#else
#include "scene/resources/shader.h"

using Shader = Shader;
#endif
