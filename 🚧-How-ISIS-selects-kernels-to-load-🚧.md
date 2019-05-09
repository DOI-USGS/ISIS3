ISIS3 selects the SPICE kernels to be used with an ISIS cube as part of running the spiceinit application.

The user can enter kernels they would like to load via `spiceinit` (either by hand, or with a parameter file) or use the spice server, but that is not covered by this write up. This documents what happens when `spiceinit` selects kernels from the default location (`$ISIS3DATA/mission/kernels` or a different location specified by the `IsisPreferences` file) using the ISIS3 kernel databases. 

In spiceinit, the `KernelDb` class is used to select which kernels to load. This class can be called directly to query the ISIS3 kernel databases if desired.

First, the allowed kernel “types” are specified for both cks and spks: 

* nadir
* predicted
* reconstructed
* smithed

kernels of a non-allowed type will not be selected, and the highest quality kernel that meets the other selection criteria will be returned.

Here, these are in order of lowest to highest quality kernel category. For more information about these kernel quality categories, and the spiceinit application, please see: https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html

1. The location of the correct kernel database file to use is determined using the mission name, the cube’s label (to get the `InstrumentID`,) and the user's `IsisPreferences` file. There is one kernel.db file for each "type" of kernel needed by the image: ck, spk, fk, ik, iak, ... but these are simple files for all but the cks and spks (more detail to be added here.) 

2. The most recent kernel database file in the appropriate directory is loaded (unless there is a kernel configuration file -- see below). Kernel databases are updated when new kernels become available or there is a change to which kernels need to be loaded communicated from the team. 

3. The `StartTime` and `StopTime` keywords from the `Instrument` group in the input cube label are used to search through the available kernels as specified by PVL groups in the kernel database file and determine the best match. `StopTime` is an optional keyword in ISIS3 cubes, so if it is not available, it is set equal to the `StartTime`

1-alternate route: A kernel configuration file is a file of the form `kernels.????.conf` that contains information about which kernel database files to load in which cases.

From an infrastructure perspective (probably not relevant here, but there are some obvious questions that the above brings up and these may help to answer some of them): 

In the ISIS3 data area, shell scripts within each ck and spk directory that receive new kernels from an automated download script are run to re-generate kernel database files when new kernels are downloaded. These scripts are usually named `makedb` and call the ISIS3 application makedb. After update, in most cases, these new kernels and kernel database files are immediately pushed to the rsync server. 
