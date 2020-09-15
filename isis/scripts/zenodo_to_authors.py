# Uses a .zenodo.json file to generate an AUTHORS.rst file. 

import os
import argparse
import json

parser = argparse.ArgumentParser(description="Generate an AUTHORS.rst file from a .zenodo.json file")
parser.add_argument("zenodo_input_file", type=str, help="The file path to the .zenodo.json file.")
parser.add_argument("authors_output_file", type=str, help="The path for the output AUTHORS.rst file.")

args = parser.parse_args()

zenodo_path = args.zenodo_input_file
authors_path = args.authors_output_file

# Read .zenodo.json file:
try: 
  with open(zenodo_path, 'r') as zenodo_file:
    parsed_json = json.load(zenodo_file)
except: 
  print("{} could not be read as the input file.".format(zenodo_path))
  raise

output_lines = ["ISIS3 Contributors", "==================\n"]

for elt in parsed_json['creators']:
  name = elt['name']
  last, first = name.split(",")
  output_lines.append("- {} {}".format(first.strip(),last.strip()))

output_lines.append("\n-----")
output_lines.append("This list was generated from the .zenodo.json file by running python zenodo_to_authors.py.")

# Write to AUTHORS.rst file
try:
  with open(authors_path, 'w') as authors_file:
    authors_file.write('\n'.join(output_lines))
except:
  print("{} could not be opened as the output file.".format(authors_path))
  raise

