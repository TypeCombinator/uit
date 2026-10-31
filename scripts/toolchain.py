#!/usr/bin/env python3
import argparse
import re
import sys
from collections.abc import Sequence
from pathlib import Path

# Build modes, kept in sync with scripts/build.py.
BUILD_MODES = ("debug", "release", "asan", "coverage")

# gcc as the trailing tool segment: gcc, gcc-14, ... (also the trailing
# segment of a cross compiler triplet like aarch64-linux-gnu-gcc).
_GCC_NAME = re.compile(r"\bgcc(-\d+)?$")


def derive_cxx_compiler(c_compiler: str) -> str:
    """Derive the C++ compiler name from the C compiler name.

    Handles versioned names (gcc-14 -> g++-14, clang-18 -> clang++-18) and
    cross compiler triplets whose tool suffix carries the compiler name
    (aarch64-linux-gnu-gcc -> aarch64-linux-gnu-g++,
    aarch64-linux-gnu-gcc-14 -> aarch64-linux-gnu-g++-14).
    cl and clang-cl drive both languages with the same executable.
    """
    if c_compiler in ("cl", "clang-cl"):
        return c_compiler
    renamed = _GCC_NAME.sub(lambda m: "g++" + (m.group(1) or ""), c_compiler)
    if renamed != c_compiler:
        return renamed
    if c_compiler == "clang" or c_compiler.startswith("clang-"):
        return "clang++" + c_compiler[len("clang") :]
    return c_compiler + "++"


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Generate a CMake toolchain file into build/<mode>/.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "compiler",
        help="C compiler name, e.g. gcc, gcc-14, aarch64-linux-gnu-gcc, clang, clang-cl",
    )
    parser.add_argument(
        "mode",
        nargs="?",
        choices=BUILD_MODES,
        default="release",
        help="build mode the toolchain file is generated for",
    )
    parser.add_argument(
        "-p",
        "--path",
        type=Path,
        default=None,
        help="directory containing the compiler; if omitted, the compiler is looked up on PATH",
    )
    args = parser.parse_args(argv)

    c_compiler = args.compiler
    cxx_compiler = derive_cxx_compiler(args.compiler)
    if args.path is not None:
        c_compiler = str(args.path / c_compiler)
        cxx_compiler = str(args.path / cxx_compiler)

    build_dir = Path(f"build/{args.mode}")
    output = build_dir / "toolchain.cmake"
    try:
        build_dir.mkdir(parents=True, exist_ok=True)
        output.write_text(
            f"set(CMAKE_C_COMPILER    {c_compiler})\n"
            f"set(CMAKE_CXX_COMPILER  {cxx_compiler})\n"
        )
        written = output.read_text()
    except OSError as e:
        print(f"error: failed to write {output}: {e}", file=sys.stderr)
        return 1
    print(f"generated {output}:\n{written}", end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
