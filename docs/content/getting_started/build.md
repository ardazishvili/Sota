Building from sourse
====================

## Build as plugin via GDExtension

### Linux build
Prerequisites are: `python, scons, gcc/clang compiler`
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=linux target=template_debug -j 15
```

### Windows build
Prerequisites are: `python, scons, mingw toolchain`
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=windows target=template_debug -j 15
```

### Android build
Prerequisites are: `python, scons`, [Android setup](https://docs.godotengine.org/en/stable/contributing/development/compiling/compiling_for_android.html)
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=android target=template_debug -j 15
```

## Build as module of Godot editor

Requires working build from source from godot. Then Sota may be added to build via additional argument `custom_modules=/path/to/Sota/repository`. For example:
```bash
scons platform=linuxbsd -j 15 custom_modules=~/repos/Sota
```

## System variables and dependencies

### Documentation
See `doc/requirement.txt` for required modules. They may by installed via `pip install <name>`.

PlantUML syntax is used to generate diagrams. While "Read the docs" has internal support, local generation requires installed java, plantUML .jar file and system variable `PLANTUML_JAR_DIR` which points to directory with .jar file. See [plantuml releases](https://github.com/plantuml/plantuml/releases) for appropriate artefacts. Sample commands to generate docs:
```bash
make html # generate required files
rm -rf api && make clean html # clean and generate
```
