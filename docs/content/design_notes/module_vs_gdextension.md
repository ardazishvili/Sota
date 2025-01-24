C++ module VS GDExtension
=========================

Sota supports 2 types of build:
1. GDExtension. Build is managed by SConstruct file. `SOTA_GDEXTENSION` macro is defined to preprocess C++ code.
2. C++ module. Build is managed by SCsub file. `SOTA_MODULE` macro is used instead of `SOTA_GDEXTENSION`.

Other macros:
* `SOTA_ENGINE` macro is set when Sota is going to be used in Godot editor. There are various methods e.g. to visualize and manipulate godot object while inside editor. Those methods are not present outside of editor and their usage will result in compilation error. To prevent it we explicitly set `SOTA_ENGINE` if editor will be used.

Related to topic:
1. [Types abstraction layer](/content/arch/tal.md) - approach to localize module/gdextension differences.
2. [module/gdextension build](/content/getting_started/build.md)
