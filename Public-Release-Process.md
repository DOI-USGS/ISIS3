This document explains the release process for Official Releases, Release Candidates and custom builds (for example: missions).

# Step-By-Step Instructions

## Step 1: Check current false positive test failures

In this step, we check the currently failing tests. This is primarily an issue on the Mac OS where the timeout on the test execution is being hit. This is currently a judgement call on whether or not the tests are false-positives. This decision should be made by 2+ developers on the development team.

## Step 2: Set Up the Local and Remote Repositories

In this step, we will prepare the local repository to build from as well as update the remote repository hosted on GitHub. Keep in mind that you will be building from this repo on other systems and plan accordingly by cloning this repo into a directory that you will still have access to as you switch between the machines.

* Clone a fresh copy of the ISIS3 repository from GitHub as isis3mgr
* Update the isis/version file to reflect the proper version number and release stage.
    * alpha - from the dev branch (not yet available)
    * beta - release candidate RC
    * stable - stable public release 
* Update the isis/CMakeLists.txt file to reflect the proper version number, release date, and release stage. NOTE: Do not add the _RC#
* Until the build process is updated to pull from github tarballs, update the recipe/meta.yaml at this stage as well: 
    * build number: should be set to 0
    * The version should be the version of ISIS you are building. Refer [here](https://semver.org/) for information on semantic versioning.
    * If you are building a Release Candidate, please include "_RCx". For example, for the first ISIS3.6.1 release candidate, it would be: "3.6.1_RC1". Our semantic versioning would call for a hyphen (ISIS3.6.1-RC1), but the conda build system requires an underscore.
    * If you are creating a custom build, please include a unique tag. For example, for a custom ISIS3.6.1 CaSSIS build, it would be: "3.6.1_cassis1".
  * The build number should be incremented for each build produced at the same version of source code, and should always begin at 0 for each version. 
  * ****Please note that this step is important as this is how the file to be uploaded to Anaconda Cloud is named by conda build. If a file with the same name already exists on USGS-Astrogeology channel in Anaconda Cloud, it will be overwritten with the new upload.****

* As isis3mgr, do a manual build and run the tests for all supported systems (this is happening in the nightly CI-builds). This may be done using the buildIsisCmakeAllSys script. Make sure you are building in /usgs/pkgs or in another area that is unique to each machine, otherwise the builds will be overwritten. Example: `buildIsisCmakeAllSys -q -b dev /usgs/pkgs/isis3.8.0_RC1-test`
  * This includes all systems tested nightly: Fedora 28 (prog29), Ubuntu 18.4 (prog28), CentOS (prog24), and MacOS 10.13 (prog27).
  * (This step should no longer be necessary once Jenkins has been properly set-up as tests will run before each merge)
* Once tests are passing, push the changes back up to the designated branch.
  * For public releases or Release Candidates, this will simply be the "dev" branch and will require someone else merge for you.
  * (This step is optional for custom builds.)
* Make a github release and tag for the build 
  * The release "Tag version" should be the \<version\> from the meta.yaml file you modified above. This is how the conda build system knows what tar.gz file to pull from the repo. (For example, if your version was 3.6.0, you should set your Release/Tag "Tag version" to 3.6.0 (**note**: no 'v' prefix to the version number.)
    * ***Please note that the recipe/meta.yaml file does not currently make use of this tag due to unresolved issues with the gtest submodule, but we would like to transition to this method for building in the future. The code to implement this exists in the recipe/build.sh file as a comment, but conda-build still makes use of the repository and the branch to clone the repository currently.***
  * Mission and non-standard builds (including release candidates) must be tagged as pre-release.
  * Release Candidate or mission-specific release "Tag version" convention: version XX.YY.ZZ_<mission#/"RC#"><release> (ex. 3.6.1_cassis2 or 3.6.1_RC3)

### The following section will apply after building from the release tarball is functioning: 

* Download the release zip file to some location
* Get the metadata necessary for a release (meta.yaml file):
  * sha256 hash: `openssl sha256 *.zip`
  * version: from the tag created above
  * build number: should be set to 0
* Update the recipes/meta.yaml file within the repo to include proper version number so that the tarball created in step 2 is being targeted. Update the sha256 and check the build number. This number should be `0` with a new version being release). The build number may need to increment if the version number is staying the same and a new  and build number
  * The version should be the version of ISIS you are building. Refer [here](https://semver.org/) for information on semantic versioning.
    * If you are building a Release Candidate, please include "_RC". For example, for the ISIS3.6.1 release candidate, it would be: "3.6.1_RC". Our semantic versioning would call for a hyphen (ISIS3.6.1-RC), but the conda build system requires an underscore.
    * If you are creating a custom build, please include a unique tag. For example, for a custom ISIS3.6.1 CaSSIS build, it would be: "3.6.1_cassis".
  * The build number should be incremented for each build produced at the same version of source code, and should always begin at 0 for each version. 
  * ****Please note that this step is important as this is how the file to be uploaded to Anaconda Cloud is named by conda build. If a file with the same name already exists on USGS-Astrogeology channel in Anaconda Cloud, it will be overwritten with the new upload.****
  * Now that the meta.yaml has been updated, go ahead and PR the updated meta.yaml to the dev branch.

## Step 3: Create the Builds for Anaconda Cloud

In this step, we will create the build(s) for Anaconda Cloud using the conda-build system. Keep in mind that there will usually be two default public builds: one for Linux (built on prog28, Ubuntu 18 LTS), and one for Mac (built on prog26, Mac OS 10.13). (Missions may need certain builds and not others. Communicate with your team as to what they are going to need.) Repeat this and the upload process process for each necessary system. You will need to be isis3mgr for this step since prog26 needs /jessetest in the PATH.

Please keep in mind that conda may be a little finicky when building for other systems. You must use an OS-specific version of the software, confirm that there exists only one version in your system PATH variable, and that all commands run use that version exclusively. Fortunately, much of the output from conda commands will state the path to the version it is using explicitly.

* ssh into the prog machine that you are creating the build from and ensure you are using a bash shell.
* Ensure you have a version of conda setup, and if not, set it up. Keep in mind that there exists versions for use building-wide in /usgs/cpkgs/ (though for the Mac system, you will probably want to use the version in /jessetest/miniconda/).
  * Run the command ```conda env list```. If conda is setup properly, this will display a list of available conda environments. If instead you see a ```Command not found``` error or the active base environment (noted by an asterisk) is not the version of conda you are wanting to use, you will need to setup the version of conda you wish to use by running the command ```source /<path-to-anaconda-version>/etc/profile.d/conda.sh```.
* In your base Anaconda environment (just confirm no environments have been activated), run `conda clean --all` to clean out your package cache and ensure you pull fresh packages for the build
* Ensure that you have anaconda-client, conda-build, and conda-verify installed in your build environment
  * You can check by running ```anaconda login```, ```conda build -h```, and ```conda-verify --help```, respectively.
* If this fails, try activating the base environment: ```conda activate```
* If any of these packages are still unavailable, they can be installed as follows: 
`conda install -c anaconda conda-build` 
`conda install -c anaconda conda-verify`
* If you do not already have an up-to-date clone of ISIS3 which includes changes made earlier in this document, 
clone ISIS3 as isis3mgr, and checkout the branch to be built.
* From the root of the ISIS3 repo run ```conda build recipe/ -c usgs-astrogeology -c conda-forge --no-test```
  * The -c options are to give conda a channel priority. Without these, conda will always attempt to download from the default Anaconda channel first. (Unlike the environment.yml files, channel priority cannot be defined within the meta.yaml.)
  * Since we do not have testing set-up through conda, the “--no-test” flag must be set in order to avoid errors. (By default, conda looks for a run_test file and will throw an error if it can not be located.)
* This command will take several minutes to run. Be looking for an "If you want to upload package(s) to anaconda.org later, type:  ..." message towards the end of the build output to confirm a good build.
  * Make a note of this output. It will contain the location of your compressed .tar.bz2 file containing your build.
  * You may also get a bunch of warnings during the building process, this is okay for now.
* Save the tar.bz2 file produced by the previous command in /work/projects/conda-bld/. (Each OS will have it's own directory within here; be sure to save each OS-specific build in it's respective directory). This ensures that we have a backup of all files that have been uploaded into Anaconda Cloud.

## Step 3: Upload the Build to Anaconda Cloud

In this step, we will upload the build(s) that we just created into the Anaconda Cloud to distribute them to our users. The location of the .tar.bz2 file to be uploaded should have been displayed at the end of the ```conda build``` command from above. In case you missed this message, you may also run this command to see the location: ```conda build recipe/ --output```. Keep in mind that this does not confirm that the file actually exists - only where it _would_ be saved with a successful build.

Before uploading the build, make sure the base environment is still activated. If not, run: ```conda activate```

Though uploading the files involves only one command each, the command will be slightly different depending on how you would like to distribute the build and should be repeated for all builds that were produced.

If this is not a special release, but is instead a standard public release, you will use the command:

```anaconda upload -u usgs-astrogeology <path-to-the-.tar.bz2-file>```

This will upload the build with the "main" label and will be the default build users would download from Anaconda Cloud.

If, however, this is a custom build and/or a Release Candidate, you will need to include a label tag in your upload command. 

* For an Release Candidate, that command would be:

```anaconda upload -u usgs-astrogeology -l RC <path-to-the-.tar.bz2-file>```

* For a special CaSSIS build that we would want to give the unique label of "cassis":

```anaconda upload -u usgs-astrogeology -l cassis <path-to-the-.tar.bz2-file>```

Remember to always ensure that custom builds include a label flag or the file will be uploaded by default with the "main" tag on Anaconda Cloud and users may receive a build not intended as a main release by default.

## Step 4: Update Data and TestData Areas on rsync Servers

This step covers how to update the data on the rysnc servers. This is where our external users will have access to the data necessary for running ISIS. One server is located on campus, while the other server is located in Phoenix. These commands must be run as isis3mgr for permission purposes.

***Please pay careful attention to where you are rsync'ing the data to on the remote servers. It is going to depend on the type of build you just completed (Public Release, Release Candidate, custom build, etc). Adding a `-n` flag to your rsync command can be useful to see what files would be copied over without actually copying them to ensure your command is correct.***

The rsync command for the local and remote servers are:

```rsync -rtpvl /usgs/cpkgs/isis3/data/ isisdist.astrogeology.usgs.gov:/work1/dist/isis3/<isis3data or mission specific>/data/```

and

```rsync -rtpvl /usgs/cpkgs/isis3/data/ isisdist:/work1/dist/isis3/<isis3data or mission specific>/data/```

***This entire step will be outdated with the upcoming migration to GitLab for our external data storage and CI integration.***

## Step 5: Create Internal Builds/Installs for Astro

This step covers creating the builds and the installation environments of ISIS for our internal users here on the ASC campus. As of 8/23/19, conda environments are the only supported way to access ISIS binaries internally, so it's not necessary to make builds for the `/usgs/pkgs` area as part of the release process any more.

Setting up the conda environments will involve installing the conda build of ISIS that we just pushed up to Anaconda, and will basically follow the general instructions of installing ISIS that can be found in the README.MD of the isis3 repository, with a few minor modifications. Note that this will need to be done once for Linux, and once for Mac.

Ensure that you have the proper build of conda set-up. For Linux:

```source /usgs/cpkgs/anaconda3_linux/etc/profile.d/conda.sh```

For MacOS:

```source /usgs/cpkgs/anaconda3_macOS/etc/profile.d/conda.sh```

Then run the following commands:

```
   conda create -n <isis version> python=3.6

   conda activate <isis version>

   conda config --env --add channels conda-forge

   conda config --env --add channels usgs-astrogeology

   # For Public Releases
   conda install -c usgs-astrogeology isis3

   # For Release Candidates
   conda install -c usgs-astrogeology/label/RC isis3

   # For Custom Builds, simply use the same label that you used to upload to Anaconda
   conda install -c usgs-astrogeology/label/<label> isis3

   python $CONDA_PREFIX/scripts/isis3VarInit.py -d /usgs/cpkgs/isis3/data -t /usgs/cpkgs/isis3/testData
```

Confirm that the environment has been set-up properly by deactivating it, reactivating it, and running a simple application of your choice.

***Don't forget to go back and do the other OS!***

## Step 6: Update Documentation

***This step only need-be done with official public releases***

This step will update the ISIS documentation on our external web site for our users worldwide. Be sure that this is the current version of ISIS to be released.

* As isis3mgr, setisis to the build directory in one of the clones you were working with earlier
* Run the ```ninja docs``` command from this build directory to build the documentation for this version of the code.
* cd into the isis/src/docsys directory that is at the same level as your build/ directory and run the command ```make wwwdoc```.

You may run into permission issues if isis3mgr does not own these files (you were not isis3mgr when you originally pulled this version of the repo). Additionally, you may be asked to provide credentials if you run the ```make wwwdoc``` command as anyone other than isis3mgr.

## Step 7: Communicate Availability of Build

You will now need to communicate with both the internal as well as the external users about a new version being available. (Feel free to use past announcements as a template.)

For the internal announcement, send an email to all of astro (GS-G-AZflg Astro <gs-g-azflg_astro@usgs.gov>) informing them of internal availability. Also inform users that the isis3 symlink will be updated a week after the public release. It may be a good idea to encourage internal users to try to use conda to access the binaries/applications. 

The external announcement will be made via AstroDiscuss. Visit AstroDiscuss and create a new topic. Again, you may make use of [past announcements](https://astrodiscuss.usgs.gov/t/the-public-release-for-isis3-7-0-is-now-available/176) to template your announcement. To create the changelog look at the commit history for the branch since the last release. The changelog and release announcement are targeted at users; so, changes targeted at developers should not be included. Two examples of changes that do not need to be included in the changelog are minor modifications to the build system and developer documentation updates.

## Problems
If you test the conda environment you created for the ISIS build, i.e., isis3.7.1, on prog26 as isis3mgr and get the following warning:
```
Could not find conda environment: <isis version>
You can list all discoverable environments with `conda info --envs`.
```

Run the following command:
```
source /usgs/cpkgs/anaconda3_macOS/etc/profile.d/conda.sh
```
This problem occurs because we are building ISIS with the shared anaconda on cpkgs instead of /jessetest/miniconda3 (which is the version of anaconda being sourced in .bashrc). You may also do a conda activate with a full path to the environment instead of running the above source command.