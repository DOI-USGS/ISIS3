# QuickStart Guide

## Building ISIS3
From the terminal window:
* Clone the repo locally:  <repo directory>
* CD into the clone directory.
* mkdir build
* cd into the build directory
* Enter the following commands:
```
cmake -DCMAKE_INSTALL_PREFIX=<install directory> -DJP2FLAG=OFF -GNinja <source directory>
ninja install -OR- make -j8
```
``` make ``` will make ISIS inside of your current working directory

``` ninja install ``` will install the binaries for user use inside of the \<install directory\>

When using ``` ninja install ``` the ISIS3 apps are placed in \<install directory\>/bin. 

\<source directory\> is the root `isis` directory of the ISIS source tree.  

The -GNinja flag specifies creating Google ninja Makefile (an alternative Make system to the traditional GNU make system).  The -DJP2FLAG=OFF disables
JP2000 support.  This is temporary.

## New Environmental Variable meanings
$ISISROOT is now the directory that holds the ISIS3 binary files.

## Custom data and test data directories
Custom data and test data directories now have to be relative to the new $ISISROOT
Therefore your data or testdata directories must be at the same hierarchical level as your build or install directories.

## Cleaning builds
Cleaning all of ISIS
```
rm -rf build install
```
Cleaning an individual app
```
cd build
rm bin/appname
```
Cleaning an individual object
```
cd build
rm `find -name ObjectName.cpp.o`
```
Using the Ninja build system:

`ninja -t clean` Removes all built objects except for those built by the build generator.

`ninja -t clean -r rules` Removes all built files specified in rules.ninja (a file which exists in the same directory as build.ninja)

`ninja -t clean \<targetname\>` Just removes all built objects for the specific target.

Getting a list of targets using Ninja is very easy:

`ninja -t targets`


## Building with Debug Flags

To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step. Then, build ISIS and everything will be built with debug flags. If ISIS is already built without debug flags, this will result in a complete re-build of ISIS.

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

