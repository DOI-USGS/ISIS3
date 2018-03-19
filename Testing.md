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

A challenge that still exists is creating new work flows to creating application tests. The old testing system was tightly coupled with the original Makefiles and the logic for tests and testing was maintained in house. Ideally, we want to eliminate the need for any of the old Makefiles. This means finding a programmatic way of moving old application tests over to a new system. The unitTests are currently compiled and ran independent of the old Make system.


## App Tests

App tests still leverage the old make system, they work using the standard ISIS app test workflow for now. As app tests are simply executable + parameters and checking the output, there exists a possibility for programmatically moving app tests over somewhere else. App tests should simply be re-branded as functional tests and taken out of the ISIS source tree. 


## Unit Tests

Unit test no longer rely on the old ISIS make system. The unitTest.cpp of each object are compiled and an executable is made and saved in the unitTest directory which is inside the build directory. 


## Make Truth Replacement

Creating new truth data currently is limited to unit tests and has the solution of a script(makeOutput.py) in the ISIS src tree inside the scripts directory. This script takes a unitTest name as an argument and creates an output file for the unitTest executable in the unitTest directory. The purpose of this is to see an example of the output before making truth. To makeTruth the script will take the same unitTest argument and a second argument "truth" to set the output to be in the unit test source directory as the new truth data. 