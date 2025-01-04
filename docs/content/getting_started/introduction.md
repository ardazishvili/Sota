Introduction
============

![intro](/pics/godot_asset_library/honeycomb_terrain_hex.png)

## Overview

Sota is a framework for creation of structures made of discrete building blocks, e.g. terrains for turn-based games, honeycombs, etc. By using trivial objects like hexagon or pentagon and organizing them into combination it let you create aggregations like hexagonal grid of various shape, e.g rectangular or hexagonal, or polyhedron made of hexagons and 12 pentagons.

Base principles of development:
1. Bind abtractions and concepts to specific engine as little as possible. While initially made for Godot 4.2 it definetely must work for newer versions. Other engines should be supported later too keeping in mind refactoring.
2. Abstractions must be kept as complete and as simple as their usage required by 3D and 2D. Originally code was written for 3D but conceptually there must be no requirement for 3D only.
3. Addition of new structures, e.g. new type of terrain, must be as simple as possible. Core part of framework should be oriented on wide variety of users needs, not on enhancement and beautification of certain terrain type. Meanwhile, if needed, sophisticated code may be added for particular terrain type but it must not require changes in core classes and/or changes must be well localized.

At the moment development of Sota is focused on 3D objects and Godot engine. However most of code written in way to make it possible to reuse it:
1. There are plans to reuse code in other engine(s). This is not trivial since requires knowledge of engines architecture.
2. Reuse code for 2D. This should be rather simpler task and may be appreciated e.g. Godot community.

## Godot notes

At least for Godot 4.2/4.3 there are possibility to write C++ code via GDExtenstion and via Godot modules system. Sota has support of both types and there are plans to maintain support in future, see [Types Abstraction Layer](../arch/tal.md).
