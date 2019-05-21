Releases and development of ISIS3 follows a time based schedule with a new release occurring every three months. Below, we illustrate a sample four month snapshot of software development.

[[images/releaseplanning.png]]


** NOTE: ** The image above erroneously indicates that RC1, RC2, etc. will be generated for subsequent releases. In reality, we will start eery release cycle with RC1. If issues are identified during the RC month, we will release x.y.z_RC# increment # with each subsequent RC release.

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
| 3.7.0_RC | Release Candidate | 4.1.19 |
| 3.7.x | Release | 4.30.19 |
| 3.8.0_RC | Release Candidate | 7.1.19 |
| 3.8.x | Release | 7.31.19 |
| 3.9.0_RC | Release Candidate | 10.1.19 |
| 3.9.x | Release | 10.31.19 |
| 3.10.0_RC | Release Candidate | 1.1.20 |
| 3.10.x | Release | 1.31.20 |
| 3.11.0_RC | Release Candidate | 4.1.20 |
| 3.11.x | Release | 4.30.20 |