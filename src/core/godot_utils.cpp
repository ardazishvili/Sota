#include "core/godot_utils.h"

#include "tal/arrays.h"  // for TypedArray
#include "tal/node.h"    // for Node, Node3D
#include "tal/object.h"  // for Object

namespace sota {

void clean_children(Node3D& parent) {
  TypedArray<Node> children = parent.get_children();
  for (int i = 0; i < children.size(); ++i) {
    Node* child = Object::cast_to<Node>(children[i].operator Object*());
    parent.remove_child(child);
    child->queue_free();
  }
}
}  // namespace sota
