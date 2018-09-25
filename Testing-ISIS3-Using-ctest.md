# Identifying Tests
The test names are as follows:
* unit test : `<modulename>_unit_test_<objectname>`
* app test : `<appname>_app_test_<testname>`
* cat test :  `<modulename>_module_test_<testname>`

All cat tests under base, database, control, qisis, and system are listed under the isis3 module.

# Running Tests

ISIS tests now work through [ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html). Unit tests are by default put into the build/unitTest directory. The most simple way to run test of a certain type is using the `-R <regex>` option, which only runs tests which match the regex.

It is important to note the many of the tests rely on an ISISROOT environment variable to be set to the build directory. If it is not set you will see almost all tests fail.
App test must have the bin in the build directory appended to the environment path.
Using the setisis script on your build directory should fix these environment issues for you.

```
# inside your build directory

ctest               # run all tests
ctest -R _unit_     # run unit tests
ctest -R _app_      # run app tests
ctest -R jigsaw     # run jigsaw's app tests
ctest -R lro        # run all lro tests
ctest -E tgo        # Run everything but tgo tests
```
# Building New Tests

The workflow for creating a new tests will be the same as the old ISIS make system besides adding test data. See Make Truth Replacement below for how this set in the process changes.


## App Tests and Category Tests

App/Category tests still leverage the old make system, they work using the standard ISIS app/category test workflow for now.
App/Category tests can be developed in the ISIS src tree similar to the old make system. As long as the path is pointing to the binaries in the build directory (build/bin); make output, make test, make compare, make truthdata, and make ostruthdata all work. You cannot run all tests from the root of the ISIS source tree. To accomplish this use ctest in the build directory, see above. If there is testdata in the ISIS source tree ctest will test with that data.

## Unit Tests

Unit test no longer rely on the old ISIS make system. The unitTest.cpp of each object are compiled and an executable is made and saved in the unitTest sub-directory of the build directory. A symbolic link of the unit test executable is created in the object's directory. This allows the unit test to get files that it needs inside the object's directory, i.e. unitTest.xml. If a unit test passes, then the symbolic link is removed. If you want to run a passing unit test in debug mode, you will have to create a symbolic link of the unit test in the object's directory yourself. If you are inside the object's directory:

`ln -s $ISISROOT/unitTest/<unit_test_name> unitTest`

Steps To Create A New UnitTest:
1. Create unitTest.cpp under new object directory in ISIS src tree
2. re-configure cmake
3. rebuild
4. Use makeOutput.py (see below) to create new truth data

## Example Ctest Output

```
98% tests passed, 7 tests failed out of 394

Total Test time (real) = 171.11 sec

The following tests FAILED:
	 11 - isis3_unit_test_Application (Failed)
	 97 - isis3_unit_test_IException (Failed)
	174 - isis3_unit_test_Pipeline (Failed)
	201 - isis3_unit_test_ProcessExportPds4 (Failed)
	211 - isis3_unit_test_ProgramLauncher (Failed)
	303 - isis3_unit_test_UserInterface (Failed)
	338 - isis3_unit_test_MosaicSceneWidget (Failed)
Errors while running CTest
```

## Make Truth Replacement

The MakeTruth functionality that exists in the Makefiles of the old make system now exists in a script (makeOutput.py) located in `ISIS3/isis/scripts/makeOutput.py`.
A developer will want output of a test before setting it as truth data. This is currently done with the following command in the form of:

    python3 makeOutput.py test

where test is the cmake name for the unit or app test. It is important to note that this command must be ran from the the src directory, i.e., the object's directory that output is being made for.
For unit tests, this will output a file in the form of <objectname>.Truth in the directory `build/testOutputDir`.
For app tests, this will output a directory (truth) in the directory `build/testOutputDir` that contains the truth data for the app test.

To check in truth data the command should be in the form of:

    python3 makeOutput.py -t test

Example makeOutput.py for object Apollo:
```shell
command:    python3 $ISISROOT/../isis/scripts/makeOutput.py apollo_unit_test_Apollo
output:     Unit Test Output In /scratch/cmake/isis3_cmake/build/testOutputDir/ As Apollo.truth
```

```shell
command:    python3 $ISISROOT/../isis/scripts/makeOutput.py apollo_unit_test_Apollo -t
output:     Checked In Truth Data To /scratch/cmake/isis3_cmake/isis/src/apollo/objs/Apollo/Apollo.truth
```

# Building in Debug Mode
1. reconfigure cmake with flag (-DCMAKE_BUILD_TYPE=DEBUG)
2. rebuild

# Further Reading

[Ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html) functionality extends beyond this wiki. Take a moment to see the ctest documentation for additional capabilities.