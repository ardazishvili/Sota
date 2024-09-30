#include "core/utils.h"

#include <cmath>  // for abs

#include <cmath>  // for sqrt, cos, sin

#include "algo/constants.h"  // for PI
#include "tal/arrays.h"      // for Array, Vector3Array
#include "tal/vector2.h"     // for Vector2
#include "tal/vector3.h"     // for Vector3
#include "tal/vector3i.h"    // for Vector3i

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

auto ico_points() -> Vector3Array {
  float s = 2 / sqrt(5);
  float c = 1 / sqrt(5);
  Vector3Array res;
  res.append(Vector3(0, 1, 0));
  for (int i = 0; i < 5; ++i) {
    res.append(Vector3(s * cos(i * 2 * PI / 5), c, s * sin(i * 2 * PI / 5)));
  }
  for (int i = 0; i < 6; ++i) {
    Vector3 v = res[i];
    res.append(Vector3(-v.x, -v.y, v.z));
  }
  return res;
}

auto ico_indices() -> Array {
  Array res;
  for (int i = 0; i < 5; ++i) {
    res.append(Vector3i((i + 1) % 5 + 1, 0, i + 1));
  }
  for (int i = 0; i < 5; ++i) {
    res.append(Vector3i((i + 1) % 5 + 7, 6, i + 7));
  }
  for (int i = 0; i < 5; ++i) {
    res.append(Vector3i(i + 1, (7 - i) % 5 + 7, (i + 1) % 5 + 1));
  }
  for (int i = 0; i < 5; ++i) {
    res.append(Vector3i(i + 1, (8 - i) % 5 + 7, (7 - i) % 5 + 7));
  }
  return res;
}
auto barycentric(Vector2 point) -> Vector3 {
  float x = point.x;
  float y = point.y;
  float l3 = y * 2.0 / sqrt(3.0);
  float l2 = x + 0.5 * (1 - l3);
  float l1 = 1 - l2 - l3;
  return Vector3(l1, l2, l3);
}

auto map2d_to_3d(Vector2 point, Vector3 s1, Vector3 s2, Vector3 s3) -> Vector3 {
  Vector3 bar_coords = barycentric(point);
  float l1 = bar_coords.x;
  float l2 = bar_coords.y;
  float l3 = bar_coords.z;
  if (std::abs(l3 - 1) < 1e-10) {
    return s3;
  }

  float l2s = l2 / (l1 + l2);
  Vector3 p12 = s1.slerp(s2, l2s);
  return p12.slerp(s3, l3);
}

}  // namespace sota
