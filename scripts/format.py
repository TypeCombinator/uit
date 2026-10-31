#!/usr/bin/env python3
import argparse
import subprocess
from collections.abc import Sequence

from build import PROJECT_ROOT

# Language -> formatter command and the git pathspecs of the sources it formats.
LANGUAGES = {
    "cpp": (
        ["clang-format", "-i"],
        ["*.c", "*.cc", "*.cpp", "*.cxx", "*.h", "*.hh", "*.hpp", "*.hxx"],
    ),
    "python": (["ruff", "format"], ["*.py"]),
    "toml": (["tombi", "format"], ["*.toml"]),
    "yaml": (
        ["prettier", "--print-width", "120", "--single-quote", "--write"],
        ["*.yml", "*.yaml", ".clang-format", ".clang-tidy"],
    ),
    "markdown": (["prettier", "--write"], ["*.md"]),
}


class Formatter:
    """Format this repository's tracked C++, Python, TOML, YAML or Markdown sources."""

    def __init__(self, language: str) -> None:
        self.command, self.patterns = LANGUAGES[language]

    @staticmethod
    def _run_subprocess(cmd: list[str]) -> int:
        """Print the command line and execute it from the project root."""
        print(f"$ {' '.join(cmd)}", flush=True)
        return subprocess.run(cmd, check=False, cwd=PROJECT_ROOT).returncode

    def sources(self) -> list[str]:
        """List the tracked sources of this language; build/ and deps are ignored."""
        result = subprocess.run(
            ["git", "ls-files", "-z", "--", *self.patterns],
            check=True,
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT,
        )
        return [path for path in result.stdout.split("\0") if path]

    def run(self) -> int:
        """Format every source in place."""
        return self._run_subprocess([*self.command, *self.sources()])


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Format this repository's tracked C++, Python, TOML, YAML or Markdown sources.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "language",
        choices=tuple(LANGUAGES),
        help="language of the sources to format",
    )
    args = parser.parse_args(argv)

    return Formatter(args.language).run()


if __name__ == "__main__":
    raise SystemExit(main())
