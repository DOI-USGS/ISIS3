As of 12/10/18 there are errors for automated processing to download kernels and do other maintenance operations.

* Kernel downloads
** Example cron message: 
*** /usgs/cpkgs/isis3/isis3mgr_scripts/distributeData: line 142: /usgs/pkgs/isis3/install/install/scripts/isis3Startup.sh: No such file or directory
** Fix
*** A work around was done for LROC to resolve the duplicated /install directories.
*** The bug is in /usgs/cpkgs/isis3/isis3mgr_scripts/distributeData's dependencies, resolving this issue will fix the multiple kernel download failures
** The rsync server which distributes these files is full, a set of no longer needed directories which will free up the needed space on the rsync server.
* buildIsisCmakeAllSys
** There are multiple errors for documentation following the testing refactor work.  Is this keeping documentation from building at all?
* Build clean up script
** cleanupBuilds - this is leftover from the old makefile build system.  It needs to be replaced with a cmake cleanup solution
* The real time backup system for the data area was failing, this may have resolved itself on 12/1/18.  Stuart will follow up to verify this is fixed.
* Beta
** The way /beta used to be done was linking to last Friday's build if all tests passed.  With CMake, this solution isn't working anymore.
*** /beta is pulling the dev branch.
*** Should we have Friday's /nightly used as a candidate for /beta?
*** Are we implying to users that /beta is more "reliable" than /nightly?
