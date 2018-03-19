# Building for Users

You need to make sure you have v007 libraries installed in the usual location. 

```
git clone https://github.com/USGS-Astrogeology/isis3_cmake.git
cd isis3_cmake 
mkdir build
cd build 

cmake -DCMAKE_INSTALL_PREFIX=<install directory> ..
make install

setisis <install location>
```

The `$ISIS3DATA` environment variable can be overwritten with `-Disis3Data=<new-path>`

# Building For Developers

Some quick vernacular:

* **Source Directory**: Where the ISIS source code lives (i.e. your local repository)
* **Build Directory**: Where generated project files live (Makefiles, Ninja files, Xcode project, etc.) and where binaries are built to.  This is where you spend most of your development time. 
* **Install Directory**: Where the binaries are placed for shipping during. 


## Generators

CMake offers [different generators](https://cmake.org/cmake/help/v3.4/manual/cmake-generators.7.html) for whatever kind of project you want to build. Simply add `-G<Generator>`, the default being Unix Makefiles if `-G` is not used. 

> **Note** The only generators tested so far are Ninja and Unix Makefiles

## Unix Makefiles 

```
git clone https://github.com/USGS-Astrogeology/isis3_cmake.git
cd isis3_cmake 
mkdir build
cd build 

# CMake will use your $ISIS3DATA and ISIS3TESTDATA environment variables or can be overwritten here
cmake -DCMAKE_INSTALL_PREFIX=<install directory> ..
make -j8

# You want the build directory to be ISISROOT
setisis `pwd` 
```

## Ninja

Ninja is bit more performant than Unix Makefiles especially for rebuilding dependencies. 

```
git clone https://github.com/USGS-Astrogeology/isis3_cmake.git
cd isis3_cmake 
mkdir build
cd build 

# CMake will use your $ISIS3DATA and ISIS3TESTDATA environment variables or can be overwritten here
cmake -GNinja -DCMAKE_INSTALL_PREFIX=<install directory> ..

# -j8 is set by default
ninja

# You want the build directory to be ISISROOT
setisis `pwd` 
```

List targets with `ninja -t targets`