#!/bin/bash
# Manual re-comparison of truth vs output for ISIS legacy Makefile tests.
# Called from isismake.tsts compare target. Allows re-running the comparison
# without re-running the test commands.
#
# Usage:
#   compare_test_results.sh TRUTH OUTPUT CASE

TRUTH="$1"
OUTPUT="$2"
CASE="$3"

CURTIMESTAMP="[$(date +'%Y-%m-%d %H:%M:%S')]"

if [ -d "$OUTPUT" ]; then
  if [ -d "$TRUTH" ]; then
    TRUTHDIR=$(ls "$TRUTH" 2>/dev/null)
    if [ "$TRUTHDIR" == "" ]; then
      echo -n "$CURTIMESTAMP" >> errors.txt
      echo "  Failed ... No files in truth directory" >> errors.txt
    else
      TRUTHFILECOUNT=$(ls -p "$TRUTH"/ | grep -v "/" | wc -l)
      OUTPUTFILECOUNT=$(ls -p "$OUTPUT"/ | grep -v "/" | wc -l)
      if [ $TRUTHFILECOUNT -ne $OUTPUTFILECOUNT ]; then
        echo -n "$CURTIMESTAMP" >> errors.txt
        echo "  Failed ... Number of files in truth and output folder do not match" >> errors.txt
      else
        make comparefiles
      fi
    fi
  else
    echo -n "$CURTIMESTAMP" >> errors.txt
    echo "  Failed ... No truth folder or files" >> errors.txt
  fi

  if [ -f casesucceeded.txt ]; then
    echo " OK ... Case [$CASE]"
    rm -f casesucceeded.txt
  else
    echo " Failed ... Case [$CASE]"
    echo "$CURTIMESTAMP"
    if [ -f errors.txt ]; then
      cat errors.txt
      rm -f errors.txt
    fi
  fi
else
  echo "Error ... Output folder does not exist"
fi
