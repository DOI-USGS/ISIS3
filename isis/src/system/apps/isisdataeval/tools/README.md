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

# <a name="#isisdatamockuptool">ISISDATA Mockup Tool - isisdata_mockup</a>

A tool has been provided with the `isisdataeval` application that generates ISISDATA mockups for this and potentially other uses. This Python tool will convert an ISISDATA installation into a mockup that is suitable to test the database lookup system and **verify** the contents and structure of mission kernel configurations in $ISISDATA. However, it has been generalized to use for any directory structure. The application help documentation describes the parameter and behavioir of the script. Use the command `isisdata_mockup -h` to produce the documentation.

In general, if you want to produce a complete inventory of the ISISDATA directory, the form to use is:
```
isisdata_mockup --saveconfig --hasher all --isisdata /opt/isis/data  --outpath $PWD/mockisisdata --ghostdir '$ISISDATA' --tojson isisdata_complete_mockup.json
```

You can also choose to process only a single directory, as shown in the following example:
```
isisdata_mockup --saveconfig --hasher all --isisdata /opt/isis/data/voyager1  --outpath $PWD/mockisisdata/voyager1 --ghostdir '$ISISDATA/voyager1' --tojson isisdata_voyager_mockup.json
```
Processing times can be significant since these examples are computing 3 different hash values per file. For this purpose, the `md5` hash algorithm is probably sufficient for ISISDATA file comparisons.


# <a name="#isisdatamockupinstall">ISISDATA Mockup Test Data Preparation</a>

With this tool and the files listed in `isisdataeval_isisdata_mockup_files.lis`, the test for isisdataeval can be recreated from any ISISDATA installation. Note that from time to time, the files used in this ISISDATA test could change which would cause failures. In this situation, maintainers will need to regenerate the ISISDATA mockup if the change is expected/needed. Here are the commands to create the `isisdataeval` test ISISDATA mockup directory originating from the Git install directory:

```
# From the Git install directory, create the test data mockup in isisdata
cd ISIS3/isis/test/data/isisdata
mkdir -p mockup mockprocessing

# Produce the mockup data. Its assumed isisdata_mockup is in a runtime path.
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/base     --outpath mockprocessing/isisdatamockup/base     --ghostdir '$ISISDATA/base'     --tojson isisdata_mockup_base.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/hayabusa --outpath mockprocessing/isisdatamockup/hayabusa --ghostdir '$ISISDATA/hayabusa' --tojson isisdata_mockup_hayabusa.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/smart1   --outpath mockprocessing/isisdatamockup/smart1   --ghostdir '$ISISDATA/smart1'   --tojson isisdata_mockup_smart1.json
isisdata_mockup --saveconfig --hasher all --isisdata $ISISDATA/voyager1 --outpath mockprocessing/isisdatamockup/voyager1 --ghostdir '$ISISDATA/voyager1' --tojson isisdata_mockup_voyager1.json

# Copy/install the desired files for the test
rsync -av --files-from=isisdataeval_isisdata_mockup_files.lis mockprocessing/isisdatamockup/ mockup/
/bin/rm -rf mockprocessing

# Run an inventory on the local ISISDATA mockup test data
isisdataeval isisdata=mockup datadir=mockup toinventory=isisdata_mockup_inventory.csv toissues=isisdata_mockup_issues.csv toerrors=isisdata_mockup_errors.csv hash=md5
```

# Other products
The `isisdata_mockup` application can also produce a summary procesing log file of the run used to create a mockup. It can also be ran standalone on any system to provide as an comparison dataset for any mockup. This command produces a summary of the full ISISDATA installation without producing the mockup in `--outpath` (it is now an optional parameter), computes only the md5 hash and writes the entire contents to the JSON output file specified in `--tojson`:

```
isisdata_mockup -v  --hasher=md5 --isisdata /opt/isis3/data --ghostdir '$ISISDATA' --tojson isisdata_full_md5.json
```

This example provides the results of a much smaller directory, $ISISDATA/base/dems, that also computes the md5 hash, including all `*.db` and `*.conf` files, does not generate, unless given, any absolute paths, and **does not** produce the mockup directory:
```
isisdata_mockup --hasher md5 --isisdata /opt/isis3/data/base/dems --ghostdir '$ISISDATA/base/dems' --tojson isisdata_base_dem_md5.json
```

And here is portion of the contents of the output JSON file, `isisdata_base_dem_md5.json`:
```
{
  "program": {
    "name": "isisdata_mockup",
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
        ...more file descriptions follow...
      }

    ]
  }
}
```

Note this file can then be shared and compared using the same runtime parameters on other platforms. I would refrain from comparing times, but all the other data (i.e., source, filesize, and hash values) can be directly compared for equivalence to help identify any differences from, say, a reputable ISISDATA source.

# <a name="#isisdatamockuptests">Running ISISDATA Mockup Tests</a>
Once the test data is installed, the contents of ./ISIS3/isis/tests/data/isisisdata/mockup will contain the test data created above. To explicitly run the `isisdataeval` tests, from the build directory, use `ctest -R IsisData`. If an error occurs, rerun with the command `ctest --rerun-failed --output-on-failure` to get specific information about which tests failed and why.

Note that during this test, the contents of files with less than 102,400 bytes will have all three hash values compute in the tests and compared with the values stored in hash keywords, _md5hash_, _sha1hash_ and _sha256hash_ for an external validation of hashing algorithms. Note also that all files in $ISISDATA/base/kernels/lsk are retained since Isis::iTime requires an LSK and it always loads one from $ISISDATA/base/lsk/naif????.tls.

