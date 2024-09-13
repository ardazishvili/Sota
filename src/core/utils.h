#pragma once

#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"

namespace sota {

constexpr double EPSILON = 0.0001;

float radius(float diameter);
float small_radius(float diameter);

float pointy_top_x_offset(float diameter);
float pointy_top_y_offset(float diameter);

template <typename T>
bool is_odd(T t) {
  return t & 1;
}

bool epsilonEqual(float lhs, float rhs);
bool epsilonEqual(float lhs, float rhs, float tol);
bool epsilonNotEqual(float lhs, float rhs);
double cosrp(double a, double b, double mu);

auto ico_points() -> godot::PackedVector3Array;
auto ico_indices() -> godot::Array;
auto barycentric(godot::Vector2 point) -> godot::Vector3;
auto map2d_to_3d(godot::Vector2 point, godot::Vector3 s1, godot::Vector3 s2, godot::Vector3 s3) -> godot::Vector3;

template <typename T, typename... Args>
T* make_non_ref(Args... args) {
  return memnew(T(args...));
}

}  // namespace sota
