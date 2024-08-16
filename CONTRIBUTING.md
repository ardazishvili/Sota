# Contributing to Sota
Below is just a zero approximation of proper contribution guidelines. Will be updated if needed.

## System setup
### Linux build
Prerequisites are: python, scons, gcc/clang compiler
```bash
git clone https://github.com/ardazishvili/Sota.git
git submodule update --init # init godot-cpp
scons platform=linux target=template_debug -j 15
```
### Windows build
To be updated. Feel free to make PR with steps to build under Windows.

## Workflow
Please use existing [Godot pull request workflow](https://docs.godotengine.org/en/stable/contributing/workflow/pr_workflow.html)

## Code style
There are no well-defined rules yet, but please:
1. Apply clang-format before making pull request. Configuration file is in root directory.
2. Stick to [Cpp Core Guildelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
3. If you are hesitant, check [Best practices (godot)](https://docs.godotengine.org/en/stable/contributing/development/best_practices_for_engine_contributors.html) and [Godot C++ usage guidelines](https://docs.godotengine.org/en/stable/contributing/development/cpp_usage_guidelines.html)

## Documentation
See /doc directory in root dir.
