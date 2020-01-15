# Test Data for ISIS software
## Introduction
Test data for ISIS is centered around the input and output data necessary for the ISIS software unit tests and regression (applications and module) tests.

Unit tests are designed to test a single class from the ISIS library (e.g., Historgram, ProcessByLine, VoyagerCamera). When other classes are used inside a unit test it it important to accept any results as valid and not test the other class. Unit tests use two frameworks, the newer gtest and the older in house make based tests. Test data for gtest based tests is usually embedded inside the test file located in $ISISROOT/isis/tests. Input test data for the make based tests can be embedded in the unittest.cpp, located in the source code directory for the class, or an external file located in the ISIS3DATA or ISIS3TESTDATA areas. Output test data for the make based tests is always located in the source code directory for the class.

Application and module tests are regression tests designed to test all of the functionality of a single ISIS application (e.g., fx, lowpass, cam2map) or a series of ISIS applications (e.g., vims2isis->spiceinit->cam2map) respectively. Regression tests have both input and truth data stored in the ISIS3TESTSDATA area. Input and truth data files can be ISIS cubes, control networks, plain text, Parameter Value Language (PVL), Planetary Data System images (PDS3, PDS4), or comma separated value.

## Setting Up an ISIS Development Environment
In order to run existing tests and develop new tests a full development environment is required. The public releases do not contain the tests or test data. Follow these steps to get a working ISIS development environment with all of the data.

1) Fork the ISIS source code repository (https://github.com/USGS-Astrogeology/ISIS3)
1) Get the ISIS source code on your local machine by cloning your fork
1) Build the ISIS library and applications (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake)
1) Download the ISIS data files see the [Full ISIS3 Data Download](https://github.com/USGS-Astrogeology/ISIS3). Be careful where you put the ISISDATA area. It is several hundred GB and growing.

## Downloading Test Data
ISIS unit and regression tests require the data and test data directories to be available and their respective environment variables (ISISDATA, ISIS3TESTDATA) be set. This allows the tests to read files from these areas and compare results to known truth data. The ISIS tests data is currently distributed using rsync servers. NOTE: Work is underway to put the test data under full version control in a Git repository. From a terminal window set your current working directory to where you want the test data to be downloaded to, and use the rsync command below to copy it to your computer.

```
# Note: Be extremely careful using the suggested --delete option with the rsync command below. 
# If the destination directory is not correct this command can delete all files at that location.
cd /where/you/want/to/locate/the/test/data
rsync -azv --partial --delete isisdist.astrogeology.usgs.gov::isis3testData .
```
When the rsync command is finished, there should be a single directory called "isis" containing about 80GB. The directory structure below this "isis" directory is identical to the ISIS source code directory structure. 

The environment variable "ISIS3TESTDATA" needs to be be set to point to this "isis" directory. Note: Each of the examples below ends with an "isis" directory.

Bash and other sh based shells:
```
export ISIS3TESTDATA=/path/to/the/test/data/for/isis
```
tcsh or other csh based shells:
setenv ISIS3TESTDATA /path/to/the/test/data/for/isis


## Where to go now
* Running tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
* Writing Unit Tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
* Writing Application and Module Tests (https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests)
* Contributing New/Modified Test Data (link)
