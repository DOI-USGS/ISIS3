- Feature/Process Name: ISIS Long Term Support Releases
- Start Date: October 14, 2021
- RFC PR:
- Issue: [#4653](https://github.com/USGS-Astrogeology/ISIS3/issues/4653)
- Author: Jesse Mapel

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
A proposal for a new Long Term Support (LTS) model for ISIS releases. If accepted, every 12 months a new LTS version of ISIS will be released. The LTS version will receive bug fixes and critical security updates but no feature additions or breaking changes. When a new LTS version is released, the previous LTS version will be supported for 6 more months before it reaches End of Life (EoL). Each LTS version will be supported for at least 18 months. The following diagram gives an overview of what this could look like.

![ISIS_LTS drawio](https://user-images.githubusercontent.com/22942817/137381387-1c1591eb-296a-41f1-99a7-57e164b4c5b5.png)

# Motivation
The current ISIS release process is a quarterly feature release with bug fix releases into the latest feature release. This process has been successful at rapidly getting code changes released and available for users, but more rapid releases has come with new challenges.

First, the user base has become split across more versions of the software. Users are generally using the latest release for day-to-day work. For larger projects, users are sticking with a single version that they chose at the start of the project and then infrequently updating. The speed at which users update varies with some users updating every few months and others updating every few years. More rapid releases created a barrier to choosing which release to target for upgrades; there are too many options that do not appear distinct.

Second, each feature release only receives bug fixes for 3 months. If a user is working with a version of ISIS that is older than 3 months and discovers a blocking bug, reports it, and the developers fix it; that user must accept new feature changes and potentially breaking changes to continue their work. For day-to-day work, this is usually not a problem, but for large projects that involve extensive processing, the effort needed to upgrade grows rapidly.

# Proposed Solution / Explanation
To alleviate these problems, we propose adding Long Term Support to one feature release each year. Long Term Support (LTS) for a release means that bug fixes and major security fixes are backported to that release for the duration of its support. Feature releases that receive Long Term Support are called LTS versions. The development environment, test data, and data area are also maintained for LTS versions. Each LTS version will be supported for 18 months so that there is a 6 month overlap between LTS versions. When support ends, the LTS version will be End of Life (EoL) and no longer receive bug fixes or major security fixes. During the overlap between LTS versions, users can upgrade and any bugs in the new LTS version can be fixed prior to the previous LTS version's End of Life (EoL).

This provides two release channels: a quarterly feature release that allows users to quickly get the latest changes to the software and an annual LTS release that users can plan larger projects and upgrades around. The user community can more easily share testing and bug reports are more consistent because more users are on the same version of the software. Users can also plan their projects and software upgrades around a slower annual cycle while still receiving fixes for any bugs they encounter.

If a bug fix is too onerous to backport to an LTS version, then the developers may label it as not being back ported. This is used only in extreme circumstances and can be re-evaluated if the bug fix has significant importance. The lifetime of LTS versions has been chosen to minimize the need for this, but because development time is limited and changes between future versions are mostly unknown, some exceptions are allowed. If a bug fix is not backported to an LTS version, it will be noted in the release notes for the next release.

Additionally, if there have not been significant changes in the software, an LTS version can be postponed for up to 1 year. If this happens, the support for the current LTS version will be extended until 6 months after the postponed LTS version is released. An LTS version cannot be postponed within 3 months of its expected release.

The table below gives an example release schedule

## Example Release Schedule
| Version # | Type | Date | Notes |
|--------|------------|-----------|------------------|
| 6.1.0 | Release | Oct 2021 | |
| 6.2.0 | LTS Release | Jan 2022 | 6.2.0 LTS starts |
| 6.3.0 | Release | Apr 2022 | |
| 7.0.0 | Release | July 2022 | |
| 7.1.0 | Release | Oct 2022 | |
| 7.2.0 | LTS Release | Jan 2023 | 7.2.0 LTS starts |
| 7.3.0 | Release | Apr 2023 | |
| 7.4.0 | Release | July 2023 | 6.2.0 EoL |
| 7.5.0 | Release | Oct 2023 | |
| 8.0.0 | LTS Release | Jan 2024 | 8.0.0 LTS starts |
| 8.1.0 | Release | Apr 2024 | |
| 8.2.0 | Release | July 2024 | 7.2.0 EoL |
| 9.0.0 | Release | Oct 2024 | |
| 9.1.0 | LTS Release | Jan 2025 | 9.1.0 LTS starts |

# Drawbacks
The largest drawback to adding LTS is complexity. Users need to know about the LTS versions and understand what LTS includes. Developers need to support 2 to 3 versions of the code base and the accompanying environments. This adds friction to the installation, support, and development processes.

Adopting LTS will only help a subset of the user community. For users who keep up with the current update schedule this adds no value. On the other end of the spectrum, users that cannot maintain an annual upgrade pace will receive more support but will still be using an unsupported version of the software for some amount of time.

Maintaining the development environment for LTS versions is potential a big challenge. ISIS has many dependencies and since transitioning to using Anaconda, they are updated more frequently. Because dependencies are not controlled or distributed by the development team, we cannot guarantee that the installation and development environments that Anaconda installs will remain fixed for the lifetime of an LTS version. This could require additional changes to an LTS version to ensure the environments still work the same as when the LTS version was first released.

# Alternatives
Long Term Support is the standard release and support model for foundational software like operating systems, compilers, and languages. The only alternative broadly used is the current ISIS release policy where releases are supported until the next release. So, the only major alternative is not doing anything. This does seem tenable and will leave a good amount of the user base with significant challenges.

There are some alternatives for tweaking numbers. Many software projects provide longer LTS lifetimes or even multiple concurrent LTS versions. These numbers were chosen based on the current funded support levels for ISIS. As the LTS lifetime gets longer, backporting becomes much riskier and more challenging. Similarly, with more concurrently supported LTS versions, development time increases and maintenance overhead grows. If support for ISIS grows and the community needs it, these numbers can be re-evaluated in the future.

# Unresolved Questions
There are a handful of questions related to this proposal:

 - Does this change impact the Release Candidate process?
 - Do we need more testing for LTS releases vs regular releases?
 - Do we want to change our versioning to be based on LTS releases and not semantic versioning?

 None of these are stopping this proposal from moving forward and they can be answered during or after implementation.

# Future Possibilities
The largest follow up to this proposal is moving to continuous deployment. We already have continuous integration for ISIS, so continuous deployment is the obvious next step. If we do move to continuous deployment does that impact long term support or just the release in between LTS versions?