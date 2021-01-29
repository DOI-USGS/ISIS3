# Creating a shared Anaconda installation

## Introduction

Here at the USGS Astrogeology Science Center (ASC) we have a software development team consisting of roughly a dozen full or part time developers, another dozen or so technical users, and a few dozen scientists who all need consistent access to software builds and releases. As we moved to using Anaconda for dependency management and software installation, our IT group quickly flagged an issue where users' Home directories were growing astronomically large from each having their own Anaconda installation. Along with that, we had many users who had never used Anaconda before having to manage and debug their own installations. Fairly quickly, we settled upon having shared Anaconda installations that users could access and use across systems without having to manage their own Anaconda installation or update their environments.

## The systems landscape at the ASC

I'm sure our IT folks could give a much better explanation than me, but here's a quick run-down of our systems at the ASC. The majority of our users have either a Windows desktop, an Ubuntu desktop, or a macbook. We primarily target our software for use on Unix systems so those users who have a windows desktop use PuTTY to connect to Linux Virtual Machines (VMs). Along side individual workstations, we also have a Linux processing cluster and have Network File System (NFS) drives that users can access. Users can mount the NFS drives directly to their Unix work station or they can access them by connecting to the VMs. The primary NFS drive that I'll be talking about is `/nfs/cpkgs/`. This drive is a shared drive that we mount on all of our Unix systems, hence the name cpkgs for common packages. Generally, users manage their own project data either locally, on other NFS drivers, or on high performance storage used by the processing cluster. Files on `/nfs/cpks/` are only updated by software developers or some technical users to setup environments for a project or a set of users. So, most users access software and general purpose data from `/nfs/cpkgs/` but don't store anything there.

## Some background on Anaconda

If you are already familiar with Anaconda, Anaconda channels, Anaconda environments, and Anaconda configuration; then you can go ahead and skip to the next section. If any of those things are new or you have questions about them, this section will give you a brief introduction and some links to further docs.

### Anaconda

First of all, Anaconda is a package manager that allows you to install a whole bunch of different pieces of software with one or two commands. Let's say you want to install opencv so that you can do some computer vision work in Python; all you have to do to get started is run `conda install opencv` and Anaconda will download the lastest version of OpenCV (and all of its dependencies) and install the Python bindings into its Python installation. You can download Anaconda and read the docs on their [Individual Edition Home Page](https://www.anaconda.com/products/individual). The company that makes Anaconda has some paid products and support that you can get, but we don't use them so I won't be talking about them.

You can use Anaconda to install packages with `conda install <package-name>`. For example, I can install the latest available version of OpenCV with `conda install opencv`. That will download and install the latest version of OpenCV from https://anaconda.org/anaconda/opencv/files, currently 4.0.1. I can instead install a specific version with `conda install <package-name>=<version-number>.` For example, I can install OpenCV 3.4.1 with `conda install opencv=3.4.1`.

### Environments

So now that you know how to find and install packages, what do you do when you don't want a package available all the time or you want different versions of a package available? By default Anaconda will install packages into the directory you install it in. For example, if I install Anaconda into `~/anaconda` and then I install latest version of OpenCV, it will install the dynamic library into `~/anaconda/lib`, the c++ headers into `~/anaconda/include`, and the Python bindings into `~/anaconda/lib/python3.7/site-packages`. If I then have Anaconda install an older version of OpenCV, let's say 3.2, it will replace everything I just installed with an older version. Uninstalling and reinstalling different versions of a package all the time can be a real pain, so instead let's use [environments](https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html) to install them and then we can active the latest OpenCV environment or the OpenCV 3.2 environment depending on what we want. Each environment is basically a separate set of packages that you can swap between whenever you want, and without having to reinstall anything. First, you can create an environment using `conda create my_envrionment`. Then, you can activate that environment by running `conda activate my_environment`. From here, you are ready to install any packages you want and when you're done with it you can deactivate it by running `conda deactivate`.

Using the same example installation as before, I can run `conda create -n my_environment` and I will get a new environment at `~/anaconda/envs/my_environment`. If I then activate that environment with `conda activate my_environment` and install the latest OpenCV with `conda install -c conda-forge opencv` it will install the library into `~/anaconda/envs/my_environment/lib`, the c++ headers into `~/anaconda/envs/my_environment/include`, and the Python bindings into `~/anaconda/envs/my_environment/lib/python3.7/site-packages`. I do some work and then I want to swap to OpenCV 3.2. First, I deactivate my latest OpenCV environment with `conda deactivate`. Next, I make a second environment with `conda create -n opencv_old` which will be located at `~/anaconda/envs/opencv_old` and activate it with `conda activate opencv_old`. Now I have a clean environment and can install OpenCV 3.2 with `conda install -c conda-forge opencv=3.2`. This will install the library into `~/anaconda/envs/opencv_old/lib`, the c++ headers into `~/anaconda/envs/opencv_old/include`, and the Python bindings into `~/anaconda/envs/opencv_old/lib/python3.7/site-packages`.

You can have many different environments in your anaconda installation and swap between them at will. The only stuff shared between different environments is what's in your base environment. Your base environment is the root directory in your Anaconda installation and you can activate it using `conda activate`. In our installation example, the base environment would be located at `~/anaconda`. Notice that by default Anaconda installs packages into your base environment when you don't have another environment active. Because anything installed here will bleed into your other environments, we highly discourage installing anything into your base environment besides what comes with your Anaconda installation.

### Configuration

Constantly specifying channels can be a real chore, so you may want to add a channel to your Anaconda config with `conda config --add channels <channel-name>`. This command will add the channel to your default search and you won't have to specify it everytime you install a package anymore. For example you can default to packages from conda-forge with `conda config --add channels conda-forge`. You can also specify a configuration for just an evironment by activating the environment and then adding `--env` to your conda config command. For example, I could setup `my_environment` from above to default to searching conda-forge for packages by first activating it with `conda activate my_environment` and then modifying the config with `conda config --env --add channels conda-forge`. This would cause Anaconda to default to conda-forge only when `my_environment` is activate.

All of this works using `.condarc` files at different places. If you run conda config, then it defaults to modifying your personal `.condarc` file at `~/.condarc`. If you use the `--env` option, then it modifies the `.condarc` file in your environment. For example, when I modified `my_environment` to default search conda-forge it added the following to `~/anaconda/envs/my_environment/.condarc`:

```
channels:
  - conda-forge
  - defaults
```

The [conda config](https://docs.conda.io/projects/conda/en/latest/commands/config.html) can set a whole bunch of other options that are beyond the scope of this document. You can read more about what you can do with `.condarc` files [here](https://conda.io/projects/conda/en/latest/user-guide/configuration/use-condarc.html).

## Our shared Anaconda installation

We wanted to setup a single Anaconda installation in our `/nfs/cpkgs/` NFS drive, but Anaconda requires separate installations for Linux and Mac. So, we instead have two Anaconda installations: `/nfs/cpkgs/anaconda3_linux` and `/nfs/cpkgs/anaconda3_macOS`. As their name suggests, `anaconda3_linux` is for Linux and `anaconda3_macOS` is for Mac. Within each installation we have separate environments for different versions of software. For example, under `/nfs/cpkgs/anaconda3_linux` we have the following environments for different versions of ISIS:
* isis3.6.0
* isis3.6.2
* isis3.7.0
* isis3.7.1
* isis3.8.0
* isis3.8.1
* isis3.9.0
* isis3.9.1
* isis4.0.0
* isis4.0.1
* isis4.1.0
* isis4.1.1
* isis4.2.0
* isis4.3.0

Whenever there is a new release or release candidate for ISIS, we make a new environment and install the new version into it. This allows users to select any version they want and they can easily upgrade or downgrade for their processing needs. The permissions on the installations are restricted so that users can access all of the files, but cannot modify any of them to prevent accidentally changing an environment.

## Using the shared Anaconda installations

Users can access the shared installation by running the `conda.sh` or `conda.csh` scripts in `/nfs/cpkgs/anaconda3_linux/etc/profile.d/` or `/nfs/cpkgs/anaconda3_macOS/etc/profile.d/`. These will modify their environment to run conda and give them access to the shared environments. Most of our users run bash, so we recommend that users simply add `source "/nfs/cpkgs/anaconda3_linux/etc/profile.d/conda.sh"` or `source "/nfs/cpkgs/anaconda3_macOS/etc/profile.d/conda.sh"` to their `.bashrc` or `.bash_profile`.

Users can even create personal environments with the shared Anaconda utilities. If a user creates an environment with the shared installation, it will create the environment in their home directory instead of inside the installation. For example, if I attempt to create an OpenCV environment with `conda create -n opencv` then it will be located at `~/.conda/envs/opencv`. This is caused by users not having write permissions to the shared installation, so conda defaults back to their home directory. It is okay for users to create one or two environments this way but if users want to have many different environments that they manage themselves and use the shared environments we recommend they setup their own Anaconda installation and follow the steps in the next section to add the shared environments.

## Using the shared Anaconda environments with your own Anaconda installation

Some users want to have their own Anaconda installation and still have access to the shared environments. Some users want to have more fine control over their processing environment while others want to be able to keep their own environments for when they are outside of our network and don't have access to the NFS drives. Thankfully, there is an easy solution to still allow them access to the share environments. Users simply need to run `conda config --add envs_dirs "/nfs/cpkgs/anaconda3_linux/envs"` or `conda config --add envs_dirs "/nfs/cpkgs/anaconda3_macOS/envs"`. This will add the following to their `/.condarc` file:

```
envs_dirs:
  - /nfs/cpkgs/anaconda3_linux/envs
```

Once this is done, they can run `conda activate <shared-env-name>` to activate a shared environment. If the shared environment files are not available because the NFS drive is not mounted, then Anaconda will still continue to operate as before.

## A few warnings

NFS drives are not 100% foolproof and you will sometimes have issues, here are a few we have run into. An Anaconda package adds some files to the NFS drive and the NFS software modifies them resulting in the Anaconda package thinking it is corrupted. An Anaconda package uses a SQLite database that does not lock and unlock properly because the NFS drive doesn't handle rapidly locking and unlocking a file well. NFS drives can be very slow for I/O bound processes such as NAIF DSK routines.

Anaconda uses fairly aggressive caching. If it finds anything locally that works, it generally doesn't check the cloud for something better. By keeping old environments with old packages around, you can sometimes get solves that use old packages. Even, just adding the environments via the conda config command from the previous section can cause Anaconda to pull old packages from them. Our developers often have to remove the shared environments from their envs_dirs config in order to reproduce user reported installation issues.

If you have a networked home directory that you share between Linux and Mac systems, you'll need to add some logic to your `.bashrc` and/or `.bash_profile` files that selects the proper Anaconda installation dependending on what OS you are on. If you have a shared home like this, then you likely already have logic like this in your start up files.