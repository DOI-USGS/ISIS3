#!/usr/bin/env python

"""
Compares two text files. All entries must be separated by spaces and/or commas
and be in one-to-one correspondence. Non-numerical values must be the same.
Numerical values must agree to within given absolute or relative tolerance.

Usage:

  textdiff.py TXT1 TXT2 -abs_err/-rel_err TOLERANCE [IGNORE_FIELDS_FILE]

The output will be empty, unless there are differences. In that case, each pair
of lines that disagree will be printed, along with max error per line 
(relative or absolute).

The last line that is printed will have the max of all discrepancies,
which is useful for updating the tolerance in the texts.
"""

import math, sys, os, re

def is_number(candidate):
  """Tests a candidate string for having a numerical format."""
  try:
    float(candidate)
    return True
  except ValueError:
    return False

def fixProblemLines(lines, problemLines):

  """" 
    Each block of problem lines must be concatenated, also with the next line
    after the last problem line in a block.
  """
  
  numLines = len(lines)
  outLines = []
  removedLines = [0] * numLines
  for lineCount in range(numLines):
  
    if removedLines[lineCount] == 1:
      # This was appended and removed
      continue
      
    if problemLines[lineCount] == 0:
      # Not a problem line
      outLines.append(lines[lineCount])
      continue
      
    # Iterate till the end until no more problem lines
    lastProblemLine = lineCount
    for probCount in range(lineCount, numLines):
      if problemLines[probCount] == 0:
        break
      lastProblemLine = probCount
    
    # We need to append the line after the last problem line, so increment.
    lastProblemLine += 1
    
    # Must not exceed the number of lines
    if lastProblemLine >= numLines:
      lastProblemLine = numLines - 1
    
    outLine = ""  
    # Append the lines and flag them as appended.
    for probCount in range(lineCount, lastProblemLine + 1):
     
      # For all lines after the first problem one, wipe all leading spaces, as
      # that is part of continuation. 
      if probCount > lineCount:
        lines[probCount] = lines[probCount].lstrip()
        
      outLine += lines[probCount]
      removedLines[probCount] = 1
      
    # Append the line to the output
    outLines.append(outLine)  
    
  return outLines

def fixPvlLines(lines):
  """ 
   Fix for PVL files. If the line has about 79 characters, and the last
   character is a dash, then it is a continuation line. Remove the dash
   and add the next line to the current line. Below will also wipe
   leading spaces on continuation lines. Also fix for quoted text
   continuing on multiple lines.
  """

  problemLines = [0] * len(lines)
  for lineCount in range(len(lines)):
    line = lines[lineCount]
    if (len(line) >= 78 and len(line) <= 80) and line[-1] == "\n" and line[-2] == "-" \
      and lineCount < len(lines) - 1 and \
      lines[lineCount + 1][0:5].strip() == '': # next line starts with lots of spaces
      # Remove the last two characters
      lines[lineCount] = line[:-2]
      # Flag as problem line
      problemLines[lineCount] = 1

  # Fix the problem lines
  lines = fixProblemLines(lines, problemLines)
  
  # Fix for some quoted text continuing on multiple lines
  numQuotes = 0
  numOpenParens = 0
  numCloseParens = 0
  problemLines = [0] * len(lines)
  for lineCount in range(len(lines)):
     # Count the number of quotes and parentheses. Unbalanced lines need merging.
     numQuotes += lines[lineCount].count("'")
     numQuotes += lines[lineCount].count('"')
     numOpenParens += lines[lineCount].count('(')
     numCloseParens += lines[lineCount].count(')')
     
     # If it is even, we are good, so continue
     if numQuotes % 2 == 0 and numOpenParens == numCloseParens:
       continue
      
     # Flag as problem line
     problemLines[lineCount] = 1

  # Fix the problem lines
  lines = fixProblemLines(lines, problemLines)  

  return lines

def skipIgnoreLines(lines, ignoreSet):
  """ 
   Skip lines that have the first word in the ignore set.  
  """
  
  outLines = []
  for lineCount in range(len(lines)):
  
    # Split the line by spaces
    words = lines[lineCount].strip().split()
    
    if len(words) > 0 and words[0] in ignoreSet:
      continue
    
    # Also skip empty lines. PVL ignores them.
    if len(words) == 0:
      continue

    # Temporary fix for a python warning that breaks tests
    if 'pkg_resources' in lines[lineCount]:
      continue

    outLines.append(lines[lineCount])
      
  return outLines
  
def read_lines(filename, ignoreSet):
  """Attempt to read all lines from a file."""
  try:
    # Will ignore binary data
    file = open(filename, "r", encoding='utf-8', errors='ignore')
  except IOError:
    sys.exit("ERROR: Unable to read '" + filename + "")

  lines = file.readlines()
  file.close()
  
  # Fix for PVL files
  if filename.lower().endswith(".pvl"):
    lines = fixPvlLines(lines)

  # Skip lines that have the first word in the ignore set.
  # Skip lines starting with comments.
  ignoreSet.add("#")
  lines = skipIgnoreLines(lines, ignoreSet)
  
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
        if len(parts) > 0:
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

print("First file: " + os.path.abspath(sys.argv[1]))
print("Second file: " + os.path.abspath(sys.argv[2]))

lines1 = read_lines(sys.argv[1], ignoreSet)
lines2 = read_lines(sys.argv[2], ignoreSet)

# The number of lines in the two files must be the same
if len(lines1) != len(lines2):
  print("ERROR: Files have different number of lines.")
  # # This is helpful with debugging but will result in complains
  # # about extraneous files later.
  # file1 = os.path.abspath(sys.argv[1]) + ".preprocessed"
  # file2 = os.path.abspath(sys.argv[2]) + ".preprocessed"
  # print("The files to compare after internal preprocessing:")
  # print("File 1: " + file1)
  # print("File 2: " + file2)
  # print("Delete these after debugging, to avoid failures with different number of files.")
  # try:
  #   with open(file1, "w") as f1:
  #     f1.writelines(lines1)
  #   with open(file2, "w") as f2:
  #     f2.writelines(lines2)
  # except IOError:
  #   print("ERROR: Unable to write '" + file1 + "' or '" + file2 + "'")
  #   sys.exit(1)
  sys.exit(1)

# Iterate through the lines of the two files
maxErr = 0.0
for i in range(len(lines1)):
  # Replace any comma and equal sign by space
  lines1[i] = re.sub(r"[,=]+", " ", lines1[i])
  lines2[i] = re.sub(r"[,=]+", " ", lines2[i])
  
  # Split the lines into a list of strings
  words1 = lines1[i].strip().split()
  words2 = lines2[i].strip().split()
  
  # Skip empty lines
  if len(words1) == 0 and len(words2) == 0:
    continue

  # Skip if first word is to be ignored
  if len(words1) > 0 and words1[0] in ignoreSet:
    continue
  if len(words2) > 0 and words2[0] in ignoreSet:
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

# Put the max observed error as the last line. It will be printed by the failing
# test and used to update the tolerance.
print("Max observed " + errType + ": " + str(maxErr))

sys.exit(status)
