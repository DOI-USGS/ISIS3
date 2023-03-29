# <a name="#testingisisdataeval">ISISDATA Mockup Procedures for Testing isisdataeval</a>

Proper, thorough testing of ISIS application `isisdataeval` is difficult due to the requirements of having a stable and flexible ISISDATA directory structure to test with. It is unfeasible to rely on the real $ISISDATA due to its volitile and every changing content. One solution is to create a mockup of the ISISDATA directory but mimimize the resources to emulate/represent a real, functioning ISISDATA installation. The system presented here generates a complete ISISDATA directory structure at a signification fraction of the actual dataset. By careful selective culling of many of the existing files in the selection mission datasets, it creates a real time snapshot of ISISDATA to help test `isisdataeval` and validate the local ISISDATA install.

The ISISDATA mockup system is a complete copy of every directory and every file in an existing ISISDATA install. However, the contents of every file has been replaced with information about that file, keeping the size of every file to about 400 bytes each. Here is the JSON format of a mocked ISISDATA file prepared by this system:

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

The size of this file has been reduced to 376 bytes from 4,097,024 bytes. Each file will contain JSON text with details about the original file. The files size, creation and modified dates, and hash values for md5, sha1 and sha256 hash algorithms are computed from the contents of the file using Python tools. This provides a an external source (Python) comparison of the Qt hash algorithnms used in `isisdataeval` for validation tests. Note it is possible, but cannot be guaranteed, the _source_ for the file will exist in every ISISDATA install, or even be the same file. But this is a good thing because it **validates** the local ISISDATA installation and test environment.

# <a name="#isisdatamockuptool">ISISDATA Mockup Tool - isisdata_mockup.py</a>

A tool has been provided with the `isisdataeval` application that generates ISISDATA mockups for this and potentially other uses. This Python tool will convert an ISISDATA installation into a mockup that is suitable to test the database lookup system and **verify** the contents and structure of mission kernel configurations in $ISISDATA. However, it has been generalized to use for any directory structure. Here is the application documentation:

`isisdata_mockup.py -h`

```
usage: isisdata_mockup.py [-h] --isisdata ISISDATA [--outpath OUTPATH] [--ghostdir GHOSTDIR]
                          [--hasher {md5,sha1,sha256,all}] [--saveconfig] [--tojson TOJSON] [--dryrun] [--verbose]

Convert ISISDATA directory to test format

optional arguments:
  -h, --help            show this help message and exit
  --isisdata ISISDATA   ISISDATA directory root
  --outpath OUTPATH     NEW ISISDATA directory to create
  --ghostdir GHOSTDIR   Replaces the --isisdata path with this string to ghost the actual input directory path
  --hasher {md5,sha1,sha256,all}
                        Add desired/supported hash algorithms to compute
  --saveconfig, -s      Retain *.db, *.conf files rather than replace with processs info
  --tojson TOJSON       Write all ISISDATA file information to output file in JSON format
  --dryrun, -n          Only print --outpath actions but do not execute
  --verbose, -v         Verbose output

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

Users can select a specific set of hash algorithms using the --hasher parameter.
Note that more than one hash algorithm can be specified on the command line by
simply provide multiple --hasher values (e.g., --hasher=md5 --hasher=sha1)
or, to compute all available, use --hasher=all.

This app sets up a full data directory structure that mimics the contents of
ISISDATA (or any directory for that matter). The size of the file, its creation
and last modification dates and the md5, sha1 and sha256 hash values are created
in a dict which is then written to the new output file if its not a special ISIS
kernel db/conf file or LSK.

Finally, by setting --ghostdir '$ISISDATA', this now provides a connection to
the source file in the ISISDATA directory. This can be used to compare hash values
compute by other sources.

If a complete summary of processing log is desired, provide a file name in
--tojson. This file will contain a JSON structure with the contents of program
details and file analysis. Note that is particularly useful if you just want the
data and not the mockup replication. Just add --dryrun to produce the log file
only.

Example:

To provide a full mock up of ISISDATA directory:

isisdata_mockup.py --saveconfig --hasher all --isisdata /opt/isis4/data --outpath $PWD/isisdatamock --ghostdir '$ISISDATA' --tojson isisdata_mockup_full.json

Author: Kris J. Becker, University of Arizona
        kbecker@orex.lpl.arizona.edu

History 2023-03-15 Kris Becker Original verison
        2023-03-29 Kris Becker Added --tojson and --hasher parameters to improve utility


```

You can also choose to process only a single directory, as shown in the following example:
```
isisdata_mockup.py --saveconfig --hasher all --isisdata /opt/isis/data/voyager1  --outpath $PWD/mockisisdata/voyager1 --ghostdir '$ISISDATA/voyager1'
```
Processing times can be significant since this script is computing 3 different hash values per file.


# <a name="#isisdatamockupinstall">ISISDATA Mockup Test Data Preparation</a>

With this tool and the files listed in `isisdataeval_isisdata_mockup_files.lis`, the test for isisdataeval can be recreated from any ISISDATA installation. Note that from time to time, the files used in this ISISDATA test could change which would cause failures. In this situation, maintainers will need to regenerate the ISISDATA mockup if the change is expected/needed. Here are the commands to create the `isisdataeval` test ISISDATA mockup directory originating from the Git install directory:

```
# From the Git install directory, create the test data mockup in isisdata
cd ISIS3/isis/test/data/isisdata
mkdir -p mockup mockprocessing

# Produce the mockup data. Its assumed isisdata_mockup.py is in a runtime path.
isisdata_mockup.py --saveconfig --hasher all --isisdata $ISISDATA/base     --outpath mockprocessing/isisdatamockup/base     --ghostdir '$ISISDATA/base'     --tojson isisdata_mockup_base.json
isisdata_mockup.py --saveconfig --hasher all --isisdata $ISISDATA/hayabusa --outpath mockprocessing/isisdatamockup/hayabusa --ghostdir '$ISISDATA/hayabusa' --tojson isisdata_mockup_hayabusa.json
isisdata_mockup.py --saveconfig --hasher all --isisdata $ISISDATA/smart1   --outpath mockprocessing/isisdatamockup/smart1   --ghostdir '$ISISDATA/smart1'   --tojson isisdata_mockup_smart1.json
isisdata_mockup.py --saveconfig --hasher all --isisdata $ISISDATA/voyager1 --outpath mockprocessing/isisdatamockup/voyager1 --ghostdir '$ISISDATA/voyager1' --tojson isisdata_mockup_voyager1.json

# Copy/install the desired files for the test
rsync -av --files-from=isisdataeval_isisdata_mockup_files.lis mockprocessing/isisdatamockup/ mockup/
/bin/rm -rf mockprocessing

# Run an inventory on the local ISISDATA mockup test data
isisdataeval isisdata=mockup datadir=mockup toinventory=isisdata_mockup_inventory.csv toissues=isisdata_mockup_issues.csv toerrors=isisdata_mockup_errors.csv hash=md5
```

# Other products
The `isisdata_mockup.py` application can also produce a summary procesing log file of the run used to create a mockup. It can also be ran standalone on any system to provide as an comparison dataset for any mockup. This command produces a summary of the full ISISDATA installation without producing the mockup in `--outpath` (it is now an optional parameter), computes only the md5 hash and writes the entire contents to the JSON output file specified in `--tojson`:

```
isisdata_mockup.py -v  --hasher=md5 --isisdata /opt/isis3/data --ghostdir '$ISISDATA' --tojson isisdata_full_md5.json
```

This example provides the results of a much smaller directory, $ISISDATA/base/dems, that also computes the md5 hash, including all `*.db` and `*.conf` files, does not generate, unless given, any absolute paths, and **does not** produce the mockup directory:
```
isisdata_mockup.py --hasher md5 --isisdata /opt/isis3/data/base/dems --ghostdir '$ISISDATA/base/dems' --tojson isisdata_base_dem_md5.json
```

And here is the contents of the output JSON file, `isisdata_base_dem_md5.json`:
```
{
  "program": {
    "name": "isisdata_mockup.py",
    "version": "0.2",
    "date": "2023-03-29",
    "runtime": "2023-03-29T21:02:03.721127 UTC",
    "endtime": "2023-03-29T21:02:39.887436 UTC",
    "elapsedtime": "0:00:36.166309",
    "parameters": {
      "isisdata": "/opt/isis3/data/base/dems",
      "outpath": null,
      "ghostdir": "$ISISDATA/base/dems",
      "hasher": [
        "md5"
      ],
      "saveconfig": false,
      "tojson": "isisdata_base_dem_md5.json",
      "dryrun": false,
      "verbose": false
    }
  },
  "inventory": {
    "hashlist": [
      "md5"
    ],
    "missing_count": 0,
    "files_count": 33,
    "missing": [],
    "files": [
      {
        "source": "$ISISDATA/base/dems/Ceres_Dawn_FC_HAMO_DTM_DLR_Global_60ppd_Oct2016_prep.cub",
        "filesize": 467404363,
        "createtime": "2016-10-17T21:20:18.000000",
        "modifiedtime": "2020-02-13T16:33:27.681881",
        "md5hash": "e8e8763630d938caf9cfc52b2820f35b"
      },
      {
        "source": "$ISISDATA/base/dems/LRO_LOLA_LDEM_global_128ppd_20100915.cub",
        "filesize": 2141164162,
        "createtime": "2011-03-19T19:18:34.000000",
        "modifiedtime": "2020-02-13T16:36:48.776844",
        "md5hash": "cb36cb569671f4db0badc29d737ed6d5"
      },
      {
        "source": "$ISISDATA/base/dems/LRO_LOLA_LDEM_global_128ppd_20100915_0002.cub",
        "filesize": 2141164162,
        "createtime": "2011-03-19T19:18:34.000000",
        "modifiedtime": "2020-02-13T16:36:48.776844",
        "md5hash": "cb36cb569671f4db0badc29d737ed6d5"
      },
      {
        "source": "$ISISDATA/base/dems/MSGR_DEM_USG_EQ_I_V02_prep.cub",
        "filesize": 531052027,
        "createtime": "2017-02-10T20:31:22.000000",
        "modifiedtime": "2020-02-13T16:33:53.794973",
        "md5hash": "4e9239e4d394e01cf15024bece226879"
      },
      {
        "source": "$ISISDATA/base/dems/README.txt",
        "filesize": 8875,
        "createtime": "2014-08-28T00:16:45.000000",
        "modifiedtime": "2020-02-13T16:33:53.795434",
        "md5hash": "72463eaddf5794bb906ac872c71c55f6"
      },
      {
        "source": "$ISISDATA/base/dems/Vesta_Dawn_HAMO_DTM_DLR_Global_48ppd.cub",
        "filesize": 601826360,
        "createtime": "2016-08-29T20:39:42.000000",
        "modifiedtime": "2020-02-13T16:34:48.095877",
        "md5hash": "a488e544c685ae511ea0ed355a30fdbc"
      },
      {
        "source": "$ISISDATA/base/dems/kaguya_LALT_0001.cub",
        "filesize": 69407789,
        "createtime": "2011-02-24T17:44:46.000000",
        "modifiedtime": "2020-02-13T16:34:58.650174",
        "md5hash": "1e6e9836fb6a0f7c3547e531bdffedda"
      },
      {
        "source": "$ISISDATA/base/dems/kaguya_LALT_0002.cub",
        "filesize": 69407789,
        "createtime": "2011-02-24T17:44:46.000000",
        "modifiedtime": "2020-02-13T16:34:58.650174",
        "md5hash": "1e6e9836fb6a0f7c3547e531bdffedda"
      },
      {
        "source": "$ISISDATA/base/dems/kaguya_LALT_0003.cub",
        "filesize": 69407789,
        "createtime": "2011-02-24T17:44:46.000000",
        "modifiedtime": "2020-02-13T16:34:58.650174",
        "md5hash": "1e6e9836fb6a0f7c3547e531bdffedda"
      },
      {
        "source": "$ISISDATA/base/dems/kaguya_LALT_0004.cub",
        "filesize": 69407789,
        "createtime": "2011-02-24T17:44:46.000000",
        "modifiedtime": "2020-02-13T16:34:58.650174",
        "md5hash": "1e6e9836fb6a0f7c3547e531bdffedda"
      },
      {
        "source": "$ISISDATA/base/dems/kernels.0003.db",
        "filesize": 294,
        "createtime": "2008-03-21T23:05:39.000000",
        "modifiedtime": "2020-02-13T16:34:58.650543",
        "md5hash": "f2270d34f669204e2c6c1eb6fcb5dd10"
      },
      {
        "source": "$ISISDATA/base/dems/kernels.0004.db",
        "filesize": 378,
        "createtime": "2011-04-12T22:00:58.000000",
        "modifiedtime": "2020-02-13T16:34:58.650894",
        "md5hash": "12198e32a7737b98bfbc2bb3eb3c5da4"
      },
      {
        "source": "$ISISDATA/base/dems/kernels.0005.db",
        "filesize": 799,
        "createtime": "2016-09-23T18:39:30.000000",
        "modifiedtime": "2020-02-13T16:34:58.651268",
        "md5hash": "c864e5e0528582ba856132df454d3a62"
      },
      {
        "source": "$ISISDATA/base/dems/kernels.0006.db",
        "filesize": 1193,
        "createtime": "2016-10-25T18:43:55.000000",
        "modifiedtime": "2020-02-13T16:34:58.651562",
        "md5hash": "36b377438beca227740baf3cb2c461a2"
      },
      {
        "source": "$ISISDATA/base/dems/kernels.0007.db",
        "filesize": 1401,
        "createtime": "2017-02-10T20:31:00.000000",
        "modifiedtime": "2020-02-13T16:34:58.651847",
        "md5hash": "18ac8d7f5ee40bc59aa2eedcb5ae73c1"
      },
      {
        "source": "$ISISDATA/base/dems/ldem_128ppd_Mar2011_clon180_radius_pad.cub",
        "filesize": 2141164162,
        "createtime": "2011-03-19T19:18:34.000000",
        "modifiedtime": "2020-02-13T16:36:48.776844",
        "md5hash": "cb36cb569671f4db0badc29d737ed6d5"
      },
      {
        "source": "$ISISDATA/base/dems/molaMarsPlanetaryRadius0001.cub",
        "filesize": 2141168028,
        "createtime": "2011-02-24T17:42:10.000000",
        "modifiedtime": "2020-02-13T16:38:45.258624",
        "md5hash": "816cdc4baf9f381c23b86d58d25ab3a2"
      },
      {
        "source": "$ISISDATA/base/dems/molaMarsPlanetaryRadius0002.cub",
        "filesize": 2141168028,
        "createtime": "2011-02-24T17:42:10.000000",
        "modifiedtime": "2020-02-13T16:38:45.258624",
        "md5hash": "816cdc4baf9f381c23b86d58d25ab3a2"
      },
      {
        "source": "$ISISDATA/base/dems/molaMarsPlanetaryRadius0003.cub",
        "filesize": 2141168028,
        "createtime": "2011-02-24T17:42:10.000000",
        "modifiedtime": "2020-02-13T16:38:45.258624",
        "md5hash": "816cdc4baf9f381c23b86d58d25ab3a2"
      },
      {
        "source": "$ISISDATA/base/dems/molaMarsPlanetaryRadius0004.cub",
        "filesize": 2141168028,
        "createtime": "2011-02-24T17:42:10.000000",
        "modifiedtime": "2020-02-13T16:38:45.258624",
        "md5hash": "816cdc4baf9f381c23b86d58d25ab3a2"
      },
      {
        "source": "$ISISDATA/base/dems/molaMarsPlanetaryRadius0005.cub",
        "filesize": 2141168028,
        "createtime": "2011-02-24T17:42:10.000000",
        "modifiedtime": "2020-02-13T16:38:45.258624",
        "md5hash": "816cdc4baf9f381c23b86d58d25ab3a2"
      },
      {
        "source": "$ISISDATA/base/dems/referenceEarthEllipsoid_NEquad_nad83.cub",
        "filesize": 235119231,
        "createtime": "2014-08-26T21:35:37.000000",
        "modifiedtime": "2020-02-13T16:38:47.194851",
        "md5hash": "86138ec09d2aafd708440e434c87d88e"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_clean_0001.cub",
        "filesize": 137237064,
        "createtime": "2011-02-24T17:31:51.000000",
        "modifiedtime": "2020-02-13T16:38:53.530936",
        "md5hash": "63366f0818e569c5d0f798a8370eb6c4"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_clean_0002.cub",
        "filesize": 137237064,
        "createtime": "2011-02-24T17:31:51.000000",
        "modifiedtime": "2020-02-13T16:38:53.530936",
        "md5hash": "63366f0818e569c5d0f798a8370eb6c4"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_clean_0003.cub",
        "filesize": 137237064,
        "createtime": "2011-02-24T17:31:51.000000",
        "modifiedtime": "2020-02-13T16:38:53.530936",
        "md5hash": "63366f0818e569c5d0f798a8370eb6c4"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_clean_0004.cub",
        "filesize": 137237064,
        "createtime": "2011-02-24T17:31:51.000000",
        "modifiedtime": "2020-02-13T16:38:53.530936",
        "md5hash": "63366f0818e569c5d0f798a8370eb6c4"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_lpo_0001.cub",
        "filesize": 34738517,
        "createtime": "2011-02-24T17:32:03.000000",
        "modifiedtime": "2020-02-13T16:38:55.205744",
        "md5hash": "891920197eaa73c98519afaef3499fc9"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_lpo_0002.cub",
        "filesize": 34738517,
        "createtime": "2011-02-24T17:32:03.000000",
        "modifiedtime": "2020-02-13T16:38:55.205744",
        "md5hash": "891920197eaa73c98519afaef3499fc9"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_lpo_0003.cub",
        "filesize": 34738517,
        "createtime": "2011-02-24T17:32:03.000000",
        "modifiedtime": "2020-02-13T16:38:55.205744",
        "md5hash": "891920197eaa73c98519afaef3499fc9"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_lpo_0004.cub",
        "filesize": 34738517,
        "createtime": "2011-02-24T17:32:03.000000",
        "modifiedtime": "2020-02-13T16:38:55.205744",
        "md5hash": "891920197eaa73c98519afaef3499fc9"
      },
      {
        "source": "$ISISDATA/base/dems/ulcn2005_lpo_0005.cub",
        "filesize": 34738517,
        "createtime": "2011-02-24T17:32:03.000000",
        "modifiedtime": "2020-02-13T16:38:55.205744",
        "md5hash": "891920197eaa73c98519afaef3499fc9"
      },
      {
        "source": "$ISISDATA/base/dems/verticalEarthDatum_contiguousUSA_navd88.cub",
        "filesize": 34553210,
        "createtime": "2014-08-26T21:36:12.000000",
        "modifiedtime": "2020-02-13T16:38:57.091315",
        "md5hash": "d7d8fd6eaf8d5cc81a09c034848e40b7"
      },
      {
        "source": "$ISISDATA/base/dems/vesta_hst_dem_0001.cub",
        "filesize": 78316,
        "createtime": "2011-06-24T22:36:56.000000",
        "modifiedtime": "2020-02-13T16:38:57.092120",
        "md5hash": "a2186af2bc310e424a1a45feafbacf98"
      }
    ]
  }
}
```
# <a name="#isisdatamockuptests">Running ISISDATA Mockup Tests</a>
Once the test data is installed, the contents of ./ISIS3/isis/tests/data/isisisdata/mockup will contain the test data created above. To explicitly run the `isisdataeval` tests, from the build directory, use `ctest -R IsisData`. If an error occurs, rerun with the command `ctest --rerun-failed --output-on-failure` to get specific information about which tests failed and why.

Note that during this test, the contents of files with less than 102,400 bytes will have all three hash values compute in the tests and compared with the values stored in hash keywords, _md5hash_, _sha1hash_ and _sha256hash_ for an external validation of hashing algorithms. Note also that all files in $ISISDATA/base/kernels/lsk are retained since Isis::iTime requires an LSK and it always loads one from $ISISDATA/base/lsk/naif????.tls.

