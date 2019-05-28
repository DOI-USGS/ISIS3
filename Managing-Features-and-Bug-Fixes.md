Emojis 👎

## What's the problem

With the release schedule we are doing constant bug releases and then regularly scheduled feature releases.

```
The future
^
|
@  = 3.8.0
| \
|  \
@ - new sensor model
|   @ - bug fix 3.7.1
|  /
| /
|/
@ - 3.7.0
DEV
```

Keeping bug fixes and feature changes separate, but not separate is causing lots of terrible git merging/cherry-picking/rebasing issues. We had a dev and release branch that had different commits with similar changes and did not merge together well. We now have bug fixes for 3.7.1 and feature changes fro 3.8.0. We need to decide how we are going to manage this right now.

## What are some options

### Back Porting
```
The future
^
@ - bug fix ----cherry----> @
|                           |
|                           |
@ - new sensor model        |
|                           |
|                           |
@ - 3.7.0 ------------------@
``` 

### Forward Porting
```
The future
^                           ^
|                           |
@ <----merge or cherry----- @ - bug fix
|                           |
|                           |
@ - new sensor model        |
|                           |
|                           |
@ - 3.7.0 ------------------@
DEV                       RELEASE
```

### Two PR
```
The future
^                           ^
|         bug fix           |
@ <---PR-----@------PR----> @
|            |              |
|            |              |
@            @ <----------- @ - 3.7.1
|           BUG             |
|                           |
@ - new sensor model        |
|                           |
|                           |
@ - 3.7.0 --- --------------@
DEV                       RELEASE
```

### Feature Branches

Every feature change lives in a separate branch. All bug fixes are made in dev. Before new feature release happens, all feature branches are merged in
```
^
|
@ <-----------------------@
|                         |
@ <------------@          |
|              |          |
|              @          |
@ - 3.7.2      |          |
|              |          |
|              @          |
|              |          |
@ - 3.7.1      |          |
|              |          @
|              @          |
|              |          |
@ - 3.7.0 ---- @ -------- @
DEV         FeatureA   FeatureB
```

### Feature Flags

All changes are made in the dev branch. All new features have a flag associated with them. When the change goes live, the flag is flipped to on.

## what do we want to try right now

Release branch will be called `<major>.<minor>`. So the current release branch will be called `3.7`. When a new feature release happens, pull a new release branch off dev and keep the old release branches around.

We will maintain a list of supported branches that have nightly testing run on them. Whenever a bug fix is made, it must be made on all supported branches. I.E. 3.8 is out, 4.0 is out, and dev is ahead of 4.0. We support 3.8, 4.0, and dev so any bug fix must go into 3.8, 4.0, and dev. We run nightly tests on 3.8, 4.0, and dev.

**Each bug fix version has a tag, each minor version has a branch, and each major version has a branch.**

**Feature changes always go into dev.**

### Choose your own adventure

1. **UPDATE YOUR CLONE FROM UPSTREAM `git fetch upstream`**
1. Choose which supported branch you want to work off of (ie. dev, 3.8, or 4.0). Call this your base branch
1. Checkout new branch from base branch
1. Make changes
1. PR changes into base branch. **PR must be squashed**

For each other supported branch

1. Checkout new branch from supported branch
1. Cherry pick changes from base branch into your new branch
1. Test to make sure bug is still fixed.
1. Make changes as needed
1. PR into supported branch

### The data/testdata area

For right now, we will keep the same structure and workflow in the data/testdata repository.

## The future

With Continuous Deployment we no longer have release branches for each minor version. We instead have release branches for major versions. I.E. 5 is out, 4 is supported, and dev is on 5.2. We have dev and 4 as supported branches.

**Each bug fix version has a tag, each minor has a tag, and each major version has a branch.**