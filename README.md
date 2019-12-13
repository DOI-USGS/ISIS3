# ISIS3

[![Join the chat at https://gitter.im/USGS-Astrogeology/isis3_cmake](https://badges.gitter.im/USGS-Astrogeology/isis3_cmake.svg)](https://gitter.im/USGS-Astrogeology/isis3_cmake?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Join the discourse at https://astrodiscuss.usgs.gov](https://img.shields.io/discourse/https/astrodiscuss.usgs.gov/topics.svg?style=flat)](https://astrodiscuss.usgs.gov/)
[![Anaconda-Server Badge](https://anaconda.org/usgs-astrogeology/isis3/badges/version.svg)](https://anaconda.org/usgs-astrogeology/isis3)

## Table of Contents

* [FAQ](README.md#FAQ)
* [Installation](README.md#Installation)
* [Start Contributing](https://github.com/USGS-Astrogeology/ISIS3/wiki/How-to-Start-Contributing)
* [ISIS Data Area](README.md#The-ISIS-Data-Area)
* [Installing Older Versions of ISIS](README.md#Installing-older-versions-of-ISIS)

## FAQ
We maintain a list of frequently encountered questions and issues. Before opening a new issue, please take a look at the [FAQ](https://github.com/USGS-Astrogeology/ISIS3/wiki/FAQ).

## Installation

This installation guide is for ISIS3 users interested in installing ISIS3 (3.6.0)+ through conda.

### ISIS3 Installation With Conda

1.  Download either the Anaconda or Miniconda installation script for your OS platform. Anaconda is a much larger distribtion of packages supporting scientific python, while Miniconda is a minimal installation and not as large: [Anaconda installer](https://www.anaconda.com/download), [Miniconda installer](https://conda.io/miniconda.html)
2.  If you are running on some variant of Linux, open a terminal window in the directory where you downloaded the script, and run the following commands. In this example, we chose to do a full install of Anaconda, and our OS is Linux-based. Your file name may be different depending on your environment.

        chmod +x Anaconda3-5.2.0-Linux-x86_64.sh
        ./Anaconda3-5.2.0-Linux-x86_64.sh


    This will start the Anaconda installer which will guide you through the installation process.

3.  If you are running Mac OS X, a pkg file (which looks similar to Anaconda3-5.3.0-MacOSX-x86\_64.pkg) will be downloaded. Double-click on the file to start the installation process.
4.  After the installation has finished, open up a bash prompt in your terminal window.
5.  Next setup your Anaconda environment for ISIS3. In the bash prompt, run the following commands:

        #Create a new conda environment to install ISIS3 in
        conda create -n isis3 python=3.6

        #Activate the environment
        #Depending on your version of Anaconda use one of the following:

        #Anaconda 3.4 and up:
        conda activate isis3

        #Prior to Anaconda 3.4:
        source activate isis3

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


6.  The environment is now ready to download ISIS3 and its dependencies:

        conda install -c usgs-astrogeology isis3

    If you would like to work with our latest Release Candidate instead run:

        conda install -c usgs-astrogeology/label/RC isis3

7.  Finally, setup the environment variables:

        #Execute the ISIS3 variable initialization script with default arguments.
        #This script prepares default values for:  $ISISROOT/$ISIS3DATA/$ISIS3TESTDATA

        python $CONDA_PREFIX/scripts/isis3VarInit.py


    Executing this script with no arguments will result in $ISIS3DATA=$CONDA\_PREFIX/data, and $ISIS3TESTDATA=$CONDA\_PREFIX/testdata. The user can specify different directories for both of these optional values:

        python $CONDA_PREFIX/scripts/isis3VarInit.py --data-dir=[path to data directory]  --test-dir=[path to test data directory]


    More information about the ISIS3DATA environment variable and the ISIS3 Data Area can be found [here]("#The-ISIS3-Data-Area"). Now everytime the isis3 environment is activated, $ISISROOT, $ISIS3DATA, and $ISIS3TESTDATA will be set to the values passed to isis3VarInit.py. This does not happen retroactively, re-activate the isis3 envionment with one of the following commands:

        for Anaconda 3.4 and up - conda activate isis3
        prior to Anaconda 3.4 - source activate isis3

### Updating

  To update to a new version of ISIS, simply run `conda update isis3`

  To update to our latest release candidate, run `conda update -c usgs-astrogeology/label/RC isis3`

### Operating System Requirements

ISIS3 runs on many UNIX variants. ISIS does not run natively on MS Windows, although it has been successfully run on Windows 10 using the Windows Subsystem for Linux (WSL). Instructions for doing this can be found [here](http://planetarygis.blogspot.com/2017/07/isis3-on-windows-10-bash.html). The UNIX variants ISIS3 has been successfully built on are:

-   Ubuntu 18.04 LTS
-   Mac OS X 10.13.6 High Sierra
-   Fedora 28
-   CentOS 7.2

ISIS3 may be run on other Linux or macOS operating systems then those listed above, but it has not been tested and is not supported.

### Hardware Requirements

Here are the minimum hardware requirements

-   64-bit (x86) processors
-   2 GB RAM
-   2.5 GB of disk space for ISIS3 binaries
-   10 GB to 510 GB disk space for ISIS3 data
-   10 GB to many TB disk space for processing images
-   A quality graphics card

To build and compile ISIS3 requires following the instructions listed below, which are given on the GitHub wiki page for the ISIS3 project:

-   [Getting Started With GitHub](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#getting-started-with-github)
-   [Building ISIS3 With cmake](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3)
-   [New ISIS3 environmental variables and their meanings](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#new-environmental-variable-meanings)
-   [Custom data and test directories](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#custom-data-and-test-data-directories)
-   [Cleaning builds](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#cleaning-builds)
-   [Building individual applications/objects](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-individual-isis3-applicationsobjects)
-   [Building ISIS3 documentation](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3-documentation)
-   [What to do if you encounter any problems](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#problems)

## The ISIS Data Area

### Ancillary Data

Many ISIS3 applications require ancillary data. For example, ingestion applications require translation tables to convert labels, calibration applications require flat files to do flat field correct, and map projection applications require DTMs to accurately compute intersections. Due to its size, this data is stored in a separate directory called the ISIS3 Data Area. Any location can be used for the ISIS3 Data Area, the software simply requires that the ISIS3DATA environment variable is set to its location.

### Structure of the ISIS3 Data Area

Under the root directory of the ISIS3 Data Area pointed to by the ISIS3DATA environment variable are a variety of sub-directories. Each mission supported by ISIS3 has a sub-directory that contains mission specific processing data such as flat files and mission specific SPICE. There are also data areas used by more generic applications. These sub-directories contain everything from templates to test data.

### Size of the ISIS3 Data Area

If you plan to work with data from all missions, then the download will require about 520 GB for all the ancillary data. However, most of this volume is taken up by SPICE files. We have a [Web service](#isis-spice-web-service) that can be used in lieu of downloading all of the SPICE files. This reduces the total download size to about 10 GB.

### Full ISIS3 Data Download

The ISIS3 Data Area is hosted on rsync servers and not through conda channels like the ISIS3 binaries. This requires using the rsync command from within a terminal window within your Unix distribution, or from within WSL if running Windows 10.  Downloading all mission data requires over 520 GB of disk space. If you want to acquire only certain mission data [click here](#Mission-Specific-Data-Downloads). To download all ISIS3 data files, continue reading.

To download all ISIS3 data, enter the following commands in the location where you want to install the ISIS3 Data Area:

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data.


> Note: The above command downloads all ISIS data including the required base data area and all of the optional mission data areas.


### Partial Download of ISIS3 Base Data (Required)

The base data area is separate from the source code. This data area is crucial to ISIS3 and must be downloaded. To do that run the following commands:

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/base .

### Partial Download of Mission Specific Data

There are many missions supported by ISIS. If you are only working with a few missions then you can save disk space by downloading only those specific data areas. If you want to limit the download even further, read the next section about the SPICE Web Service. Otherwise [jump](#Mission-Specific-Data-Downloads) to the mission specific sections.

### ISIS SPICE Web Service

ISIS can now use a service to retrieve the SPICE data for all instruments ISIS supports via the internet. To use this service instead of your local SPICE data, click the WEB check box in the spiceinit program GUI or type spiceinit web=yes at the command line. Using the ISIS SPICE Web Service will significantly reduce the size of the downloads from our data area. If you want to use this new service, without having to download all the SPICE data, add the following argument to the mission-specific rsync command:

    --exclude='kernels'

For example:

<pre>
cd $ISIS3DATA
rsync -azv <b>--exclude='kernels'</b> --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/cassini .
</pre>

**WARNING:** Some instruments require mission data to be present for radiometric calibration, which is not supported by the SPICE Web Server, and some programs that are designed to run an image from ingestion through the mapping phase do not have an option to use the SPICE Web Service. For information specific to an instrument, see the documentation for radiometric calibration programs.

### Mission Specific Data Downloads

**Apollo Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo15 .
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo16 .
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo17 .


**Cassini Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/cassini .


**Chandrayaan Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/chandrayaan1 .


**Clementine Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/clementine1 .


**Dawn Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/dawn .


**ExoMars Trace Gas Orbiter Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/tgo .


**Galileo Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/galileo .


**Hayabusa Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/hayabusa .
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/hayabusa2 .


**Juno Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/juno .


**Kaguya Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/kaguya .


**Lunar Orbiter Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/lo .


**Lunar Reconnaissance Orbiter Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/lro .


**Mars Exploration Rover Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mer .


**Mariner10 Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mariner10 .


**Messenger Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/messenger .


**Mars Express Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mex .


**Mars Global Surveyor Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mgs .


**Mars Reconnaissance Orbiter Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mro .


**Mars Odyssey Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/odyssey .


**Near Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/near .


**New Horizons Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/newhorizons .


**Rolo Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/rolo .


**Rosetta Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/rosetta .


**Smart1 Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/smart1 .


**Viking Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/viking1 .
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/viking2 .


**Voyager Mission:**

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/voyager1 .
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/voyager2 .


## Installing older versions of ISIS
---------------------------------

### How do I install ISIS2?

If you are looking for ISIS2, please [refer to the ISIS 2 Installation Guide](http://isis.astrogeology.usgs.gov/Isis2/isis-bin/installation.cgi) for instructions on downloading and installing ISIS 2.

### How do I install ISIS3.5.2 or earlier?

If you are looking for a version of ISIS3 prior to 3.6.0, please [refer to the Legacy ISIS3 Installation Guide](https://isis.astrogeology.usgs.gov/documents/LegacyInstallGuide/index.html) for instructions on downloading and installing ISIS3, versions prior to 3.6.0.
