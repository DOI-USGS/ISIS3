- Feature/Process Name: ASC Internal Distribution of ISIS
- Start Date: 1/29/19
- RFC PR:
- Issue:
- Author: Jesse Mapel

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary
This RFC is seeking feedback on proposed changes to how builds of ISIS are made available to internal Astrogeology Science Center (ASC) users (anyone working on the ASC network file system).

For versions of ISIS released publicly on [Anaconda Cloud](https://anaconda.org/usgs-astrogeology/isis3), an isolated environment will be created on a globally available Anaconda install that contains that version of ISIS. Internal users can then activate the environment that contains the version of ISIS they would like to use. For internal testing or special internal builds, developers will create a custom install that users can access with the "setisis" script.

Old builds of ISIS will remain in the `/usgs/pkgs` directory, but no new builds of ISIS will be added. Nightly builds will continue to support development but they will **not** be supported for internal users. Weekly beta builds will stop. If an internal user needs access to a bug fix, the bug fix will be released publicly according to [RF2](https://github.com/USGS-Astrogeology/ISIS3/wiki/RFC2:-Release-Process) and then an internal build will be made available according to the previous paragraph. If an internal user needs access to a feature prior to its public release, they need to work with the development team to get a custom build.

# Motivation
The current internal distribution system is based on the old rsync public release and Redmine ticketing systems. We have moved to a new public release system with ISIS 3.6.0 and are in the process of moving from Redmine to Github issues (see this [issue](https://github.com/USGS-Astrogeology/ISIS3/issues/678) for more information); so we should re-evaluate how we distribute ISIS to internal ASC users.

# Proposed Solution / Explanation
Explain the proposal as if it were already included in the code base or in place. This may mean:

- Introducing new named concepts.
- Explaining changes in terms of example usage.
- Explaining how developers or consumers might think about a new feature. Conceptually what is changing?
- If applicable, what might sample error messages, deprecation warnings, or migration guidance look like?
- If applicable, how might this alter current workflows or processes?
- If applicable, a more technical portion that describes:
  - how the feature might interact with other features
  - what the system architecture might look like
  - how testing might be done
  - what corner cases could exist that are not covered
  - this section should link back to the examples provided above

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