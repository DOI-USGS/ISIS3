#!/usr/bin/env python

"""
Compares two text files. All entries must be separated by spaces and/or commas
and be in one-to-one correspondence. Non-numerical values must be the same.
Numerical values must agree to within given absolute or relative tolerance.

Usage:

  textdiff.py TXT1 TXT2 -abs_err/-rel_err TOLERANCE

The output will be empty, unless there are differences. In that case, each pair
of lines that disagree will be printed, along with max error per line 
(relative or absolute).
"""

import math, sys

def is_number(candidate):
  """Tests a candidate string for having a numerical format."""
  try:
    float(candidate)
    return True
  except ValueError:
    return False

def read_lines(filename):
  """Attempt to read all lines from a file."""
  try:
    file = open(filename, "r")
  except IOError:
    sys.exit("ERROR: Unable to read '" + filename + "")
  
  lines = file.readlines()
  file.close()
  
  return lines

def parseIgnoreFile(ignoreFile):
  """Read the ignore file. Split each line by equal sign and spaces.
     Store the first entry in each line in a set.
  """
  ignoreSet = set()
  try:
    with open(ignoreFile, 'r') as f:
      for line in f:
        if line.strip() == "":
          continue
        # Replace equal with space
        line = line.replace("=", " ")
        # Split by spaces
        parts = line.split()
        if len(parts) > 1:
          ignoreSet.add(parts[0].strip())
  except IOError:
    sys.exit("ERROR: Unable to read '" + ignoreFile + "'")
  
  return ignoreSet
  
# Check that we got four values on the command line
if len(sys.argv) < 4:
  print("Not enough input arguments. Usage:\n" +
      "  textdiff.py TXT1 TXT2 -abs_err/-rel_err TOLERANCE")
  sys.exit(1)

# Convert third argument to lower case. Must then be either "-abs_err" or "-rel_err".
relative = False
errType = ""
if sys.argv[3].lower() == "-abs_err":
  relative = False
  errType = "absolute error"
elif sys.argv[3].lower() == "-rel_err":
  relative = True
  errType = "relative error"
else:
  print("ERROR: Fourth argument must be 'abs_err' or 'rel_err'.")
  sys.exit(1)

# Fourth argument must be convertible to a positive tol
try:
  tolerance = float(sys.argv[4])
  if tolerance < 0.0:
    print("ERROR: Tolerance must be a positive number.")
    sys.exit(1)
except ValueError:
  print("ERROR: Tolerance must be a number.")
  sys.exit(1)

# The 5th argument may be the file having the fields to ignore
ignoreSet = set()
if len(sys.argv) > 5:
  ignoreFile = sys.argv[5]
  ignoreSet = parseIgnoreFile(ignoreFile)
  
# Start with success status
status = 0

lines1 = read_lines(sys.argv[1])
lines2 = read_lines(sys.argv[2])

# The number of lines in the two files must be the same
if len(lines1) != len(lines2):
  print("ERROR: Files have different number of lines.")
  sys.exit(1)

# Iterate through the lines of the two files
maxErr = 0.0
for i in range(len(lines1)):
  # Replace any comma by space
  lines1[i] = lines1[i].replace(",", " ")
  lines2[i] = lines2[i].replace(",", " ")
  
  # Split the lines into a list of strings
  words1 = lines1[i].strip().split()
  words2 = lines2[i].strip().split()
  
  # Skip empty lines
  if len(words1) == 0 and len(words2) == 0:
    continue
    
  # Must have the same number of words
  if len(words1) != len(words2):
    print("ERROR: Lines have different number of words.")
    print("First  file line: " + str(i+1) + ": " + lines1[i].strip())
    print("Second file line: " + str(i+1) + ": " + lines2[i].strip())
    sys.exit(1)
  
  # Skip if first word is to be ignored
  if len(words1) > 0 and words1[0] in ignoreSet:
    continue
  if len(words2) > 0 and words2[0] in ignoreSet:
    continue
    
  # Iterate through the words
  for j in range(len(words1)):

    # Skip if not both are numbers
    is_num1 = is_number(words1[j])
    is_num2 = is_number(words2[j])
    if not is_num1 and not is_num2:
      continue
   
    # Convert to float 
    val1 = float(words1[j])
    val2 = float(words2[j])
    
    err = 0.0
    if relative:
      if val1 == 0.0 and val2 == 0.0:
        err = 0.0
      else: 
        err = abs(val1 - val2)/max(abs(val1), abs(val2))
    else:
      err = abs(val1 - val2)
    
    maxErr = max(maxErr, err)
    
    if err > tolerance:
      print("ERROR: Got " + errType + " of " + \
            str(err) + " at line " + str(i+1) + ", word " + str(j+1))
      print("First  file line: " + str(i+1) + ": " + lines1[i].strip())
      print("Second file line: " + str(i+1) + ": " + lines2[i].strip())
      # Record failure but keep going, so we can print all failures
      status = 1

print("Input tolerance: " + str(tolerance))
print("Max observed " + errType + ": " + str(maxErr))
sys.exit(status)
