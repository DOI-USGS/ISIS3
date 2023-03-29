# <a name="#testingisisdataeval">ISISDATA Mockup Procedures for Testing isisdataeval</a>

Proper, thorough testing of ISIS application `isisdataeval` is difficult due to the requirements of having a stable and flexible ISISDATA directory structure to test with. It is unfeasible to rely on the real $ISISDATA due to its volitile and every changing content. One solution is to create a mockup of the ISISDATA directory but mimimize the resources to emulate a real, functioning ISISDATA installation. The system presented here generates a complete ISISDATA directory structure at a signification fraction of the actual dataset. By careful selective culling of many of the existing files in the selection mission datasets, it creates a real time snapshot of ISISDATA to help test `isisdataeval`.

The ISISDATA mockup system is a complete copy of every directory and every file in an existing ISISDATA install. However, the contents of every file has been replaced with information about that file, keeping the size of every file to about 300 bytes each. Here is the format of a file prepared by this system:

`cat  isisdatamockup/base/kernels/spk/de118.bsp`
```
{
    "source": "$ISISDATA/base/kernels/spk/de118.bsp",
    "filesize": 4097024,
    "createtime": "2022-08-12T22:13:17.000000",
    "modifiedtime": "2022-11-24T12:24:04.449174",
    "md5hash": "24a225d8262be0e0a7140dd40c708428",
    "sha256hash": "118774c31125cf0051faeb340532d03cab1b81adb63369db5842daefc1684a3c",
    "sha1hash": "dd6071b1a47193fe7abd6e9fc9db808408ef0429"
}
```

The size of this file has been reduced to 376 bytes from 4,097,024 bytes. Each file will contain JSON text with details about the original file. The files size, creation and modified dates, and hash values for the md5, sha1 and sha256 hash algorithms computed from the contents of the file using Python tools - and independt. The hash values are expressly provided to provide comparisons of the Qt algorithms used in `isisdataeval`. Note it is possible, but cannot be guaranteed, the _source_ for the file will exists in every ISISDATA install, or even be the same file. But this is a good thing to test.

# <a name="#isisdatamockuptool">ISISDATA Mockup Tool - isisdata_mockup.py</a>

A tool has been provided with the `isisdataeval` application that generates ISISDATA mockups for this and potentially other purposes. This Python tool will convert an ISISDATA installation into a mockup that is suitable to test the database lookup system and verify the contents and structure of mission kernel configurations in $ISISDATA. However, it has been generalized to use for any directory structure. Here is the application documentation:

`isisdata_mockup.py -h`

```
usage: isisdata_mockup.py [-h] --isisdata ISISDATA --outpath OUTPATH [--ghostdir GHOSTDIR]
                          [--saveconfig] [--dryrun] [--verbose]

Convert ISISDATA directory to test format

optional arguments:
  -h, --help           show this help message and exit
  --isisdata ISISDATA  ISISDATA directory root
  --outpath OUTPATH    NEW ISISDATA directory to create
  --ghostdir GHOSTDIR  Replaces the --isisdata path with this string to ghost the actual input
                       directory path
  --saveconfig, -s     Retain *.db, *.conf files rather than replace with processs info
  --dryrun, -n         Only print actions but do not execute
  --verbose, -v        Verbose output

isisdata_mockup.py will create a copy of all the files found in the directory specified by
the --isisdata parameter. All files and directories contained in --isisdata will
be copied to the --outpath destination, which specifies a new directory. This
directory will be created if it does not exist.

All *.db and *.conf files are copied as is, fully intact. Also needed to use this
mock up of ISISDATA is the NAIF LSK. This LSK kernel is always loaded by the iTime
class to concert UTC and Et times. Otherwise, all other files encountered will
be created but the contents are replaced by information regarding the --isisdata
source file. This information includes the original file opath, creation and
modification dates in UTC format, the size of the file and its hash values.

This app sets up a full data directory structure that mimics the contents of
ISISDATA (or any directory for that matter). The size of the file, its creation
and last modification dates and the md5, sha1 and sha256 hash values are created
in a dict which is then written to the new output file if its not a special ISIS
kernel db/conf file or LSK.

Finally, by setting --ghostdir '$ISISDATA', this now provides a connection to
the source file in the ISISDATA directory. This can be used to compare hash values
compute by other sources.

Example:

To provide a full mock up of ISISDATA directory:

isisdata_mockup.py --saveconfig --isisdata /opt/isis/data --outpath $PWD/mockisisdata --ghostdir '$ISISDATA'

Author: Kris J. Becker, University of Arizona
        kbecker@orex.lpl.arizona.edu

History 2023-03-15 Kris Becker - Original verison
```

You can also choose to process only a single directory, as shown in the following example:

```
isisdata_mockup.py --saveconfig --isisdata /opt/isis/data/voyager1  --outpath $PWD/mockisisdata/voyager1 --ghostdir '$ISISDATA/voyager1'
```
Processing times can be significant since this script is computing 3 different hash values per file.


# <a name="#isisdatamockupinstall">ISISDATA Mockup Test Data Preparation</a>

With this tool and the files listed in `isisdataeval_isisdata_mockup_files.lis`, the test for isisdataeval can be recreated from any ISISDATA installation. Note that from time to time, the files used in this ISISDATA test could change which would cause failures. Here are the commands to create the `isisdataeval` test ISISDATA mockup directory - from the Git install directory:

```
# Create the test data area for isisdata
cd ISIS3/isis/test/isisdata
mkdir -p mockup mockprocessing

# Produce the mockup data. Its assumed isisdata_mockup.py is in a runtime path.
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/base     --outpath mockprocessing/isisdatamockup/base     --ghostdir '$ISISDATA/base'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/hayabusa --outpath mockprocessing/isisdatamockup/hayabusa --ghostdir '$ISISDATA/hayabusa'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/smart1   --outpath mockprocessing/isisdatamockup/smart1   --ghostdir '$ISISDATA/smart1'
isisdata_mockup.py --saveconfig --isisdata $ISISDATA/voyager1 --outpath mockprocessing/isisdatamockup/voyager1 --ghostdir '$ISISDATA/voyager1'

# Copy/install the desired files for the test
rsync -av --files-from=isisdataeval_isisdata_mockup_files.lis mockprocessing/isisdatamockup/  mockup/
/bin/rm -rf mockprocessing

# Run an inventory test for the mockup
isisdataeval isisdata=mockup datadir=mockup toinventory=isisdata_mockup_inventory.csv toissues=isisdata_mockup_issues.csv toerrors=isisdata_mockup_errors.csv hash=md5

```


# <a name="#isisdatamockuptests">Running ISISDATA Mockup Tests</a>
Once the test data is installed, the contents ./ISIS3/isis/tests/data/isisisdata/mockup will contain the test data created above. To explicitly run the `isisdataeval` tests, use `ctest -R IsisData`. If an error occurs, rerun with the command `ctest --rerun-failed --output-on-failure` to get specific information about which tests failed and why.

Note that during this test, the contents of files with less than 102400 bytes will have all three hash values compute in the tests and compared with the values stored in hash keywords, _md5hash_, _sha1hash_ and _sha256hash_ for an external validation hashing. Note also that the all files $ISISDATA/base/kernels/lsk are retained since Isis::iTime requires an LSK and it always loads one from $ISISDATA/base/lsk/naif????.tls.

