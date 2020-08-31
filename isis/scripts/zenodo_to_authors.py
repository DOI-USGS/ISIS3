# Uses the .zenodo.json file to generate an AUTHORS.rst file. 
# No arguments are necessary. Just set $ISISROOT and run: python zenodo_to_authors.py

import os

# Files to open
if "ISISROOT" in os.environ:
    zenodo_path = os.path.expandvars("$ISISROOT/../.zenodo.json")
    authors_path = os.path.expandvars("$ISISROOT/../AUTHORS.rst")
else:
    print("Please set $ISISROOT and re-run this script.")
    exit()

# Read .zenodo.json file:
lines = []
with open(zenodo_path, 'r') as zenodo_file:
   lines = zenodo_file.readlines() 
  
output_lines = ["ISIS3 Contributors", "==================\n"]

for line in lines:
    # Pull the names out of the zenodo file and reformat 
    if "\"name\"" in line:
        name = line.strip().split(":")[1].strip("\", ")
        last, first = name.split(",")
        output_lines.append("- {} {}".format(first.strip(),last.strip()))

output_lines.append("\n-----")
output_lines.append("This list was generated from the .zenodo.json file by running python zenodo_to_authors.py.")

# Write to AUTHORS.rst file
with open(authors_path, 'w') as authors_file:
   authors_file.write('\n'.join(output_lines))

