#include "core/utils.h"

#include <cmath>

#include "algo/constants.h"  //

namespace sota {

float radius(float diameter) { return diameter / 2.0f; }

float small_radius(float diameter) { return radius(diameter) * std::sqrt(3) / 2; }

float pointy_top_x_offset(float diameter) { return small_radius(diameter) * 2; }
float pointy_top_y_offset(float diameter) { return diameter * 3.0f / 4.0f; }

bool epsilonEqual(float lhs, float rhs) { return std::abs(lhs - rhs) < EPSILON; }
bool epsilonEqual(float lhs, float rhs, float tol) { return std::abs(lhs - rhs) < tol; }
bool epsilonNotEqual(float lhs, float rhs) { return !epsilonEqual(lhs, rhs); }
double cosrp(double a, double b, double mu) {
  double mu2 = (1 - std::cos(mu * PI)) / 2;
  return a * (1 - mu2) + b * mu2;
};

}  // namespace sota
