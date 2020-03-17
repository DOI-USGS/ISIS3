- Feature/Process Name: Migration of ISIS Data to Git
- Start Date: 5.21.19
- RFC PR: (empty until a PR is opened)
- Issue: [https://github.com/USGS-Astrogeology/ISIS3/issues/3727](https://github.com/USGS-Astrogeology/ISIS3/issues/3727) (Please comment here!)
- Author: Laura

# Updates
- [2020-03-16](#Update_2020-03-16) : Current plan, status, expected feature release

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
The ISIS data area (test data and spice/base data) are currently not under standard version control. Additionally, the data areas are not able to modified (via PR) by external users. This RFC proposes moving the data areas under publicly accessible version control using GitLab and Amazon S3 buckets.

# Motivation
Currently, the test data are not under version control. We do have periodic backups, but these are not under standard version control that supports rollbacks, PRs, branches, etc. Therefore, we have an open source project that does not have an open source data component. This change is motivated by the desire to have the entirety of ISIS under version control.

# Proposed Solution / Explanation
Users wishing to utilize ISIS or make changes to the data area would have a Git Large File Support (LFS;[https://git-lfs.github.com](https://git-lfs.github.com)) enabled repository from which they could pull data. The proposed solution would require that users install both a standard git client, and the [git lfs extension](https://git-lfs.github.com). Once installed, a clone of the isis test data would look like:

```bash
$ git clone https://astroservices.usgs.gov/gitlab/isistestdata
```
[Source](https://www.atlassian.com/git/tutorials/git-lfs#clone-respository).
The LFS directory and default branch (usually master) would be cloned locally for the user. The cloned repository is then identical to the current ISIS3Test data directory as it currently stands.

Similarly, mission data (e.g., the SPICE data) will be available in a a separate GitHub repo (maybe even a 1:1 repo to mission mapping) to allow users to clone mission data.

For users wishing to propose changes, this model will support pull requests. We recently saw a user requesting an update to an ISIS translation table. This was a simple fix that could have been an externally initiated PR had the data been under version control. We also see instances where custom build (typically a pre-release for a mission team) would benefit from a custom data area. We can achieve this using branches in a git-like model. Finally, we have an implicit hard mapping between data area version and code release version. It would be valuable to make this an explicit mapping and adopt a standard release cycle for both code and data. We saw this issue crop up with the recent upgrade to the .bsp files used by ISIS and a change in Mars barycenter.

# Drawbacks
  - For organizations that maintain a shared data area, the use of Git and explicitly versioned data would require that the organization's ISIS installation be kept in sync with the data area. For example, updating to version x.y.z would require an identical update of the data area. If multiple users wish to utilize different versions, they would need different data area.
  - For developers, the ~60GB ISIS test area would need to be managed locally.
  - Use of LFS (and the underlying S3 buckets) has a non-trivial cost. 

# Alternatives
  - Continue to maintain the rsync servers and have users rsync the data without any versioning capability.

# Unresolved Questions
  - What would the workflow for initial pulls, PRs, etc. look like in a GitLFS model? For an initial overview on using LFS, [see this tutorial](https://github.com/git-lfs/git-lfs/wiki/Tutorial).
  - How would CI work? Would the CI make a checkout of a data area, identified by hash and then use said data area for all tests?
  - What would the impact be to the external user base to migrate to a Git-like solution for data?
  - What does a reasonable project / directory structure look like for data? For example, do we mirror the 1:1 NAIF:Mission mapping or do we push everything into a single directory and then provide instructions to users on how to download?

# Future Possibilities
  - Refactor the SPICE data area to remove the *_v00x.<ext> versioning and go to a true git-like solution. We would need to do a cost/benefit analysis on this see if any meaningful data volume savings were achievable.

# Update_2020-03-16
Plan
Status