# Step-By-Step Instructions

This document describes the process of building ISIS for a release and pushing the build to Anaconda Cloud.

## Set Up the Local and Remote Repositories

In this step, we will carefully prepare the local repository to build from as well as update the remote repository hosted on GitHub. Keep in mind that you will be building from this repo on other systems and plan accordingly by cloning this repo into a directory that you will still have access to.

* Clone a fresh copy of the ISIS3 repository from GitHub
* Update the recipes/meta.yaml file to include proper version number, branch, and build number
  * The version should be the version of ISIS you are building.
  * The branch should be the branch from the remote repository that you would like to build from. This will usually be the "release" branch for public builds, but it is also possible to do special builds, for example for missions that require a specific build.
  * The build number should be incremented for each build produced for a certain version number, for example when bug fixes are released.
* Update the isis/version file to reflect the proper version number
* Cherry-pick in all changes that need to go into the release 
* Do a manual build and run the tests for all systems
  * This includes Linux 28 (prog29), Ubuntu 18.4 (prog28) and MacOS 10.13 (prog27).
* Once tests are passing, push the changes back up to the release branch
* Make a github release and tag for the build 
  * The release name should be the same as the version name. 
  * Mission and non-standard builds must be tagged as pre-release.
  * Mission release naming convention: version XX.YY.ZZ_mission (ex. 3.6.1_cassis)

## Create the Build for Anaconda Cloud

In this step, we will create the build(s) for Anaconda Cloud. Keep in mind that there will usually be two default public builds: one for Linux (built on prog28), and one for Mac (built on prog27). Repeat this and the upload process process for each necessary system.

* ssh into the prog machine that you are creating the build from
* In your base Anaconda environment (just confirm no environments have been activated), run `conda clean --all` to clean out your package cache and ensure you pull fresh packages for the build
* Ensure that you have anaconda-client, conda-build, and conda-verify installed in your build environment
  * You can check by running ```anaconda login```, ```conda build -h```, and ```conda verify -h```, respectively.
* From the root of the ISIS3 repo run ```conda build recipe/ -c usgs-astrogeology -c conda-forge --no-test```
  * The -c options are to give conda a channel priority. Without these, conda will always attempt to download from the default anaconda channel first. (Unlike the environment.yml files, channel priority cannot be defined within the meta.yaml.)
  * Since we do not have testing set-up through conda, the “--no-test” flag must be set in order to avoid errors. (By default, conda looks for a run_test file and will throw an error if it can not be located.)
* Be looking for a "If you want to upload package(s) to anaconda.org later, type:  ..." message towards the end of the build output to confirm good build
  * Make a note of this output. The ```anaconda upload``` command that is displayed will contain the location of your compressed .tar.bz2 file containing your build.

## Upload the Build to Anaconda Cloud

In this step, we will upload the builds that we just created into the Anaconda Cloud to distribute them to our users. The location of the .tar.bz2 file to be uploaded should have been displayed at the end of the ```conda build``` command from above. Though this step involves only one command, the command will be slightly different depending how you would like to distribute the build and should be repeated for all builds produced.

If this is not a special release, but is instead a standard public release, you will use the command:

```anaconda upload -u usgs-astrogeology <path-to-the-.tar.bz2-file>```

This will upload the build with the "main" label and will be the default build users would download from Anaconda Cloud.

If, however, this build has been specially produced and/or you do not wish for this build to be the default build downloaded for users, you will need to include a label tag in your upload command. The following example would be for a special CaSSIS build that we would want to give the unique label of "cassis":

```anaconda upload -u usgs-astrogeology -l cassis <path-to-the-.tar.bz2-file>```

Remember to always ensure that special builds include a label flag or you may inadvertently overwrite the main label build on Anaconda Cloud and users may receive a build not intended as a main release.

