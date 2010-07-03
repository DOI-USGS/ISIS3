#!/bin/csh
# $Id: DeployDarwin.csh,v 1.1 2010/04/09 23:19:58 slambright Exp $

if ($?ISISROOT == 0) then
  echo "ISISROOT not set...must be set to target ISIS build!"
  exit (1)
endif

#  Set runtime paths for all ISIS binary applications
/bin/rm -f DarwinLibs.lis DarwinErrors.lis
SetRunTimePath --bins --libmap=isis_bins_paths.lis --liblog=DarwinLibs.lis \
               --relocdir=$ISISROOT/3rdParty/lib:$ISISROOT/3rdParty:$ISISROOT \
               --errlog=DarwinErrors.lis \
                  `find $ISISROOT/bin -maxdepth 1 -name '*' -type f`


#  Patch all libraries in $ISISROOT/lib
#SetRunTimePath --libs --libmap=darwin_paths.lis --liblog=DarwinLibs.lis \
#                  --relocdir=$ISISROOT/3rdParty/lib --update \
#                  --errlog=DarwinErrors.lis \
#                  `find $ISISROOT/lib  -maxdepth 1 -name '*.dylib' -type f`

#  Process third party Qt frameworks
SetRunTimePath --libs --libmap=qt_paths.lis --liblog=DarwinLibs.lis \
               --relocdir=$ISISROOT/3rdParty/lib:$ISISROOT/3rdParty  --update \
               `find $ISISROOT/3rdParty/lib -name 'Qt*' -print -mindepth 3 -maxdepth 4 -type f`

#  Process third party generic libraries
chmod u+w $ISISROOT/3rdParty/lib/libcrypto.*.dylib
chmod u+w $ISISROOT/3rdParty/lib/libssl.*.dylib
SetRunTimePath --libs --libmap=darwin_lib_paths.lis --liblog=DarwinLibs.lis \
               --relocdir=$ISISROOT/3rdParty/lib:$ISISROOT/3rdParty  --update \
               --errlog=DarwinErrors.lis \
               `find $ISISROOT/3rdParty/lib -name '*.dylib' -maxdepth 1 -type f`
chmod u-w $ISISROOT/3rdParty/lib/libcrypto.*.dylib
chmod u-w $ISISROOT/3rdParty/lib/libssl.*.dylib

#  Patch IDL/IsisDlm
if (-e $ISISROOT/IsisDlm/lib/IsisDlm.so) then
  SetRunTimePath --bundles --libmap=darwin_IsisDlm_paths.lis --liblog=DarwinLibs.lis \
                 --relocdir=$ISISROOT/3rdParty:$ISISROOT/3rdParty/lib --update \
                 --errlog=DarwinErrors.lis   $ISISROOT/IsisDlm/lib/IsisDlm.so
endif

#  Finally, patch the (Qt) plugins
SetRunTimePath --bundles --libmap=qt_plugins_paths.lis --liblog=DarwinLibs.lis --update \
               --relocdir=$ISISROOT/3rdParty/lib:$ISISROOT/3rdParty \
               --errlog=DarwinErrors.lis \
               `find $ISISROOT/3rdParty/plugins -name '*.bundle' -type f`



