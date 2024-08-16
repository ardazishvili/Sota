#pragma once

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

}  // namespace sota
