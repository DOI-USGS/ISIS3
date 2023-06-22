#!/bin/sh
#set -o pipefail
###############################################################################
#
# Run findfeatures, produce a registered control network
#
# Provide image cube base file name that overlaps all images
#
# Create the input file using:
#
#  ls -1 $PWD/Lev1/*.cub > lev1.lis
#
# Then run as:
#
#  ./run_findfeatures.sh EW0218118239G
#
# The base name specified must exist in the lev1.lis file. Only files
# that have common overlap will be included in the output network.
#
# Prerequisites:
#   ISIS        - must have an appropriate version of system initilized
#   Run 00_basic_proc.sh script on MESSENGER EDRs
#
# @author  2023-06-18 Kris Becker Original Version
###############################################################################
# Check input arguments and provide usage
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 image_basename"
  exit 1
fi

base=$1

fgrep -v "${base}" lev1.lis > fromlist.lis
/bin/rm -f *.prt *cubes.lis *notmatched.lis *nogeom.lis *.png *.csv *.log *.net *.txt *_check_Island.*

findfeatures algorithm='fastx@threshold:25@type:2/brief/parameters@Maxpoints:7000' \
             match="$PWD/Lev1/${base}.cal.cub" \
             fromlist=fromlist.lis \
             fastgeom=true \
             geomtype=camera \
             geomsource=both \
             fastgeompoints=50 \
             epitolerance=7.0 \
             ratio=0.99 \
             hmgtolerance=7.0 \
             globals='FastGeomDumpMapping:true@SaveRenderedImages:true@FastGeomAlgorithm:grid' \
             networkid="${base}_Mosiac" \
             pointid="${base}_????" \
             onet="${base}.net" \
             tolist="${base}_cubes.lis" \
             tonotmatched="${base}_notmatched.lis" \
             tonogeom="${base}_nogeom.lis" \
             description='Create MESSENGER MDIS image-image control network' \
             debug=true \
             debuglog="${base}.debug.log"

pointreg fromlist="${base}_cubes.lis" \
         cnet="${base}.net" \
         onet="${base}_reg.net" \
         deffile=pointreg_P31x31_S51x51.def \
         flatfile="${base}_reg.txt"  \
         points=all measures=all

cnetcombinept cnetbase="${base}_reg.net" \
              onet="${base}_clean.net" \
              imagetol=0.0 \
              minmeasures=2 \
              cleannet=true \
              cleanmeasures=true

cnetcheck fromlist="${base}_cubes.lis" \
          cnet="${base}_clean.net" \
          prefix="${base}_check_"






