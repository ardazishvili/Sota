#!/usr/bin/env python3
"""Build Sota's Godot module and GDExtension configurations with SCons."""

import argparse
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
ARCHITECTURES = {
    "linux": ("x86_64", "arm64", "rv64"),
    "windows": ("x86_64",),
    "android": ("x86_64", "arm64"),
    "macos": ("x86_64", "arm64"),
}
TEMPLATES = ("template_debug", "template_release")
TARGETS = ("editor", *TEMPLATES)
MODES = ("module", "extension")


def build_matrix(args):
    for platform in args.platform or ARCHITECTURES:
        for mode in MODES if args.mode == "both" else (args.mode,):
            arches = ARCHITECTURES[platform]
            if platform == "macos" and mode == "extension":
                arches = ("universal",)
            for arch in arches:
                if args.arch and arch not in args.arch:
                    continue
                targets = TARGETS
                if platform == "android" and (mode, arch) != ("module", "arm64"):
                    targets = TEMPLATES
                for target in targets:
                    if not args.target or target in args.target:
                        yield platform, mode, arch, target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--godot-source", type=Path, default=os.environ.get("GODOT_SOURCE"),
                        help="Godot source directory (also read from GODOT_SOURCE)")
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

        print(f"{directory}  {shlex.join(command)}", flush=True)
        if args.dry_run:
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
                result = subprocess.run(command, cwd=directory)
                if result.returncode == 0:
                    continue
                error = f"SCons exited with status {result.returncode}"
            except OSError as exc:
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
