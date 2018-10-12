# The ISIS3 Conda Environment

_Note: This page is describes dependency management for ISIS3 versions 3.6.0 and above._

With the release of isis3.6.0, the ISIS3 dependencies are being managed with a package manager called [conda](https://conda.io/docs/). These packages are hosted on a publicly available servers at [anaconda.org](https://anaconda.org).

## Why do we use conda?

One of the primary benefits of using conda is that we do not have to internally build and manage as many libraries that ISIS3 depends on; instead, we can use "channels" that already maintain hundreds of packages (e.g. [conda-forge](https://conda-forge.org/)). 

If we do need to build a custom version of a library, we can build our own with the conda-build system and upload them to our own [usgs-astrogeology channel](https://anaconda.org/usgs-astrogeology). All of these recipes are publicly accessible via our [isis3_dependencies GitHub repo](https://github.com/usgs-astrogeology/isis3_dependencies).

## How are dependencies specified for ISIS3?

The third-party dependencies required to build and run ISIS3 versions 3.6.0 and above are specified in the [environment.yml](https://github.com/USGS-Astrogeology/ISIS3/blob/cmake/environment.yml) file located at the root of the ISIS3 repo. For more info on editing a conda environment file, view the [Conda Documentation](https://conda.io/docs/user-guide/tasks/manage-environments.html)

Most of the packages listed in this environment file are pulled from conda-forge, but there are a few custom builds that we have uploaded to [our usgs-astrogeology channel](https://anaconda.org/usgs-astrogeology/repo), such as Qt. The recipes to build and upload these packages can be found on the [isis3_dependencies](https://github.com/USGS-Astrogeology/isis3_dependencies) repo.
 

:building_construction: _**should the following sections be on a page on the isis3_dependencies wiki?**_

# Environment Setup

1. [Download and install the latest version of Anaconda](https://www.anaconda.com/download).
1. Fork and clone the [isis3_dependencies repo](https://github.com/usgs-astrogeology/isis3_depenedencies).
1. Change directory into the isis3_dependencies clone (e.g. ```cd isis3_dependencies```)
1. Since we pull most of our dependencies from conda-forge and some custom ones from our usgs-astrogeology account, add the conda-forge and usgs-astrogeology anaconda.org channels to your anaconda configuration:
```bash
conda config --add channels conda-forge --add channels usgs-astrogeology
```
_Note: If your environment pulls SuperLu from usgs-astrogeology instead of conda-forge, try changing their order_
# Important files in the isis3_dependencies repo

There are three directories in the `isis3_dependencies` repo: `bin/`, `meta/`, and `recipies/`.

The `bin/` directory contains some python scripts that are used for determining dependencies and available packages, as well as a script for building packages. The `build_package.py` script is the script we will use to build a package for uploading to the [USGS-Astrogeology anaconda.org cloud](https://anaconda.org/usgs-astrogeology). It is essentially a wrapper around a conda-build command. 

The `meta/` directory contains general metadata about the version numbers and configurations of the packages. The package-specific metadata will pull some values from this directory.

The `recipes/` directory contains build scripts, package-specific metadata, and patch files. Some of the metadata files here will have an .tmpl extension, which means that they are templates that will generate the real meta.yml file during a build.

_Note: For information on the justification behind each custom build, check the comments at the top of each recipe's build.sh script._ 

For more information on how conda builds work, see the [Conda build recipes documentation](https://conda.io/docs/user-guide/tasks/build-packages/recipe.html).

# Using the Python Build and Upload Scripts

To build and upload a package to anaconda, run `python bin/build_package.py <package>` from the root of the isis_dependencies repo. This requires you to log into an anaconda account with write permissions for the usgs-astrogeology repo. This command will use the build.sh script and meta.yaml specifications found in the  recipies/<package> directory. If there is no meta.yaml file, but a meta.yaml.tmpl file exists, one will be created using the template. 

Building some of the packages will require conda-forge and usgs-astrogeolgy to be in your channels, so add them with `conda config --add channels conda-forge --add channels usgs-astrogeology`