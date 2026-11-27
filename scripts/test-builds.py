#!/usr/bin/env python3
"""Build Sota with SCons and run desktop Godot integration checks."""

import argparse
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
from tempfile import TemporaryDirectory


ROOT = Path(__file__).resolve().parents[1]
ARCHITECTURES = {
    "linux": "x86_64",
    "windows": "x86_64",
    "android": "arm64",
}
TEMPLATES = ("template_debug", "template_release")
TARGETS = ("editor", *TEMPLATES)
MODES = ("module", "extension")
SMOKE_CASES = (("module", "editor"), ("extension", "template_debug"))
RUNTIME_SCRIPT = """extends SceneTree

func _initialize() -> void:
    var grid = ClassDB.instantiate("RectHexGrid")
    if grid == null:
        push_error("Sota's RectHexGrid could not be instantiated")
        quit(1)
        return
    grid.free()
    print("Sota runtime check passed")
    quit()
"""


def build_matrix(args):
    for platform in args.platform or ARCHITECTURES:
        arch = ARCHITECTURES[platform]
        if args.arch and arch not in args.arch:
            continue
        cases = (SMOKE_CASES if args.suite == "smoke"
                 else [(mode, target) for mode in MODES for target in TARGETS])
        for mode, target in cases:
            if args.mode in ("both", mode) and (not args.target or target in args.target):
                yield platform, mode, arch, target


def runtime_binary(platform, mode, arch, target, godot, binaries):
    if mode == "module":
        engine_platform = "linuxbsd" if platform == "linux" else platform
        suffix = ".console.exe" if platform == "windows" else ""
        return godot / "bin" / f"godot.{engine_platform}.{target}.{arch}{suffix}"
    if target == "editor":
        if platform == "windows":
            return binaries / "Godot_v4.4-stable_win64_console.exe"
        return binaries / "Godot_v4.4-stable_linux.x86_64"
    variant = target.removeprefix("template_")
    if platform == "windows":
        return binaries / f"windows_{variant}_{arch}_console.exe"
    return binaries / f"linux_{variant}.{arch}"


def run_godot(platform, mode, arch, target, binary):
    command = [str(binary), "--headless", "--path", ".", "--script", "res://check.gd"]
    if platform == "windows" and sys.platform != "win32":
        command.insert(0, "wine")
    print(shlex.join(command), flush=True)
    with TemporaryDirectory(prefix="sota-runtime-") as directory:
        project = Path(directory)
        (project / "project.godot").write_text(
            'config_version=5\n\n[application]\nconfig/name="Sota build test"\n'
            '\n[debug]\nfile_logging/enable_file_logging=false\n'
            'file_logging/enable_file_logging.pc=false\n', encoding="utf-8")
        (project / "check.gd").write_text(RUNTIME_SCRIPT, encoding="utf-8")
        (project / ".godot").mkdir()
        (project / ".godot/global_script_class_cache.cfg").write_text("list=[]\n", encoding="utf-8")
        if mode == "extension":
            addon = project / "addons/sota"
            (addon / "bin").mkdir(parents=True)
            source = ROOT / "project/addons/sota"
            shutil.copy2(source / "sota.gdextension", addon)
            variant = "release" if target == "template_release" else "debug"
            suffix = "dll" if platform == "windows" else "so"
            library = f"libsota.{platform}.{variant}.{arch}.{suffix}"
            shutil.copy2(source / "bin" / library, addon / "bin")
            (project / ".godot/extension_list.cfg").write_text(
                "res://addons/sota/sota.gdextension\n", encoding="utf-8")

        env = dict(os.environ, XDG_DATA_HOME=str(project / "user-data"))
        output = subprocess.run(command, cwd=project, env=env, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True, errors="replace", timeout=120)
    print(output.stdout, end="", flush=True)
    if output.returncode or "Sota runtime check passed" not in output.stdout.splitlines():
        raise RuntimeError(f"Godot runtime check failed (exit status {output.returncode})")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--godot-source", type=Path, default=os.environ.get("GODOT_SOURCE"),
                        help="Godot source directory (also read from GODOT_SOURCE)")
    parser.add_argument("--godot-binaries", type=Path, default=os.environ.get("GODOT_BINARIES"),
                        help="directory of official desktop Godot binaries (also read from GODOT_BINARIES)")
    parser.add_argument("--suite", choices=("smoke", "full"), default="full")
    parser.add_argument("--build-only", action="store_true", help="skip desktop runtime checks")
    parser.add_argument("--mode", choices=("both", *MODES), default="both")
    parser.add_argument("--platform", action="append", choices=ARCHITECTURES,
                        help="limit to this platform (repeatable, default is all)")
    parser.add_argument("--arch", action="append",
                        help="limit to this architecture (repeatable)")
    parser.add_argument("--target", action="append", choices=TARGETS,
                        help="limit to this SCons target (repeatable)")
    parser.add_argument("-j", "--jobs", type=int, default=2, help="parallel jobs per build (default: 2)")
    parser.add_argument("--dry-run", action="store_true", help="print the selected SCons commands")
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error("--jobs must be positive")

    builds = list(build_matrix(args))
    if not builds:
        parser.error("no build configurations match the filters")

    scons = shutil.which("scons")
    if not args.dry_run and not scons:
        parser.error("SCons is required on PATH")

    godot = args.godot_source.expanduser().resolve() if args.godot_source else None
    binaries = args.godot_binaries.expanduser().resolve() if args.godot_binaries else None
    if not args.dry_run and not args.build_only and not binaries and any(
        platform != "android" and mode == "extension" for platform, mode, _, _ in builds
    ):
        parser.error("provide --godot-binaries or GODOT_BINARIES to run desktop extensions")

    failures = []
    for platform, mode, arch, target in builds:
        label = f"{platform}/{mode}/{arch}/{target}"
        print(f"\n[{label}]", flush=True)
        scons_platform = "linuxbsd" if platform == "linux" and mode == "module" else platform
        command = [scons or "scons", f"-j{args.jobs}", f"platform={scons_platform}",
                   f"arch={arch}", f"target={target}"]
        directory = ROOT
        if mode == "module":
            directory = godot or Path("<godot-source>")
            command += [f"custom_modules={ROOT}", "custom_modules_recursive=no",
                        f"module_{ROOT.name}_enabled=yes"]

        run_desktop = platform != "android" and not args.build_only
        binary = None
        if run_desktop:
            binary = runtime_binary(platform, mode, arch, target, directory,
                                    binaries or Path("<godot-binaries>"))
        print(f"{directory}  {shlex.join(command)}", flush=True)
        if args.dry_run:
            if binary:
                print(f"Run headless: {binary}", flush=True)
            continue
        if mode == "module" and ROOT.name != "Sota":
            error = "the module checkout directory must be named Sota for Godot's registration hooks"
        elif mode == "module" and not all((directory / name).is_file()
                                        for name in ("SConstruct", "version.py")):
            error = "provide the Godot source directory with --godot-source or GODOT_SOURCE"
        elif mode == "extension" and not (ROOT / "godot-cpp/SConstruct").is_file():
            error = "initialize godot-cpp with git submodule update --init"
        else:
            try:
                subprocess.run(command, cwd=directory, check=True)
                if binary:
                    run_godot(platform, mode, arch, target, binary)
                continue
            except (OSError, RuntimeError, subprocess.SubprocessError) as exc:
                error = str(exc)
        print(error, flush=True)
        failures.append(label)

    if args.dry_run:
        print(f"\n{len(builds)} configurations selected.")
    else:
        print(f"\n{len(builds) - len(failures)} passed, {len(failures)} failed.")
        for label in failures:
            print(f"  {label}")
    return int(bool(failures))


if __name__ == "__main__":
    sys.exit(main())
