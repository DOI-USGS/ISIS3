#!/bin/csh
################################################################################
# This file should be sourced within your current shell using the "source"
# command.  Since this is only a beta version we do not suggest you add this
# command to your startup file
#
# On the command line type:
# > setenv ISISROOT ????
# > source isis3Startup.csh
#
# Replace the "????" in the above command line with the path you installed
# the Isis distribution
#
#_HIST  FEB 21 2006 - Jac Shinaman - USGS, Astrogeology - added code to prevent
#               duplication of paths
#       FEB 22 2006 - JRS - changed OsType to Platform to avoid conflicts with
#               internal USGS initIsis.csh script
#       JUL 12 2006 - Robert Wallace - Add ISIS3TESTDATA environment variable
#       AUG 25 2008 - Kris Becker - Changed all references of DYLD_LIBRARY_PATH
#                       to DYLD_FALLBACK_LIBRARY_PATH.  See
#                       http://www.osxfaq.com/man/1/dyld.ws.
#       MAR 12 2009 - Christopher Austin - Changed the way ISIS3TESTDATE is set
#                       to prevent its setting for outside groups and default
#                       to "/usgs/cpkgs/isis3/testData"
#       SEP 08 2017 - Christopher Combs - Removed setting of QT_PLUGIN_PATH as
#                       an environment variable. Fixes #4617
#_VER   $Id: isis3Startup.csh,v 1.5 2010/03/16 19:40:22 ehyer Exp $
#_END
################################################################################

# Establish a platform switch variable
set Platform = `uname -s`

# Initialize the ISISROOT environment variable if it doesn't exist
# TODO: Test is obsolete, fail if no isisroot is set
if ($?ISISROOT == 0) then
  setenv ISISROOT /usgs/pkgs/isis3/isis
endif

# Initialize the ISIS3DATA environment variable
if (-d $ISISROOT/../data) then
  setenv ISIS3DATA $ISISROOT/../data
else
  setenv ISIS3DATA /usgs/cpkgs/isis3/data
endif

# Initialize the ISIS3TESTDATA environment variable
if (-d $ISISROOT/../testData) then
  setenv ISIS3TESTDATA $ISISROOT/../testData
else
  setenv ISIS3TESTDATA /usgs/cpkgs/isis3/testData
endif

# Insert ISISROOT/bin in the PATH environment variable if it's not already there
if ($?PATH == 0) then
  setenv PATH "$ISISROOT/bin"
else
  printenv PATH |grep $ISISROOT/bin >& /dev/null
  if  ( $status != 0) then
    setenv PATH "${PATH}:$ISISROOT/bin"
  endif
endif

unset Platform

if ( -f $ISISROOT/scripts/tabcomplete.csh ) then
  source $ISISROOT/scripts/tabcomplete.csh;
endif
