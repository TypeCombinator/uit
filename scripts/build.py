#!/usr/bin/env python3
import argparse
import shutil
import subprocess
from collections.abc import Sequence
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]

# Requested build mode -> CMAKE_BUILD_TYPE; "asan" reuses RelWithDebInfo with sanitizers.
CMAKE_BUILD_TYPES = {
    "debug": "Debug",
    "release": "Release",
    "asan": "RelWithDebInfo",
    "coverage": "Debug",
}

# Coverage instrumentation requires clang; used when neither -t nor a
# generated build/<mode>/toolchain.cmake is available.
COVERAGE_TOOLCHAIN = Path("cmake/toolchain/clang.cmake")


class Builder:
    """Configure, build and test this project with CMake/Ninja."""

    def __init__(self, build_mode: str, toolchain: Path | None = None) -> None:
        self.build_mode = build_mode
        self.toolchain = toolchain
        self.cmake_build_type = CMAKE_BUILD_TYPES[build_mode]
        self.build_dir = Path(f"build/{build_mode}")

    def _run_subprocess(self, cmd: list[str]) -> int:
        """Print the command line and execute it from the project root."""
        print(f"$ {' '.join(cmd)}", flush=True)
        return subprocess.run(cmd, check=False, cwd=PROJECT_ROOT).returncode

    def _resolve_toolchain(self) -> Path | None:
        """Pick the toolchain file: explicit -t path, then the generated
        build/<mode>/toolchain.cmake, then the coverage default, then none."""
        if self.toolchain is not None:
            return self.toolchain
        generated = self.build_dir / "toolchain.cmake"
        if generated.is_file():
            return generated
        if self.build_mode == "coverage":
            return COVERAGE_TOOLCHAIN
        return None

    def config(self) -> int:
        """Generate the CMake/Ninja build files (mirrors scripts/config.sh)."""
        cmd = [
            "cmake",
            "-B",
            str(self.build_dir),
            "-G",
            "Ninja",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            "-DUIT_BUILD_EXAMPLES=ON",
            "-DUIT_BUILD_TESTS=ON",
            f"-DCMAKE_BUILD_TYPE={self.cmake_build_type}",
            f"-DUIT_CMAKE_BUILD_MODE={self.build_mode}",
        ]
        toolchain_file = self._resolve_toolchain()
        if toolchain_file is not None:
            cmd += ["--toolchain", str(toolchain_file)]
        return self._run_subprocess(cmd)

    def build(self) -> int:
        """Build an already configured directory with Ninja (mirrors scripts/build.sh)."""
        return self._run_subprocess(["ninja", "-C", str(self.build_dir)])

    def clean(self) -> int:
        """Remove built artifacts with Ninja's clean tool."""
        return self._run_subprocess(["ninja", "-C", str(self.build_dir), "-t", "clean"])

    def remove(self) -> int:
        """Remove the entire build directory of the specified build mode."""
        self.clean()
        shutil.rmtree(self.build_dir, ignore_errors=True)
        return 0

    def test(self) -> int:
        """Run the unit tests with CTest (mirrors scripts/test.sh)."""
        build_dir = self.build_dir / "tests" / "unit"
        if self.build_mode != "coverage":
            return self._run_subprocess(
                ["ctest", "--test-dir", str(build_dir), "--output-on-failure"]
            )
        # Coverage test only for Linux.
        llvm_profile_dir = self.build_dir / "profiles"
        shutil.rmtree(llvm_profile_dir, ignore_errors=True)
        # Please refer to https://clang.llvm.org/docs/SourceBasedCodeCoverage.html.
        # If LLVM_PROFILE_FILE specifies a path to a non-existent directory, the missing directory structure will be
        # created.
        return self._run_subprocess(
            [
                "env",
                f"LLVM_PROFILE_FILE={(llvm_profile_dir.resolve() / '%m_%p.profraw')}",
                "ctest",
                "--test-dir",
                str(build_dir),
                "--output-on-failure",
            ]
        )


ACTIONS = {
    "config": "configure the build directory",
    "build": "build an already configured directory",
    "clean": "remove built artifacts",
    "remove": "remove the entire build directory",
    "test": "run the unit tests",
}


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Configure, build, clean and test this project with CMake/Ninja.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    subparsers = parser.add_subparsers(dest="action", required=True, metavar="action")
    for action, help_text in ACTIONS.items():
        subparser = subparsers.add_parser(action, help=help_text)
        subparser.add_argument(
            "build_mode",
            nargs="?",
            choices=tuple(CMAKE_BUILD_TYPES),
            default="release",
            help="build mode to operate on",
        )
        if action == "config":
            subparser.add_argument(
                "-t",
                "--toolchain",
                type=Path,
                default=None,
                help="toolchain .cmake file to configure with; if omitted, "
                "build/<mode>/toolchain.cmake is used when present, otherwise "
                "CMake selects the system default",
            )
    args = parser.parse_args(argv)

    builder = Builder(args.build_mode, toolchain=getattr(args, "toolchain", None))
    return getattr(builder, args.action)()


if __name__ == "__main__":
    raise SystemExit(main())
