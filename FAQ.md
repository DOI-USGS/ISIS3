## How do I Install ISIS3?
There are two main ways of installing ISIS3:

__As a user__: ISIS releases are distributed using the Anaconda package manager. Through the package manager, you can download precompiled versions of ISIS including older versions. You can find the instructions [here][8] for installing Anaconda. 


__As a developer__: Anaconda is also used to maintain ISIS dependencies, so we generally advise developers to use Anaconda environments and provided environment files to build ISIS. You can find the instructions [here][9].

[8]: https://github.com/USGS-Astrogeology/ISIS3/blob/dev/README.md#isis3-installation-with-conda
[9]: https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake


## How do I install a specific version of ISIS/update my current copy of ISIS?
During the installation process, you can use Anaconda’s syntax for specifying version numbers. 

To update your version of ISIS, you simply run `conda update -c usgs-astrogeology isis3=<version number>`.

ISIS release candidates are tagged with “RC”. Installing/updating RC’s would require you to explicitly mention the RC label with `-c usgs-astrogeology/label/RC `. 

Examples: 
`conda install -c usgs-astrogeology isis`, install the latest version of ISIS
`conda update -c usgs-astrogeology isis`, update to the latest version of ISIS
`conda update -c usgs-astrogeology isis=3.9.1`, update to ISIS version 3.9.1
`conda install -c usgs-astrogeology isis=3.9.1`, install ISIS version 3.9.1
`conda install -c usgs-astrogeology/label/RC isis=3.9.1`, install release candidate for ISIS version 3.9.1 

Full list of supported versions including release candidates: https://anaconda.org/usgs-astrogeology/isis3/files 

Info on maintaining your anaconda packages:https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-pkgs.html

## How do I keep two different versions of ISIS on my system and switch between them?
An example of when this functionality may be useful is to test out a release candidate (RC) version of the ISIS3 software (if you have questions about RC’s, click [here][1]). The solution is to use multiple environments. Follow the steps for installing ISIS using a new environment with a different name (e.g. `isis3.9` vs `isis3.9_RC`).

[1]: https://github.com/USGS-Astrogeology/ISIS3/wiki/Release-Schedule

## I Installed ISIS but I get “<app>: command not found” or similar error.
If you successfully installed ISIS with Anaconda, but still get this error, it’s because the installation directory was not added to your PATH. It’s generally not recommended you manually set your PATH to your Anaconda environment and instead should ensure your environment is activated by running “conda activate <isis3 env name>”. 

## How do I install ISIS on Windows?
We do not officially support ISIS on windows. It may be possible using the Linux subsystem (look here for info in the Windows Subsystem for Linux: https://docs.microsoft.com/en-us/windows/wsl/install-win10) this approach isn’t tested and we strongly recommend ISIS users operate MacOS, Linux or a Linux VM on Windows machines. 

## When trying to run an ISIS command, I get `Please set ISISROOT before running any Isis applications`
The environment variable `ISISROOT` needs to be set to the root of your installation directory. Otherwise, no application can’t run since it’s critical to operating ISIS applications.

Run  `isisVarInit.py`, located in the ISIS3 installation directory ). This will make ISISROOT, ISIS3DATA, and ISIS3TESTDATA be set on environment activation.  


## Why is `spiceinit` failing?
Spiceinit attaches kernel information to ingested ISIS cubes for later use in commands that require camera models (campt, cam2map, etc.). See ISIS3 [documentation][3] for more details on spiceinit. If spiceinit is erroring there are two likely causes:
* Base data area is not installed (installation directions [here][4])
* Mission specific area is not installed (installation directions [here][5])

These errors can also be remedied by using the [SPICE Web Server][6] (web=true in command line or activating the web check box in GUI). __However__, some instruments require mission data to be present for calibration, which may not be supported by the SPICE Web Server exclusively, and some programs that are designed to run an image from ingestion through the mapping phase do not have an option to use the SPICE Web Service. For information specific to an instrument, see the documentation for radiometric calibration programs. 

If spiceinit fails with “**PROGRAMMER ERROR** No value or default value to translate for translation group [MissionName]”, this is typical behavior in `spiceinit` when `spiceinit` input was output from `pds2isis`. `pds2isis` generates a cube without instrument specific data. Instead use an ingestion app specific to the image’s instrument. See https://github.com/USGS-Astrogeology/ISIS3/wiki/Locating_and_Ingesting_Image_Data for basic ingestion workflows in ISIS. 

## Why is my mission calibration command (‘ctxcal’,’lronaccal’, etc.) producing cubes filled with zero data?
The default setting for ‘RadiometricType’ on a lot of calibration commands is ‘IOF’ which stands for incidence solar flux. The calculation of this metric requires knowledge of the distance from the target body to the sun. Currently, the SPICE Web Server currently does not attach this information to the cube, so local version of the mission data are still necessary for this calculation. Using ‘RadiometricType=RADIANCE’ will not result in this error. To fix this error, download the base ISIS3 data and the mission specific data:
* Base data installation directions [here][4]
* Mission specific area installation directions [here][5]

## I updated my ISIS version and now my mission calibration command is throwing errors.
Some ISIS software updates include mission specific command updates motivated by new information (updated kernels, format changes, etc.) from mission teams. When the software is updated the data area should also be updated. To do this see the mission specific kernel download instructions [here][5].

[3]: https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html
[4]: https://isis.astrogeology.usgs.gov/documents/InstallGuide/#ISIS3DataDownload
[5]: https://isis.astrogeology.usgs.gov/documents/InstallGuide/#MissionSpecific
[6]: https://isis.astrogeology.usgs.gov/documents/InstallGuide/#SPICEWebService


## Additional Helpful Docs
[Exhaustive list of ISIS commands](https://isis.astrogeology.usgs.gov/Application/index.html)

[Glossary of terminology used in ISIS documentation](https://isis.astrogeology.usgs.gov/documents/Glossary/Glossary.html)

[Dictionary explaining label terms for ISIS cubes](https://isis.astrogeology.usgs.gov/documents/LabelDictionary/LabelDictionary.html)
