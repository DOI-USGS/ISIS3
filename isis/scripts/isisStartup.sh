#!/bin/bash
#
# This file should be executed within your current shell using the "." or "source"
# command.
#
# On the command line type:
# > set ISISROOT=????
# > . isis3Startup.sh
#
# Replace the "????" in the above command line with the path where you installed
# the Isis distribution
#
if [ ! "$ISISROOT" ]; then
  ISISROOT=/usgs/pkgs/isis3/install
  export ISISROOT
fi

if [ ! "$ISISDATA" ]; then
  if [ -d $ISISROOT/../data ]; then
    ISISDATA=$ISISROOT/../isis_data
  else
    ISISDATA=/usgs/cpkgs/isis3/isis_data
  fi
  export ISISDATA
fi

if [ ! "$ISISTESTDATA" ]; then
  if [ -d $ISISROOT/../testData ]; then
    ISISTESTDATA=$ISISROOT/../testData
  else
    ISISTESTDATA=/usgs/cpkgs/isis3/isis_testData
  fi
  export ISISTESTDATA
fi

if [ "$PATH" ]; then
  PATH="${PATH}:${ISISROOT}/bin"
else
  PATH="$ISISROOT/bin"
fi
export PATH
