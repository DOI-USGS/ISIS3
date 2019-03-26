- Feature/Process Name: ISIS3 Release Process
- Start Date: 1.18.19
- RFC PR: (empty until a PR is opened)
- Issue: https://github.com/USGS-Astrogeology/ISIS3/issues/684
- Author: jlaura

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
This RFC describes a release process and timetable for the ISIS3 code base covering scheduled releases, bug fix releases, and versioning. The goal of this RFC is to provide a standard set of expectations for all contributors and end-users. This RFC *does not* address the way in which ASC personnel `set isis' within the ASC system.

# Motivation
The ISIS3 release schedule and processes are not well codified in a way that contributors and consumers can make decisions with well defined bug fix, code freeze, and code release timelines. This causes friction within the development team and for external consumers of the code base (both API consumers and users). Additionally, we have identified a misalignment in expectations with respect to when and how bug fixes and feature changes will be made available. This RFC seeks to provide mechanisms to align expectations regarding the availability of such changes.

# Proposed Solution / Explanation
Explain the proposal as if it were already included in the code base or in place. This may mean:

## Terms
- *breaking change* - any change that does not maintain backwards compatability. For example, changing from `foo a b c` to `foo a b c d=1` (the addition of an argument) would not be a backward breaking change as code written to consume `foo` in the `foo a b c` case would still function. Changing from `foo a b c` to `foo c b a` would break all consuming code.
- *bug fix* - the application of a fix to a bug or issue in the code base
- *feature addition* - the creation or introduction of new capability into the code base

## Versioning
The ISIS3 code base will adopt the [semantic versioning](https://semver.org/) standard. Briefly, semantic versioning takes the form of MAJOR.MINOR.PATCH where, MAJOR version changes indicate API incompatibility. While ISIS3 remains a tightly coupled collection of applications and an API this means that breaking changes to either API signatures (methods, functions) or apps (`spiceinit`, `pds2isis`, `jigsaw`, etc.) would constitute a breakage. MINOR version incrementing occurs with feature additions. For example, if an application were given a new command line argument to expose a new capability the MINOR would increment. Finally, PATCH version incrementing occurs every time a non-API changing backwards-compatible bug fix is applied. 

For code contributors the above versioning requires that a distinction between `features` or `enhancements` and `bugs`. The former will be released in pre-compiled binaries (described below) at a regular cadence. The latter will be released as they are completed. Therefore, end users can expect to see new features at a regular cadence and bug fixes as they are completed.

## Release Roadmap
Releases and development of ISIS3 follows a time based schedule with a new release occurring every three months. Below, we illustrate a sample four month snapshot of software development.

[[images/releaseplanning.png]]

At the start of Month 1, a Release Candidate (RC1) is created from the `dev` branch of our GitHub repository. This RC contains all development from the previous (not shown) three months. RC1 is made publicly available as both a labelled branch and via our Anaconda.org (conda) download page. During Month 1, we solicit input and testing from the broader community. Any issues identified in RC1 will be fixed during Month 1. At the conclusion of Month 1, the release is packaged and the next ISIS3 release is made available for the general public using Anaconda.org (and the default `main` label).

During Month 1 through Month 3, we continue with new feature development for RC2. At the start of Month 4, we repeat the same release candidate and release process as described above.

> **NOTE**: We are currently transitioning to this release cadence and plan to release again May 1, 2019. After that release, we will fully adopt the above schedule.

### Feature Freeze
When a Release Candidate is branched from the `dev` branch, a feature freeze is put into effect. Any feature additions that occur after a release candidate has been branched will be included in a future RC (and release). In other words, features added prior to the creation of a RC will be included in the next release. The only instances where this may not hold true is if significant, previously unidentified issues are identified during the testing of a RC that are associated with a new feature addition. In that case, we would back out the feature and recreate the RC.

### Release
As described above, we will release on a three month cadence. Releases will be labelled via GitHub for those users that wish to build from scratch. Additionally, releases will be uploaded to our Anaconda.org account for `conda` installation.

### Release Schedule
| Version # / Label | Type | Date | 
|-------------------|------|------------|
| 3.7.0-RC | Release Candidate | 4.1.19 |
| 3.7.x | Release | 4.30.19 |
| 3.8.0-RC | Release Candidate | 7.1.19 |
| 3.8.x | Release | 7.31.19 |
| 3.9.0-RC | Release Candidate | 10.1.19 |
| 3.9.x | Release | 10.31.19 |
| 3.10.0-RC | Release Candidate | 1.1.20 |
| 3.10.x | Release | 1.31.20 |
| 3.11.0-RC | Release Candidate | 4.1.20 |
| 3.11.x | Release | 4.30.20 |

## Bug Fixes
In isolation, the above release model would require that bug fixes be deployed on the same three month cadence as feature releases. This pace is unreasonably long as contributors (issue openers, issue commenters, and PR creators) wish to see their bug fixes in a release as soon as is reasonably possible. The recent changes in how ISIS3 is built and [deployed](https://github.com/USGS-Astrogeology/ISIS3#installation) have allowed the development team to streamline the creation and release of binaries. In line with the above versioning discussion, the ISIS3 software will be compiled and uploaded to the Anaconda.org cloud provider with each bug fix. This implies a number of things

  - each fix increments the PATCH version of ISIS3. 
  - each fix is applied in isolation.
  - past versions of uploaded binaries will be maintained on Anaconda.org in the cases where a rollback is required.

## Mission Releases
Periodically, we create a mission specific release for some contractual work. This generally occurs when a mission team has an enhancement in and they would like access to the enhancement outside the normal release cycle. These releases will be versioned `x.y.z-missionname` in a way identical to our release candidates.

# Drawbacks
The above introduces issues where users will have to rollback (for example, from version 3.7.11 to 3.7.10) as bug fixes can have cascading impacts. As a development team, we are working to improve testing quality to catch these cascading issues.

Having stated the above, it is challenging to advocate against adopting leading practices in software versioning and release. Maintaining the expectation misalignment propagates an (potentially conflict rife) environment where end-users are expecting software stability that is not inline with the available levels of effort from the development team. Continuing with a more ad-hoc versioning system means that API consumers will continue to express frustration about a changing API and the lack of forewarning. Finally, a non-standard release schedule means that decision makers external to the project have little to no mechanism to contribute to or understand when new (breaking and non-breaking) releases could be occurring.
 
# Alternatives
  - Maintain the status quo - the expectation misalignment will not be addressed
  - Move to quarterly releases for all code - this significantly slows the pace at which users can access fixes and pushes the burden of builds with incremental (between release) fixes onto the user base
  - Move to a 100% continuous integration / continuous deploy (CI/CD) environment - the length of time required to test the code base means that overlapping (and possibly conflicting changes) would be (much?) more likely to occur.

# Unresolved Questions
  - How will internal (ASC) processes for `set isis` and release deploy be managed? (Another RFC will be created to address this issue.)

# Future Possibilities
  - In the future, it may be advisable to address long term support (LTS) models where an even/odd approach is taken. This could be used to support mission pipeline deploys or as a mean by which the stable (even versions) are mixed with unstable (odd versions); this effectively lengthens the time a release candidate is made publicly available.