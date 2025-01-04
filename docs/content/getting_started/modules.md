Modules
==========

Sota framefork consists of different modules, i.e. groups of classes. For example, see sub-directories in 'src' directory.

## Core and primitives

* Meshes. `SotaMesh`, `HexMesh` , etc. are aim to solve problem of tesselation of some object, e.g. hexagon or pentagon. They store all required properties such as vertices, normals, tangents as well as well as Sota-specific fields, e.g. tesselation type.

    ![mesh_object](/pics/mesh_objects.png)
* Utilities. There are 2 types of utilities: godot-related and general. For example, general utilities may be used to get distance from inner point of some polygon to nearest border line. Godot-related are useful e.g. for freeing resouces of Godot engine
* Grids. Grids are aggregations of discrete structures, e.g hexagons. For example, hexagons may be arranges into rectangular grid (`RectHexGrid` class) or into hexagonal one (`HexagonalHexGrid` class)

    ![grid_object](/pics/grid_objects.png)
* Primitives. Basic geometric objects, like polygon, triangle, edge, etc. Must be engine-agnostic.

## Terrain

* Base object of prism-based terrain is prism. It may be pentagonal or hexagonal. Arranged prisms may differ in height according to some logic. Prism-based terrain is base for prism-based polyhedron.
* Base object of ridge-based terrain is hexagon of one of four types which use data about ridges. Ridges are straight lines and vertices of hexagons are elevated according to positions of these lines. There are 4 types of hexagons: water, plain, hill and mountain. For details see [Terrain section](../design_notes/terrain.md)

![terrain_object](/pics/terrain_object.png)

## Honeycomb

Honeycomb object consists from hexagonal cells. Each cell contains cell mesh and honey mesh. Parameters may be adjusted, e.g. honey level.

![honeycomb_object](/pics/honeycomb_object.png)

## Polyhedron

There are 2 types polyhedron. Each type contain 12 pentagons.
* Ridge-based. Uses ridge-based terrain.

    ![ridge_based_polyhedron](/pics/ridge_polyhedron_object.png)

* Prism-based. Uses prism-based terrain.

    ![prism_based_polyhedron](/pics/prism_polyhedron_object.png)
