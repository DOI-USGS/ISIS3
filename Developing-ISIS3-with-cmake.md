## Getting Started With GitHub
To get started, you want a fresh copy of ISIS to work on. You first want to create a fork of the ISIS3 repo by going to the [main ISIS3 repo page](https://github.com/USGS-Astrogeology/ISIS3) and clicking on "Fork" at the top of the page.

Next, you want to create a clone of your fork on your machine. Go to your fork of ISIS3 on the GitHub website, url should be `github.com/<username>/ISIS3`, and click on the "Clone or Download" green button on the right and copy the link that is displayed. Next, you are going to open a terminal, go to the directory you want to make a clone in, and in the terminal, type:

`git clone --recurse-submodules <paste the link>`

This will copy all files in your fork to your current location in the terminal. Then, in your terminal, navigate to the ISIS3 directory by typing:

`cd ISIS3`

### How to initialize the gtest submodule in an old clone

If your clone is older than November 26th 2018 and you update from dev, you will get the gtest submodule but it will be empty! This is because git does not initialize submodules by default. In order to initialize the gtest submodule run the following command at the root of the repository

`git submodule update --init --recursive`


## Anaconda and ISIS3 Dependencies
To start building ISIS3 with cmake, you first need anaconda installed. **If you are developing internally to USGS, please expand the `Details` drop-down for more information; otherwise, continue reading the instructions in this section**. 

<details>

When developing internally, it is *recommended* when you start working with cmake and anaconda that you use the shared anaconda environments in `/usgs/cpkgs/`. These anaconda environments have the isis3 dependencies installed that are needed for development. This makes setup simple and can make sharing builds easier.
You will need to modify your `~/.bashrc` as follows:

(Linux)
```bash
echo -e "\n# Adding shared /usgs/cpkgs/ anaconda3 environment" >> ~/.bashrc
echo 'source /usgs/cpkgs/anaconda3_linux/etc/profile.d/conda.sh' >> ~/.bashrc
```

(macOS)
```bash
echo -e "\n# Adding shared /usgs/cpkgs/ anaconda3 environment" >> ~/.bashrc
echo 'source /usgs/cpkgs/anaconda3_macOS/etc/profile.d/conda.sh' >> ~/.bashrc
```

You will then need to `source ~/.bashrc` or open a new `bash` terminal to get the anaconda3 binaries added to your path.

To activate the isis3 environment and start developing, you can run:
```bash
source activate isis3
```

You can continue to the [Building ISIS3](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3) section.
</details>

<br/>

Go to [Anaconda's download page](https://www.anaconda.com/download/) and follow the instructions for your operating system. ISIS3 dependencies are managed through Anaconda and ISIS3 uses Anaconda environments when building. Third party libraries are added inside of an environment. The cmake build configuration system expects an active [Anaconda environment](https://conda.io/docs/user-guide/tasks/manage-environments.html#activating-an-environment) containing these dependencies. There is an environment.yml file in the ISIS3 directory of your clone.  To create the required Anaconda environment, go into the ISIS3 directory and enter the following command:

`conda env create -n <environment-name> -f environment.yml` 

Give the environment whatever name you choose by substituting it for `<environment-name>`

Building ISIS requires that the anaconda environment be activated. Activate your anaconda environment with:

`source activate <environment-name>`

## Building ISIS3
**At the top-level directory of an ISIS3 clone**:
* Create a `build` and an `install` directory at this level:
  * `mkdir build install`
  * There should now be a build/ install/ and isis/ directory.

* Set your ISISROOT to `/the/path/to/your/build`:
  * `export ISISROOT=$(pwd)`
  * **For internal instructions, see `Details` section below.**

<details>

Run the `setisis` command for your build directory:
```bash
setisis .
```

The following error is expected and can be ignored:

```
Warning: Unable to find binaries.
 Warning: Unable to find initialization scripts in <build directory>.
          Unable to set up third party, or data directories. (Use -h for help)
 Warning: Only ISISROOT set.
 ISISROOT set to: <build directory>
```
(Where `<build directory>` is the directory you have run the setisis command in.) 

If this does not work (i.e. `no setisis in PATH`), run the following command to add an alias to your `~/.bashrc`:
```bash
echo -e "alias setisis='. /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh'" >> ~/.bashrc
```

</details>
<br/>

* If you want to run the tests set the ISISDATA and ISISTESTDATA directories
  * `export ISISDATA=/path/to/your/data/directory`
  * `export ISISTESTDATA=/path/to/your/test/data/directory`

* cd into the build directory and configure your build:
  * `cmake -DJP2KFLAG=OFF -GNinja <source directory>`
  * \<source directory\> is the root `isis` directory of the ISIS source tree, i.e. `/scratch/this_is_an_example/ISIS3/isis`. From the build directory, this is `../isis`

* Build ISIS inside of your build directory and install it to your install directory:
  * `ninja install`


**Notes**
* The -GNinja flag specifies creating Google [Ninja](https://ninja-build.org/manual.html) Makefile (an alternative Make system to the traditional GNU make system). If you instead want to use make, dont set this flag, and replace the ninja commands with their make counterparts.

* -Disis3Data is used to set the location of the isis3 data directory, which includes kernels, icons, templates, etc. *This is needed to successfully run the app and module tests.*

* -Disis3TestData is used to the location of the isis3 testData directory, which includes input and expected truth files for running and validating tests.

* -DJP2KFLAG=OFF disables JP2000 support.  **If you are internal, you should turn this ON.**

* -Dpybindings=OFF disables the bundle adjust python bindings.  This is temporary.

* To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step.

* Executables are no longer in an application's directory. When running in debug mode, it is important to give the correct path to an application's executable. Executables are located in build/bin and install/bin. Example using ddt with $ISISROOT set to the build directory:
  * `ddt $ISISROOT/bin/<application_name>`

 
## New Environmental Variable Meanings
`$ISISROOT` is no longer the ISIS3 source directory. `$ISISROOT` is now either the CMake build directory for development or the install directory for running a deployed copy of ISIS. 

* **Source Directory**: Where the ISIS source code lives. This is the isis directory. If you are in build, this would be ../isis (i.e. your local repository)
* **Build Directory**: Where generated project files live (Makefiles, Ninja files, Xcode project, etc.) and where binaries are built to.  This is where you spend most of your development time. 
* **Install Directory**: Where the binaries are placed on install. 

## Custom Data and Test Data Directories
Custom data and test data directories now have to be relative to the new $ISISROOT
Therefore your data or testdata directories must be at the same hierarchical level as your build or install directories.

## Cleaning Builds

**Using the Ninja Build System:**

Removes all built objects except for those built by the build generator:
`ninja -t clean` 

Remove all built files specified in rules.ninja:
`ninja -t clean -r rules` 

Remove all built objects for a specific target:
`ninja -t clean \<target_name\>` 

Get a list of Ninja's targets:
`ninja -t targets`

**Manual Cleans**

Cleaning all of ISIS: 
`rm -rf build install`

Cleaning an individual app:
`cd build && rm bin/<app_name>`

Cleaning an individual object:
 ``cd build && rm `find -name ObjectName.cpp.o` ``

## Building Individual ISIS3 Applications/Objects

### Applications 

The command (from the build directory) is:

`make install <appname>`

To build fx:  `make install fx`

### Objects

`make install isis3 -j7`
If you make a change to one class in the ISIS3 API, 
it compiles and builds everything.  This can take awhile.

If you are using Ninja the command is:

`ninja install libisis3.so`

The nice thing about Ninja is that it re-compiles whatever class you modified,
and then re-links all the applications/objects/unit tests which have dependencies
on that object.  In the case of a heavily used class like Pixel, this equates to 865 objects.
It's still a lot faster then using cmake generated Makefiles.

### Plugins

## CMake Behavior When Adding/Removing/Modifying an Object

The cmake configure command needs to be executed when adding/removing a new object so that the system sees and compiles it.  

## Building ISIS3 Documentation

At present under the current system there is no way to build documentation for individual applications/objects.  To build all documentation using the ninja build system, CD into the build directory and enter the following command:

`ninja docs -j7`

If CMake is being used to produce GNU Makefiles, the process is the same, but the command is:

`make docs -j7`

The documentation is placed in install/docs (after being copied over from build/docs).

## Building in Debug Mode
1. reconfigure cmake with flag (-DCMAKE_BUILD_TYPE=DEBUG)
2. rebuild

## Identifying Tests
The test names are as follows:
* unit test : `<modulename>_unit_test_<objectname>`
* app test : `<appname>_app_test_<testname>`
* cat test :  `<modulename>_module_test_<testname>`

All cat tests under base, database, control, qisis, and system are listed under the isis3 module.

## Running Tests

ISIS tests now work through [ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html). Unit tests are by default put into the build/unitTest directory. The most simple way to run test of a certain type is using the `-R <regex>` option, which only runs tests which match the regex.

It is important to note the many of the tests rely on an ISISROOT environment variable to be set to the build directory. If it is not set you will see almost all tests fail.
App test must have the bin in the build directory appended to the environment path.
Using the setisis script on your build directory should fix these environment issues for you.

```
# inside your build directory

ctest               # run all tests
ctest -R _unit_     # run unit tests
ctest -R _app_      # run app tests
ctest -R _module_   # run module (cat) tests
ctest -R jigsaw     # run jigsaw's app tests
ctest -R lro        # run all lro tests
ctest -E tgo        # Run everything but tgo tests
```
## Building New Tests

The workflow for creating a new tests will be the same as the old ISIS make system besides adding test data. See [Make Truth Replacement](#make-truth-replacement) below for how this set in the process changes.


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
For unit tests, this will output a file in the form of <objectname>.truth in the directory `build/testOutputDir`.
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
When making OS specific truth data, do not add the "-t" flag. Instead, you will need to rename the output and copy it to the directory of the object or app you are making truth data for.

* For unit tests, you will need to rename the output put in build/testOutputDir as `<objectname>_<OStype>_x86_64_<OSname>.truth`. If we wanted to make Mac truth data for ProgramLauncher: "ProgramLauncher_Darwin_x86_64_MacOSX10_13.truth".

* For app tests, you will need to rename the truth directory put in build/testOutputDir as
`truth.<OStype>.x86_64.<OSname>`. If we wanted to make Mac truth data: "truth.Darwin.x86_64.MacOSX10_13".

## Further Reading

[Ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html) functionality extends beyond this wiki. Take a moment to see the ctest documentation for additional capabilities.

## Problems

**If you get the following error message when trying to set up your environment**:

```
conda env create -n cmake -f environment.yml
Using Anaconda Cloud api site https://api.anaconda.org
Error: invalid package specification: ninja==1.7.2=0
```

Update your conda installation using `conda update` and then try again. 


**If you get the following error message while testing on a Mac**:
```
bash: line 10: /usr/local/bin/grep: No such file or directory
bash: line 11: /usr/local/bin/grep: No such file or directory
bash: /usr/local/bin/grep: No such file or directory
```

Check to see if grep is installed in `/usr/local/bin`. If grep is not installed, install grep with Homebrew:

`brew install grep -with-default-names`

This will install GNU grep under the name "grep". If you do not add the flag at the end, GNU grep will be installed under "ggrep" instead. 

If ggrep is installed, run the following commands:
```
brew uninstall grep
brew install grep -with-default-names
```

## Setting Up an ISIS3 cmake Project in Your Integrated Development Environment (IDE)

For Qt Creator: