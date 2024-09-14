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

#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/physics/collision_shape_3d.h"
#include "scene/3d/physics/static_body_3d.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/3d/sphere_shape_3d.h"

using PrimitiveMesh = PrimitiveMesh;
using MeshInstance3D = MeshInstance3D;
using StaticBody3D = StaticBody3D;
using CollisionShape3D = CollisionShape3D;
using SphereShape3D = SphereShape3D;

#endif
