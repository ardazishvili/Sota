# Contributing to Sota
Below is just a zero approximation of proper contribution guidelines. Will be updated if needed.

To build Sota from sources either as GDExtension or module see [doc](docs/content/getting_started/build.md)

## Workflow
* Please use existing [Godot pull request workflow](https://docs.godotengine.org/en/stable/contributing/workflow/pr_workflow.html)
* Check guidelines related to [Types Abstraction Layer](docs/content/arch/tal.md)

## Standalone C++ unit tests

Install SCons, a C++20 compiler, and the doctest headers (`doctest-dev` on
Ubuntu). Run `scons test` from the repository root. The target builds and
runs the tests in `tests/standalone` without building or launching Godot.

## Godot module C++ unit tests

Use the Godot 4.4 source checkout at `~/repos/godot` for both local and
self-hosted PR testing. The checkout must be at commit
`4c311cbee68c0b66ff8ebb8b0defdd9979dd2a41`. On Linux, adjust the Sota
module path in the command if your Sota checkout is elsewhere.

```bash
cd ~/repos/godot
scons platform=linuxbsd target=editor arch=x86_64 tests=yes \
  custom_modules="$HOME/repos/Sota" custom_modules_recursive=no -j2
./bin/godot.linuxbsd.editor.x86_64 --headless --test --test-case='*[Sota]*'
```

The **godot-related unit tests** PR check runs after approval on the
self-hosted Linux runner labeled `sota-linux`. In CI, `~` is the runner
service user's home directory.

## Code style
There are no well-defined rules yet, but please:
1. Apply clang-format before making pull request. Configuration file is in root directory.
2. Stick to [Cpp Core Guildelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
3. If you are hesitant, check [Best practices (godot)](https://docs.godotengine.org/en/stable/contributing/development/best_practices_for_engine_contributors.html) and [Godot C++ usage guidelines](https://docs.godotengine.org/en/stable/contributing/development/cpp_usage_guidelines.html)

### C++ casts

Use implicit conversions when they preserve the value and express the intent clearly.
When a conversion needs to be explicit, use a named C++ cast. Avoid C-style casts
such as `(int)value`, which can hide the kind of conversion being performed.

- Use `static_cast` for intentional numeric and enum conversions. Check that the
  target type can represent the value. For example, use
  `static_cast<std::uint64_t>(group.meshes().size())` when passing a group size
  to Godot's `print` so it selects the same `Variant` constructor on macOS and iOS.
- Use `Object::cast_to<T>` when downcasting a Godot object. For polymorphic C++
  types outside the Godot object hierarchy, use `dynamic_cast` when the type
  needs a runtime check. Check pointer results for null before dereferencing.
- Reserve `const_cast` and `reinterpret_cast` for cases that require removing
  qualifiers or reinterpreting a type at a low-level boundary. Document the
  reason and the assumptions that make the conversion safe.
