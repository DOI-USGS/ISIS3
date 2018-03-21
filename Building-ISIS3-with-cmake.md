# QuickStart Guide

From the terminal window:
* Clone the repo locally:  <repo directory>
* CD into the clone directory.
* mkdir build
* cd into the build directory
* Enter the following command:
```
cmake -DCMAKE_INSTALL_PREFIX=<install directory> -DJP2FLAG=OFF -GNinja <source directory>
ninja install
```
ISIS3 apps are placed in \<install directory\>/bin. \<source directory\> is the root `isis` directory of the ISIS source tree.  The -GNinja flag specifies creating Google ninja
Makefile (an alternative Make system to the traditional GNU make system).  The -DJP2FLAG=OFF disables
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
## Building with Debug Flags

To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step. Then, build ISIS and everything will be built with debug flags. If ISIS is already built without debug flags, this will result in a complete re-build of ISIS.

## Building Individual ISIS3 Applications/Objects

# Applications 

The command (from the build directory) is:

`make install <appname>_app`

To build fx:  `make install fx`

# Objects

`make install isis3`
If you make a change to one class in the ISIS3 API, 
it compiles and builds everything.

