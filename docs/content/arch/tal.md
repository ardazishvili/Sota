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
