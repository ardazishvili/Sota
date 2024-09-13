#include "prism_impl/prism_hex_mesh.h"

#include "godot_cpp/classes/primitive_mesh.hpp"
#include "misc/types.h"

namespace sota {

using namespace gd;

void PrismHexMesh::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_height"), &PrismHexMesh::get_height);
  ClassDB::bind_method(D_METHOD("set_height", "p_height"), &PrismHexMesh::set_height);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height"), "set_height", "get_height");
}

void PrismHexMesh::set_height(const float p_height) {
  _height = p_height;
  init();
}

float PrismHexMesh::get_height() const { return _height; }

void PrismHexMesh::calculate_heights() {
  for (auto& v : vertices_) {
    v += v.normalized() * _height;
  }

  for (int i = 0; i < 6; ++i) {
    auto corner_points = _hex.points();

    auto a = corner_points[i];
    auto b = corner_points[(i + 1) % 6];

    auto c = a + a.normalized() * _height;
    auto d = b + b.normalized() * _height;

    vertices_.push_back(a);
    vertices_.push_back(b);
    vertices_.push_back(c);

    vertices_.push_back(c);
    vertices_.push_back(b);
    vertices_.push_back(d);
  }
}

}  // namespace sota
