The testing environment for ISIS includes unit test for nearly every C++ class or struct in the ISIS library, one or more regression tests for all non-interactive applications, and several regression tests for chains of multiple applications (module tests). Many of the tests require input and/or truth files. There are over 80GB of these test files, so they are not included in the Github code repository.

Unit tests are designed to test a single class from the ISIS library (e.g., Historgram, ProcessByLine, VoyagerCamera). Unit tests use two frameworks, the newer gtest and the older in house make based tests. Test data for gtest based tests is usually embedded in the test source code file located in $ISISROOT/isis/tests. Input data for the make based tests can be embedded in the unittest.cpp, located in the source code directory for the class, or an external file located in the ISIS3DATA or ISIS3TESTDATA areas. Output test data for the make based tests is always located in the source code directory for the class.

Application and module tests are regression tests designed to test all of the functionality of a single ISIS application (e.g., fx, lowpass, cam2map) or a series of ISIS applications (e.g., vims2isis->spiceinit->cam2map) respectively. Regression tests have both input and truth data stored in the ISIS3TESTSDATA area. Input and truth data files can be ISIS cubes, control networks, plain text, Parameter Value Language (PVL), Planetary Data System images (PDS3, PDS4), or comma separated value (CSV) files.

## Setting Up an ISIS Development Environment
In order to run existing tests and develop new tests a full development environment is required. The public releases do not contain the tests or test data or the source code for the tests. Follow these steps to get a working ISIS development environment with all of the data.

1) Fork the ISIS source code repository (https://github.com/USGS-Astrogeology/ISIS3)
1) Get the ISIS source code on your local machine by cloning your fork
1) Build the ISIS library and applications (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake)
1) Download the ISIS data files see the [Full ISIS3 Data Download](https://gith**ub.com/USGS-Astrogeology/ISIS3). **NOTE: Be careful where you put the ISISDATA area. It is several hundred GB and growing.**

### Downloading Test Data
ISIS unit and regression tests require the data and test data directories to be available and their respective environment variables (ISISDATA, ISIS3TESTDATA) be set. This allows the tests to read files from these areas and compare results to known truth data. The ISIS tests data is currently distributed using rsync servers, but work is underway to put the test data under full version control in a Git repository. To download all of the test data, use a terminal window, set your current working directory to where you want the test data to be stored, and use the rsync command below to copy it to your computer:

**Note: Be extremely careful using the suggested --delete argument with the rsync command below. If the destination directory exists and contains files not on the rsync server it will delete all files at that location and replace them the test data.**
```
rsync -azv --partial --delete isisdist.astrogeology.usgs.gov::isis3testData .
```
When the rsync command is finished, there should be a single directory called "isis" containing about 80GB. The directory structure below this "isis" directory is required to be identical to the ISIS source code directory structure.

### Setup the ISIS environment variables
The environment variable "ISIS3TESTDATA" needs to be be set to point to the "isis" directory created by the rsync command above. Note: Each of the examples below ends with an "isis" directory.

Bash and other sh based shells:
```
export ISIS3TESTDATA=/path/to/the/test/data/for/isis
```
tcsh or other csh based shells:
setenv ISIS3TESTDATA /path/to/the/test/data/for/isis

# Contributing New and Modified Tests Data
The source code for the unit and regression tests is located with the ISIS source code repository on Github, but the data used by the tests is currently internal to USGS. Is it made available through the read-only rsync servers. Use the following command to download the current version of of the test data:
**NOTE: Be very careful using the --delete argument below. If there are existing files in the destination folder that are not in the test data download they will be deleted.**
'''
rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3testData .
'''

Efforts are underway to make all of the test data available in a publicly accessible revision control system. Once the new system is in place, test data will be handled in a similar way as the ISIS source code \(i.e., using pull requests) 

If you are writing or modifying ISIS library classes or applications it is required they be well tested. This may include modifications to existing test data and/or new test data. Once a GitHub pull request has been created for ISIS source code changes the data needs to be made available to USGS personnel through a web or ftp link. If this is not an option for you, please contact the USGS through the [discussion forum](https://astrodiscuss.usgs.gov/) and we can make other arrangements.

## Where to go now
* Running tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
* Writing Unit Tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
* Writing Application and Module Tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
