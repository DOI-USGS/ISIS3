# Roadmap 2019 
In 2019 ASC has a number of different software targets. These are largely focused on our core software packages: ISIS, the Community Sensor Model toolchain, plio, PyHAT, and AutoCNet.

We have labelled some tasks as investigations in order to identify those places where we need additional information before we can estimate the full complexity and scope of a task. These tasks can lead to future work, a decision to defer, or a decision that additional information is required. 

Tasks on this roadmap are living. They may move ahead to future years, be identified as being completed, or be identified as not needed. Our ultimate goal is to engage the user community to identify where we can add value.

Tasks status is identified using the following shorthand:

| Mark | Meaning |
|------|---------|
| bullet| work not yet started|
| check | work completed|
| :runner: | on-going work |
| :muscle: | stretch goal |

## Themes
Funded  developmemnt for ISIS is generally aligned by named projects. We organize the roadmap thematically due to the amount of synergy between projects. In FY19 (October 1, 2018 - September 30, 2019), we are working in the following thematic areas:

- Streamline the build, test, deploy trains
- Begin improving the ISIS testing infrastructure
- Reduce friction for external development on projects
- (Re-)Build the online community around our software packages; all types of contributors.
- Operationalize ASC Community Sensor Model
- Begin re-architecture of ISIS

### Release Train :train:
  - [x] Transition to CMake and anaconda deploys (binaries)
  - :runner: Transition data area from rsync to version controlled solution
  - [x] Adopt versioning standard. This allows us to alert users to enhancements and breaking changes.
  * Separate source/deploy a la conda-forge in order to reduce the release friction.
  - :runner: Move to 4 releases per year and immediate bug fix releases in order to get new code out to users at the fastest reasonable cadence.
  - :muscle: Unpin version on 1+ current dependencies in order to make installation of ISIS alongside other libraries easier.

### Testing
  - [x] Ingestigate methodology to write unit tests in a testing framework. The goal is to reduce test data volumes and the number of false positive failures.
  - Investigate how to write app tests. Rationale as above.
  - [x] investigate and deploy a testing framework; GTest.
  - investigate and deploy a CI environment to remove hand crafted build/test scripts. The goal is to reduce the number of false positive build fails due to tooling.
  - :muscles: Transition all unit tests to GTest
  - :muscles: Migrate 1+ app tests using the identified framework.

### Sensor Models (CSM/ISIS)
  - [x] Finalize Image Support Data Specification
  - [x] Implement CTX sensor model
  - [x] Verify MDIS-NAC/-WAC sensor models
  - [x] Implement Kaguya TC sensor model (CSM/ISIS)
  - [x] Implement Dawn FC sensor model
  - :runner: Verify GXP functionality
  - Implement CSM HiRISE sensor model
  - :muscles: CSM/ISIS integration

### SPICE / Architecture / ALE
  - :runner: Begin ISIS rearchitecture by refactoring SpicePosition and SpiceRotation classes
  - :runner: Refactor SPICE management scripts to reduce maintaince costs and rebuild institutional knowledge
  - :muscle: Refactor all ISIS spice calls to use ALE
  - :muscle: Refactor CSM to utilize common ALE interface
  - :muscle: Transition all ISIS spice selection code to ALE.

### Documentation
  - :runner: Continue to improve our process, developer, and user documentation, migrating to openly available solutions (e.g., GitHub wikis, discourse forums, etc. as appropriate).
  - :runner: Generate project level roadmaps for all large scope projects that are publicly facing.
  - :muscle: Ensure the above are publicized at a monthly cadence

### Community Support
  - [x] Get the community support forums turned back on
  - [x] Fully transition from RedMine (performance and usability issues) to GitHub
  - :runner: Provide support via GitHub, discussion forums, and other communication platforms
  - :muscle: Solicit, review, and merge community PR

## Other ASC Libraries
  * PyHAT
    - [x] Add support for M3 derived products
    - [x] Add support for CRISM derived products
    - [x] Build ipynb documentation
    :muscles: QGIS plugin deploy on 3 major OSes
  * AutoCNet
    - :runner: Add capability to tie to ground
    - :runner: Refactor to deprecate server
    - :runner: Improve ipynb documentation for use cases
    - :muscle: Identify how to stream new images into a project
  * Plio
    - :muscle: Solidify deploy train via conda-forge

## Summary
Above a high level overview of the tasks targeted for ASC FY19 development have been enumerated. We are focusing heavily on addressing technical debt, engaging our user base, and making external development more viable. We anticipate that these tasks will continue into FY20 as we transition to a true open source project. We solicit any and all engagement from that users that we seek to serve.