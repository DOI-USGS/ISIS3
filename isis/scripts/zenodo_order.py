#!/usr/bin/env python
"""
This program reads the .zenodo.json file finds the
designated first author, puts them first, and then puts the
remainder in alphabetical order.
"""

import argparse
import json
import sys

from operator import itemgetter

# This is nominally the ISIS project lead or whomever should be
# the first author in the Zenodo authors listing.
first_author = "Laura, Jason"

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
    help="The file path to the .zenodo.json file to read."
)
parser.add_argument(
    "json_out",
    type=str,
    help="The path to the output .json file to write."
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

# Find and extract the project lead
creators_list = parsed_json["creators"]

for i, d in enumerate(creators_list):
    if d["name"] == first_author:
        first_idx = i
        break
else:
    sys.exit(
        f"The designated first author ({project_lead_name}) was not found "
        f"in {args.zenodo_json}"
    )

first = creators_list.pop(first_idx)

# Create new creators list, put lead first, and then alphabetize
new_creators = [first]
new_creators += sorted(creators_list, key=itemgetter("name"))

# Replace
parsed_json["creators"] = new_creators

# Write out new json file
try:
    with open(args.json_out, 'w') as f:
        json.dump(parsed_json, f, indent=2)
except OSError as err:
    sys.exit(f"Could not write out {args.json_out}: {err}")
