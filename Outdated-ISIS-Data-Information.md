### Full ISIS Data Download

The ISIS Data Area is hosted on rsync servers and not through conda channels like the ISIS binaries. This requires using the rsync command from within a terminal window within your Unix distribution, or from within WSL if running Windows 10.  Downloading all mission data requires over 520 GB of disk space. If you want to acquire only certain mission data [click here](#Mission-Specific-Data-Downloads). To download all ISIS data files, continue reading.

To download all ISIS data, enter the following commands in the location where you want to install the ISIS Data Area, for versions of ISIS 4.1.0 and later:

    cd $ISISDATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isisdata/data/ .

For earlier versions, use:

    cd $ISIS3DATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/ .

> Note: The above command downloads all ISIS data including the required base data area and all of the optional mission data areas.

### Partial Download of ISIS Base Data

This data area contains data that is common between multiple missions such as DEMS and leap second kernels. As of ISIS 4.1, the base data area is no longer required to run many applications as data such as icons and templates has been moved into the binary distribution. If you plan to work with any applications that use camera models (e.g., cam2map, campt, qview), it is still recommended you download the base data area. To download the base data area run the following commands:

    cd $ISISDATA
    rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isisdata/data/base .

For versions of ISIS prior to ISIS 4.1.0, please use `isis3data` instead of `isisdata` in the above command.

### Partial Download of Mission Specific Data

There are many missions supported by ISIS. If you are only working with a few missions then you can save disk space by downloading only those specific data areas. If you want to limit the download even further, read the next section about the SPICE Web Service. Otherwise [jump](#Mission-Specific-Data-Downloads) to the mission specific sections.