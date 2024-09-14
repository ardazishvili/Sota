#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/input_event_mouse.hpp"

using InputEvent = godot::InputEvent;
using InputEventMouse = godot::InputEventMouse;

constexpr godot::MouseButtonMask MOUSE_BUTTON_MASK_LEFT = godot::MouseButtonMask::MOUSE_BUTTON_MASK_LEFT;
constexpr godot::MouseButtonMask MOUSE_BUTTON_MASK_RIGHT = godot::MouseButtonMask::MOUSE_BUTTON_MASK_RIGHT;
#else
#include "core/input/input_event.h"

constexpr MouseButtonMask MOUSE_BUTTON_MASK_LEFT = MouseButtonMask::LEFT;
constexpr MouseButtonMask MOUSE_BUTTON_MASK_RIGHT = MouseButtonMask::RIGHT;
#endif
