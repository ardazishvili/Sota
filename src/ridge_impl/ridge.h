#pragma once

#include <vector>

#include "godot_cpp/variant/vector3.hpp"
#include "misc/types.h"

namespace sota {

class Ridge {
 public:
  Ridge(gd::Vector3 start, gd::Vector3 end);
  void set_points(std::vector<gd::Vector3> points);
  const std::vector<gd::Vector3> get_points() const;
  gd::Vector3 start() const { return _start; }
  gd::Vector3 end() const { return _end; }

 private:
  std::vector<gd::Vector3> _points;
  gd::Vector3 _start;
  gd::Vector3 _end;
};

}  // namespace sota
