# QuickStart Guide

## Getting Started With GitHub
To get started, you want a fresh copy of ISIS to work on. You first want to create a fork of the ISIS3 repo by going to the [main ISIS3 repo page](https://github.com/USGS-Astrogeology/ISIS3) and clicking on "Fork" at the top of the page.

Next, you want to create a clone of your fork on your machine. Go to your fork of ISIS3 on the GitHub website, url should be `github.com/<username>/ISIS3`, and click on the "Clone or Download" green button on the right and copy the link that is displayed. Next, you are going to open a terminal, go to the directory you want to make a clone in, and in the terminal, type:

`git clone <paste the link>`

This will copy all files in your fork to your current location in the terminal. Then, in your terminal, navigate to the ISIS3 directory by typing:

`cd ISIS3`

For now, pull down the cmake branch. This is temporary until cmake is merged into dev.


## Anaconda and ISIS3 dependencies
To started building ISIS3 with cmake, you first need anaconda installed. Go to [Anaconda's download page](https://www.anaconda.com/download/) and follow the instructions for your operating system. ISIS3 dependencies are managed through Anaconda and ISIS3 uses Anaconda environments when building. Third party libraries are added inside of an environment. The cmake build configuration system expects an active [Anaconda environment](https://conda.io/docs/user-guide/tasks/manage-environments.html#activating-an-environment) containing these dependencies. There is an environment.yml file in the ISIS3 directory of your clone.  To create the required Anaconda environment, go into the ISIS3 directory and enter the following command:

`conda env create -n <environment-name> -f environment.yml` (you may name the environment whatever you want)

Building ISIS requires that the anaconda environment be activated. Activate your anaconda environment with:

`source activate <environment-name>`

## Building ISIS3
**At the top-level directory of an ISIS3 clone**:
* Create a `build` and an `install` directory at this level:
  * `mkdir build install`
  * There should now be a build/ install/ and isis/ directory.

* cd into the build directory and configure your build:
  * `cmake -DCMAKE_INSTALL_PREFIX=<install directory> -DJP2KFLAG=OFF -Dpybindings=OFF -GNinja <source directory>`
  * \<source directory\> is the root `isis` directory of the ISIS source tree, i.e. `/scratch/this_is_an_example/ISIS3/isis`. From the build directory, this is `../isis`

* Set your ISISROOT to `/the/path/to/your/build`:
  * `setisis .`

* Copy header files to build/inc (This is temporary):
  * `ninja incs`

* Build ISIS inside of your build directory and install it to your install directory:
  * `ninja install`


**Notes**
* The -GNinja flag specifies creating Google [Ninja](https://ninja-build.org/manual.html) Makefile (an alternative Make system to the traditional GNU make system). If you instead want to use make, dont set this flag, and replace the ninja commands with their make counterparts.

* -DJP2KFLAG=OFF disables JP2000 support.  This is temporary.

* -Dpybindings=OFF disables the bundle adjust python bindings.  This is temporary.

* To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step.

* Executables are no longer in an application's directory. When running in debug mode, it is important to give the correct path to an application's executable. Executables are located in build/bin and install/bin. Example using ddt with $ISISROOT set to the build directory:
  * `ddt $ISISROOT/bin/<application_name>`

 
## New Environmental Variable meanings
`$ISISROOT` is no longer the ISIS3 source directory. `$ISISROOT` is now either the CMake build directory for development or the install directory for running a deployed copy of ISIS. 

* **Source Directory**: Where the ISIS source code lives. This is the isis directory. If you are in build, this would be ../isis (i.e. your local repository)
* **Build Directory**: Where generated project files live (Makefiles, Ninja files, Xcode project, etc.) and where binaries are built to.  This is where you spend most of your development time. 
* **Install Directory**: Where the binaries are placed on install. 

## Custom data and test data directories
Custom data and test data directories now have to be relative to the new $ISISROOT
Therefore your data or testdata directories must be at the same hierarchical level as your build or install directories.

## Cleaning builds

**Using the Ninja build system:**

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


## Problems

If you get the following error message when trying to set up your environment:

```
conda env create -n cmake -f environment.yml
Using Anaconda Cloud api site https://api.anaconda.org
Error: invalid package specification: ninja==1.7.2=0
```

Update your conda installation using `conda update` and then try again. 