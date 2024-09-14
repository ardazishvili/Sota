#pragma once
#include "tal/arrays.h"
#include "tal/vector2.h"
#include "tal/vector3.h"

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

auto ico_points() -> Vector3Array;
auto ico_indices() -> Array;
auto barycentric(Vector2 point) -> Vector3;
auto map2d_to_3d(Vector2 point, Vector3 s1, Vector3 s2, Vector3 s3) -> Vector3;

template <typename T, typename... Args>
T* make_non_ref(Args... args) {
  return memnew(T(args...));
}

}  // namespace sota
