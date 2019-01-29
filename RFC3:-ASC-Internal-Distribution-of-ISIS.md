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
1. Once a bug fix is made, the bug fix will be released publicly according to [RF2](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC2:-Release-Process) and then an internal build will be made available according to #1.
1. If an internal user needs access to a feature prior to its public release, they need to work with the development team to get a custom build.

# Motivation
The current internal distribution system is based on the old rsync public release system. With rsync distribution, an internal build was created and then those binaries and runtime environment were packaged and delivered to the public. With Anaconda distribution, the public release is created in an isolated environment and then delivered to the public. The internal build is then an entirely separate build that is tied to the development Anaconda environment.

Having the public release and internal build use different build and run time environment specifications can result in inconsistencies between internal and external users. Internal builds are also implicitly tied to a specific Anaconda environment. If that environment is modified or removed it will impact all of the internal builds previously created with it.

With [RFC2](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC2:-Release-Process), we are committing to making bug fixes rapidly available to the public. Internal users will no longer need builds to get access to bug fixes in a timely manner. For feature changes, we are also committing to a more timely feature release schedule. Instead of features being released every 6 months to a year, they will be available quarterly. This will reduce the amount of time between when features are finished and they are available to users.

# Proposed Solution / Explanation
Anaconda inherently provides a solution to the first two problems. We should stage the public release build for internal use. We can isolate each version in a separate Anaconda [environment](https://docs.conda.io/projects/conda/en/master/user-guide/tasks/manage-environments.html). This will ensure that users know explicitly which version of ISIS and its dependencies they are using. Separate environments will also isolate internal users from the Anaconda environments used by developers to do day-to-day work and nightly testing.

In order to further isolate internal users from the volatile development environment, we should no longer nightly builds as "use at your own risk" for internal users. They need to continue to support nightly automated testing, but they should not be "used" to do any work. Similarly, weekly beta builds should be discontinued. They are inherently tied to the development environment because they are created outside of the release build environments. If an internal user needs a feature before it is publicly available they should work with the development team to get a custom build. It should be explicitly stated how long that custom build will be used for and what features are required. The development team, should be responsive to user requests for custom builds and provide them in a reasonable time frame.

## How Would Things Change for Internal Users

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

# Drawbacks
Why should we *not* do this?

# Alternatives
  - Why is this design or solution the best of all possible solutions?
  - What other designs have been considered and what is the rationale for not choosing them?
  - What is the impact of not doing this?

# Unresolved Questions
  - What parts of the design will be merged through the RFC process prior to merging?
  - What parts of the RFC are expected to be addressed through implementation before stabilization?
  - What related issues are out of scope for this RFC, but could be addressed in future RFCs?

# Future Possibilities
  - What future extensions could be made from this RFC?
  - What other ideas might you have? This is a great place to 'dump related ideas'.