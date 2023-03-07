This page shows the steps on creating an LTS (Long Term Support) release and supporting older LTS versions. 

Quick rundown of what LTS entails:
- An LTS release is a feature release tagged as `LTS`
- An LTS release occurs every 12 months
- An LTS version is supported for at least 18 months before End of Life (EoL)
- At maximum **two** LTS versions to support concurrently 

For a more in-depth overview regarding our LTS approach, check out [RFC8 - ISIS Long Term Support](https://github.com/USGS-Astrogeology/ISIS3/discussions/4691).

# Instructions to Create a New LTS Release
* Tag the Feature Release as `LTS`


# Instructions to Support Older LTS Version
_**Note: Automation of Steps 1 - 3 is TBD. The PR in Step 3 will be manually reviewed and approved before merge._
 
## Step 1: Check the PRs for Labels
* Any bugfix PR that addressed an issue with the `bug` label should have a `bug` label as well. 
* Manually update PRs with the `bug` label in preparation of the next step.

## Step 2: Cherry-pick the PRs
* Pick and choose the bugfix PRs that will be brought down into the LTS version. 

## Step 3: Create PR
* Create a PR and manually make the selected bugfixes by hand. 
* Be sure to reference the bugfix PRs in the body of this PR. 
* Merge the PR into the LTS version of interest.

## Step 4: Increment LTS Patch Version
* Increment the patch version of the LTS build.
* Release the newly versioned build via Anaconda.
* Further guidelines on the [Public Release Process](https://github.com/DOI-USGS/ISIS3/wiki/Public-Release-Process)
