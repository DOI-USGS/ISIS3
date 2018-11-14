# Release Plan for ISIS3.6.0 and Above

## Policies for PRs into dev 
In order to facilitate the release team, PRs merged into dev that break the following nightly will
need to be unmerged to keep dev more stable. We will need to determine how to do this for multiple
PRs merged in a single day.

## Create a freeze date
Announce a freeze date internally via astroprogrammers email to inform the internal development team
of a hard-deadline for feautres and fixes to be in dev.

This ensures that dev is "clean" and that the development team has the time to successfully
integrate features and fixes into dev that will be cut into the upcoming release.

This also allows us to annouce an estimated release date (e.g. roadmap) to the public. Depending
on process and process refinement, the release-candidate date can be estimated to be 2-4 weeks
after the freeze date, the the release following 1-2 weeks after (this is all subject to change
and open for internal discussion).

## On the freeze date
At the freeze date, the source code and test data will need to be cut into a release candidate.
1. Create a release branch from dev on freeze date at 00:01
1. Mv the old /usgs/cpkgs/isis3/isis3.X.XTestData to the new version /usgs/cpkgs/isis3/isis3.X.YTestData
1. rsync the /usgs/cpkgs/isis3/testData to the new version /usgs/cpkgs/isis3/isis3.X.YTestData
1. Add a cronjob as isis3mgr@prog24 to build and test the release branch with the custom data area
1. Make sure that there is a symlink to the release candidate for internal testing.

## After the freeze
After the freeze date, verify and fix any issues with the release. This could entail updating 
test data, fixing any new infrastructure slated for the release, fixing bugs in the release, etc.
Any changes that are version controlled need to be PR'd into release and then subsequently PR'd
into dev.

## The release branch is clean and tested.









## Conda build

Modify the `isis3_dependencies/recipes/isis/meta.yaml` as follows:
* If this is a **new** version of isis3 (e.g. isis3.6.0 to isis3.6.1), update the `version` variable and set build number to 0
* Otherwise, if this is a **patch** of an **existing** version of isis3, increment the build nummber 