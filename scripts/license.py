#!/usr/bin/env python3
import argparse
import subprocess
from collections.abc import Sequence

from build import PROJECT_ROOT

SPDX_LICENSE = "MPL-2.0"
TEMPLATE = "uit"

# Directories to annotate with `license.py add`; extend this list to cover more.
ADD_TARGETS = ("examples",)


class License:
    """Manage project licenses with the REUSE tool."""

    @staticmethod
    def _run_subprocess(cmd: list[str]) -> int:
        """Print the command line and execute it from the project root."""
        print(f"$ {' '.join(cmd)}", flush=True)
        return subprocess.run(cmd, check=False, cwd=PROJECT_ROOT).returncode

    def add(self) -> int:
        """Add SPDX license headers to the configured source directories."""
        return self._run_subprocess(
            [
                "reuse",
                "annotate",
                "--recursive",
                "--year",
                "2026",
                "--copyright",
                "TypeCombinator <typecombinator@foxmail.com>",
                "--template",
                TEMPLATE,
                "--license",
                SPDX_LICENSE,
                *ADD_TARGETS,
            ]
        )

    def download(self) -> int:
        """Download the license text of the project's SPDX license."""
        return self._run_subprocess(
            ["reuse", "download", "--output", "LICENSE", SPDX_LICENSE]
        )


ACTIONS = {
    "add": "add SPDX license headers to sources",
    "download": "download the license text into LICENSE",
}


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Manage project licenses with the REUSE tool.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    subparsers = parser.add_subparsers(dest="action", required=True, metavar="action")
    for action, help_text in ACTIONS.items():
        subparsers.add_parser(action, help=help_text)
    args = parser.parse_args(argv)

    return getattr(License(), args.action)()


if __name__ == "__main__":
    raise SystemExit(main())
