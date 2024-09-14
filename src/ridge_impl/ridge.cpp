#include "ridge.h"

#include "tal/vector3.h"

namespace sota {

Ridge::Ridge(Vector3 start, Vector3 end) : _start(start), _end(end) {}

void Ridge::set_points(std::vector<Vector3> points) { _points = points; }

const std::vector<Vector3> Ridge::get_points() const { return _points; }

}  // namespace sota

