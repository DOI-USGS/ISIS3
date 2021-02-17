#!/bin/csh
################################################################################
# This file should be sourced within your current shell using the "source"
# command.
#
# On the command line type:
# > setenv ISISROOT ????
# > source isis3Startup.csh
#
# Replace the "????" in the above command line with the path you installed
# the ISIS distribution
#
################################################################################

if ($?ISISROOT == 0) then
  echo "ISISROOT environment variable is not set"
  exit 1
endif

if ($?ISISDATA == 0) then
  if (-d $ISISROOT/../data) then
    setenv ISISDATA $ISISROOT/../isis_data
  else
    setenv ISISDATA /usgs/cpkgs/isis3/isis_data
  endif
endif

if ($?ISISTESTDATA == 0) then
  if (-d $ISISROOT/../testData) then
    setenv ISISTESTDATA $ISISROOT/../testData
  else
    setenv ISISTESTDATA /usgs/cpkgs/isis3/isis_testData
  endif
endif

if ( -f $ISISROOT/scripts/tabcomplete.csh ) then
  source $ISISROOT/scripts/tabcomplete.csh;
endif
