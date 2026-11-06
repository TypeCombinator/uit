#!/usr/bin/env python3
import argparse
import json
import re
import subprocess
from collections.abc import Sequence
from pathlib import Path

from build import CMAKE_BUILD_TYPES

PROJECT_ROOT = Path(__file__).resolve().parents[1]

# Only report diagnostics from this repository's own headers. Fetched dependencies
# (fmt, Catch2) live under build/ and must stay out of the report.
HEADER_FILTER = rf"^{re.escape(str(PROJECT_ROOT))}/(include|examples|tests/unit)/"

# Translation units matching these patterns are skipped by clang-tidy:
# dependencies fetched into build/ and benchmark sources.
SOURCE_EXCLUDE = re.compile(
    rf"^{re.escape(str(PROJECT_ROOT))}/(build|tests/bench)(/|$)"
)


class ClangTidy:
    """Run clang-tidy over this project's own sources and headers."""

    def __init__(
        self, build_mode: str, fix: bool = False, jobs: int | None = None
    ) -> None:
        self.build_mode = build_mode
        self.build_dir = Path(f"build/{build_mode}")
        self.fix = fix
        self.jobs = jobs

    def _run_subprocess(self, cmd: list[str]) -> int:
        """Print the command line and execute it from the project root."""
        print(f"$ {' '.join(cmd)}", flush=True)
        return subprocess.run(cmd, check=False, cwd=PROJECT_ROOT).returncode

    def sources(self) -> list[str]:
        """Collect the project's translation units from the compilation database."""
        compile_db = PROJECT_ROOT / self.build_dir / "compile_commands.json"
        entries = json.loads(compile_db.read_text(encoding="utf-8"))
        return sorted(
            {
                entry["file"]
                for entry in entries
                if not SOURCE_EXCLUDE.search(str(Path(entry["file"]).resolve()))
            }
        )

    def run(self) -> int:
        """Check every project source with run-clang-tidy."""
        cmd = [
            "run-clang-tidy",
            "-p",
            str(self.build_dir),
            "-header-filter",
            HEADER_FILTER,
        ]
        if self.fix:
            cmd.append("-fix")
        if self.jobs is not None:
            cmd.extend(["-j", str(self.jobs)])
        cmd.extend(self.sources())
        return self._run_subprocess(cmd)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Run clang-tidy over this project with a compilation database.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "build_mode",
        nargs="?",
        choices=tuple(CMAKE_BUILD_TYPES),
        default="release",
        help="build mode whose compile_commands.json is used",
    )
    parser.add_argument(
        "--fix",
        action="store_true",
        help="apply the suggested fixes in place",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=None,
        help="number of clang-tidy processes to run in parallel",
    )
    args = parser.parse_args(argv)

    clang_tidy = ClangTidy(args.build_mode, fix=args.fix, jobs=args.jobs)
    return clang_tidy.run()


if __name__ == "__main__":
    raise SystemExit(main())
