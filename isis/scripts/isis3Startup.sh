#!/bin/sh
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
if [ -d /usgs/cpkgs/isis3/testData ]; then
  ISIS3TESTDATA=/usgs/cpkgs/isis3/testData
  export ISIS3TESTDATA
else
  if [ -d $ISISROOT/../testData ]; then
    ISIS3TESTDATA=$ISISROOT/../testData
    export ISIS3TESTDATA
  fi
fi

if [ "$PATH" ]; then
  PATH="${PATH}:${ISISROOT}/bin"
else
  PATH="$ISISROOT/bin"
fi
export PATH

# Create QT_PLUGIN_PATH env variable
QT_PLUGIN_PATH="$ISISROOT/3rdParty/plugins"
export QT_PLUGIN_PATH
