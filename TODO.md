# Sota TODO

Backlog from the [open GitHub issues](https://github.com/ardazishvili/Sota/issues?q=is%3Aissue+is%3Aopen), reviewed on 2026-11-25.

## Godot compatibility and builds

- [ ] Finish Godot 4.6 support on the `4.6-stable` branch for GDExtension and module builds. A module build succeeded at `16f3de6`. [#99](https://github.com/ardazishvili/Sota/issues/99)
- [ ] Reproduce the reported Godot 4.5 crash. Establish the OS, godot-cpp version, and build mode. The maintainer built and imported the Linux GDExtension successfully and listed a module build and Asset Library upload as follow-ups. [#96](https://github.com/ardazishvili/Sota/issues/96)
- [ ] Add macOS build support and document how to build on macOS. [#79](https://github.com/ardazishvili/Sota/issues/79)
- [ ] Find a way for the C++ module to handle process and physics process callbacks like the GDExtension build. [#76](https://github.com/ardazishvili/Sota/issues/76)
- [ ] Resolve compilation errors when exposing Godot's `String` through TAL. [#89](https://github.com/ardazishvili/Sota/issues/89)

## Terrain and mesh generation

- [ ] Verify the reported problem creating a single mountain and fix it if reproducible. [#92](https://github.com/ardazishvili/Sota/issues/92)
- [ ] Fix intermittent holes at biome seams. Investigate whether the result depends on how neighboring meshes calculate corner-to-border distances. [#9](https://github.com/ardazishvili/Sota/issues/9)
- [ ] Add clipping for recursive tessellation. [#12](https://github.com/ardazishvili/Sota/issues/12)
- [ ] Define frame behavior for `RidgeHexGridMap` and its subclasses. [#15](https://github.com/ardazishvili/Sota/issues/15)
- [ ] Add optional noise to `PolyhedronMesh` for shapes such as asteroids. [#19](https://github.com/ardazishvili/Sota/issues/19)

## APIs and refactoring

- [ ] Define the polyhedron tile API with an action, return value, and use case for each method. Cover tile selection and item properties, and consider tile swaps and animations discussed in the issue. [#20](https://github.com/ardazishvili/Sota/issues/20)
- [ ] Extend the polygon, hexagon, and pentagon interfaces to support construction outside the XZ plane. [#16](https://github.com/ardazishvili/Sota/issues/16)
- [ ] Define how regular polygons represent their centers when grid tiles need an in-plane center and polyhedron tiles can use an off-plane center. [#17](https://github.com/ardazishvili/Sota/issues/17)
- [ ] Extract `BiomeTile` creation from `RidgeHexGrid` so modifiers such as a terraformer can reuse it. Account for parameters currently spread across the grid and mesh parameter types. [#82](https://github.com/ardazishvili/Sota/issues/82)
- [ ] Simplify vertex updates by providing a way to read, transform, and write vertices in one pipeline. [#44](https://github.com/ardazishvili/Sota/issues/44)
- [ ] Consolidate shared behavior in `FlatMeshProcessor` and `VolumeMeshProcessor`. [#39](https://github.com/ardazishvili/Sota/issues/39)
- [ ] Convert `HexMeshParams` to a class whose constructors require mandatory fields. The maintainer deferred this pending a review of how flat meshes and polyhedra use polygon dimensions. [#41](https://github.com/ardazishvili/Sota/issues/41)

## Tooling and diagnostics

- [ ] Automate include-what-you-use for module and GDExtension builds while preserving TAL includes and accounting for Android build requirements. [#62](https://github.com/ardazishvili/Sota/issues/62)
- [ ] Tighten linter checks for issues such as unused variables. Labeled `good first issue`. [#53](https://github.com/ardazishvili/Sota/issues/53)
- [ ] Add debug logging with verbosity levels, including messages for the main phases of terrain construction. [#52](https://github.com/ardazishvili/Sota/issues/52)

## Documentation and editor integration

- [ ] Document frequently used core classes. [#54](https://github.com/ardazishvili/Sota/issues/54)
- [ ] Add a guide section on C++ casts and update the code in line with the changes from #72. [#73](https://github.com/ardazishvili/Sota/issues/73)
- [ ] Make class documentation available in the Godot editor for module and GDExtension builds. Document a few classes and establish a shared workflow for adding more. [#26](https://github.com/ardazishvili/Sota/issues/26)
- [ ] Add editor icons for a simple hexagon and a basic grid, with a way to add more icons. Depends on #26. [#27](https://github.com/ardazishvili/Sota/issues/27)
