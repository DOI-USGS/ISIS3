# ISIS Contributing Guide

The goal of this document is to create a contribution process that:

* Encourages new contributions.
* Encourages contributors to remain involved.
* Avoids unnecessary processes and bureaucracy whenever possible.
* Creates a transparent decision making process which makes it clear how
contributors can be involved in decision making.


## Vocabulary

* A **Contributor** is any individual creating or commenting on an issue or pull request.
* A **Committer** is a subset of contributors who have been given write access to the repository.

# Logging Issues

Log an issue for any question or problem you might have. When in doubt, log an issue,
any additional policies about what to include will be provided in the responses. The only
exception is security disclosures which should be sent privately.

Committers may direct you to another repository, ask for additional clarifications, and
add appropriate metadata before the issue is addressed.

Please be courteous, respectful, and every participant is expected to follow the
project's Code of Conduct.

# Contributions

Any change to resources in this repository must be through pull requests. This applies to all changes
to documentation, code, binary files, etc.

No pull request can be merged without being reviewed.

The default for each contribution is that it is accepted once no committer has an objection.
During review committers may also request that a specific contributor who is most versed in a
particular area gives a "LGTM" before the PR can be merged. There is no additional "sign off"
process for contributions to land. Once all issues brought by committers are addressed it can
be landed by any committer.

In the case of an objection being raised in a pull request by another committer, all involved
committers should seek to arrive at a consensus by way of addressing concerns being expressed
by discussion, compromise on the proposed change, or withdrawal of the proposed change.

When making a bug fix contribution, please add the [bug](https://github.com/USGS-Astrogeology/ISIS3/pulls?q=is%3Apr+label%3Abug+) label to your pull request. This will automatically cherry-pick your PR into the latest release branch so that it can be distributed in the next bug fix release. 

Every pull request making changes to ISIS's source needs a [changelog entry](https://raw.githubusercontent.com/DOI-USGS/ISIS3/dev/CHANGELOG.md). PRs are limited to one changelog entry per PR. If you have multiple bug fixes or feature adds, they should be split up. If PRs need to be merged in a particular order, include that as a comment on your PRs. If PR is one changelog, but high-volume changes such as large feature branches (many changes across many files requiring several hours of review time), then the PR isn't guaranteed to be reviewed promptly. Authors should contact the repo owners, see [code.json](https://github.com/DOI-USGS/ISIS3/blob/dev/code.json), to coordinate reviews outside typical USGS Astro support hours. We plan out our schedules in 6 week increments, so plan to coordinate with repo owners a minimum of 6 weeks before you expect the changes to be ready. 

When making a contribution for the first time, please add your name to the `.zenodo.json file.` We strongly recommend adding your affiliation and ORCiD to the `zenodo.json` file. These additions only have to happen once.

