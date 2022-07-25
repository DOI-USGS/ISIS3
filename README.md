<p align="center">
  <img src="rtd_docs/ISIS_Logo.svg" alt="ISIS" width=200> 
</p>

# ISIS

[![Join the chat at https://gitter.im/USGS-Astrogeology/isis3_cmake](https://badges.gitter.im/USGS-Astrogeology/isis3_cmake.svg)](https://gitter.im/USGS-Astrogeology/isis3_cmake?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Join the discourse at https://astrodiscuss.usgs.gov](https://img.shields.io/discourse/https/astrodiscuss.usgs.gov/topics.svg?style=flat)](https://astrodiscuss.usgs.gov/)
[![Anaconda-Server Badge](https://anaconda.org/usgs-astrogeology/isis3/badges/version.svg)](https://anaconda.org/usgs-astrogeology/isis3)
[![Anaconda-Server Badge](https://anaconda.org/usgs-astrogeology/isis/badges/version.svg)](https://anaconda.org/usgs-astrogeology/isis)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2563341.svg)](https://doi.org/10.5281/zenodo.2563341)

## Table of Contents

* [Requests for Comment](README.md#Requests-for-Comment)
* [FAQ](README.md#FAQ)
* [Installation](README.md#Installation)
* [ISIS Tutorials](README.md#ISIS-Tutorials)
* [Citing ISIS](README.md#Citing-ISIS)
* [Start Contributing](https://github.com/USGS-Astrogeology/ISIS3/wiki/How-to-Start-Contributing)
* [ISIS Data Area](README.md#The-ISIS-Data-Area)
* [ISIS Test Data](README.md#ISIS-Test-Data)
* [Installing Older Versions of ISIS](README.md#Installing-older-versions-of-ISIS)


## Requests for Comment
The ISIS project uses a Request for Comment (RFC) model whereby major potential changes to the code base, data area, or binary delivery process are proposed, iterated on by any interested parties, and potentially adopted. Right now, RFCs are being housed in this repository's [wiki](https://github.com/USGS-Astrogeology/ISIS3/wiki) with associated discussions occurring on [astrodiscuss](https://astrodiscuss.usgs.gov).

Current open RFCs:
  * No Requests for Comment are currently open

  We encourage all contributors and users to review open RFCs and comment as these proposed changes will impact use of the software.

## FAQ
We maintain a list of frequently encountered questions and issues. Before opening a new issue, please take a look at the [FAQ](https://github.com/USGS-Astrogeology/ISIS3/wiki/FAQ).

## Installation

This installation guide is for ISIS users interested in installing ISIS (3.6.0)+ through conda.

### ISIS Installation With Conda

1. Download either the Anaconda or Miniconda installation script for your OS platform. Anaconda is a much larger distribtion of packages supporting scientific python, while Miniconda is a minimal installation and not as large: [Anaconda installer](https://www.anaconda.com/download), [Miniconda installer](https://conda.io/miniconda.html)
1. If you are running on some variant of Linux, open a terminal window in the directory where you downloaded the script, and run the following commands. In this example, we chose to do a full install of Anaconda, and our OS is Linux-based. Your file name may be different depending on your environment.
    
    ```bash
    chmod +x Anaconda3-5.3.0-Linux-x86_64.sh
    ./Anaconda3-5.3.0-Linux-x86_64.sh
    ```
    This will start the Anaconda installer which will guide you through the installation process.

1. If you are running Mac OS X, a pkg file (which looks similar to Anaconda3-5.3.0-MacOSX-x86\_64.pkg) will be downloaded. Double-click on the file to start the installation process.
1. After the installation has finished, open up a bash prompt in your terminal window.
1. Next setup your Anaconda environment for ISIS. In the bash prompt, run the following commands:

    ```bash
    #Create a new conda environment to install ISIS in
    conda create -n isis

    #Activate the environment
    conda activate isis

    #Add the following channels to the environment
    conda config --env --add channels conda-forge
    conda config --env --add channels usgs-astrogeology

    #Verify you have the correct channels:
    conda config --show channels

    #You should see:

    channels:
        - usgs-astrogeology
        - conda-forge
        - defaults

    #The order is important.  If conda-forge is before usgs-astrogeology, you will need to run:

    conda config --env --add channels usgs-astrogeology
    ```

1. The environment is now ready to download ISIS and its dependencies:

    ```bash
    conda install -c usgs-astrogeology isis=7.0.0
    ```

1. Finally, setup the environment variables:

    ISIS requires several environment variables to be set in order to run correctly.
    The variables include: ISISROOT and ISISDATA.

    More information about the ISISDATA environment variable and the ISIS Data Area can be found [here](#The-ISIS-Data-Area).

    The following steps are only valid for versions of ISIS after 4.2.0.
    For older versions of ISIS follow the instructions in [this readme file.](https://github.com/USGS-Astrogeology/ISIS3/blob/adf52de0a04b087411d53f3fe1c9218b06dff92e/README.md)

    There are two methods to configure the environment variables for ISIS:

    1. Using `conda env config vars` *preferred*

       Conda has a built in method for configuring environment variables that are specific to a conda environment since version 4.8.
       This version number applies only to the conda package, not to the version of miniconda or anaconda that was installed.

       To determine if your version of conda is recent enough run:

           conda --version

       If the version number is less than 4.8, update conda to a newer version by running:

           conda update -n base conda

       The version number should now be greater than 4.8.

       To use the built in environment variable configuration feature, first activate the environment by first running:

           conda activate isis

       After activation, the environment variables can be set using the syntax: `conda config vars set KEY=VALUE`.
       To set all the environment variables ISIS requires, run the following command, updating the path to `ISISDATA` as needed:

           conda env config vars set ISISROOT=$CONDA_PREFIX ISISDATA=[path to data directory]

       To make these changes take effect, re-activate the isis environment by running:

           conda activate isis

       The environment variables are now set and ISIS is ready for use every time the isis environment is activated.

       **Note** This method will not enable tab completion for arguments in C-Shell.


    1. Using the provided isisVarInit.py script:

       To use the default values for: `$ISISROOT` and `$ISISDATA`, run the ISIS variable initialization script with default arguments:

           python $CONDA_PREFIX/scripts/isisVarInit.py

       Executing this script with no arguments will result in $ISISROOT=$CONDA\_PREFIX and $ISISDATA=$CONDA\_PREFIX/data. The user can specify different directories for `$ISISDATA` using the optional value:

           python $CONDA_PREFIX/scripts/isisVarInit.py --data-dir=[path to data directory]

       Now every time the isis environment is activated, $ISISROOT and $ISISDATA will be set to the values passed to isisVarInit.py.
       This does not happen retroactively, so re-activate the isis environment with one of the following commands:

           for Anaconda 3.4 and up - conda activate isis
           prior to Anaconda 3.4 - source activate isis


### Installation with Docker
The ISIS production Dockerfile automates the conda installation process above.
You can either build the Dockerfile yourself or use the
[usgsastro/isis](https://hub.docker.com/repository/docker/usgsastro/isis)
image from DockerHub.

#### To build the Dockerfile
1. Download [the production Docker file](./docker/production.dockerfile)
2. Build the Dockerfile
  ```
  docker build -t isis -f production.dockerfile .
  ```
3. Run the Dockerfile
  ```
  docker run -it isis bash
  ```

#### Run run the prebuilt image
```
docker run -it usgsastro/isis bash
```

#### Usage with the ISIS data area
Usually you'll want to mount an external directory containing the ISIS data.
The data is not included in the Docker image.

```
docker run -v /my/data/dir:/opt/conda/data -v /my/testdata/dir:/opt/conda/testData -it usgsastro/isis bash
```

Then [download the data](#the-isis-data-area) into /my/data/dir to make it accessible inside your
container.

### Practical Usage with other conda packages

If you don't use conda for anything else on your computer, you can
skip this section.

If you use conda to install other packages, you may run into
difficulties with adding the isis conda package to those environments
or adding other conda packages to the isis environment you just
created above.  This is because the isis conda package pins a number
of requirements that may clash with other packages.

At this time, we recommend creating the isis environment as detailed
above, and then not adding any other conda packages to it.  This
is similar to the best practice usage of not adding any conda
packages to your 'base' conda environment.

Instead, when you need to have a conda environment with other
packages that also needs to be able to run ISIS programs, we have
two different options.  In both cases, we'll assume that you create
a new environment called 'working' (but it could be named anything)
that you want to add some conda packages to, but from which you
also want ISIS access.

The first step is to create 'working' and add whatever conda packages you want.

#### Easy mode, with stacking

1.  `conda activate isis`

2.  `conda activate --stack working`

That's it.  Told you it was easy.

This activates the isis environment, gets it all set up, and
then it 'stacks' the new working environment on top of it.  To get
out, you'll have to `conda deactivate` two times to get out of
`working` and then out of `isis`.


#### Harder mode, with activation script hacking

The above stacking situation may have issues if you have a particularly
complicated set of packages or other dependencies.  The idea here is that
the only thing you *really* need in your 'working' environment are
the ISIS environment variables and the path to the ISIS executables.

If the above paragraph sounded like gibberish, please seek help
from your system administrator or local computer guru.

And we can do this via customizations in the conda environment's
activate.d/ and deactivate.d/ directories.  Adding these things can
also be done manually from the command line, but encoding them in
the activate.d/ and deactivate.d/ scripts is handy.

1.  Create your conda environment however you like, adding whatever
packages you need.  If you were reading the directions above, you've
already done this.

2.  Locate the path to your conda environments:

        conda activate
        echo $CONDA_PREFIX
        conda deactivate

    You'll probably get a directory that is in your home directory and
    is named `anaconda3` or `miniconda3` or something similar.  For the
    rest of this set of instructions, we'll refer to it as `$HOME/anaconda3`
    to represent a directory named `anaconda3` in your home directory, but
    this should be whatever you get from the above echo command.

3.  Locate the path to your ISIS conda environment:

        conda activate isis
        echo $CONDA_PREFIX
        conda deactivate

    This should probably be `$HOME/anaconda3/envs/isis`.  You can see
    that it starts with whatever you got from step 1, and ends in the
    name of your isis environment, if you followed the installation
    instructions above, you called that environment 'isis'.

    You can do the same thing to find the path to your new 'working'
    environment, but in this example, it will be at
    `$HOME/anaconda3/envs/working`.

4.  Copy the ISIS activation and deactivation scripts to your new
environment.  Please note that the directory names in the instructions
below are based on how you installed conda and what you named the
'isis' environment and the 'working' environment.  You may *not* be
able to just copy and paste these instructions directly, they are
an example.  Likewise, if your shell doesn't take the bash syntax
in the .sh files, then you may need to select one of the other
`env_vars.*` files in the isis directories.

        cd $HOME/anaconda3/envs/
        mkdir -p working/etc/conda/activate.d/
        mkdir -p working/etc/conda/deactivate.d/
        cp isis/etc/conda/activate.d/env_vars.sh working/etc/conda/activate.d/env_vars.sh
        cp isis/etc/conda/deactivate.d/env_vars.sh working/etc/conda/deactivate.d/env_vars.sh

5.  Edit the copied activation file in
`$HOME/anaconda3/envs/working/etc/conda/activate.d/` to add the
ISIS executable directory to the path, by adding this line at the
end:

        export PATH=$PATH:$ISISROOT/bin

    Or whatever is appropriate for your shell if you aren't using
    the .sh file.  No matter how you do it, it is important that
    you add `$ISISROOT/bin` to the end of the current path in your
    working environment, and not at the beginning.


6.  Edit the copied deactivation file in
`$HOME/anaconda3/envs/working/etc/conda/deactivate.d/` to remove
the path, by adding this line at the end:

        export PATH=`echo -n $PATH | awk -v RS=: -v ORS=: '/isis/ {next} {print}' | sed 's/:$//'`;`

    Or whatever is appropriate for your shell if you aren't using the
    .sh file.  If your ISIS environment is not called `isis`, then you
    need to replace that part in the awk line above.  You can look in
    the `activate.d/env_vars.sh` file to see what it should be.

Adding the lines in steps 5 and 6 manually adds the 'bin/'
directory of the ISIS environment to your path (step 5), and then
manually removes it (step 6) on deactivation.  If you are using
some other shell, you may need to use a different syntax to add and
remove these elements to and from your path.



### Updating

  To update to the newest version of ISIS, run `conda update -c usgs-astrogeology isis`

  To update to our latest release candidate , run `conda update -c usgs-astrogeology/label/RC isis`

  Note that for ISIS versions 3.10 and above, new versions and release candidates will only be
  available under the package name `isis` and `conda update isis3` and
  `conda update -c usgs-astrogeology -c usgs-astrogeology/label/RC isis3`
  will not work for additional updates. Instead, after installing an `isis` package,
  `conda update isis` should be used to update to a new version and
  `conda update -c usgs-astrogeology/label/RC isis` to update to a new release candidate.

### Operating System Requirements

ISIS runs on many UNIX variants. ISIS does not run natively on MS Windows, although it has been successfully run on Windows 10 using the Windows Subsystem for Linux (WSL). Instructions for doing this can be found [here](http://planetarygis.blogspot.com/2017/07/isis3-on-windows-10-bash.html). The UNIX variants ISIS has been successfully built on are:

-   Ubuntu 18.04 LTS
-   Mac OS X 10.13.6 High Sierra
-   Fedora 28
-   CentOS 7.2

ISIS may be run on other Linux or macOS operating systems then those listed above, but it has not been tested and is not supported.

### Hardware Requirements

Here are the minimum hardware requirements

-   64-bit (x86) processors
-   2 GB RAM
-   2.5 GB of disk space for ISIS binaries
-   10 GB to 510 GB disk space for ISIS data
-   10 GB to many TB disk space for processing images
-   A quality graphics card

To build and compile ISIS requires following the instructions listed below, which are given on the GitHub wiki page for the ISIS project:

-   [Getting Started With GitHub](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#getting-started-with-github)
-   [Building ISIS With cmake](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3)
-   [New ISIS environmental variables and their meanings](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#new-environmental-variable-meanings)
-   [Custom data and test directories](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#custom-data-and-test-data-directories)
-   [Cleaning builds](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#cleaning-builds)
-   [Building individual applications/objects](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-individual-isis3-applicationsobjects)
-   [Building ISIS documentation](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3-documentation)
-   [What to do if you encounter any problems](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#problems)

## ISIS Tutorials
Please refer to the GitHub wiki page [ISIS Online Workshops](https://github.com/USGS-Astrogeology/ISIS3/wiki/ISIS_Online_Workshops) for current ISIS tutorials.

## Citing ISIS
This project uses a [Zenodo](https://zenodo.org) generated DOI. The badge at the top of this README links to the DOI for the [latest release](https://doi.org/10.5281/zenodo.2563341). It is [good practice](https://help.zenodo.org) (See 'Which DOI Should I Use in Citations?') to cite the version of the software being used by the citing work. To obtain this DOI, one can follow the [link to the latest version](https://doi.org/10.5281/zenodo.2563341) and then check the right sidebar area titled **Versions** for a listing of all ISIS versions that currently have a Zenodo DOI.

## The ISIS Data Area

### Ancillary Data

Many ISIS applications require ancillary data. For example, calibration applications require flat files to do flat field corrections, and map projection applications require DTMs to accurately compute intersections. Due to its size, this data is stored in a separate directory called the ISIS Data Area. Any location can be used for the ISIS Data Area, the software simply requires that the ISISDATA environment variable is set to its location.

### Structure of the ISIS Data Area

Under the root directory of the ISIS Data Area pointed to by the ISISDATA/ISIS3DATA environment variable are a variety of sub-directories. Each mission supported by ISIS has a sub-directory that contains mission specific processing data such as flat files and mission specific SPICE. There are also data areas used by more generic applications. These sub-directories contain everything from templates to test data.

### Versions of the ISIS Data Area

In ISIS version 4.1.0 and later, several files previously stored in the data area closely associated with ISIS applications were moved into version control with the ISIS source code. To support the use of data in ISIS versions predating 4.1.0 the `downloadIsisData` application will need to download the data named `legacybase`. This is explained further in the [Full ISIS Data Download](README.md#Full-ISIS-Data-Download) section. 


### Size of the ISIS Data Area

If you plan to work with data from all missions, then the download will require about 520 GB for all the ancillary data. However, most of this volume is taken up by SPICE files. We have a [Web service](#isis-spice-web-service) that can be used in lieu of downloading all of the SPICE files. This reduces the total download size to about 10 GB.

### Full ISIS Data Download

> Warning if you are looking to download ISIS data via rsync the servers will be shutdown in November of 2022.
the outdated rsync download information can be found [here](https://github.com/USGS-Astrogeology/ISIS3/wiki/Outdated-ISIS-Data-Information)


The ISIS Data Area is hosted on a combination of AWS S3 buckets and public http servers e.g. NAIF, Jaxa, ESA and not through conda channels like the ISIS binaries. This requires using the `downloadIsisData.py` script from within a terminal window within your Unix distribution, or from within WSL if running Windows 10. Downloading all mission data requires over 520 GB of disk space. If you want to acquire only certain mission data [click here](#Mission-Specific-Data-Downloads). To download all ISIS data files, continue reading.

To download all ISIS data, use the following command:

    downloadIsisData  all $ISISDATA

> Note: this applicaion takes in 3 parameters in the following order \<rclone command> \<mission> \<download destination>

> Note: The above command downloads all ISIS data including the required base data area and all of the optional mission data areas.

### Partial Download of ISIS Base Data

This data area contains data that is common between multiple missions such as DEMS and leap second kernels. As of ISIS 4.1, the base data area is no longer required to run many applications as data such as icons and templates has been moved into the binary distribution. If you plan to work with any applications that use camera models (e.g., cam2map, campt, qview), it is still recommended you download the base data area. To download the base data area run the following commands:

    downloadIsisData base $ISISDATA

> Note: For accessing ISIS Data for versions of ISIS prior to ISIS 4.1.0, you must download the `legacybase` area and not the base area when using this application as shown below:

    downloadIsisData legacybase $ISISDATA
### Partial Download of Mission Specific Data

There are many missions supported by ISIS. If you are only working with a few missions then you can save disk space by downloading only those specific data areas. If you want to limit the download even further, read the next section about the SPICE Web Service. Otherwise [jump](#Mission-Specific-Data-Downloads) to the mission specific sections.

### ISIS SPICE Web Service

ISIS can now use a service to retrieve the SPICE data for all instruments ISIS supports via the internet. To use this service instead of your local SPICE data, click the WEB check box in the spiceinit program GUI or type spiceinit web=yes at the command line. Using the ISIS SPICE Web Service will significantly reduce the size of the downloads from our data area. If you want to use this new service, without having to download all the SPICE data, add the following argument to the mission-specific rsync command:

    --exclude='kernels'

For example:

<pre>
cd $ISISDATA
rsync -azv <b>--exclude='kernels'</b> --delete --partial isisdist.astrogeology.usgs.gov::isisdata/data/cassini .
</pre>

**WARNING:** Some instruments require mission data to be present for radiometric calibration, which is not supported by the SPICE Web Server, and some programs that are designed to run an image from ingestion through the mapping phase do not have an option to use the SPICE Web Service. For information specific to an instrument, see the documentation for radiometric calibration programs.

### Mission Specific Data Downloads

For versions of ISIS prior to ISIS 4.1.0, please use the `--legacy` flag

| Mission | Command |
| ------ | ------ |
| Apollo 15 | `downloadIsisData apollo15 $ISISDATA` |
| Apollo 16 | `downloadIsisData apollo16 $ISISDATA` |
| Apollo 17 | `downloadIsisData apollo17 $ISISDATA` |
| Cassini | `downloadIsisData cassini $ISISDATA` | 
| Chandrayaan 1 | `downloadIsisData chandrayaan1 $ISISDATA` |
| Clementine 1 | `downloadIsisData clementine1 $ISISDATA` |
| Dawn | `downloadIsisData dawn $ISISDATA` |
| ExoMars | `downloadIsisData tgo $ISISDATA` |
| Galileo | `downloadIsisData galileo $ISISDATA` | 
| Hayabusa 2 | `downloadIsisData hayabusa2 $ISISDATA` |
| Juno | `downloadIsisData juno $ISISDATA` |
| Kaguya | `downloadIsisData kaguya $ISISDATA` |
| Lunar Orbiter | `downloadIsisData lo $ISISDATA` |
| Lunar Reconnaissance Orbiter | `downloadIsisData lro $ISISDATA` |
| Mars Exploration Rover  | `downloadIsisData mer $ISISDATA` |
| Mariner10  | `downloadIsisData mariner10 $ISISDATA` |
| Messenger | `downloadIsisData messenger $ISISDATA` |
| Mars Express  | `downloadIsisData mex $ISISDATA` |
| Mars Global Surveyor  | `downloadIsisData mgs $ISISDATA` |
| Mars Reconnaissance Orbiter  | `downloadIsisData mro $ISISDATA` |
| Mars Odyssey  | `downloadIsisData odyssey $ISISDATA` |
| Near  | `downloadIsisData near $ISISDATA` |
| New Horizons  | `downloadIsisData newhorizons $ISISDATA` |
| OSIRIS-REx  | `downloadIsisData osirisrex $ISISDATA` |
| Rolo  | `downloadIsisData rolo $ISISDATA` |
| Rosetta  | `downloadIsisData rosetta $ISISDATA` |
| Smart1  | `downloadIsisData smart1 $ISISDATA` |
| Viking 1 | `downloadIsisData viking1 $ISISDATA` |
| Viking 2 | `downloadIsisData viking2 $ISISDATA` |
| Voyager 1 | `downloadIsisData voyager1 $ISISDATA` |
| Voyager 2 | `downloadIsisData voyager2 $ISISDATA` |

### ISIS Test Data 
ISIS is comprised of two types of tests, custom Makefile based tests, and GTest based tests. Those that are GTest based, make economical use of data that exists on the ISIS3 repo along with the source, so no special data is required to run those other than the ISIS data area. The Makefile tests depend on a separate source of data that consists of a few gigabytes of input and expected output data used for testing ISIS applications. The Makefile based tests use the `ISISTESTDATA` environment variable to know where the required data are located. The total size of this test data decreases as we work towards converting Makefile tests to GTests.  
 
###How to download the ISIS test data with rclone  
Test data is hosted using Amazon S3 storage buckets. We recommend using rclone to pull the data into a local directory. You can download rclone using their instructions (see: https://rclone.org/downloads/) or by using an anaconda environment (see: https://docs.anaconda.com/anaconda/install/). If you already have an anaconda environment up, install rclone with: `conda install –c conda-forge rclone` 

Next, you will want to configure rclone using a default S3 configuration. See: https://rclone.org/s3/ for detailed information on how to configure S3, but for the purposes of downloading the ISIS3 test data, you simply run rclone config which will start an interactive menu. Press enter through it all except for these details: 

1. Set S3 as both your storage type and storage provider
2. Set us-west-2 as both your region to connect to and as the location constraint. 
3. Everything else, just leave as the default. 

Example output: https://gist.github.com/Kelvinrr/706bbd54b1c2c30d0ce3d12f7dcaa10a

Once rclone is configured, simply run: `rclone sync remote:asc-isisdata/isis_testData/ $ISISTESTDATA`
where:
  - $ISISTESTDATA is the environment variable defining the location of the ISISTESTDATA
  - remote: is the name of the configuration you created earlier. This can be whatever you want to name it, in this case it is named remote. 
  - asc-isisdata/isis_testData/ is the name of the S3 bucket you’re downloading from

$ISISTESTDATA should now contain a full clone of the ISIS test data for running Makefile based tests. 

Notes:
  - Users can download specific files from the bucket by adding path data or file information to the first argument, that is, to download only the ‘base’ folder from the isis_testData bucket, the user could call:
rclone sync remote:asc-isisdata/isis_testData/base
  - It is important that users understand the difference in rclone’s ‘sync’ and ‘copy’ methods.  ‘copy’ will overwrite all data in the destination with data from source.  ‘sync’ replaces only changed data.
  - Syncing / copying in either direction (local -> remote or remote -> local) results in any changed data being overwritten.  There is no warning message on overwrite.

## Installing older versions of ISIS
---------------------------------

### How do I install ISIS2?

If you are looking for ISIS2, please [refer to the ISIS 2 Installation Guide](http://isis.astrogeology.usgs.gov/Isis2/isis-bin/installation.cgi) for instructions on downloading and installing ISIS 2.

### How do I install ISIS3.5.2 or earlier?

If you are looking for a version of ISIS prior to 3.6.0, please [refer to the Legacy ISIS3 Installation Guide](https://isis.astrogeology.usgs.gov/documents/LegacyInstallGuide/index.html) for instructions on downloading and installing ISIS, versions prior to 3.6.0

### How do I access the ISISDATA download script with ISIS 7.0.0 or earlier 

You can download the script and config file from the repo:

```
# install rclone 
conda install -c conda-forge rclone

# download the script and rclone config file
curl https://github.com/USGS-Astrogeology/ISIS3/raw/dev/isis/scripts/downloadIsisData.py -o downloadIsisData.py

curl https://github.com/USGS-Astrogeology/ISIS3/raw/dev/isis/config/rclone.conf -o rclone.conf

# run the script as normal, using --config to point to where you downloaded the config file 
python downloadIsisData.py sync <mission> $ISISDATA --config rclone.conf

```
