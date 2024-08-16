#include "ridge.h"

#include "godot_cpp/variant/vector3.hpp"

namespace sota {

using namespace gd;

Ridge::Ridge(Vector3 start, Vector3 end) : _start(start), _end(end) {}

void Ridge::set_points(std::vector<Vector3> points) { _points = points; }

const std::vector<Vector3> Ridge::get_points() const { return _points; }

}  // namespace sota

