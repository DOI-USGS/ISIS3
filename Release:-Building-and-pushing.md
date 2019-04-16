# Step-By-Step Instructions

This document describes the process of building ISIS for a release and pushing the build to Anaconda Cloud. This includes public releases as well as custom releases (for missions, for example).

## Set Up the Local and Remote Repositories

In this step, we will carefully prepare the local repository to build from as well as update the remote repository hosted on GitHub. Keep in mind that you will be building from this repo on other systems and plan accordingly by cloning this repo into a directory that you will still have access to as you switch between the machines.

* Clone a fresh copy of the ISIS3 repository from GitHub
* Update the recipes/meta.yaml file to include proper version number, branch, and build number
  * The version should be the version of ISIS you are building.
    * If you are building a Release Candidate, please include "_RC". For example, for the ISIS3.6.1 release candidate, it would be: "3.6.1_RC". Our semantic versioning would call for a hyphen (ISIS3.6.1-RC), but the conda build system requires an underscore.
    * If you are creating a custom build, please include a unique tag. For example, for a custom ISIS3.6.1 CaSSIS build, it would be: "3.6.1_cassis".
  * The build number should be incremented for each build produced for a certain version number, for example when bug fixes are released, and should always begin at 1 for each version.
  * Please note that this step is important as this is how the file to be uploaded to Anaconda Cloud is named by conda build. If a file with the same name already exists on USGS-Astrogeology channel in Anaconda Cloud, it will be overwritten with the new upload.
* Update the isis/version file to reflect the proper version number
* Confirm that the ```ninja docs``` line from the recipe/build.sh file has been removed
   * The documentation requires a lot of space and we are only allowed 5GB of space on Anaconda Cloud. For more information on this issue, visit the [RFC1](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC1:-Documentation-Delivery).
* Do a manual build and run the tests for all systems
  * This includes Linux 28 (prog29), Ubuntu 18.4 (prog28) and MacOS 10.13 (prog27).
* Once tests are passing, push the changes back up to the designated branch
  * For public releases, this will simply be the "dev" branch and will require someone else merge for you.
  * (This step is optional for custom builds.)
* Make a github release and tag for the build 
  * The release "Tag version" should be the <version>_<build_number> from the meta.yaml file you modified above. This is how the conda build system knows what tar.gz file to pull from the repo. (For example, if your version was 3.6.0 and your build_number was 2, you should set your Release/Tag "Tag version" to 3.6.0_2
  * Mission and non-standard builds (including release candidates) must be tagged as pre-release.
  * Mission release "Tag version" convention: version XX.YY.ZZ_mission_build (ex. 3.6.1_cassis_2)

## Create the Build for Anaconda Cloud

In this step, we will create the build(s) for Anaconda Cloud. Keep in mind that there will usually be two default public builds: one for Linux (built on prog28), and one for Mac (built on prog27). (Missions may need certain builds and not others. Communicate with your team as to what they are going to need.) Repeat this and the upload process process for each necessary system.

Please keep in mind that conda may be a little finicky when building for other systems. You must use an OS-specific version of the software, confirm that there exists only one version in your system PATH variable, and that all commands run use that version exclusively. Fortunately, much of the output from conda commands will state the path to the version it is using explicitly.

* ssh into the prog machine that you are creating the build from
* In your base Anaconda environment (just confirm no environments have been activated), run `conda clean --all` to clean out your package cache and ensure you pull fresh packages for the build
* Ensure that you have anaconda-client, conda-build, and conda-verify installed in your build environment
  * You can check by running ```anaconda login```, ```conda build -h```, and ```conda verify -h```, respectively.
* From the root of the ISIS3 repo run ```conda build recipe/ -c usgs-astrogeology -c conda-forge --no-test```
  * The -c options are to give conda a channel priority. Without these, conda will always attempt to download from the default Anaconda channel first. (Unlike the environment.yml files, channel priority cannot be defined within the meta.yaml.)
  * Since we do not have testing set-up through conda, the “--no-test” flag must be set in order to avoid errors. (By default, conda looks for a run_test file and will throw an error if it can not be located.)
* Be looking for an "If you want to upload package(s) to anaconda.org later, type:  ..." message towards the end of the build output to confirm good build
  * Make a note of this output. The ```anaconda upload``` command that is displayed will contain the location of your compressed .tar.bz2 file containing your build.

## Upload the Build to Anaconda Cloud

In this step, we will upload the build(s) that we just created into the Anaconda Cloud to distribute them to our users. The location of the .tar.bz2 file to be uploaded should have been displayed at the end of the ```conda build``` command from above. In case you missed this message, you may also run this command to see the location: ```conda build recipe/ --output```. Keep in mind that this does not confirm that the file actually exists - only where it _would_ be saved with a successful build.

Though this step involves only one command, the command will be slightly different depending how you would like to distribute the build and should be repeated for all builds produced.

If this is not a special release, but is instead a standard public release, you will use the command:

```anaconda upload -u usgs-astrogeology <path-to-the-.tar.bz2-file>```

This will upload the build with the "main" label and will be the default build users would download from Anaconda Cloud.

If, however, this is a custom build and/or you do not wish for this build to be the default build downloaded for users, you will need to include a label tag in your upload command. The following example would be for a special CaSSIS build that we would want to give the unique label of "cassis":

```anaconda upload -u usgs-astrogeology -l cassis <path-to-the-.tar.bz2-file>```

Remember to always ensure that custom builds include a label flag or the file will be uploaded by default with the "main" tag on Anaconda Cloud and users may receive a build not intended as a main release by default.

