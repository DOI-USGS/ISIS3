#!/usr/bin/env python

"""
Compares two comma-separated-value files cell by cell, optionally using a
tolerance file for numeric types.  The tolerance file must contain one
tolerance per line of the form:

  COLUMN=VALUE

where COLUMN is the name of the tolerance value for every cell in that column,
and VALUE is the actual tolerance that will be compared against the difference
between the values in the two files.

Usage:

  csvdiff.py CSV1 CSV2 [TOLERANCES]

Output will be a message stating either SUCCESS or FAILURE followed by an
explanation of why.  Difference failures will also include a reference to the
exact line and column where the offending difference occurred.  The program
will terminate with an error signal when a failure occurs during processing.

Before any cells are compared the program will first ensure that the two CSV
files to be compared have the same number of columns and rows, additionally
checking that their headers match.

Future versions will support an option for submitting files without headers.
"""

import math
import sys


def is_number(candidate):
  """Tests a candidate string for having a numerical format."""
  try:
    float(candidate)
    return True
  except ValueError:
    return False


def open_file(filename, mode):
  """Attempt to open a file in the given access mode, exit on a failure."""
  try:
    file = open(filename, mode)
  except IOError:
    sys.exit("FAILURE Unable to open '" + filename + "'!")
  else:
    return file


# Open the first CSV file to be compared
csv_1_file = open_file(sys.argv[1], "r")

# Store off the header as a list for reference purposes
csv_1 = csv_1_file.readlines()
csv_1_header = csv_1[0].strip().upper().split(",")
csv_1 = csv_1[1:]

# Done with the file
csv_1_file.close()

# Open the second CSV file to be compared
csv_2_file = open_file(sys.argv[2], "r")

# Store off the header to compare against the other one
csv_2 = csv_2_file.readlines()
csv_2_header = csv_2[0].strip().upper().split(",")
csv_2 = csv_2[1:]

# Done with the file
csv_2_file.close()

# Make sure CSV files have the same number of columns
if len(csv_1_header) != len(csv_2_header):
  sys.exit("FAILURE Files have different number of columns!")

# Make sure CSV files have the same number of rows
if len(csv_1) != len(csv_2):
  sys.exit("FAILURE Files have different number of rows!")

# Make sure all column headers match between CSV files
for i in range(len(csv_1_header)):
  column_1 = csv_1_header[i]
  column_2 = csv_2_header[i]

  if column_1 != column_2:
    sys.exit("FAILURE Columns " +
        "'%s' and '%s' do not match!" % (column_1, column_2))

# Build a map of tolerances if the user specified one
tolerance_map = {}
if len(sys.argv) >= 4:
  # Open tolerances file
  tolerance_file = open_file(sys.argv[3], "r")

  # Break each line into a column part for each CSV and a tolerance component
  tolerances = tolerance_file.readlines()
  for line in tolerances:
    if line.strip() != '':
      column, tolerance = line.strip().upper().split("=")
      # Strip column and tolerance to allow for whitespace around "=" character
      tolerance_map[column.strip()] = float(tolerance.strip())

  # Close the file
  tolerance_file.close()

# Diff the CSV files
for i in range(len(csv_1)):
  # Break each row up into a set of cells
  cells_1 = csv_1[i].strip().split(",")
  cells_2 = csv_2[i].strip().split(",")

  # Header was removed, 0-based indexing, so we need to increment by 2 for
  # accurate line number
  line_num = i + 2
  if len(cells_1) != len(cells_2):
    sys.exit("FAILURE Rows have a different number of cells (line %d)!" %
        (line_num))

  # Compare the cells one at a time
  for j in range(len(cells_1)):
    # Get the value in each file's cell at the current index
    value_1 = cells_1[j]
    value_2 = cells_2[j]

    # Get the name of the column for this cell
    column_1 = j + 1
    if j < len(csv_1_header):
      column_1 = csv_1_header[j]

    if is_number(value_1) and is_number(value_2):
      # Generate the error and get the tolerance if provided, else assume 0.0
      error = math.fabs(float(value_1) - float(value_2))
      tolerance = 1e-9
      if column_1 in tolerance_map:
        tolerance = tolerance_map[column_1]

      # If the error is greater than the tolerance, fail the program
      if error > tolerance:
        sys.exit("FAILURE Error '%.14f' is greater than tolerance '%.14f' " %
            (error, tolerance) + "(line %d, column %s)!" % (line_num, column_1))
    
    elif column_1 == "GISINTERSECTIONFOOTPRINT":
        if len(value_1) != len(value_2):
          sys.exit("FAILURE Value " +
            "'%s' length does not equal '%s' length " % (value_1, value_2) +
            "(line %d, column %s)!" % (line_num, column_1))
    else:
      # At least one of the values is a non-numeric type, so compare as strings
      if value_1 != value_2:
        sys.exit("FAILURE Value " +
            "'%s' does not equal '%s' " % (value_1, value_2) +
            "(line %d, column %s)!" % (line_num, column_1))

# No errors, all was good
print("SUCCESS All error values are within expected tolerance!")
