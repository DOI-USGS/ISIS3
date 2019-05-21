- Feature/Process Name: Removed Documentation from Anaconda Tarball
- Start Date: 12.21.18
- RFC PR: (empty until a PR is opened)
- Issue: https://github.com/USGS-Astrogeology/ISIS3/issues/674#issuecomment-449488114
- Author: jlaura
- **Status: This RFC has been adopted. Documentation is no longer shipped with the ISIS binaries.**

# Summary
The ISIS3 user and developer documentation is currently being shipping in the binary, installable tarballs. This is causing the Linux download to be over 400MB in size and the OSX download to be over 200MB in size. In reality, the binary code is < 100MB for both operating systems.

# Motivation
The anaconda package management system is limited to 5GB per organizational account. We rapidly exceed that amount when a single version (e.g., 3.6.0) requires > 700MB of total storage. As bug fix releases are created (e.g., 3.6.1, 3.6.2, ...) we wish to maintain some number of previous versions on anaconda cloud so that users can easily roll back should a bug fix have unintended side effects.  Therefore, we need the total volume to be as small as is reasonable. 

Finally, we have been in contact with the Anaconda cloud team and it is not possible to purchase additional storage.

# Proposed Solution / Explanation
The ISIS3 user and developer documentation should be removed from the binary distributions being shipped via Anaconda cloud in order to reduce total data volumes. Said documentation must be made widely accessible to the user community though. Therefore, the following options could be utilized:

1. Make the HTML user and developer documentation available via https://usgs-astrogeology.github.io/isis3/<rest_of_the_url>. This can be fully automated in the CI environment and we could utilize [breathe](https://github.com/michaeljones/breathe) to maintain documentation for multiple versions in a manner similar to how the Python community does this.
1. Use pandoc to take the printer friendly documentation and generate a PDF that would be made available via our standard data download mechanisms (perhaps Astropedia?). I would suggest that said document only exist for the user level docs at the present.

# Drawbacks
Some users are accustomed to having local documentation that has historically been shipped with ISIS3. Having this documentation shipped alongside the released code is a convenience to these users.

# Alternatives
Two potential options exist to handle the tarball file size issue: (1) we could clean our anaconda cloud to only store the newest ISIS3 release. This has many issues such as, how are rollbacks supported, how are mission builds supported, how are release candidate builds supported, etc.  (2) We could focus on cleaning the docs to reduce the total file sizes and strip out the doxygen/graphviz generated information. This is non-ideal as the effort to optimize this is non-trivial, the total benefit is unknown, and the information is pertinent.

# Unresolved Questions
This RFC does not address the mechanics of making this happen. Therefore, implementation is wide open.

# Future Possibilities
None currently.