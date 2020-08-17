# ISIS Issue Lifecycle Policy Document

This document describes the issue lifecycle adopted by this project.

## Policy
- An issue has a maximum inactivity duration of 12 calendar months. Issues that have not had additional comments, up voting using the GitHub emoji mechanism, or activity on a linked issue will be closed.
- After six months of inactivity, a comment will be left indicating that the issues is being labelled `inactive` and is a candidate for removal in six months.
- After 11 months a second comment will be left pinging all watchers to either close the issue or perform some activity to reset the issue close countdown.
- After 12 months of inactivity the issue will be closed with an `automatically_closed` tag.

## Enacting this policy
This policy will take effect August 2020. As a one time cleanup all issues >= 11 months in age will be closed with a message to the original poster (sent via GitHub) to reopen the issue, add a reaction emoji, or comment to have the issue go back to being live. In cases of a bug, we will ask the re-opener to confirm they can reproduce on the current ISIS version.

## Associated Issue
This is policy is a result of input from the community of active project contributors. The issue associated with the initial adoption of this policy is [#3951](https://github.com/USGS-Astrogeology/ISIS3/issues/3951).
