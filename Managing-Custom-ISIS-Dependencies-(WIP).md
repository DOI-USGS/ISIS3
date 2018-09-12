# The ISIS Conda Environment

The third-party dependencies required to build and run ISIS are specified in the environment.yml file located at the root of the ISIS repo. For more info on editing a conda environment file, view the [Conda Documentation](https://conda.io/docs/user-guide/tasks/manage-environments.html)

Most of the packages listed in the file are pulled from conda-forge, but there are a few custom builds that we have uploaded to [usgs-astrogeology](https://anaconda.org/usgs-astrogeology/repo), such as qt. The recipes to build and upload these packages can be found on the [isis_dependencies](https://github.com/USGS-Astrogeology/isis3_dependencies) repo.

# Using the Anaconda Upload Python Scripts

To build and upload a package to anaconda, run `python bin/build_package.py <package>` from the root of the isis_dependencies repo. This requires you to log into an anaconda account with write permissions for the usgs-astrogeology repo. This command will use the build.sh script and meta.yaml specifications found in the  recipies/<package> directory. If there is no meta.yaml file, but a meta.yaml.tmpl file exists, one will be created using the template. 

Building some of the packages will require conda-forge and usgs-astrogeolgy to be in your channels, so add them with `conda config --add channels conda-forge --add channels usgs-astrogeology`