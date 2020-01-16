This document describes how to contribute changes to the ISIS Application Documentation, which is available online for the most recent ISIS release at: https://isis.astrogeology.usgs.gov/Application/index.html

# Getting Started
Documentation for ISIS applications is located within the ISIS source code and written in XML. The XML is processed to create the documentation that can be viewed on the ISIS website at: https://isis.astrogeology.usgs.gov/Application/index.html

For many documentation contributions, very little understanding of XML is required. Changes in existing wording or additional information can be added where appropriate in the current documentation without needing to change any of the XML. 

Since the ISIS documentation is located within the source code, the first step is to create a local copy of the ISIS source code to work with. To do the necessary initial setup, see [Getting Started with Github](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#getting-started-with-github) and [Anaconda and ISIS3 Dependencies](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#getting-started-with-github). 
 
# Making Your Documentation Change

Documentation for each ISIS application is located in the same directory as the source code for the application itself and is named `name_of_application.xml` For example, to update the documentation for voy2isis, go to `ISIS3/isis/src/voyager/apps/voy2isis` in your ISIS repo clone and begin editing `voy2isis.xml`. To make your documentation change, open the xml file in the text editor of your choice, and edit the text to make your update. Most documentation changes will be made in the `<description>` section of the XML file, which creates the "Description" section on the built html documentation.

# How to Preview Your Change

In order to preview your documentation change, it is necessary to build the documentation and view it in a web browser. 
The next step depends on whether you already have an existing ISIS build, or if you need to create 
a new ISIS build. 

## Building ISIS Documentation

### Creating a New ISIS build: 

Building ISIS requires that the anaconda environment you created in [Anaconda and ISIS3 Dependencies](https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#anaconda-and-isis3-dependencies) be activated.

Activate your anaconda environment with:

`conda activate <environment-name>`

* Navigate to the top-level directory of your ISIS3 clone (i.e `cd ISIS3`)

* Create a `build` and an `install` directory at this level:
  * `mkdir build install`

* Navigate to the build directory: `cd build` 
* Run the following command to configure your build:
  * `cmake -Disis3Data=<path-to-isis3-data> -Disis3TestData=<path-to-isis3-test-data> -DJP2KFLAG=OFF -DCMAKE_BUILD_TYPE=RELEASE -GNinja ../isis`

* Set your ISISROOT to `/the/path/to/your/build`:
  * `export ISISROOT=$(pwd)`

* And finally, build the documentation by running:

```
ninja docs -j7
```

It is not necessary to build the rest of ISIS to build and preview documentation-only changes. 

### If You Already Have an Existing ISIS Build: 

To build and view your documentation changes, first `cd` to your `$ISISROOT` directory in the terminal and run the following: 

```
ninja docs -j7
```

If you configured for `make`, instead of `ninja`, run:

```
make docs -j7
```

The built documentation will be available in the `docs` directory below the current build directory. 
(If the release type was set to install during the build process, the built docs will also be copied over into `install/docs`, where `install` refers to the installation directory specified during the build.)


## Previewing the Built Documentation
To view your documentation, open the index.html file immediately under the docs directory ($ISISROOT/docs/index.html) using a web browser and use the web browser to navigate to the modified file. It is possible to do this by:

* running either of the following in a terminal: `google-chrome index.html` or `firefox index.html`
* copy-and-pasting the full file path to `index.html` (for example: `/home/username/dev/ISIS3/isis-build/docs/index.html`) into the address bar of a web browser.


If you see changes that need to be made to the documentation at this point, you can make the changes in your documentation XML files, re-run your documentation build command, and then just refresh the web-page in your browser to view your updated changes. 

# Adding Your Documentation Changes to ISIS
To contribute your documentation changes to ISIS, please put in a pull request containing your documentation changes, following the instructions at [How to Put in a PR](https://github.com/USGS-Astrogeology/ISIS3/wiki/How-to-Start-Contributing#create-a-pull-request). Be sure to read the [Contributing](https://github.com/USGS-Astrogeology/ISIS3/blob/dev/CONTRIBUTING.md) document, and to select the "Documentation change" under "Types of change" in the PR template which will appear when you put in your pull request. Your pull request will be reviewed by the software team at the Astrogeology Science Center. 

# Additional Information About User Documentation in ISIS
For a comprehensive overview of ISIS documentation, see the following:
https://isis.astrogeology.usgs.gov/documents/HowToGeneralDocumentation/index.html

# Updating or Adding New Application Examples to User Documentation
Examples of each ISIS application's use are provided as part of the user documentation. If you would like to modify an existing example or add an additional example to application documentation, please see the instructions at: 
https://isis.astrogeology.usgs.gov/documents/HowToApplicationExamples/index.html