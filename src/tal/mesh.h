#pragma once

#ifdef SOTA_GDEXTENSION
#include "godot_cpp/classes/collision_shape3d.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/primitive_mesh.hpp"
#include "godot_cpp/classes/sphere_shape3d.hpp"
#include "godot_cpp/classes/static_body3d.hpp"

using PrimitiveMesh = godot::PrimitiveMesh;
using MeshInstance3D = godot::MeshInstance3D;
using StaticBody3D = godot::StaticBody3D;
using CollisionShape3D = godot::CollisionShape3D;
using SphereShape3D = godot::SphereShape3D;

#else
#endif
