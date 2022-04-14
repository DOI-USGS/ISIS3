Releases and development of ISIS3 follows a time based schedule with a new release occurring every three months. Below, we illustrate a sample four month snapshot of software development.

[[images/releaseplanning.png]]


** NOTE: ** The image above erroneously indicates that RC1, RC2, etc. will be generated for subsequent releases. In reality, we will start every release cycle with RC1. If issues are identified during the RC month, we will release x.y.z_RC# increment # with each subsequent RC release.

At the start of Month 1, a Release Candidate (RC1) is created from the `dev` branch of our GitHub repository. This RC contains all development from the previous (not shown) three months. RC1 is made publicly available as both a labelled branch and via our Anaconda.org (conda) download page. During Month 1, we solicit input and testing from the broader community. Any issues identified in RC1 will be fixed during Month 1. At the conclusion of Month 1, the release is packaged and the next ISIS3 release is made available for the general public using Anaconda.org (and the default `main` label).

During Month 1 through Month 3, we continue with new feature development for RC2. At the start of Month 4, we repeat the same release candidate and release process as described above.

### Feature Freeze
When a Release Candidate is branched from the `dev` branch, a feature freeze is put into effect. Any feature additions that occur after a release candidate has been branched will be included in a future RC (and release). In other words, features added prior to the creation of a RC will be included in the next release. The only instances where this may not hold true is if significant, previously unidentified issues are identified during the testing of a RC that are associated with a new feature addition. In that case, we would back out the feature and recreate the RC.

### Release
As described above, we will release on a three month cadence. Releases will be labelled via GitHub for those users that wish to build from scratch. Additionally, releases will be uploaded to our Anaconda.org account for `conda` installation.

### Release Schedule
| Version # / Label | Type | Date | 
|-------------------|------|------------|
| 4.3.0 | Release | 10.26.20 |
| 4.4.0 | Release | 2.8.21 |
| 5.0.0_RC | Release Candidate | 4.1.21 |
| 5.0.0 | Release | 4.27.21 |
| 6.0.0_RC | Release Candidate | 8.1.21 | 
| 6.0.0 | Release | 7.25.21 |
| 7.0.0_RC1 | Release Candidate | 3.4.22 |
| 7.0.0_RC2 | Release Candidate | 4.15.22 |
| 7.0.0 | Release | 5.2.22 |
| 7.1.0_RC | Release Candidate | 6.6.22 |
| 7.1.0 | Release | 7.5.22 |