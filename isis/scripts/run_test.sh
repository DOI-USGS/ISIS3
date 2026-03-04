#!/bin/bash
# Run an ISIS legacy Makefile test: generate output, compare, report.
# Called from isismake.tsts test target (after clean).
#
# Usage:
#   run_test.sh TRUTH OUTPUT CASE NOCLEAN
#
# Arguments:
#   TRUTH   - truth directory path
#   OUTPUT  - output directory path
#   CASE    - test case name (for display)
#   NOCLEAN - "yes" to keep output after success, anything else to clean

TRUTH="$1"
OUTPUT="$2"
CASE="$3"
NOCLEAN="$4"

TIME1=$(date +"%s")

if [ -d "$TRUTH" ]; then
  TRUTHDIR=$(ls "$TRUTH" 2>/dev/null)
  if [ "$TRUTHDIR" == "" ]; then
    CURTIMESTAMP="[$(date +'%Y-%m-%d %H:%M:%S')]"
    echo -n "$CURTIMESTAMP" >> errors.txt
    echo "  Failed ... No files in truth directory" >> errors.txt
  else
    make output
    TRUTHFILECOUNT=$(ls -p "$TRUTH"/ | grep -v "/" | wc -l)
    OUTPUTFILECOUNT=$(ls -p "$OUTPUT"/ | grep -v "/" | wc -l)
    if [ $TRUTHFILECOUNT -ne $OUTPUTFILECOUNT ]; then
      CURTIMESTAMP="[$(date +'%Y-%m-%d %H:%M:%S')]"
      echo -n "$CURTIMESTAMP" >> errors.txt
      echo "  Failed ... Number of files in truth and output folder do not match" >> errors.txt
      echo "Failed ... Number of files in truth and output folder do not match"
      echo "Truth file count: $TRUTHFILECOUNT"
      echo "Output file count: $OUTPUTFILECOUNT"
    else
      make comparefiles
    fi
  fi
else
  CURTIMESTAMP="[$(date +'%Y-%m-%d %H:%M:%S')]"
  echo -n "$CURTIMESTAMP" >> errors.txt
  echo "  Failed ... No truth folder or files" >> errors.txt
fi

TIME2=$(date +"%s")
TIMEDIFF=$((TIME2 - TIME1))
SEC60=$((TIMEDIFF % 60))
MIN=$((TIMEDIFF / 60))
MIN60=$((MIN % 60))
HOUR=$((MIN / 60))
HOUR60=$((HOUR % 60))

CURTIMESTAMP="[$(date +'%Y-%m-%d %H:%M:%S')]"
echo -n "$CURTIMESTAMP"

if [ -f casesucceeded.txt ]; then
  echo -n " OK ... Case [$CASE] "
  printf "Duration [%.2i:%.2i:%.2i]\n" $HOUR60 $MIN60 $SEC60
  rm -f casesucceeded.txt
  if [ "$NOCLEAN" != "yes" ]; then
    make ccsafeclean
  fi
else
  echo -n " Failed ... Case [$CASE] "
  printf "Duration [%.2i:%.2i:%.2i]\n" $HOUR60 $MIN60 $SEC60
  echo "Summary error file: $(pwd)/errors.txt"
fi
