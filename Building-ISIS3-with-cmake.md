# QuickStart Guide

## Anaconda and ISIS3 dependencies
ISIS3 dependencies are managed through Anaconda. The cmake build configuration system expects an active [Anaconda environment](https://conda.io/docs/user-guide/tasks/manage-environments.html#activating-an-environment) containing these dependencies. There is an environment.yml file  at the top level of the ISIS3 repo. To create the required Anaconda environment, enter the following command:

`conda env create -n <environment-name> -f environment.yml`

Building ISIS requires that the anaconda environment be activated. Activate your anaconda environment with:

`source activate <environment-name>`

## Building ISIS3
**At the top-level directory of an ISIS3 clone**:
* Create a `build` directory and an `install` directory at this level:
  * `mkdir build install`

* Set your ISISROOT to `/the/path/to/your/build`:
  * `export ISISROOT=/scratch/this_is_an_example/ISIS3/build`

* cd into the build directory and configure your build:
  * `cmake -DCMAKE_INSTALL_PREFIX=<install directory> -DJP2KFLAG=OFF -Dpybindings=OFF -GNinja <source directory>`

* Build ISIS inside of your build directory and install it to your install directory:
  * `ninja install`

**Notes**
* The -GNinja flag specifies creating Google [Ninja](https://ninja-build.org/manual.html) Makefile (an alternative Make system to the traditional GNU make system). If you instead want to use make, dont set this flag, and replace the ninja commands with their make counterparts.

* To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step.

* -DJP2KFLAG=OFF disables JP2000 support.  This is temporary.

* -Dpybindings=OFF disables the bundle adjust python bindings.  This is temporary.

* \<source directory\> is the root `isis` directory of the ISIS source tree, i.e. `/scratch/this_is_an_example/ISIS3/isis`. From the build directory, this is `../isis`

 


## New Environmental Variable meanings
`$ISISROOT` is no longer the ISIS3 source directory. `$ISISROOT` is now either the CMake build directory for development or the install directory for running a deployed copy of ISIS. 

* **Source Directory**: Where the ISIS source code lives (i.e. your local repository)
* **Build Directory**: Where generated project files live (Makefiles, Ninja files, Xcode project, etc.) and where binaries are built to.  This is where you spend most of your development time. 
* **Install Directory**: Where the binaries are placed on install. 

## Custom data and test data directories
Custom data and test data directories now have to be relative to the new $ISISROOT
Therefore your data or testdata directories must be at the same hierarchical level as your build or install directories.

## Cleaning builds
**Manual Cleans**

Cleaning all of ISIS: 
`rm -rf build install`

Cleaning an individual app:
`cd build && rm bin/<app_name>`

Cleaning an individual object
 ``cd build && rm `find -name ObjectName.cpp.o` ``

**Using the Ninja build system:**

`ninja -t clean` Removes all built objects except for those built by the build generator.

`ninja -t clean -r rules` Removes all built files specified in rules.ninja (a file which exists in the same directory as build.ninja)

`ninja -t clean \<targetname\>` Removes all built objects for the specific target.

Getting a list of targets using Ninja is very easy:

`ninja -t targets`

## Building Individual ISIS3 Applications/Objects

### Applications 

The command (from the build directory) is:

`make install <appname>_app`

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