This document explains both the release process for Official Releases as well as Release Candidates. In addition, it describes the process for custom builds (for missions, for example).

# Step-By-Step Instructions

## Step 1: Check current false positive test failures

In this step, we check the currently failing tests. This is primarily an issue on the Mac OS where the timeout on the test execution is being hit. This is currently a judgement call on whether or not the tests are false-positives. This decision should be made by 2+ developers on the development team.

## Step 2: Set Up the Local and Remote Repositories

In this step, we will carefully prepare the local repository to build from as well as update the remote repository hosted on GitHub. Keep in mind that you will be building from this repo on other systems and plan accordingly by cloning this repo into a directory that you will still have access to as you switch between the machines.

* Clone a fresh copy of the ISIS3 repository from GitHub
* Update the isis/version file to reflect the proper version number and release stage. 
* Update the isis/CMakeLists.txt file to reflect the proper version number, release data, and release stage.
* Until the build process is updated to pull from github tarballs, update the `meta.yaml` at this stage as well: 
    * build number: should be set to 0
    * The version should be the version of ISIS you are building. Refer [here](https://semver.org/) for information on semantic versioning.
    * If you are building a Release Candidate, please include "_RC". For example, for the ISIS3.6.1 release candidate, it would be: "3.6.1_RC". Our semantic versioning would call for a hyphen (ISIS3.6.1-RC), but the conda build system requires an underscore.
    * If you are creating a custom build, please include a unique tag. For example, for a custom ISIS3.6.1 CaSSIS build, it would be: "3.6.1_cassis".
  * The build number should be incremented for each build produced at the same version of source code, and should always begin at 0 for each version. 
  * ****Please note that this step is important as this is how the file to be uploaded to Anaconda Cloud is named by conda build. If a file with the same name already exists on USGS-Astrogeology channel in Anaconda Cloud, it will be overwritten with the new upload.****

* Do a manual build and run the tests for all supported systems (this is happening in the nightly CI-builds).
  * This includes all systems tested nightly: Linux 28 (prog29), Ubuntu 18.4 (prog28), prog24, and MacOS 10.13 (prog27).
  * (This step should no longer be necessary once Jenkins has been properly set-up as tests will run before each merge)
* Once tests are passing, push the changes back up to the designated branch.
  * For public releases or Release Candidates, this will simply be the "dev" branch and will require someone else merge for you.
  * (This step is optional for custom builds.)
* Make a github release and tag for the build 
  * The release "Tag version" should be the \<version\> from the meta.yaml file you modified above. This is how the conda build system knows what tar.gz file to pull from the repo. (For example, if your version was 3.6.0, you should set your Release/Tag "Tag version" to 3.6.0 (**note**: no 'v' prefix to the version number.)
    * ***Please note that the recipe/meta.yaml file does not currently make use of this tag due to unresolved issues with the gtest submodule, but we would like to transition to this method for building in the future. The code to implement this exists in the recipe/build.sh file as a comment, but conda-build still makes use of the repository and the branch to clone the repository currently.***
  * Mission and non-standard builds (including release candidates) must be tagged as pre-release.
  * Release Candidate or mission-specific release "Tag version" convention: version XX.YY.ZZ_<mission/"RC"><release> (ex. 3.6.1_cassis2 or 3.6.1_RC3)

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

In this step, we will create the build(s) for Anaconda Cloud using the conda-build system. Keep in mind that there will usually be two default public builds: one for Linux (built on prog28), and one for Mac (built on prog26). (Missions may need certain builds and not others. Communicate with your team as to what they are going to need.) Repeat this and the upload process process for each necessary system.

Please keep in mind that conda may be a little finicky when building for other systems. You must use an OS-specific version of the software, confirm that there exists only one version in your system PATH variable, and that all commands run use that version exclusively. Fortunately, much of the output from conda commands will state the path to the version it is using explicitly.

* ssh into the prog machine that you are creating the build from and ensure you are using a bash shell.
* Ensure you have a version of conda setup, and if not, set it up. Keep in mind that there exists versions for use building-wide in /usgs/cpkgs/ (though for the Debian system, you will probably want to use the version in /jessetest/miniconda/).
  * Run the command ```conda env list```. If conda is setup properly, this will display a list of available conda environments. If instead you see a ```Command not found``` error or the active base environment (noted by an asterisk) is not the version of conda you are wanting to use, you will need to setup the version of conda you wish to use by running the command ```source /<path-to-anaconda-version>/etc/profile.d/conda.sh```.
* In your base Anaconda environment (just confirm no environments have been activated), run `conda clean --all` to clean out your package cache and ensure you pull fresh packages for the build
* Ensure that you have anaconda-client, conda-build, and conda-verify installed in your build environment
  * You can check by running ```anaconda login```, ```conda build -h```, and ```conda verify -h```, respectively.
* From the root of the ISIS3 repo run ```conda build recipe/ -c usgs-astrogeology -c conda-forge --no-test```
  * The -c options are to give conda a channel priority. Without these, conda will always attempt to download from the default Anaconda channel first. (Unlike the environment.yml files, channel priority cannot be defined within the meta.yaml.)
  * Since we do not have testing set-up through conda, the “--no-test” flag must be set in order to avoid errors. (By default, conda looks for a run_test file and will throw an error if it can not be located.)
* This command will take several minutes to run. Be looking for an "If you want to upload package(s) to anaconda.org later, type:  ..." message towards the end of the build output to confirm a good build.
  * Make a note of this output. It will contain the location of your compressed .tar.bz2 file containing your build.
* Save the tar.bz2 file produced by the previous command in /work/projects/conda-bld/. (Each OS will have it's own directory within here; be sure to save each OS-specific build in it's repsective directory). This ensures that we have a backup of all files that have been uploaded into Anaconda Cloud.

## Step 3: Upload the Build to Anaconda Cloud

In this step, we will upload the build(s) that we just created into the Anaconda Cloud to distribute them to our users. The location of the .tar.bz2 file to be uploaded should have been displayed at the end of the ```conda build``` command from above. In case you missed this message, you may also run this command to see the location: ```conda build recipe/ --output```. Keep in mind that this does not confirm that the file actually exists - only where it _would_ be saved with a successful build.

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

This step covers creating the builds and the installation environments of ISIS for our internal users here on the ASC campus. 

The builds should be created for each of the prog machines (prog24, prog26, prog28 and prog29) and should be saved in /usgs/pkgs/. The process can be handled with the same script that the current nightly builds use. The script can be found at /usgs/cpkgs/isis3/isis3mgr_scripts and is entitled buildIsisCmakeAllSys. It is best to run this script from a screen session so as not to have it interrupted by any network issues. Run these commands as isis3mgr from one of the prog machines:

```screen -RD```

This starts your screen session. You can exit screen by holding `<cntrl><A><D>`, and return to the same screen session with the previous command. 

```buildIsisCmakeAllSys -t <tag version> --no-test /usgs/pkgs/<isis version>```

This command will run the script, building on all of the prog machines at once. The <tag version> value should be whatever Tag Version you set for the release on the GitHub repository, and the <isis version> value should be the version of ISIS you are building. (The <isis version> will generally be the same as your <tag version>.)

Keep in mind that this script will build on prog24, so the build will be accessible via the astrovms as well.

***While we are in the process of getting Astro caught-up with the process of using conda for our internal builds of ISIS, this building process still needs to be maintained until we officially make the official switch.***

Setting up the conda environments will involve installing the conda build of ISIS that we just pushed up to Anaconda, and will basically follow the general instructions of installing ISIS that can be found in the README.MD of the isis3 repository, with a few minor modifications. Not that this will need to be done once for Linux, and once for Debian.

Ensure that you have the proper build of conda set-up. For Linux:

```source /usgs/cpkgs/anaconda3_linux/etc/profile.d/conda.sh```

For MacOS (Debian):

```source /usgs/cpkgs/anaconda3_macOS/etc/profile.d/conda.sh```

Then run the following commands:

```
   conda create -n <isis version> python=3.6

   conda activate isis3

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

## Step 6: Communicate Availability of Build

You will now need to communicate with both the internal as well as the external users about a new version being available. (Feel free to use past announcements as a template.)

For the internal announcement, send an email to all of astro (GS-G-AZflg Astro <gs-g-azflg_astro@usgs.gov>) informing them of internal availability. It may be a good idea to encourage internal users to try to use conda to access the binaries/applications. 

The external announcement will be made via AstroDiscuss. Visit AstroDiscuss and create a new topic. Again, you may make use of [past announcements](https://astrodiscuss.usgs.gov/t/the-public-release-for-isis3-7-0-is-now-available/176) to template your announcement. 