Canonical way to build:

* Clone the repo locally:  <repo directory>
* CD into the clone directory.
* mkdir build
* cd into the build directory
```
cmake -DCMAKE_INSTALL_PREFIX=<install directory> -DJP2FLAG=OFF -GNinja <source directory>
```
ISIS3 apps are placed in \<install directory\>/bin.  The -GNinja flag specifies creating Google ninja
Makefile (an alternative Make system to the traditional GNU make system).  The -DJP2FLAG=OFF disables
JP2000 support.  This is temporary.