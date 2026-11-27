Building from source
====================

## Choose a build configuration

Use `--mode` to select how Sota is integrated. `extension` builds the Sota
shared library for a standard Godot installation. `module` builds a custom
Godot editor or export template with Sota compiled in. Use `--target` to
select the editor, debug export, or release export configuration.

| Mode | Target | When to use it | Example |
| --- | --- | --- | --- |
| `extension` | `editor` | Develop and test Sota's editor-specific features in a standard Godot editor. | Generate tiles in the editor, save the scene, then reopen it to verify the generated nodes were saved. |
| `extension` | `template_debug` | Develop the add-on and test it in debug exports. | Run a debug export and use Godot's debugger to investigate a tile-generation error. |
| `extension` | `template_release` | Distribute a release export that uses the Sota add-on. | Export a Linux release build and verify that it loads the bundled Sota shared library on another machine. |
| `module` | `editor` | Open and edit projects in a custom Godot editor with Sota built in. | Open the demo project and edit a honeycomb scene using the built-in Sota classes. |
| `module` | `template_debug` | Debug an exported game that uses Sota as a built-in module. | Set the resulting binary as the custom debug export template and attach the debugger to investigate an export-only error. |
| `module` | `template_release` | Distribute an exported game with Sota built into its runtime. | Set the resulting binary as the custom release export template and build the game for distribution. |

The `extension/editor` and `extension/template_debug` builds write to the same
debug library path for a given platform and architecture. Build the variant you
want to use last. When the test script runs both, `template_debug` runs second
and replaces the editor variant, which contains Sota's editor-specific code.
The module configurations have separate outputs.

## Test the module and GDExtension builds

The full suite selects six configurations per platform, combining both modes
with all three SCons targets. The smoke suite selects `extension/template_debug`
and `module/editor` on each platform.

| Platform | Architecture | Smoke | Full | Validation |
| --- | --- | --- | --- | --- |
| Linux | x86_64 | 2 | 6 | Build and run headless |
| Windows | x86_64 | 2 | 6 | Build and run headless |
| Android | arm64 | 2 | 6 | Build native shared libraries |
| **Total** | | **6** | **18** | |

Each desktop case opens a temporary Godot project and instantiates Sota's
`RectHexGrid` through `ClassDB`. The extension cases use the matching stock
Godot editor or export template and load the library produced by that case.
The module cases use the custom Godot binary produced by that case. Android
uses the NDK to compile the native libraries and requires no emulator or device.

Install Python 3.9 or newer, SCons 4 or newer, and the toolchains listed below.
Initialize the pinned bindings with `git submodule update --init`. Module cases
require a Godot source checkout and the Sota checkout directory must be named
`Sota` for Godot's module registration. Godot `4.4-stable` matches the pinned
bindings and the demo project.

Pass the Godot source directory with `--godot-source` or `GODOT_SOURCE`. For
desktop extension tests, pass `--godot-binaries` or `GODOT_BINARIES` with the
directory containing your extracted Godot 4.4 desktop binaries. Put the
platform's editor and the contents of the export archive's `templates/`
directory alongside each other and keep their original filenames. For smoke
tests, only the desktop debug export templates are needed. On Linux, make
the Linux editor and templates executable. [Godot 4.4 downloads](https://github.com/godotengine/godot-builds/releases/tag/4.4-stable)

| Platform | Toolchain |
| --- | --- |
| Linux | GCC or Clang with C++20 support and the [Godot Linux build dependencies](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_linuxbsd.html). |
| Windows | For Linux cross-builds, use the MinGW-w64 POSIX toolchain and run the headless checks under Wine, for example with `xvfb-run -a`. On Windows, use Visual Studio with Desktop development with C++. [Godot Windows build guide](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_windows.html) |
| Android | Set `ANDROID_HOME` to the SDK directory and install NDK r23c with `sdkmanager 'ndk;23.2.8568313'`. [Godot Android build guide](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_android.html) |

```bash
python3 scripts/test-builds.py --suite smoke --dry-run
python3 scripts/test-builds.py --suite full --dry-run
python3 scripts/test-builds.py --suite smoke --godot-source ~/repos/godot --godot-binaries ~/godot-4.4 -j4
python3 scripts/test-builds.py --platform linux --godot-source ~/repos/godot --godot-binaries ~/godot-4.4 -j4
python3 scripts/test-builds.py --platform android --mode extension --target template_release
```

The script defaults to the full suite. Use `--platform`, `--arch`, `--mode`,
and `--target` to narrow the selection, or `--build-only` to skip desktop
execution locally. The script continues through the selected cases and
returns a nonzero status if any build or runtime check fails.

### Pull request checks

**smoke build test** and **standalone unit tests** are queued for every PR. The
synthetic test is a standalone C++ doctest that checks `2 + 2 == 4` and
depends only on the C++ toolchain and doctest. **full build test** is
queued when a maintainer adds the `full-build` PR label and on updates
while the label is present.

All three jobs wait for approval from the required reviewer on the
`sota-pr-checks` environment. Open the workflow run, select
**Review deployments**, then approve `sota-pr-checks` to start the waiting
jobs for that run. The jobs use GitHub-hosted Linux runners, with Wine
for the Windows runtime checks.

In the target branch's GitHub ruleset or branch protection, require
**smoke build test** from GitHub Actions and leave **full build test**
optional. [GitHub branch protection](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/about-protected-branches)

## System variables and dependencies

### Documentation
See `doc/requirement.txt` for required modules. They may by installed via `pip install <name>`.

PlantUML syntax is used to generate diagrams. While "Read the docs" has internal support, local generation requires installed java, plantUML .jar file and system variable `PLANTUML_JAR_DIR` which points to directory with .jar file. See [plantuml releases](https://github.com/plantuml/plantuml/releases) for appropriate artefacts. Sample commands to generate docs:
```bash
make html # generate required files
rm -rf api && make clean html # clean and generate
```
