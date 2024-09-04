#pragma once

#include <vector>

#include "godot_cpp/variant/vector3.hpp"

namespace sota {

class RegularPolygon {
 public:
  RegularPolygon(godot::Vector3 center, std::vector<godot::Vector3> points, godot::Vector3 normal)
      : _center(center), _points(points), _normal(normal) {}
  RegularPolygon(godot::Vector3 center, godot::Vector3 normal) : _center(center), _normal(normal) {}

  RegularPolygon(const RegularPolygon& other) = default;
  RegularPolygon(RegularPolygon&& other) = default;
  RegularPolygon& operator=(const RegularPolygon& rhs) = default;
  RegularPolygon& operator=(RegularPolygon&& rhs) = default;
  virtual ~RegularPolygon() = default;

  // getters
  godot::Vector3 center() const { return _center; }
  godot::Vector3 normal() const { return _normal; }
  std::vector<godot::Vector3> points() const { return _points; }

  // modifiers
  void add_point(const godot::Vector3& p) { _points.push_back(p); }
  void sort_points();

  // validation
  virtual void check() const = 0;

 protected:
  godot::Vector3 _center;
  std::vector<godot::Vector3> _points;
  godot::Vector3 _normal;
};

}  // namespace sota
