Below is a tentative plan for the completion of the CMake work for FY18. If you are interested in grabbing a piece of this to work on, see Jay (@jlaura)

## Baseline Goals: 

* CMake is the build system for the ISIS3 package starting with the first release of FY19.
* All ‘small’ bugs that are currently identified have been squashed.
  - Tests timing out (cam2map related?) - (Jesse to lead)
  - Update protobuf files to protobuf3 (currently 2) - (Jesse to lead)
    - Done
  - Anaconda Qt build is missing PosgreSQL - (Kelvin or Ian, Post GCC update)
  - Linux has to use GCC 4.8.5 - update to a modern gcc - 7.x (whatever the current folks are using)
    - Kelvin has demoed doing this in CSM (meta.yaml and build.sh in https://github.com/USGS-Astrogeology/conda-libcsm/tree/master/recipes and documentation: https://github.com/USGS-Astrogeology/conda-libcsm/pull/1)
  - Code coverage support (https://doc.froglogic.com/squish-coco/3.3/integration.html#sec221)
  - Old process for making Isis Dlm (IDL “library”) cannot be easily ported to cmake (low priority...contact k. Becker- do we have users?) 
    - from K Becker: Used largely by the Europeans. ASU and U of A use it sporadically.
* Release
  - How do we do internal continuous delivery?
  - How do we do external delivery? - conda install ISIS3
* Appropriate deprecation warnings are put into the standard ISIS3 communication channels ASAP and release planning documentation is updated to reflect the planned cut over. (Ian?)
* Personal / Team communications are used to alert: Cassis, ORex, LROC, HiRise, Juno, other? (Jesse)
  - There are a handful of individual scientists who directly fund is for some missions, Hyabusa1/2, Dawn, others?
* Build instructions are updated and an alpha CMake branch made available via GitHub for early adopters. (Kelvin)
  - These instructions should include something like gitflow for early adopters?
* ‘How will this change my life?’ documentation is generated. Need to make sure that Ken and Jeannie know how to use this - maybe they can test the docs?

## Stretch: 
* Jenkins continuous integration is enabled with the ability to inject new test data as needed for individual tests.
  - Connect Jenkins to Git (Jay or Ian)
  - Confirm that Pass/Fail are being passed back to git
  - Passing Test Data
  - Question: How is code coverage being passed?
* Jenkins CI builds are pushed to USGS Anaconda organization for download.
  - Make a conda recipe for ISIS3 (Kelvin)

## Other Concerns:
* When working on deps - can we just note which need to be snowflake builds?
