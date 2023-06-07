## 1. Deprecation Proposal
Those who wish to deprecate functionality in ISIS should propose the deprecation via an issue on the [ISIS repository](https://github.com/DOI-USGS/ISIS3/issues).  This allows for discussion among the developers and the user community.  If a consensus is met that the functionality should be deprecated, then a deprecation warning should be written.

## 2. Deprecation Warning
A deprecation warning should be presented to the user whenever the deprecated functionality is used.  Though no standard has yet been agreed upon by the developers or the ISIS user base, previous deprecation warnings have taken the form of a simple message printed via `cout`.  In addition to creating the deprecation warning message, the deprecated functionality should also be noted in the [changelog](https://github.com/DOI-USGS/ISIS3/blob/dev/CHANGELOG.md).  This warning should include a link to the issue that proposed the deprecation.

## 3. Removal of Deprecated Functionality
No less than 6 months after the release that includes the deprecation notice, a commit containing the removal of the deprecated functionality should be PR'd into the development branch.  This change should also be captured in the changelog.