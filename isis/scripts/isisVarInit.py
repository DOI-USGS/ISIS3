#!/usr/bin/env python
"""
This program builds shell scripts that define the ISIS environment
variables that are sourced during conda environment activation and
removed at deactivation.

If the directories don't exist, they are created.  If their path is
not specified, they are placed in $CONDA_PREFIX (which is $ISISROOT).
"""

import argparse
import logging
import os
from pathlib import Path

# This is free and unencumbered software released into the public domain.
#
# The authors of ISIS do not claim copyright on the contents of this file.
# For more details about the LICENSE terms and the AUTHORS, you will
# find files of those names at the top level of this repository.
#
# SPDX-License-Identifier: CC0-1.0


class ResolveAction(argparse.Action):
    """A custom action that returns the absolute version of the provided
    pathlib.Path argument.
    """

    def __init__(self, option_strings, dest, type=None, **kwargs):
        if issubclass(Path, type):
            super(ResolveAction, self).__init__(
                option_strings, dest, type=type, **kwargs
            )
        else:
            raise TypeError(
                f"The type= parameter of argument {dest} must be a "
                f"class or subclass of pathlib.Path."
            )

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, values.resolve())


def mkdir(p: Path) -> str:
    """Returns a string with a message about the creation or existance
    of the path, *p*."""
    if p.exists():
        return f"{p} exists, don't need to create."
    else:
        p.mkdir(parents=False)
        return f"Created {p}"


def activate_text(shell: dict, env_vars: dict, cat=False) -> str:
    """Returns the formatted text to write to the activation script
    based on the passed dictionaries."""

    lines = [shell["shebang"]]
    for k, v in env_vars.items():
        lines.append(shell["activate"].format(k, v))

    if shell["activate_extra"] != "":
        lines.append(shell["activate_extra"])

    if cat:
        lines.append("cat $ISISROOT/version")

    return "\n".join(lines)


def deactivate_text(shell: dict, env_vars: dict) -> str:
    """Returns the formatted text to write to the deactivation script
    based on the passed dictionaries."""

    lines = [shell["shebang"]]
    for k in env_vars.keys():
        lines.append(shell["deactivate"].format(k))

    return "\n".join(lines)


# Set up and then parse the command line:
parser = argparse.ArgumentParser(description=__doc__)

parser.add_argument(
    "-d",
    "--data-dir",
    default=Path(os.environ["CONDA_PREFIX"] + "/data"),
    type=Path,
    action=ResolveAction,
    help="ISIS Data Directory, default: %(default)s",
)
parser.add_argument(
    "-t",
    "--test-dir",
    default=Path(os.environ["CONDA_PREFIX"] + "/testData"),
    type=Path,
    action=ResolveAction,
    help="ISIS Test Data Directory, default: %(default)s",
)

parser.add_argument(
    "-a",
    "--ale-dir",
    default=Path(os.environ["CONDA_PREFIX"] + "/aleData"),
    type=Path,
    action=ResolveAction,
    help="ISIS Ale Data Directory, default: %(default)s",
)
parser.add_argument(
    "-c", "--cat",
    action="store_true",
    help="If given, the activation script will include a 'cat' action that "
         "will print the ISIS version information to STDOUT every time the "
         "ISIS environment is activated."
)
parser.add_argument(
    "-q", "--quiet",
    action="store_true",
    help="If given, will suppress all regular text output."
)
args = parser.parse_args()

log_lvl = {False: "INFO", True: "ERROR"}

logging.basicConfig(format="%(message)s", level=log_lvl[args.quiet])

logging.info("-- ISIS Data Directories --")
# Create the data directories:
logging.info(mkdir(args.data_dir))
logging.info(mkdir(args.test_dir))
logging.info(mkdir(args.ale_dir))

logging.info("-- Conda activation and deactivation scripts --")
# Create the conda activation and deactivation directories:
activate_dir = Path(os.environ["CONDA_PREFIX"]) / "etc/conda/activate.d"
deactivate_dir = Path(os.environ["CONDA_PREFIX"]) / "etc/conda/deactivate.d"

logging.info(mkdir(activate_dir))
logging.info(mkdir(deactivate_dir))

# Set the environment variables to manage
env_vars = dict(
    ISISROOT=os.environ["CONDA_PREFIX"],
    ISISDATA=args.data_dir,
    ISISTESTDATA=args.test_dir,
    ALESPICEROOT=args.ale_dir
)

# These dicts define the unique aspects of the shell languages
# for setting and unsetting the environment variables.

sh = dict(  # this covers bash and zsh
    extension=".sh",
    shebang="#!/usr/bin/env sh",
    activate="export {}={}",
    activate_extra="",
    deactivate="unset {}"
)

csh = dict(
    extension=".csh",
    shebang="#!/usr/bin/env csh",
    activate="setenv {} {}",
    activate_extra="source $CONDA_PREFIX/scripts/tabCompletion.csh",
    deactivate="unsetenv {}"
)

fish = dict(
    extension=".fish",
    shebang="#!/usr/bin/env fish",
    activate="set -gx {} {}",
    activate_extra="",
    deactivate="set -e {}"
)

# Loop over the shell types and create the correct scripts associated with
# each:
for shell in (sh, csh, fish):
    a_path = activate_dir / ("isis-activate" + shell["extension"])
    a_path.write_text(activate_text(shell, env_vars, cat=args.cat))
    logging.info(f"Wrote {a_path}")

    d_path = deactivate_dir / ("isis-deactivate" + shell["extension"])
    d_path.write_text(deactivate_text(shell, env_vars))
    logging.info(f"Wrote {d_path}")
