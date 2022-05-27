This document explains the release process for Official Releases, Release Candidates and custom builds (for example: missions). 

  

# Step-By-Step Instructions 

  

## Step 1: Check current false positive test failures 


In this step, we check the currently failing tests. This is currently a judgement call on whether or not the tests are false-positives. This decision should be made by 2+ developers on the development team. 



## Step 2: Update the Github documents

**This step is only required for release candidates and bug fix release. If you are creating a full feature release, skip this step.**

In this step we will update the documents that are stored in the Github repository. Our changes will be going into the dev branch so create a fresh local branch off of the dev branch.


### Part A: Collecting the Changes in the Release

* Update the Changelog 
  * For release candidates we need to update the Changelog to label all of the currently unreleased changes as part of this release. Follow the instructions in [CHANGELOG.md](https://raw.githubusercontent.com/USGS-Astrogeology/ISIS3/dev/CHANGELOG.md) for how to do this.
  * For bug fix releases we need to update the Changelog to label **only the bug fixes** as part of this release. Follow the instructions in [CHANGELOG.md](https://raw.githubusercontent.com/USGS-Astrogeology/ISIS3/dev/CHANGELOG.md) for how to do this.
* Update code.json with the new version number and the date last modified.
  

### Part B: Update the Authors List 

* If there are any new contributors to the project since the last release ensure that they are added to the [.zenodo.json](https://github.com/USGS-Astrogeology/ISIS3/blob/dev/.zenodo.json) document, and update the `AUTHORS.rst` file from the .zenodo.json file by running `python $ISISROOT/scripts/zenodo_to_authors.py <path_to_your_clone>/.zenodo.json <path_to_your_clone>/AUTHORS.rst`.


### Part C: Submit a Pull Request

* Submit a pull request into the dev branch with your modifications to the Changelog and the Authors list.
* For a bug fix release you will also need to [cherry-pick](https://www.atlassian.com/git/tutorials/cherry-pick) the squashed commit from your pull request into the version branch. If you run into merge conflicts it may be easier to simply redo the above steps with the version branch instead of dev.


## Step 3: Set Up the Local and Remote Repositories 

In this step, we will prepare the local repository to build from as well as update the remote repository hosted on GitHub. Keep in mind that you will be building from this repo on other systems and plan accordingly by cloning this repo into a directory that you will still have access to as you switch between the machines. 


### Part A: Setup Repository 

* Create a fresh branch off of the appropriate version branch. Branches are created for each minor (i.e. 3.x or 4.x) version of ISIS, and each then specific release is associated with a minor version (i.e. 3.x.x or 4.x.x) tag on that version branch. 

    * For releases, there should already be a branch for this version created during the release candidate process. If there is not already a branch for this version, you will need to create a branch for this release off of `dev`. If you are doing a release for ISIS 3.10, the git command to create a branch and checkout to it is `git checkout -b 3.10 upstream/dev`. If there is already a version branch: `git checkout -b 3.10 upstream/3.10` 

    * For release candidates, there may or may not be a branch for the version. If there is not, create a branch off of `dev`. If there is a version branch: `git checkout -b 3.10 upstream/3.10`.


### Part B: Update isis/CMakeLists.txt 

* Update the VERSION variable to the latest version number. NOTE: Do not add the _RC# 

* Update the RELEASE_STAGE variable: 

    * 'stable' is for full releases. 

    * 'beta' is for RC's. 

    * 'alpha' is for developer release or custom mission builds. 

  

### Part C: Update recipe/meta.yaml 

* Update the version variable to the version of ISIS you are building. 

    * If you are building a standard release, use the same version number as in [Part B](#part-b-update-isiscmakeliststxt). 

    * If you are building a release candidate, include "_RCx".  

        * For the first ISIS3.6.1 release candidate, it would be: "3.6.1_RC1". 

    * If you are creating a custom build, include a unique tag.  

        * For a custom ISIS3.6.1 CaSSIS build, it would be: "3.6.1_cassis1". 

* Ensure the build_number is set to 0. 

  * The build number should be incremented for each build produced at the **same version** of source code and should always begin at 0 for each release version.  

  * ****Please note that this step is important as this is how the file to be uploaded to Anaconda Cloud is named by conda build. If a file with the same name already exists on USGS-Astrogeology channel in Anaconda Cloud, it will be overwritten with the new upload.**** 

* Update the `build` section by copying the current contents of `environment.yaml` into the `build` section. Update the `run` section to include any new packages and remove any packages that are no longer needed. 

  

### Part D: Create a Pull Request

* Make a pull request with your local changes into the version (i.e., the version number created above) branch of the repository. 

  

<!--- 
   * To create a new branch, first prepare your local repo by pulling down the merged changes you made earlier (e.g. `git pull upstream dev`) 

   * Next, create a branch with the appropriate name for the release version in the format `<major version>.<minor verson>`, by running for example: `git branch -b 3.10`. After creation, this branch can be pushed directly to upstream. (`git push upstream 3.10`.)--->

  

### Part E: Make Github Release 

* Draft a new github release.  

  * Tag the release.  

      * Make sure to change the target from `dev` to the appropriate version branch for your release. 

      * The release "Tag version" must be the \<version\> from the meta.yaml file you modified in [Part C](#part-c-update-recipemetayaml). 

      * Mission and non-standard builds (including release candidates) must be tagged as pre-release. 

  * Name the release. 

      * If it is a standard release, name it "ISISX.Y.Z Public Release". 

      * If it is a release candidate, name it "ISISX.Y.Z Release Candidate N". 

  * Describe the release. 

      * If it is a standard release, description should state "The official release for ISIS version X.Y.Z". 

      * If it is a release candidate, description should state "[First/Second/Third/...] release candidate for ISIS X.Y.Z". 

  

## Step 4: Create the Builds for Anaconda Cloud 


In this step, we will create the build(s) for Anaconda Cloud using the conda-build system. There are two builds for standard releases: one for Linux (built on Ubuntu 18 LTS), and one for Mac (built on Mac OS 10.14). Missions may only need one build and not the other. Communicate with your team as to what they need. Repeat this and the upload process process for each necessary system. 

Anaconda leverages caching in many places which can cause issues. If you are getting unexpected issues, try a different installation and/or a different machine. 


### Part A: Operating System 

* Ensure the OS on the machine you are building the release on is the appropriate operating system (Mac OS 11.6 or Ubuntu 18 LTS). 

    * If you do not have access to a Ubuntu 18 LTS, you can ssh into prog28. 


### Part B: Setup Anaconda 

* Run `conda clean --all` to clean out your package cache and ensure you pull fresh packages for the build. 

* Activate the base environment: ```conda activate```. 

* Ensure that you have anaconda-client, conda-build, and conda-verify installed in your base environment 

  * You can check by running ```anaconda login```, ```conda build -h```, and ```conda-verify --help```, respectively. 

    * If any of these packages are not in your base environment, they can be installed using the following  

commands: 

`conda install anaconda-client` 

`conda install -c anaconda conda-build`  

`conda install -c anaconda conda-verify` 

    * If running ```anaconda login``` resulted in an error message similar to ```[ERROR] API server not found. Please check your API url configuration``` run the following command and try again: ```anaconda config --set ssl_verify False```  


### Part C: Run the Build 

* Go to the root of the repository you set up in [Step 3 Part A](#part-a-setup-repository). Make sure it is up to date. 

    * Switch to the appropriate version branch 

```git checkout <version branch>``` 

    * Ensure you are at the head of the release branch ```git pull upstream <version branch>``` 

* Run the bash script in the root of the directory: ``` buildCondaRelease.sh```. This script will remove a CMake parameter that is used for jenkins and create a conda build running the conda build command:```conda build recipe/ -c conda-forge -c usgs-astrogeology --override-channels``` 

  * The -c options are to give conda a channel priority. Without these, conda will always attempt to download from the default Anaconda channel first. (Unlike the environment.yml files, channel priority cannot be defined within the meta.yaml.)

  

## Step 5: Test the Conda Build 


After the conda build completes, it should be tested by uploading it to your personal anaconda cloud account and conda installing it locally.  

* Use the command ```anaconda upload -u <conda-cloud-username> <path-to-the-.tar.bz2-file>``` to upload the conda build to your personal anaconda channel. 

* Follow the standard [installation instructions](https://github.com/USGS-Astrogeology/ISIS3#isis3-installation-with-conda) to install this package locally for testing, but at the installation step, instead of running `conda install -c usgs-astrogeology isis`, run `conda install -c <conda-cloud-username> -c usgs-astrogeology isis`  

* Run an ISIS application, like `spiceinit -h` and insure that it runs without error. This is testing whether the conda environment set up during the install is complete enough to run ISIS.   


## Step 6: Upload the Build to Anaconda Cloud 


In this step, we will upload the build(s) that we just created into the Anaconda Cloud to distribute them to our users. Uploading the .tar.bz2 file requires one command, however, non-standard builds (release candidates or custom builds), must be uploaded with a label.  

* If you missed the location of the .tar.bz2 file at the bottom of the build, use ```conda build recipe/ --output``` to reprint. This command does not confirm the file exists - only where it *would* be saved with a successful build. 

* For a standard release, use the command ```anaconda upload -u usgs-astrogeology <path-to-the-.tar.bz2-file>``` 

* For an Release Candidate, use the command ```anaconda upload -u usgs-astrogeology -l RC <path-to-the-.tar.bz2-file>``` 

* For a custom build, specify a custom label and use the command ```anaconda upload -u usgs-astrogeology -l <custom-label> <path-to-the-.tar.bz2-file>``` 

   * For example, when generating a custom build for the CaSSIS team, we would use the "cassis" label and the command ```anaconda upload -u usgs-astrogeology -l cassis <path-to-the-.tar.bz2-file>``` 

If the upload fails or displays a prompt for a username and password, try adding an API token for usgs-astrogeology to your environment by running `export ANACONDA_API_TOKEN=<token>`. Ask another developer for the API token. This approach is recommended over adding `-t <token>` to your anaconda upload command, because of a known bug where `-t` is either interpreted as a package type or a token depending on its position in the `anaconda upload` command.  


## Step 7: Back up the Build  

Back up the build by copying the .tar.bz2 to: 

  * /work/projects/conda-bld/osx-64/ for Mac OS 11.6.  

  * /work/projects/conda-bld/linux-64/ for Ubuntu 18 LTS. 


## Step 8: Update Data and TestData Areas on rsync Servers 


This step covers how to update the data on the rysnc servers. This is where our external users will have access to the data necessary for running ISIS. One server is located on campus, while the other server is located in Phoenix. These commands must be run as isis3mgr for permission purposes. 

**Please pay careful attention to where you are rsync'ing the data to on the remote servers. It is going to depend on the type of build you just completed (Public Release, Release Candidate, custom build, etc).** 


### Part A: Update the Local Server 

* Conduct a dry run using the command ```rsync -rtpvln /usgs/cpkgs/isis3/isis_data/ isisdist:/work1/dist/isis3/<isisdata or mission specific>/data/``` and ensure that the output is reasonable. You should see kernel updates for active missions and a smaller number of other updates made by developers. 

* Actually copy the files using ```rsync -rtpvl /usgs/cpkgs/isis3/isis_data/ isisdist:/work1/dist/isis3/<isisdata or mission specific>/data/```. 


### Part B: Update the Remote Server 

* Conduct a dry run using the command ```rsync -rtpvln /usgs/cpkgs/isis3/isis_data/ isisdist.astrogeology.usgs.gov:/work1/dist/isis3/<isisdata or mission specific>/data/```. 

* Actually copy the files using ```rsync -rtpvl /usgs/cpkgs/isis3/isis_data/ isisdist.astrogeology.usgs.gov:/work1/dist/isis3/<isisdata or mission specific>/data/```. 


## Step 9: Create Internal Builds/Installs for Astro 


This step covers creating the builds and the installation environments of ISIS for our internal users here on the ASC campus using the shared anaconda installs. Setting up the conda environments involve installing the conda build of ISIS that we just pushed up to Anaconda, and will follow the instructions found in the README.MD of the isis3 repository. These commands must be run as isis3mgr for permission purposes. 


### Part A: Shared Anaconda Installs 

* You will need to install the new version of ISIS into the two shared Anaconda installs on the ASC campus. 

    * For Linux: `/usgs/cpkgs/anaconda3_linux` 

    * For MacOS: `/usgs/cpkgs/anaconda3_macOS` 


### Part B: Installing ISIS 

* Follow the standard [installation instructions](https://github.com/USGS-Astrogeology/ISIS3#isis3-installation-with-conda) to install the latest version of ISIS into a new environment. 

    * For a standard release, the environment should be named `isisX.Y.Z`. 

    * For a release candidate, the environment should be named `isisX.Y.Z-RC#`. 

    * For a custom build, the environment should be named `isisX.Y.Z-<custom-label>`. 

    * For the step which sets up the data and testData areas, make sure to use the new isis_data and isis_testData directories, i.e.: `python $CONDA_PREFIX/scripts/isisVarInit.py --data-dir=/usgs/cpkgs/isis3/isis_data  --test-dir=/usgs/cpkgs/isis3/isis_testData` 

* Confirm that the environment has been set-up properly by deactivating it, reactivating it, and running an application of your choice. 


## Step 10: Update Documentation 


**This step is only done for standard feature releases.** 

This step will update the ISIS documentation on our [website](https://isis.astrogeology.usgs.gov/UserDocs/) for our users worldwide. These commands must be run as isis3mgr for permission purposes. 


### Part A: Build the documentation 


* Perform a local build (not a conda build) using the instructions available [here](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake) 

* setisis to the build directory from [Step 3 Part A](#part-a-setup-repository). 

* Run the ```ninja docs``` command from this build directory to build the documentation for this version of the code. 


### Part B: Upload the documentation

This step requires that you have an rclone config for the `asc-docs` bucket. You can get credentials from vault.

In the `$ISISROOT/docs` directory run the following commands, but replace <your-config> with your config for `asc-docs` and <version-number> with the version of ISIS you are releasing. For example if you config is called `s3-docs` and you are releasing 8.1.0, the first command would be `rclone sync --dry-run docs/8.1.0 s3-docs://asc-docs/isis-site/8.1.0/

* Optionally add the `--dry-run` flag to test prior to actually uploading, `rclone sync docs/<version-number> <your-config>://asc-docs/isis-site/<version-number>/`
* `rclone copy docs/index.html <your-config>://asc-docs/isis-site/`


## Step 11: Communicate Availability of Build 


This step will will communicate that a new version of ISIS is available. 


### Part A: External Announcement 

* Create a new topic under the [ISIS Release Notes](https://astrodiscuss.usgs.gov/c/ISIS/isis-release-notes) category on [astrodiscuss](https://astrodiscuss.usgs.gov/). 

* Fill in the following template for the release notes post: 

``` 

## How to install or update to <X.Y.Z> 


Installation instructions of ISIS can be found in the README on our [github page ](https://github.com/USGS-Astrogeology/ISIS3). 


If you already have a version of ISIS 4.0.0 or later installed in an anaconda environment, you can update to <X.Y.Z> by activating your existing isis conda environment and running `conda update isis` . 


### How to get access to <X.Y.Z> at the ASC 


The new process proposed in the internal [RFC](https://astrodiscuss.usgs.gov/t/internal-rfc-distribution-of-isis3-at-asc/52/26) is now in full effect. Please review the process of using anaconda environments to activate isis [here](https://astrodiscuss.usgs.gov/t/using-the-asc-conda-environment-for-isisx-y-z/106). 


Once a version of conda is active, run the command: `conda activate isis<X.Y.Z>` to use this newest version of ISIS. 


## Changes for <X.Y.Z> 

<!---
Copy this release's section of the Changelog here
-->


## Notes 


The following operating systems are supported for this release: 


* Ubuntu 18.04 
* macOS Mohave 11.6

(Other Linux/macOS variants may be able to run this release, but are not officially supported.) 


If you find a problem with this release, please create an issue on our [github issues page](https://github.com/USGS-Astrogeology/ISIS3/issues/new/choose/) 

``` 

* For a Release Candidate, add the following under "Notes":  


``` 

There are some important considerations to keep in mind when using this release candidate: 


* Do not use this version for production work. A stable isisX.XX.XX release will be uploaded after a month. 
* The ISIS online documentation will not be updated until the stable release is announced. 

``` 


### Part B: Internal Announcement 


* Send an email to all of astro (GS-G-AZflg Astro <gs-g-azflg_astro@usgs.gov>) informing them of internal availability. 

    * Your e-mail can simply be a link to the external announcement. 


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