#!/bin/sh

# Create the test data area for isisdata
# cd ISIS3/isis/test/data/isisdata
mkdir -p mockup mockprocessing

# Produce the mockup data. Its assumed isisdata_mockup is in a runtime path.
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/base     --outpath mockprocessing/isisdatamockup/base     --ghostdir '$ISISDATA/base'     --tojson isisdata_mockup_base.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/hayabusa --outpath mockprocessing/isisdatamockup/hayabusa --ghostdir '$ISISDATA/hayabusa' --tojson isisdata_mockup_hayabusa.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/smart1   --outpath mockprocessing/isisdatamockup/smart1   --ghostdir '$ISISDATA/smart1'   --tojson isisdata_mockup_smart1.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/voyager1 --outpath mockprocessing/isisdatamockup/voyager1 --ghostdir '$ISISDATA/voyager1' --tojson isisdata_mockup_voyager1.json

# Copy/install the desired files for the test
rsync -av --files-from=isisdataeval_isisdata_mockup_files.lis mockprocessing/isisdatamockup/ mockup/
/bin/rm -rf mockprocessing

# Run an inventory test for the mockup
isisdataeval isisdata=mockup datadir=mockup toinventory=isisdata_mockup_inventory.csv toissues=isisdata_mockup_issues.csv toerrors=isisdata_mockup_errors.csv hash=md5

