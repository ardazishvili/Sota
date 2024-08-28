# Contributing to Sota
Below is just a zero approximation of proper contribution guidelines. Will be updated if needed.

## System setup
Under Linux
### Linux build
Prerequisites are: python, scons, gcc/clang compiler
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=linux target=template_debug -j 15
```

### Windows build
Prerequisites are: python, scons, mingw toolchain
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=windows target=template_debug -j 15
```

### Android build
Prerequisites are: python, scons, [Android setup](https://docs.godotengine.org/en/stable/contributing/development/compiling/compiling_for_android.html)
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=android target=template_debug -j 15

## Workflow
Please use existing [Godot pull request workflow](https://docs.godotengine.org/en/stable/contributing/workflow/pr_workflow.html)

## Code style
There are no well-defined rules yet, but please:
1. Apply clang-format before making pull request. Configuration file is in root directory.
2. Stick to [Cpp Core Guildelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
3. If you are hesitant, check [Best practices (godot)](https://docs.godotengine.org/en/stable/contributing/development/best_practices_for_engine_contributors.html) and [Godot C++ usage guidelines](https://docs.godotengine.org/en/stable/contributing/development/cpp_usage_guidelines.html)

## Documentation
See /doc directory in root dir.
