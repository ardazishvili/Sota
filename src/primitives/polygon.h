#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "tal/vector3.h"

namespace sota {

class RegularPolygon {
 public:
  RegularPolygon(Vector3 center, std::vector<Vector3> points, Vector3 normal)
      : _center(center), _points(points), _normal(normal) {}
  RegularPolygon(Vector3 center, Vector3 normal) : _center(center), _normal(normal) {}

  RegularPolygon(const RegularPolygon& other) = default;
  RegularPolygon(RegularPolygon&& other) = default;
  RegularPolygon& operator=(const RegularPolygon& rhs) = default;
  RegularPolygon& operator=(RegularPolygon&& rhs) = default;
  virtual ~RegularPolygon() = default;

  // getters
  Vector3 center() const { return _center; }
  Vector3 normal() const { return _normal; }
  std::vector<Vector3> points() const { return _points; }

  // modifiers
  void add_point(const Vector3& p) { _points.push_back(p); }
  void sort_points();

  // validation
  virtual void check() const = 0;

 protected:
  Vector3 _center;
  std::vector<Vector3> _points;
  Vector3 _normal;
};

}  // namespace sota
