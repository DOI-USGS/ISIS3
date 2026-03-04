#!/bin/bash
# Compare truth and output files for ISIS legacy Makefile tests.
# Called from isismake.tsts comparefiles target.
#
# Usage:
#   compare_test_files.sh TRUTH OUTPUT INPUT ISISROOT TEST_ARGS \
#     BINFILES TOLERANCES IGNORESPECIALS
#
# Arguments:
#   TRUTH          - truth directory path
#   OUTPUT         - output directory path
#   INPUT          - input directory path
#   ISISROOT       - ISIS root (build dir)
#   TEST_ARGS        - test arguments (e.g., -preference=...)
#   BINFILES       - space-separated binary file extensions
#   TOLERANCES     - Make-collected tolerance string
#                    (e.g., "file.cub.TOLERANCE;0.001 file2.cub.TOLERANCE;0.01")
#   IGNORESPECIALS - Make-collected ignore-special string
#                    (e.g., "file.cub.IGNORESPECIAL;yes")

TRUTH="$1"
OUTPUT="$2"
INPUT="$3"
ISISROOT="$4"
TEST_ARGS="$5"
BINFILES="$6"
TOLERANCES="$7"
IGNORESPECIALS="$8"

FILES=$(ls -1 "$TRUTH"/ 2>/dev/null | grep -v '/$')
TRUTHFILECOUNT=$(echo "$FILES" | grep -c .)
SUCCESSFULCOMPARISONS=0

for FILE in $FILES; do

  if [ ! -f "$OUTPUT/$FILE" ]; then
    echo "Failed ... File does not exist [$FILE]"
    echo "  Failed ... File does not exist [$FILE]" >> errors.txt
  else
    FILETYPE=${FILE##*.}
    FAILURE_REPORT="$OUTPUT/$FILE.err.txt"

    if [ "$FILETYPE" == "cub" ]; then
      # Parse .cub tolerance based on cub name
      TOLERANCESTR=""
      if [ -n "$TOLERANCES" ]; then
        TOLERANCESTR=$(echo "$TOLERANCES" \
                       | perl -p -e "s#\s#\n#g" \
                       | grep "$FILE" | grep TOLERANCE \
                       | perl -p -e "s#^.*?TOLERANCE;##g")
      fi
      if [ "$TOLERANCESTR" != "" ]; then
        echo "Tolerance from Makefile for $FILE: $TOLERANCESTR"
      else
        TOLERANCESTR=1e-8
        echo "Default tolerance for $FILE: $TOLERANCESTR"
      fi

      # Parse ignore-special
      IGNORESTR=""
      if [ -n "$IGNORESPECIALS" ]; then
        for IGNOREDFILE in $IGNORESPECIALS; do
          IGNOREDNAME=$(echo "$IGNOREDFILE" | sed "s/;.*//")
          if [ "$IGNOREDNAME" == "$FILE.IGNORESPECIAL" ]; then
            IGNOREDVALUE=$(echo "$IGNOREDFILE" | sed "s/^.*;//")
            IGNORESTR="ignorespecial=$IGNOREDVALUE"
            break
          fi
        done
      fi
    else
      # Parse tolerance for non-cub
      TOL_FILE="$INPUT/$FILE.TOL"
      REPO_TOL_FILE="$(pwd)/$FILE.TOL"
      if [ -f "$REPO_TOL_FILE" ]; then
        TOL_FILE="$REPO_TOL_FILE"
      fi
      TOLERANCESTR=1e-8
      if [ -f "$TOL_FILE" ]; then
        TOLERANCESTR=$(cat "$TOL_FILE")
      fi
    fi

    DIFFFILE_CANDIDATE="$INPUT/$FILE.DIFF"
    DIFFFILE="$DIFFFILE_CANDIDATE"
    if [ ! -f "$DIFFFILE" ]; then
      DIFFFILE=""
    fi

    # cub
    if [ "$FILETYPE" == "cub" ]; then
      "$ISISROOT/bin/cubediff" \
         $TEST_ARGS \
         "from=$TRUTH/$FILE" \
         "from2=$OUTPUT/$FILE" \
         "to=$FAILURE_REPORT" \
         "TOLERANCE=$TOLERANCESTR" \
         $IGNORESTR 1>>/dev/null 2> "$FAILURE_REPORT"
      res=$?
      if [ $res != 0 ]; then
        # cubediff cannot handle different number of special pixels.
        echo "cubediff failed. Try GDAL."
        truthStats="${FILE}.truth.txt"; currStats="${FILE}.curr.txt"
        gdalinfo -stats "$TRUTH/$FILE" > "$truthStats"
        gdalinfo -stats "$OUTPUT/$FILE" > "$currStats"
        rm -f "$TRUTH/$FILE.aux.xml" "$OUTPUT/$FILE.aux.xml"
        python "$ISISROOT/scripts/textdiff.py" "$truthStats" \
            "$currStats" -abs_err "$TOLERANCESTR" $DIFFFILE \
            > "$FAILURE_REPORT" 2>&1
        res=$?
      fi

      if [ $res != 0 ]; then
        COMPRESULT="ERROR"
      else
        COMPRESULT="identical"
      fi
      if [ "$COMPRESULT" != "identical" ]; then
        echo "Failed ... File [$FILE]"
        echo "  Failed ... File [$FILE]" >> errors.txt
        if [ "$COMPRESULT" == "ERROR" ]; then
          cat "$FAILURE_REPORT" >> errors.txt
        fi
        echo "Failed comparing cubes. Report: $(pwd)/$FAILURE_REPORT"
        echo "Add/update the truth cube tolerance in the test Makefile."
        echo "Truth cube that needs tolerance: $FILE"
        echo "File with fields to ignore: $DIFFFILE_CANDIDATE"
        tail -n 1 "$FAILURE_REPORT"
      else
        SUCCESSFULCOMPARISONS=$((SUCCESSFULCOMPARISONS + 1))
        rm -f "$FAILURE_REPORT" 2>/dev/null
        rm -f "$truthStats"     2>/dev/null
        rm -f "$currStats"      2>/dev/null
        rm -f errors.txt        2>/dev/null
      fi

    # txt, csv, pvl
    elif [ "$FILETYPE" == "txt" ] || [ "$FILETYPE" == "csv" ] || \
         [ "$FILETYPE" == "pvl" ]; then
      python "$ISISROOT/scripts/textdiff.py" "$TRUTH/$FILE" \
          "$OUTPUT/$FILE" -abs_err "$TOLERANCESTR" $DIFFFILE \
          > "$FAILURE_REPORT" 2>&1

      if [ $? -ne 0 ]; then
        echo "Failed ... File [$FILE]"
        echo "  Failed ... File [$FILE]" >> errors.txt
        cat "$FAILURE_REPORT" >> errors.txt
        echo "Failed comparing txt. Report: $(pwd)/$FAILURE_REPORT"
        echo "Tolerance: $TOLERANCESTR"
        echo "Tolerance file to update: $TOL_FILE"
        echo "File with fields to ignore: $DIFFFILE_CANDIDATE"
        tail -n 1 "$FAILURE_REPORT"
      else
        SUCCESSFULCOMPARISONS=$((SUCCESSFULCOMPARISONS + 1))
        if [ -f "$FAILURE_REPORT" ]; then
          rm -f "$FAILURE_REPORT"
        fi
      fi

    # cnet
    elif [ "$FILETYPE" == "net" ]; then
      "$ISISROOT/bin/cnetdiff" \
          $TEST_ARGS \
          "from=$TRUTH/$FILE" \
          "from2=$OUTPUT/$FILE" \
          to=compare.txt \
          "diff=$DIFFFILE" 1>>/dev/null 2>cnetdiffError.txt
      if [ $? != 0 ]; then
        COMPRESULT="ERROR"
      else
        COMPRESULT=$("$ISISROOT/bin/getkey" from=compare.txt \
          grp=Results keyword=Compare | tr '[:upper:]' '[:lower:]')
      fi
      if [ "$COMPRESULT" != "identical" ]; then
        echo "Failed ... File [$FILE]"
        echo "  Failed ... File [$FILE]" >> errors.txt
        if [ "$COMPRESULT" == "ERROR" ]; then
          cat cnetdiffError.txt >> errors.txt
        fi
        echo "Failed comparing cnet"
        cat compare.txt
        echo "File with fields to ignore: $DIFFFILE_CANDIDATE"
      else
        SUCCESSFULCOMPARISONS=$((SUCCESSFULCOMPARISONS + 1))
      fi
      rm -f compare.txt
      if [ -f cnetdiffError.txt ]; then
        rm -f cnetdiffError.txt
      fi

    # binary
    elif echo "$BINFILES" | grep -qw "$FILETYPE"; then
      diff "$TRUTH/$FILE" "$OUTPUT/$FILE" > /dev/null
      res=$?
      if [ $res != 0 ]; then
        # Fallback to GDAL-based comparison
        truthCub="${FILE}.truth.cub"; currCub="${FILE}.curr.cub"
        truthStats="${FILE}.truth.txt"; currStats="${FILE}.curr.txt"
        raw2isis lines=100 samples=100 "from=$TRUTH/$FILE" \
          "to=$truthCub"
        raw2isis lines=100 samples=100 "from=$OUTPUT/$FILE" \
          "to=$currCub"
        gdalinfo -stats "$truthCub" > "$truthStats"
        gdalinfo -stats "$currCub" > "$currStats"
        rm -f "${truthCub}.aux.xml" "${currCub}.aux.xml"
        python "$ISISROOT/scripts/textdiff.py" "$truthStats" \
            "$currStats" -abs_err "$TOLERANCESTR" $DIFFFILE \
            > "$FAILURE_REPORT" 2>&1
        res=$?
      fi
      if [ $res != 0 ]; then
        echo "Failed ... File [$FILE]"
        echo "  Failed ... File [$FILE]" >> errors.txt
        cat "$FAILURE_REPORT" >> errors.txt
        echo "Failed comparing binary."
        echo "Report: $(pwd)/$FAILURE_REPORT"
        echo "Tolerance: $TOLERANCESTR"
        echo "Tolerance file to update: $TOL_FILE"
        echo "File with fields to ignore: $DIFFFILE_CANDIDATE"
        tail -n 1 "$FAILURE_REPORT"
      else
        SUCCESSFULCOMPARISONS=$((SUCCESSFULCOMPARISONS + 1))
        rm -f "$FAILURE_REPORT" 2>/dev/null
        rm -f "$truthCub"       2>/dev/null
        rm -f "$currCub"        2>/dev/null
        rm -f "$truthStats"     2>/dev/null
        rm -f "$currStats"      2>/dev/null
      fi

    # unknown
    else
      echo "Failed ... Unknown file type [$FILE]"
      echo "  Failed ... Unknown file type [$FILE]" >> errors.txt
    fi
  fi
done

if [ $SUCCESSFULCOMPARISONS -eq $TRUTHFILECOUNT ]; then
  touch casesucceeded.txt
elif [ ! -s errors.txt ]; then
  echo "  Failed ... Not all files in truth and output folders match" >> errors.txt
fi
