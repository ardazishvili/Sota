#include "primitives/Pentagon.h"

#include "algo/constants.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "utils.h"

namespace sota {

using namespace godot;
void Pentagon::check() const {
  if (_points.size() != 5) {
    godot::UtilityFunctions::printerr("Pentagon has != 5 points");
    godot::UtilityFunctions::printerr(_points.size());
  }
}

std::vector<godot::Vector3> Pentagon::calculate_points(godot::Vector3 center, float diameter) {
  std::vector<godot::Vector3> result;
  float R = radius(diameter);

  for (int i = 0; i < 5; ++i) {
    result.push_back(center + godot::Vector3(std::cos(i * 2 * PI / 5) * R, 0, std::sin(i * 2 * PI / 5) * R));
  }
  return result;
}

Pentagon make_unit_pentagon() {
  Vector3 center = Vector3(0, 0, 0);
  return Pentagon(center, Pentagon::calculate_points(center, 1), Vector3(0, 1, 0));
}
}  // namespace sota
