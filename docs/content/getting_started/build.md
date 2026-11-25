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

Install Python 3.9 or newer, SCons 4 or newer, and a C++20 toolchain for each target. Initialize the
pinned bindings from the Sota checkout with `git submodule update --init`.
For module builds, also obtain a Godot **source** checkout and keep the Sota
checkout directory named `Sota`, as required by its registration functions.
Godot `4.4-stable` matches the pinned `godot-cpp` API and the demo project's
Godot feature version.
Pass the source checkout for the version you want to test with
`--godot-source` or the `GODOT_SOURCE` environment variable.

```bash
python3 scripts/test-builds.py --dry-run
python3 scripts/test-builds.py --godot-source ~/repos/godot --platform linux --arch x86_64 -j4
python3 scripts/test-builds.py --godot-source ~/repos/godot --platform windows -j4
python3 scripts/test-builds.py --mode extension --platform android --arch arm64 --target template_release
```

With no filters the script selects both build forms for every platform in the
add-on manifest. It compiles debug and release templates, plus editor builds
on desktop targets and for the Android ARM64 module. It continues after failures and exits nonzero if any
selected build fails. Use `--platform`, `--arch`, `--mode`, and `--target`
to run the appropriate portion on each build host. A macOS GDExtension uses
`--arch universal`. The macOS module builds separately for Intel and Apple silicon.

| Target | Toolchain |
| --- | --- |
| Linux x86_64, arm64, rv64 | GCC or Clang for the target architecture. Run the Linux cases on matching Linux hosts or in matching Linux guests. For module builds, install the [Godot Linux build dependencies](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_linuxbsd.html). |
| Windows x86_64 | MinGW-w64 with POSIX threads when cross-compiling from Linux, or Visual Studio with Desktop development with C++ on Windows. See [Godot's Windows build guide](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_windows.html). |
| Android x86_64, arm64 | Set `ANDROID_HOME` to the Android SDK path and install NDK r23c (`23.2.8568313`) for the pinned bindings and Godot 4.4. See [Godot's Android build guide](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_android.html) for SDK and JDK setup. [Download NDK r23c for Linux x86_64](https://dl.google.com/android/repository/android-ndk-r23c-linux.zip). |
| macOS | Run on a Mac with Xcode or the Xcode Command Line Tools for the compiler and SDK. For module builds, also set up MoltenVK. See [Godot's macOS build guide](https://docs.godotengine.org/en/4.4/contributing/development/compiling/compiling_for_macos.html). |

On Debian or Ubuntu, the following packages provide SCons and the Windows x86_64
cross-compiler.

```bash
sudo apt install python3 scons g++-mingw-w64-x86-64-posix
```

### Android ARM64 builds on Linux

Set `ANDROID_HOME` to your SDK directory. The example below assumes the NDK
ZIP from the table is in `~/Downloads` and the SDK is in `~/Android/Sdk`.
If `ndk/23.2.8568313` already exists in your SDK, skip the extraction steps.

```bash
export ANDROID_HOME="$HOME/Android/Sdk"
mkdir -p "$ANDROID_HOME/ndk"
unzip "$HOME/Downloads/android-ndk-r23c-linux.zip" -d "$ANDROID_HOME/ndk"
mv "$ANDROID_HOME/ndk/android-ndk-r23c" "$ANDROID_HOME/ndk/23.2.8568313"
```

Run both native build checks from the Sota checkout in the same terminal so
they inherit `ANDROID_HOME`. Adjust `--godot-source` to match your Godot checkout.

```bash
cd "$HOME/repos/Sota"

python3 scripts/test-builds.py --platform android --arch arm64 \
  --mode extension --target template_debug -j4

python3 scripts/test-builds.py --platform android --arch arm64 \
  --mode module --target editor --godot-source "$HOME/repos/godot" -j4
```

## System variables and dependencies

### Documentation
See `doc/requirement.txt` for required modules. They may by installed via `pip install <name>`.

PlantUML syntax is used to generate diagrams. While "Read the docs" has internal support, local generation requires installed java, plantUML .jar file and system variable `PLANTUML_JAR_DIR` which points to directory with .jar file. See [plantuml releases](https://github.com/plantuml/plantuml/releases) for appropriate artefacts. Sample commands to generate docs:
```bash
make html # generate required files
rm -rf api && make clean html # clean and generate
```
