#!/usr/bin/env python
"""
This program builds shell scripts that define the ISIS environment
variables that are sourced during conda environment activation and
removed at deactivation.

If the directories don't exist, they are created.  If their path is
not specified, they are placed in $ISISROOT.
"""

import argparse
import os
import shutil
from pathlib import Path
from typing import List, Tuple

# This is free and unencumbered software released into the public domain.
#
# The authors of ISIS do not claim copyright on the contents of this file.
# For more details about the LICENSE terms and the AUTHORS, you will
# find files of those names at the top level of this repository.
#
# SPDX-License-Identifier: CC0-1.0

# This is the full path to this environment:
CONDA_PREFIX = os.environ["CONDA_PREFIX"]
ENVS_DIR = Path(CONDA_PREFIX).parent
THIS_ENV = Path(CONDA_PREFIX).name


def get_conda_envs() -> List[Path]:
    """Returns a list of paths to the found conda envs.

    This method is used because
    (1) there is no API for `conda env`,
    (2) `subprocess.check_output` and `sh.conda` are not producing the same output.
    """
    print(f"Searching for other conda envs in {ENVS_DIR} ...")
    paths = [d for d in ENVS_DIR.iterdir() if d.is_dir()]
    return [p.name for p in paths if p.name != THIS_ENV]


def get_envs_to_init() -> List[Path]:
    """Find conda envs and ask user which ones should initialize ISIS."""
    env_names = get_conda_envs()
    print("Found the following other conda envs:")
    for i, env in enumerate(env_names):
        print(i, env)
    print("Which envs should receive the ISIS path initialization?")
    print("(Press RETURN for skipping.)")
    answer = ""
    answer = input("Comma-separated list of env indexes (e.g. 1,2,4):")
    if not answer:
        return []
    try:
        envs_to_init = [env_names[int(i)] for i in answer.split(",")]
    except ValueError:
        raise ValueError(
            "Could not parse your answer.\nDid you use a comma-separated list?"
        )
    return envs_to_init


def mkdir(p: Path) -> str:
    """Create directory `p` if not exists, and notify user when created."""

    if p.exists():
        return p
    else:
        p.mkdir(parents=False)
        return f"Created {p}"


def activate_text(shell: dict, env_vars: dict) -> str:
    """Returns the formatted text to write to the activation script
    based on the passed dictionaries."""

    lines = [shell["shebang"]]
    for k, v in env_vars.items():
        lines.append(shell["activate"].format(k, v))

    if shell["activate_extra"] != "":
        lines.append(shell["activate_extra"])

    lines.append(shell['path_update'])
    lines.append("echo 'ISIS version info:'")
    lines.append("cat $ISISROOT/version")

    return "\n".join(lines)


def deactivate_text(shell: dict, env_vars: dict) -> str:
    """Returns the formatted text to write to the deactivation script
    based on the passed dictionaries."""

    lines = [shell["shebang"]]
    for k in env_vars.keys():
        lines.append(shell["deactivate"].format(k))

    return "\n".join(lines)


def get_paths_make_dirs(env: str) -> Tuple[Path]:
    "Create act/deact paths, create the dirs, and return paths."

    # Create the conda activation and deactivation directories:
    activate_dir = ENVS_DIR / f"{env}/etc/conda/activate.d"
    deactivate_dir = ENVS_DIR / f"{env}/etc/conda/deactivate.d"

    mkdir(activate_dir)
    mkdir(deactivate_dir)
    return activate_dir, deactivate_dir


# Set up and then parse the command line:
parser = argparse.ArgumentParser(description=__doc__)


parser.add_argument(
    "-d",
    "--data-dir",
    default=CONDA_PREFIX + "/data",
    type=Path,
    help="ISIS Data Directory, default: %(default)s",
)
parser.add_argument(
    "-t",
    "--test-dir",
    default=CONDA_PREFIX + "/testData",
    type=Path,
    help="ISIS Test Data Directory, default: %(default)s",
)

parser.add_argument(
    "-a",
    "--ale-dir",
    default=CONDA_PREFIX + "/aleData",
    type=Path,
    help="ISIS Ale Data Directory, default: %(default)s",
)

parser.add_argument(
    "--init-other-envs",
    action="store_true",
    help="Launch dialog to find other conda environments to be initialized.",
)

args = parser.parse_args()

print("-- ISIS Data Directories --")
# Create the data directories:
print(mkdir(args.data_dir))
print(mkdir(args.test_dir))
print(mkdir(args.ale_dir))
print()

# always init this env:
envs_to_init = [THIS_ENV]

# if wanted, ask user which other conda envs should get ISIS-initialized
if args.init_other_envs:
    envs_to_init += get_envs_to_init()

# Set the environment variables to manage
env_vars = dict(
    ISISROOT=CONDA_PREFIX,
    ISISDATA=args.data_dir,
    ISISTESTDATA=args.test_dir,
    ALESPICEROOT=args.ale_dir,
)

# These dicts define the unique aspects of the shell languages
# for setting and unsetting the environment variables.

sh = dict(  # this covers bash and zsh
    name="bash/zsh",
    extension=".sh",
    shebang="#!/usr/bin/env sh",
    activate="export {}={}",
    activate_extra="",
    deactivate="unset {}",
    path_update="export PATH=$PATH:$ISISROOT/bin"
)

csh = dict(
    name="csh",
    extension=".csh",
    shebang="#!/usr/bin/env csh",
    activate="setenv {} {}",
    activate_extra="source $CONDA_PREFIX/scripts/tabCompletion.csh",
    deactivate="unsetenv {}",
    path_update="setenv PATH $PATH:$ISISROOT/bin"
)

fish = dict(
    name="fish",
    extension=".fish",
    shebang="#!/usr/bin/env fish",
    activate="set -gx {} {}",
    activate_extra="",
    deactivate="set -e {}",
    path_update="set -agx PATH $ISISROOT/bin"
)

# Loop over the shell types and create the correct scripts associated with
# each:
print("\nInit files will be written for the following conda environments:")
print(envs_to_init)

for env in envs_to_init:
    print(f"\n-- Initializing {env} conda env --\n")
    a_dir, d_dir = get_paths_make_dirs(env)
    print(f"Activation in {a_dir}")
    print(f"Deactivaton in {d_dir}")
    for shell in (sh, csh, fish):
        print(f"{shell['name']} shell:")
        a_path = a_dir / ("isis-activate" + shell["extension"])
        a_path.write_text(activate_text(shell, env_vars))
        print(f"Wrote {a_path.name}")

        d_path = d_dir / ("isis-deactivate" + shell["extension"])
        d_path.write_text(deactivate_text(shell, env_vars))
        print(f"Wrote {d_path.name}")
