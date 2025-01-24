Sota C++ module build
=====================

1. Due to different approach to manage `_process`/`_physics_process` while not using GDExtension, tiles interaction does not work for Sota built as a C++ module. Related:
  * [PR with example for GDExtension]( https://github.com/ardazishvili/Sota/pull/71 )
  * [Issue to track]( https://github.com/ardazishvili/Sota/issues/76 )
  * [Forum thread 1]( https://forum.godotengine.org/t/override-process-physics-process-while-in-c-module/99260/3 )
  * [Forum thread 2]( https://forum.godotengine.org/t/game-as-c-engine-module-node-inheritance-and-lifecycle/71668/8 )
