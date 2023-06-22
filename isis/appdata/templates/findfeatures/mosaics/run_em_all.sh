#!/bin/bash
#set -o pipefail
###############################################################################
#
# Batch process MESSENGER MDIS mosaic with findfeatures
#
# This script produces a controlled mosaic compkete with photometric
# correction (because it needs it) from a list of MESSENGER EDRs.
#
# Create the input file using:
#
#  ls -1 $PWD/EDR/*.IMG > mdis_edrs.lis
#
# Then run as:
#
#  ./run_em_all.sh mdis_edrs.lis
#
# Prerequisites:
#   ISIS        - must have an appropriate version of system initilized
#   Install wget and parallel using this command after ISIS is initialized:
#      conda install wget parallel
#   Download MESSENGER EDRs in ./EDRS directory by using the following
#     commands:
#       cd EDRS
#       ./fetch_edrs.sh
#
# Defaults;
#   Current directory set to processing root dir
#
# @author  2021-03-02 Kris Becker Original Version
# @history 2023-06-18 Kris Becker Updated for findfeatures processing
###############################################################################
usage() { echo "Usage: $0 edrs.lis" 1>&2; exit 1; }

dryrun=0
verbose=0
priority="ontop"

# Check input arguments and provide usage
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 messenger_edr_files.lis"
  exit 1
fi

flist=$1
cat "${flist}" | parallel --no-notice ./00_basic_proc.sh -v -e messenger_import.err {}

# Create the regional network from a grid
ls -1 $PWD/Lev1/*.cub > lev1.lis
base="EW0218118239G"
./run_findfeatures.sh "${base}"

# Set up default mapping parameters to get the same size mosaic for all products
mapranges="minlat=0.2 maxlat=26.65 minlon=143.95 maxlon=177.15 grange=user priority=${priority}"

# Create the uncontrolled mosaic
./run_cam2map.sh  -f lev1.lis -o $PWD/Lev2/Uncontrolled
ls -1 $PWD/Lev2/Uncontrolled/*.cub > lev2_uncontrolled.lis
automos fromlist=lev2_uncontrolled.lis mosaic=messenger_uncontrolled_mos.cub matchbandbin=false ${mapranges}

# Do the photometrically corrected mosaic
./run_photomet.sh -o $PWD/Phot/Uncontrolled -f lev2_uncontrolled.lis
ls -1 $PWD/Phot/Uncontrolled/*.cub > phot_uncontrolled.lis
automos fromlist=phot_uncontrolled.lis mosaic=messenger_uncontrolled_phot_mos.cub matchbandbin=false ${mapranges}

## Run control processing!

# Just run the jigsaw final solution which updates ephemeris
mkdir -p $PWD/Updated_Lev1
cp -p  ./Lev1/*.cub $PWD/Updated_Lev1/
ls -1 $PWD/Updated_Lev1/*.cub > lev1_updated.lis

# Run jigsaw to control images
./run_jigsaw.sh  -u -f lev1_updated.lis -c "${base}_clean.net"

# Project all the updated images
./run_cam2map.sh  -f lev1_updated.lis -o $PWD/Lev2/Controlled

# Create controlled mosaics
ls -1 $PWD/Lev2/Controlled/*.cub > lev2_controlled.lis
automos fromlist=lev2_controlled.lis mosaic=messenger_controlled_mos.cub matchbandbin=false ${mapranges}

# Apply photometric control
./run_photomet.sh -o $PWD/Phot/Controlled -f lev2_controlled.lis

# Create controlled photometric mosaics
ls -1 $PWD/Phot/Controlled/*.cub > phot_controlled.lis
automos fromlist=phot_controlled.lis mosaic=messenger_controlled_phot_mos.cub matchbandbin=false ${mapranges}

exit 0
