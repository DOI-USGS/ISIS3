# QuickStart Guide

## Getting Started With GitHub
You will not be directly making changes to the main ISIS3 repo. Instead, make a fork of the ISIS3 repo by clicking on "Fork" at the top of the page. This is the repo that you will be pushing your changes to. If you notice, there is a branch in in the ISIS3 repo called "dev". This is the main branch of the repo where all changes are merged into and the branch where you will be pulling down the latest changes from. Next, you are going to create a clone of your fork on your machine so that you are able to make changes to ISIS. Go to your fork on the GitHub website and click on the "Clone or Download" green button on the right and copy the link that is displayed. Next, you are going to open a terminal and in the terminal, type:

`git clone <paste the link>`

This will copy all files in your fork to your current location in the terminal. Then in your terminal, navigate to the ISIS3 directory by typing:

`cd ISIS3`

Now, you will need to add a remote, so you are able to pull down changes from the main ISIS3 repo. Type:

`git remote add upstream git@github.com:USGS-Astrogeology/ISIS3.git`

When you want to get the latest changes, you can type:

`git pull upstream <branch you want to pull down>`

You will most likely be pulling down changes from dev. Finally, we want to create a branch for cmake where all of the cmake files will be located. In your terminal, inside of your clone, type:

`git branch cmake`

This will create a new branch called cmake. Then, to switch from dev to the cmake branch, type:

`git checkout cmake'

Now, if you type `git branch` again, the cmake branch should have an asterisk next to it (meaning this is the active branch). Now you will want to get to the latest changes from the cmake branch on the main ISIS3 repo. So, we will be using that command from earlier:

`git pull upstream cmake`

When you want to start making changes to ISIS, make a new branch from the cmake branch where all your changes will be made. When you are done making changes, you will want to push your changes to your fork and then make a pull request (PR) to get your changes merged into the main ISIS3 repo. To start, we want to push changes to your fork:

`git push origin <name of the branch>` (origin is the name of the remote for your fork)

Now, on the GitHub website on your fork's page, you should see your new branch come up. Click "New Pull Request" and make sure that the main ISIS3 repo's name and "dev" on the left side of the arrow, ensuring that we are making a PR into dev. Now, you can click "Create Pull Request". Your PR should be open, and another developer will review it and merge it.

You now have the latest cmake files and can start building ISIS3 with cmake!

## Anaconda and ISIS3 dependencies
ISIS3 dependencies are managed through Anaconda. The cmake build configuration system expects an active [Anaconda environment](https://conda.io/docs/user-guide/tasks/manage-environments.html#activating-an-environment) containing these dependencies. There is an environment.yml file  at the top level of the ISIS3 repo. To create the required Anaconda environment, enter the following command:

`conda env create -n <environment-name> -f environment.yml`

Building ISIS requires that the anaconda environment be activated. Activate your anaconda environment with:

`source activate <environment-name>`

## Building ISIS3
**At the top-level directory of an ISIS3 clone**:
* Create a `build` directory and an `install` directory at this level:
  * `mkdir build install`

* cd into the build directory and configure your build:
  * `cmake -DCMAKE_INSTALL_PREFIX=<install directory> -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData -DJP2KFLAG=OFF -Dpybindings=OFF -GNinja <source directory>`
  * \<source directory\> is the root `isis` directory of the ISIS source tree, i.e. `/scratch/this_is_an_example/ISIS3/isis`. From the build directory, this is `../isis`

* Set your ISISROOT to `/the/path/to/your/build`:
  * `setisis .`

* Copy header files to build/inc (This is temporary):
  * `ninja incs`

* Build ISIS inside of your build directory and install it to your install directory:
  * `ninja install`


**Notes**
* The -GNinja flag specifies creating Google [Ninja](https://ninja-build.org/manual.html) Makefile (an alternative Make system to the traditional GNU make system). If you instead want to use make, dont set this flag, and replace the ninja commands with their make counterparts.

* To build with debug flags add `-DCMAKE_BUILD_TYPE=Debug` to the cmake configuration step.

* -DJP2KFLAG=OFF disables JP2000 support.  This is temporary.

* -Dpybindings=OFF disables the bundle adjust python bindings.  This is temporary.

* Executables are no longer in an application's directory. When running in debug mode, it is important to give the correct path to an application's executable. Executables are located in build/bin and install/bin. Example using ddt with $ISISROOT set to the build directory:
  * `ddt $ISISROOT/bin/<application_name>`

 

## New Environmental Variable meanings
`$ISISROOT` is no longer the ISIS3 source directory. `$ISISROOT` is now either the CMake build directory for development or the install directory for running a deployed copy of ISIS. 

* **Source Directory**: Where the ISIS source code lives (i.e. your local repository)
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