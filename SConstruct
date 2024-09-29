#!/usr/bin/env python
from glob import glob
from pathlib import Path

# TODO: Do not copy environment after godot-cpp/test is updated <https://github.com/godotengine/godot-cpp/blob/master/test/SConstruct>.
env = SConscript("godot-cpp/SConstruct")

env.Append(CXXFLAGS=["-std=c++20", "-g"])
env.Append(CPPDEFINES=["SOTA_GDEXTENSION"])

# Add source files.
env.Append(CPPPATH=["."])
env.Append(CPPPATH=["src"])
env.Append(CPPPATH=["src/tal"])
env.Append(CPPPATH=["src/polyhedron"])
env.Append(CPPPATH=["src/primitives"])
env.Append(CPPPATH=["src/core"])
env.Append(CPPPATH=["src/prism_impl"])
env.Append(CPPPATH=["src/ridge_impl"])
env.Append(CPPPATH=["src/honeycomb"])
env.Append(CPPPATH=["src/algo"])
env.Append(CPPPATH=["src/misc"])

sources = Glob("register_types.cpp")
sources += Glob("src/*.cpp")
sources += Glob("src/polyhedron/*.cpp")
sources += Glob("src/primitives/*.cpp")
sources += Glob("src/core/*.cpp")
sources += Glob("src/prism_impl/*.cpp")
sources += Glob("src/ridge_impl/*.cpp")
sources += Glob("src/honeycomb/*.cpp")
sources += Glob("src/algo/*.cpp")
sources += Glob("src/misc/*.cpp")

(extension_path,) = glob("project/addons/*/*.gdextension")

addon_path = Path(extension_path).parent

project_name = Path(extension_path).stem

debug_or_release = "release" if env["target"] == "template_release" else "debug"

if env["target"] == "editor":
    env_sota.Append(CPPDEFINES=["SOTA_ENGINE"])

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

Default(library)
