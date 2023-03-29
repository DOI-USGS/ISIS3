#!/bin/sh

# Create the test data area for isisdata
# cd ISIS3/isis/test/isisdata
mkdir -p mockup mockprocessing

# Produce the mockup data. Its assumed isisdata_mockup.py is in a runtime path.
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/base     --outpath mockprocessing/isisdatamockup/base     --ghostdir '$ISISDATA/base'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/hayabusa --outpath mockprocessing/isisdatamockup/hayabusa --ghostdir '$ISISDATA/hayabusa'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/smart1   --outpath mockprocessing/isisdatamockup/smart1   --ghostdir '$ISISDATA/smart1'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/voyager1 --outpath mockprocessing/isisdatamockup/voyager1 --ghostdir '$ISISDATA/voyager1'

# Copy/install the desired files for the test
rsync -av --files-from=isisdataeval_isisdata_mockup_files.lis mockprocessing/isisdatamockup/ mockup/
/bin/rm -rf mockprocessing

# Run an inventory test for the mockup
isisdataeval isisdata=mockup datadir=mockup toinventory=isisdata_mockup_inventory.csv toissues=isisdata_mockup_issues.csv toerrors=isisdata_mockup_errors.csv hash=md5

