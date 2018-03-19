# Running Tests

ISIS tests now work through [ctest](https://cmake.org/cmake/help/v3.4/manual/ctest.1.html). Tests are by default put into the build directory. The most simple way to run test of a certain type is using the `-R <regex>` option, which only runs tests which match the regex. Test names are generally in the form `app_test_<app>`, `unit_test_<obj>`, `module_test_<module>` and so on. We use the naming system to define which test to run using the regex option as outlined below.

It is important to note the many of the tests rely on an ISISROOT environment variable to be set to the build directory. If it is not set you will see almost all tests fail.
App test must have the bin in the build directory appended to the environment path.

```
# inside your build directory

ctest               # run all tests
ctest -R unit       # run unit tests
ctest -R app        # run app tests
ctest -R jigsaw     # run jigsaw's app tests
ctest -R ControlNet # Run all Control Net related tests
```
# Building New Tests

The workflow for creating a new tests will be the same as the old ISIS make system besides adding test data. See Make Truth Replacement below for how this set in the process changes.


## App Tests

App tests still leverage the old make system, they work using the standard ISIS app test workflow for now. As app tests are simply executable + parameters and checking the output, there exists a possibility for programmatically moving app tests over somewhere else. App tests should simply be re-branded as functional tests and taken out of the ISIS source tree. 


## Unit Tests

Unit test no longer rely on the old ISIS make system. The unitTest.cpp of each object are compiled and an executable is made and saved in the unitTest directory which is inside the build directory. 


## Make Truth Replacement

The MakeTruth functionality that exists in the Makefiles of the old make system now exists in a script (makeOutput.py) located in (ISIS3/isis/scripts/makeOutput.py)
A developer will want output of a test before setting it as truth data. This is currently done with the following command in the form of:
    python3 makeOutput.py test
where test is the cmake name for the unit (modulename_unit_test_objectname) or app (modulename_app_test_appname) test.
For unit test this will output a file in the form of Object.Truth in the directory(build/testOutputDir)
For app test this will output a directory(truth) in the directory(build/testOutputDir) that contains the truth data for the app test.

To check in truth data the command should be in the form of:
    python makeOutput.py -t test
Example makeOutput.py for object Apollo:
```shell
command:    python3 ../isis/scripts/makeOutput.py apollo_unit_test_Apollo
output:     Unit Test Output In /scratch/cmake/isis3_cmake/build/testOutputDir/ As Apollo.truth
```

```
command:    python3 ../isis/scripts/makeOutput.py apollo_unit_test_Apollo -t
output:     Checked In Truth Data To /scratch/cmake/isis3_cmake/isis/src/apollo/objs/Apollo/Apollo.truth
```