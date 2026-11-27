Types Abstraction Layer
=======================

## Overview

Types Abstraction Layer (TAL) aims to abstract from specific types to some extension while developing Sota. Originally, Sota was just a Godot GDExtension. At some point it become reasonable to support Sota as a module of Godot editor. Few reasons for this:
1. GDExtension (at the moment of writing current stable is 4.3) is experimental. E.g. GDExtension plugin written for 4.3 won't work for 4.2.
2. Modules are deeper integrated in the engine.

Overview:
```{eval-rst}
.. include:: ../diagrams/tal.rst
```

Since number of used header files may be quite big, `tal` directory contains headers which aggregates headers from Godot with similar meaning, which commonly are used together. There is no clear policy and generally we can just map single TAL-header to Godot header.

As an example let's take a look on how printing to standart output is abstracted. It should be done differently without TAL:
```cpp
/* src/tal/godot_core.h */

#pragma once

#ifdef SOTA_GDEXTENSION
#include <gdextension_interface.h>
<...>
#include "godot_cpp/variant/utility_functions.hpp"
<...>
template <typename... Args>
auto print(Args&&... args) -> decltype(UtilityFunctions::print(std::forward<Args>(args)...)) {
  return UtilityFunctions::print(std::forward<Args>(args)...);
}

#else
<...>
#include "core/string/print_string.h"
template <typename... Args>
auto print(Args&&... args) -> decltype(print_line(std::forward<Args>(args)...)) {
  return print_line(std::forward<Args>(args)...);
}
<...>
#endif
```

## How build works
### GDExtensions

GDExtension uses `SConstruct` in root directory, which registers types via `register_types.h/cpp`. `SConstruct` file defines `SOTA_GDEXTENSION` define.

### Module

Same `register_types.h/cpp` files is also used by Godot's build system in case of module build. But module build requires `SCsub` file instead of `SConstruct` as well as `config.py` file. `SCsub` file defines `SOTA_MODULE` define.

## TAL-related coding policy

1. All code must be tested both as GDExtension and module. Differences between godot-cpp classes and native classes are rare but exist.
2. Sota classes must not have dependencies on native Godot classes as well on godot-cpp classes. If you need to use any headers, you are free to update existing TAL header with new lines (at least for 2 cases), or create new one.

## Header contracts

Include the TAL header that provides the symbol your file uses. Each header
includes the declarations for its listed symbols in both build modes, with the
editor and extension exceptions below. For example, a header that uses
`GDCLASS` includes `tal/wrapped.h` directly, even if it also includes a Godot
base class through another TAL header.

| Header in `src/tal` | Provides |
| --- | --- |
| `arrays.h` | `Array`, `TypedArray<T>`, `Vector2Array`, `Vector3Array`, `TangentsArray`, `WeightsArray`, `ColorsArray`, `ByteArray`, `IntArray` |
| `callable.h` | `Callable` |
| `camera.h` | `Camera3D` |
| `color.h` | `Color` |
| `engine.h` | `Engine`. Also provides `EditorInterface` for GDExtension and native editor builds. |
| `event.h` | `Input`, `InputEvent`, `InputEventMouse`, and the left-button and left/right button-mask constants |
| `godot_core.h` | `ClassDB`, `D_METHOD`, `GDREGISTER_CLASS`, `GDREGISTER_ABSTRACT_CLASS`, `Variant`, `PropertyInfo`, `PROPERTY_HINT_RESOURCE_TYPE`, `memnew`, module initialization levels, `print`, and `printerr`. Also provides the GDExtension entry-point declarations in extension builds. |
| `godot_string.h` | `String` |
| `material.h` | `Material`, `ShaderMaterial` |
| `mesh.h` | `PrimitiveMesh`, `MeshInstance3D`, `StaticBody3D`, `CollisionShape3D`, `SphereShape3D` |
| `node.h` | `Node`, `Node3D` |
| `noise.h` | `FastNoiseLite` |
| `object.h` | `Object` |
| `reference.h` | `Ref<T>`, `RefCounted` |
| `shader.h` | `Shader` |
| `texture.h` | `Texture` |
| `tmath.h` | `Math` |
| `vector2.h`, `vector2i.h`, `vector3.h`, `vector3i.h` | `Vector2`, `Vector2i`, `Vector3`, `Vector3i`, respectively |
| `wrapped.h` | `GDCLASS` |

When adding a TAL export, include the header that defines it in each build
branch and update the table. Treat other declarations brought in by those
headers as transitive dependencies.
