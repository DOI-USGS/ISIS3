# ISIS3

[![Join the chat at https://gitter.im/USGS-Astrogeology/isis3_cmake](https://badges.gitter.im/USGS-Astrogeology/isis3_cmake.svg)](https://gitter.im/USGS-Astrogeology/isis3_cmake?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Join the discourse at https://astrodiscuss.usgs.gov](https://img.shields.io/discourse/https/astrodiscuss.usgs.gov/topics.svg?style=flat)](https://astrodiscuss.usgs.gov/)

## Table of Contents

* [Installation](README.md##Installation)

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
          Anaconda 3.4 and up - conda activate isis3
          prior to Anaconda 3.4 - source activate isis3

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
                

7.  Finally, setup the environment variables:

        #Execute the ISIS3 variable initialization script with default arguments.
        #This script prepares default values for:  $ISISROOT/$ISIS3DATA/$ISIS3TESTDATA

        python $CONDA_PREFIX/scripts/isis3VarInit.py
                

    Executing this script with no arguments will result in $ISIS3DATA=$CONDA\_PREFIX/data, and $ISIS3TESTDATA=$CONDA\_PREFIX/testdata. The user can specify different directories for both of these optional values:

        python $CONDA_PREFIX/scripts/isis3VarInit.py --data-dir=[path to data directory]  --test-dir=[path to test data directory]
                

    Directions for running rsync to download ISIS3 data can be found [here.](#ISIS3DataDownload) Now everytime the isis3 environment is activated, $ISISROOT, $ISIS3DATA, and $ISIS3TESTDATA will be set to the values passed to isis3VarInit.py. This does not happen retroactively, re-activate the isis3 envionment with one of the following commands:

        for Anaconda 3.4 and up - conda activate isis3
        prior to Anaconda 3.4 - source activate isis3
        

### Operating System Requirements

ISIS3 runs on many UNIX variants. ISIS does not run natively on MS Windows, although it has been successfully run on Windows 10 using the Windows Subsystem for Linux (WSL). Instructions for doing this can be found [here.](#RunningOnWindows) The UNIX variants ISIS3 has been successfully built on are:

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

### Mission Requirements

ISIS3 supports many planetary missions; in fact, over 40 different instruments including some flown as early as the 1960s. Ancillary data are required to process images from these instruments. For example, translation definition files to help convert from PDS format to ISIS cubes, dark current and flat file images for radiometric calibration, and large quantities of SPICE files (spacecraft pointing and position) for map projecting images. If you plan to work with data from all missions, then the download will require about 530 GB for all the ancillary data. However, most of this volume is taken up by SPICE files. We have a SPICE Web service that can be used in lieu of downloading all of the SPICE files which can reduce the download size to 10 GB. When downloading ISIS, you will have the option of choosing which mission data to acquire as well as if you only want the translation and calibration files and not SPICE files.

### DTM Requirements

The strength of ISIS3 lies in its capabilities for planetary cartography. The image orthorectification process requires a digital terrain model (DTM). The DTMs can be quite large and take some time to download. They exist for many planetary bodies (e.g., the Moon, Mars, etc.). Therefore, there are options for selecting which DTMs to download if you are only working with a particular target body.

<span id="ISIS3DataDownload"></span>

### Full ISIS3 Data Download

Mission data is hosted on rsync servers and not through conda channels like the ISIS3 distribution. This requires using the rsync command from within a terminal window within your Unix distribution, or from within WSL if running Windows 10. Downloading all mission data requires over 520 GB of disk space. If you want to acquire only certain mission data [click here](#MissionSpecific). To download all ISIS3 data files, continue reading.

To download all ISIS3 data (approximately 520 GB), enter the following commands at the command prompt:

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data .
          

> Note: The above command downloads all ISIS data including the required base data area and all of the optional mission data areas.


### Partial Download of ISIS3 Base Data (Required)

The base data area is separate from the source code. This data area is crucial to ISIS3 and must be downloaded.

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/base data/

### Partial Download of Mission Specific Data

There are many missions supported by ISIS. If you are only working with a few missions then you can save disk space by downloading only those specific data areas. If you want to limit the download even further, read the next section about the SPICE Web Service. Otherwise [jump](README.md#ApolloMission) to the mission specific sections.

### ISIS SPICE Web Service

ISIS can now use a service to retrieve the SPICE data for all instruments ISIS supports via the internet. To use this service instead of your local SPICE data, click the WEB check box in the spiceinit program GUI or type spiceinit web=yes at the command line. Using the ISIS SPICE Web Service will significantly reduce the size of the downloads from our data area. If you want to use this new service, without having to download all the SPICE data, add the following argument to the mission-specific rsync command:

    --exclude='kernels'

For example: `rsync -azv **--exclude='kernels'** --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/cassini data/`

<span style="font-size:120%; color:red; font-weight:bold"> WARNING: Some instruments require mission data to be present for radiometric calibration, which may not be supported by the SPICE Web Server exclusively, and some programs that are designed to run an image from ingestion through the mapping phase do not have an option to use the SPICE Web Service. For information specific to an instrument, see the documentation for radiometric callobration programs. </span> 

#### Apollo Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo15 data/
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo16 data/
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/apollo17 data/
    

<span id="CassiniMission"></span>

Cassini Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/cassini data/
            

<span id="ChandrayaanMission"></span>

Chandrayaan Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/chandrayaan1 data/
            

<span id="ClementineMission"></span>

Clementine Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/clementine1 data/
            

<span id="DawnMission"></span>

Dawn Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/dawn data/
            

ExoMars Trace Gas Orbiter Mission (kernels can be excluded):

<span id="TGOMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/tgo data/
            

Galileo Mission (kernels can be excluded):

<span id="GalileoMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/galileo data/
    

Hayabusa Mission (kernels can be excluded):

<span id="HayabusaMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/hayabusa data/
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/hayabusa2 data/
            

Juno Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/juno data/
            

<span id="KaguyaMission"></span>

Kaguya Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/kaguya data/
            

<span id="LunarOrbiterMission"></span>

Lunar Orbiter Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/lo data/
            

<span id="LunarReconnaissanceOrbiterMission"></span>

Lunar Reconnaissance Orbiter Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/lro data/
            

<span id="MarsExplorationRoverMission"></span>

Mars Exploration Rover Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mer data/
            

<span id="Mariner10Mission"></span>

Mariner10 Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mariner10 data/
            

<span id="MessengerMission"></span>

Messenger Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/messenger data/
            

<span id="MarsExpressMission"></span>

Mars Express Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mex data/
            

<span id="MarsGlobalSurveyorMission"></span>

Mars Global Surveyor Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mgs data/
            

<span id="MarsReconnaissanceOrbiterMission"></span>

Mars Reconnaissance Orbiter Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/mro data/
            

<span id="MarsOdysseyMission"></span>

Mars Odyssey Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/odyssey data/
            

<span id="NearMission"></span>

Near Mission (kernels can be excluded):

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/near data/
            

New Horizons Mission (kernels can be excluded):

<span id="NewHorizonsMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/newhorizons data/
            

Odyssey Mission (kernels can be excluded):

<span id="OdysseyMission"></span>

            rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/odyssey data/
            

Rolo Mission (kernels can be excluded):

<span id="RoloMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/rolo data/
            

Rosetta Mission (kernels can be excluded):

<span id="RosettaMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/rosetta data/
            

Smart1 Mission (kernels can be excluded):

<span id="Smart1Mission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/smart1 data/
            

Viking Mission (kernels can be excluded):

<span id="VikingMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/viking1 data/
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/viking2 data/
            

Voyager Mission (kernels can be excluded):

<span id="VoyagerMission"></span>

    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/voyager1 data/
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/voyager2 data/
            

Installing older versions of ISIS
---------------------------------

### How do I install ISIS2?

If you are looking for ISIS2, please [refer to the ISIS 2 Installation Guide](http://isis.astrogeology.usgs.gov/Isis2/isis-bin/installation.cgi) for instructions on downloading and installing ISIS 2.

### How do I install ISIS3.5.2 or earlier?

If you are looking for a version of ISIS3 prior to 3.6.0, please [refer to the Legacy ISIS3 Installation Guide](../../documents/LegacyInstallGuide/index.html) for instructions on downloading and installing ISIS3, versions prior to 3.6.0.
