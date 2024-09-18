#pragma once

#include <vector>

#include "misc/types.h"
#include "tal/vector3.h"

namespace sota {

class Ridge {
 public:
  Ridge(Vector3 start, Vector3 end);
  Ridge(const Ridge& other) = default;
  Ridge(Ridge&& other) = default;
  Ridge& operator=(const Ridge& other) = default;
  Ridge& operator=(Ridge&& other) = default;

  void set_points(std::vector<Vector3> points);
  const std::vector<Vector3> get_points() const;
  Vector3 start() const { return _start; }
  Vector3 end() const { return _end; }

 private:
  std::vector<Vector3> _points;
  Vector3 _start;
  Vector3 _end;
};

}  // namespace sota
