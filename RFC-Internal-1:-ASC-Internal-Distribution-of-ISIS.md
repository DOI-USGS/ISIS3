- Feature/Process Name: ASC Internal Distribution of ISIS
- Start Date: 1/29/19
- RFC PR:
- Issue:
- Author: Jesse Mapel

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
This RFC is seeking feedback on proposed changes to how builds of ISIS are made available to internal Astrogeology Science Center (ASC) users. Starting with the release of 3.7.0, the following changes are proposed:

1. New versions of ISIS will be made internally available as environments on a global Anaconda install.
1. New versions of ISIS will **not** be available in the `/usgs/pkgs` directory. Old versions will remain here.
1. Nightly builds will continue for automated testing purposes but they will **not** be supported for internal users.
1. Weekly beta builds will stop.
1. Once a bug fix is made, the bug fix will be released publicly according to [RF2](https://github.com/DOI-USGS/ISIS3/wiki/RFC2:-Release-Process) and then an internal build will be made available according to #1.
1. If an internal user needs access to a feature prior to its public release, they need to work with the development team to get a custom build.

# Motivation
The current internal distribution system is based on the old rsync public release system. With rsync distribution, an internal build was created and then those binaries and runtime environment were packaged and delivered to the public. With Anaconda distribution, the public release is created in an isolated environment and then delivered to the public. The internal build is then an entirely separate build that is tied to the development Anaconda environment.

Having the public release and internal build use different build and run time environment specifications can result in inconsistencies between internal and external users. Internal builds are also implicitly tied to a specific Anaconda environment. If that environment is modified or removed it will impact all of the internal builds previously created with it.

With [RFC2](https://github.com/DOI-USGS/ISIS3/wiki/RFC2:-Release-Process), we are committing to making bug fixes rapidly available to the public. Internal users will no longer need builds to get access to bug fixes in a timely manner. For feature changes, we are also committing to a more timely feature release schedule. Instead of features being released every 6 months to a year, they will be available quarterly. This will reduce the amount of time between when features are finished and they are available to users.

# Proposed Solution / Explanation
Anaconda inherently provides a solution to the first two problems. We should stage the public release build for internal use. We can isolate each version in a separate Anaconda [environment](https://docs.conda.io/projects/conda/en/master/user-guide/tasks/manage-environments.html). This will ensure that users know explicitly which version of ISIS and its dependencies they are using. Separate environments will also isolate internal users from the Anaconda environments used by developers to do day-to-day work and nightly testing.

In order to further isolate internal users from the volatile development environment, we should no longer nightly builds as "use at your own risk" for internal users. They need to continue to support nightly automated testing, but they should not be "used" to do any work. Similarly, weekly beta builds should be discontinued. They are inherently tied to the development environment because they are created outside of the release build environments. If an internal user needs a feature before it is publicly available they should work with the development team to get a custom build. It should be explicitly stated how long that custom build will be used for and what features are required. The development team, should be responsive to user requests for custom builds and provide them in a reasonable time frame.

## What Changes for Internal Users

### Findings a specific version of ISIS
Previously users would look in the `/usgs/pkgs` directory for the version of ISIS they would to use.
```
ls /usgs/pkgs/
```
```
isis3
isis3.5.0
isis3.5.1
isis3.5.2
isis3.6.0
isis3.6.1
```

Under the proposed system, users would be able see all of the versions of ISIS available through Anaconda.
```
conda info --envs
```
```
base                  *  /work/users/jmapel/anaconda3
csm_tests                /work/users/jmapel/anaconda3/envs/csm_tests
jupyter                  /work/users/jmapel/anaconda3/envs/jupyter
isis3                    /usgs/cpkgs/anaconda3_linux/envs/isis3
isis3.7.0                /usgs/cpkgs/anaconda3_linux/envs/isis3.7.0
isis3.7.1                /usgs/cpkgs/anaconda3_linux/envs/isis3.7.1
isis3.7.2                /usgs/cpkgs/anaconda3_linux/envs/isis3.7.2
isis3.8.0                /usgs/cpkgs/anaconda3_linux/envs/isis3.8.0
```

### Using a specific version of ISIS
Previously users would set an alias in their cshrc/bashrc/bash_profile defining `setisis`. Then, users can use `setisis` to use different versions of ISIS.

```
setisis isis3.6.1
```
```
 ISIS3 Setup In: /usgs/pkgs/isis3.6.1/install - Successful
```

Under the proposed system, users would activate the Anaconda environment of the version they want.
```
source activate isis3.6.1
```

### Version of ISIS prior to 3.7.0
For versions prior to 3.7.0, nothing changes. They will remain in `/usgs/pkgs` and used via `setisis`.

# Drawbacks
## Moving from Unix tools to Anaconda
The Unix tools that we use for managing different versions of ISIS are well documented, tested, and already known by the majority of internal users. Anaconda is a comparatively new tool that the majority of our users are not familiar with. Additional effort will be needed to train everyone on how to use Anaconda and provide support during/after roll out.

## Managing a building-wide Anaconda installation
Anaconda can support multiple user, multiple machine installations, but it requires careful management of permissions and conda config files. We already have experience managing a shared installation for development, but there will undoubtedly be some amount of bumps managing an installation for the entire building. Personal installations of Anaconda will not conflict with the building-wide installation.

## Using ISIS in other Anaconda environments
If an internal user wants to use ISIS in their own custom Anaconda environment, they will need to install ISIS into their own Anaconda environment. Simply adding the shared environment `bin` directory to their path could cause issues with Anaconda.

## The anaconda environments will contain more than just ISIS
Because of how Anaconda works, there will be some number of additional executables in the environment. The vast majority of these are not important, but Python is included. This will make using ISIS with python 2 more challenging.

# Alternatives
## Sticking with the current system
If we continue with the current system we will be distancing our internal users from external users. This can make accurate bug reporting and diagnosis challenging. Additionally, as we produce more internal builds we will have to manage an increasing number of environments to support them and will need another system to manage this relationship. We may as well have Anaconda manage that for us. There is also a greater possibility of an environment being changed and a build being accidentally impacted.

## Having users manage their own Anaconda environments and install the versions of ISIS they need.
This would create a large number of installs across our network file system. Even with just the developers having their own Anaconda install, we saw significant impacts on the backup times for the entire building. It is also more convenient for internal users to already have the versions staged and ready to go. This way, they can move to any version of ISIS they need without having to re-download it.

# Unresolved Questions
## Additional User Input
This has been actively discussed within the ASC development team, but not with most of our internal users. There are potentially better options and drawbacks that internal users are aware of. More input from the end-users of this system is needed prior to roll out.

## Custom Builds
This will reduce the number of custom builds but won't eliminate all of them. Specifically for testing bug fixes and issues, there will still need to be custom builds shared between developers and internal users/testers. This RFC does not attempt to make the process simpler, but it does not make it more complex.

# Future Possibilities
  - What future extensions could be made from this RFC?
  - What other ideas might you have? This is a great place to 'dump related ideas'.