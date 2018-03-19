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
ISIS3 apps are placed in \<install directory\>/bin.  The -GNinja flag specifies creating Google ninja
Makefile (an alternative Make system to the traditional GNU make system).  The -DJP2FLAG=OFF disables
JP2000 support.  This is temporary.

## New ENV meanings
ISISROOT is now the directories that hold the binaries.

## Custom data and test data directories
Custom data and test data directories now have to be relative to the new ISISROOT
Therefore your data or testdata directories must be next to your build or install directories.