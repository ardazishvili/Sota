# Contributing to Sota
Below is just a zero approximation of proper contribution guidelines. Will be updated if needed.

To build Sota from sources either as GDExtension or module see [doc](docs/content/getting_started/build.md)

## Workflow
* Please use existing [Godot pull request workflow](https://docs.godotengine.org/en/stable/contributing/workflow/pr_workflow.html)
* Check guidelines related to [Types Abstraction Layer](docs/content/arch/tal.md)

## Code style
There are no well-defined rules yet, but please:
1. Apply clang-format before making pull request. Configuration file is in root directory.
2. Stick to [Cpp Core Guildelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
3. If you are hesitant, check [Best practices (godot)](https://docs.godotengine.org/en/stable/contributing/development/best_practices_for_engine_contributors.html) and [Godot C++ usage guidelines](https://docs.godotengine.org/en/stable/contributing/development/cpp_usage_guidelines.html)

## Include What You Use (IWYU)

IWYU reports which C++ includes a file needs and which it may be able to drop.
On Ubuntu, install it with:

```bash
sudo apt install iwyu
include-what-you-use --version
```

Ubuntu installs the compilation database driver as `iwyu_tool`. If you install
IWYU from [upstream](https://github.com/include-what-you-use/include-what-you-use),
use its `iwyu_tool.py` in the commands below.

### Generate the compilation database

For a Linux GDExtension build, run these commands from the Sota checkout:

```bash
git submodule update --init -- godot-cpp
scons platform=linux use_llvm=yes compiledb=yes compiledb
```

If IWYU reports missing generated Godot headers, first run the
[Linux GDExtension build](docs/content/getting_started/build.md#linux-build)
with `use_llvm=yes`.

For a Godot module build, run this from the **Godot source checkout** and use
your Sota checkout's absolute path:

```bash
scons platform=linuxbsd custom_modules=/absolute/path/to/Sota compiledb=yes compile_commands.json
```

### Review a small report first

From the Sota checkout, replace the example path with the `.cpp` file you are
working on. Use `-p /absolute/path/to/godot` in place of `-p .` to read the
module compilation database.

```bash
# One translation unit and its associated headers
iwyu_tool -p . src/core/hex_mesh.cpp

# One directory, saved for later review
iwyu_tool -p . src/core/ > /tmp/sota-iwyu-core.txt

# All Sota sources under src/
iwyu_tool -p . src/ > /tmp/sota-iwyu.txt
```

These commands only report suggestions. Review the proposed additions and
removals before applying them. Keep the [TAL wrappers](docs/content/arch/tal.md)
when IWYU suggests replacing them with direct Godot headers. For example,
`tal/wrapped.h` provides `GDCLASS` in both build modes. Check include changes
against both the GDExtension and module builds, and verify the
[Android build](docs/content/getting_started/build.md#android-build) when
changing platform-sensitive includes.
