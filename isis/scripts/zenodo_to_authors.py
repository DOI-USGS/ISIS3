# Uses the .zenodo.json file to generate an AUTHORS.rst file. 
# No arguments are necessary if $ISISROOT is set.

import os
import argparse
import json

parser = argparse.ArgumentParser(description="Generate an AUTHORS.rst file from a .zenodo.json file")
parser.add_argument("zenodo_input_file", nargs='?', type=str, 
                    help="The file path to the .zenodo.json file.", default="$ISISROOT/../.zenodo.json")
parser.add_argument("authors_output_file", nargs='?', type=str, 
                    help="The path for the output AUTHORS.rst file.", default="$ISISROOT/../AUTHORS.rst")

args = parser.parse_args()

zenodo_path = args.zenodo_input_file
authors_path = args.authors_output_file

# Files to open
if (("$ISISROOT" in args.zenodo_input_file) or ("$ISISROOT" in args.authors_output_file)):
  if ("ISISROOT" in os.environ):
    zenodo_path = os.path.expandvars(args.zenodo_input_file)
    authors_path = os.path.expandvars(args.authors_output_file)
  else:
    print("Please set $ISISROOT and re-run this script or specify the input and output files.")
    exit()

# Read .zenodo.json file:
lines = []
try: 
  with open(zenodo_path, 'r') as zenodo_file:
    parsed_json = json.load(zenodo_file)
except: 
  print("{} could not be opened as the input file.".format(zenodo_path))
  exit()

output_lines = ["ISIS3 Contributors", "==================\n"]

for elt in parsed_json['creators']:
  name = elt['name']
  last, first = name.split(",")
  output_lines.append("- {} {}".format(first.strip(),last.strip()))

output_lines.append("\n-----")
output_lines.append("This list was generated from the .zenodo.json file by running python zenodo_to_authors.py.")

# Write to AUTHORS.rst file
with open(authors_path, 'w') as authors_file:
   authors_file.write('\n'.join(output_lines))

