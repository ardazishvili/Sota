#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"

template <typename T>
using TypedArray = godot::TypedArray<T>;
using Array = godot::Array;

using Vector2Array = godot::PackedVector2Array;
using Vector3Array = godot::PackedVector3Array;

using TangentsArray = godot::PackedFloat32Array;
using WeightsArray = godot::PackedFloat32Array;
using ColorsArray = godot::PackedColorArray;
using ByteArray = godot::PackedByteArray;
using IntArray = godot::PackedInt32Array;
#else
#endif

