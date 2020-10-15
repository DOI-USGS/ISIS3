#!/usr/bin/env python
"""
This program builds reads a .zenodo.json file and generates a more readable
reStructuredText file, suitable for AUTHORS.rst.
"""

import argparse
import json
import textwrap
import sys

from operator import itemgetter

# This is free and unencumbered software released into the public domain.
#
# The authors of ISIS do not claim copyright on the contents of this file.
# For more details about the LICENSE terms and the AUTHORS, you will
# find files of those names at the top level of this repository.
#
# SPDX-License-Identifier: CC0-1.0

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument(
    "zenodo_json",
    type=str,
    help="The file path to the .zenodo.json file."
)
parser.add_argument(
    "authors_rst",
    type=str,
    help="The path for the output AUTHORS.rst file."
)

args = parser.parse_args()

# Read .zenodo.json file:
try:
    with open(args.zenodo_json, 'r') as zenodo_file:
        parsed_json = json.load(zenodo_file)
except OSError as err:
    sys.exit(f"Could not open {args.zenodo_json} for reading: {err}")
except json.JSONDecodeError as err:
    sys.exit(f"Could not correctly parse JSON from {args.zenodo_json}: {err}")

output_lines = [
    """\
Integrated Software for Imagers and Spectrometers Contributors
==============================================================
"""
]

for author in sorted(parsed_json['creators'], key=itemgetter('name')):
    # family, given = author['name'].split(",")
    # line = f"- {given.strip()} {family.strip()}"
    line = author['name'].strip()
    if "affiliation" in author:
        line += f" ({author['affiliation'].strip()})"
    output_lines.extend(textwrap.wrap(
        line, initial_indent="- ", subsequent_indent="  "
    ))


output_lines.append("\n-----")
output_lines.extend(textwrap.wrap(
    "This list was generated from the .zenodo.json file by running the "
    "isis/scripts/zenodo_to_authors.py Python program."
))

# Write to AUTHORS.rst file
try:
    with open(args.authors_rst, 'w') as authors_file:
        authors_file.write('\n'.join(output_lines))
except OSError as err:
    sys.exit(f"Could not write out {args.authors_rst}: {err}")
