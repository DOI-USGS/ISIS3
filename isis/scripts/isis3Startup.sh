#!/bin/bash
#
# This file should be executed within your current shell using the "." command
# Since this is only a beta version we do not suggest you add this
# command to your startup file
#
# On the command line type:
# > set ISISROOT=????
# > . isis3Startup.sh
#
# Replace the "????" in the above command line with the path you installed
# the Isis distribution
#
if [ ! "$ISISROOT" ]; then
  ISISROOT=/usgs/pkgs/isis3/isis
  export ISISROOT
fi

if [ -d $ISISROOT/../data ]; then
  ISIS3DATA=$ISISROOT/../data
else
  ISIS3DATA=/usgs/cpkgs/isis3/data
fi
export ISIS3DATA

# Do not export when used by outside groups
if [ -d $ISISROOT/../testData ]; then
  ISIS3TESTDATA=$ISISROOT/../testData
else
  ISIS3TESTDATA=/usgs/cpkgs/isis3/testData
fi
export ISIS3TESTDATA

if [ "$PATH" ]; then
  PATH="${PATH}:${ISISROOT}/bin"
else
  PATH="$ISISROOT/bin"
fi
export PATH
