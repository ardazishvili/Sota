#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/fast_noise_lite.hpp"

using FastNoiseLite = godot::FastNoiseLite;
#else
#include "modules/noise/fastnoise_lite.h"

using FastNoiseLite = FastNoiseLite;
#endif
