<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Level-0-ISS"></span>

# Level 0 ISS [¶](#Level-0-ISS-)

-----

  - [Level 0 ISS](#Level-0-ISS-)
      - [Data Acquisition](#Data-Acquisition-)
      - [Ingestion](#Ingestion-)
      - [SPICE (Spacecraft & Planetary ephemeredes, Instrument C-matrix
        and Event
        kernels)](#SPICE-Spacecraft--Planetary-ephemeredes-Instrument-C-matrix-and-Event-kernels-)
      - [Raw Camera Geometry](#Raw-Camera-Geometry-)

<span id="Data-Acquisition"></span>

## Data Acquisition [¶](#Data-Acquisition-)

-----

A major official source for Cassini ISS image data is through the
**Planetary Data System** ( [PDS](http://pds.nasa.gov/) ).

  - [ISS](CSS_)

For Level 0 processing of ISS image data, ISIS3 expects PDS raw
**Experiment Data Record** ( [EDR](EDR) ) images. EDR data has not been
radiometrically calibrated and contains the "mission-specific" label
keyword information that is required by ISIS3 for cartographic
processing.

The PDS ISS EDR data will have two files to download for ISIS, a
detached PDS label file (img\_number.LBL) and the image data file
(img\_number.IMG).

<span id="Ingestion"></span>

## Ingestion [¶](#Ingestion-)

-----

In order to cartographically process ISS image data within ISIS3, the
raw PDS EDR data must be converted to ISIS with the appropriate
application that correctly imports the required mission specific
keywords. The application is
[ciss2isis](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/ciss2isis/ciss2isis.html)
.

Example:

    ciss2isis from=xxxx.LBL  to=xxxx.cub

Note:

  - Both the .LBL and .IMG must be in the same directory together
  - The LBL and IMG extensions must be in upper or lower case consistent
    with each other (.LBL/.IMG or .lbl/.img).

<span id="SPICE-Spacecraft-amp-Planetary-ephemeredes-Instrument-C-matrix-and-Event-kernels"></span>

## [SPICE (Spacecraft & Planetary ephemeredes, Instrument C-matrix and Event kernels)](SPICE) [¶](#SPICE-Spacecraft--Planetary-ephemeredes-Instrument-C-matrix-and-Event-kernels-)

-----

The application
[spiceinit](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)
will add the appropriate SPICE information to the ISIS image cube.

Example:

    spiceinit from=xxxx.cub

Successful spiceinit allows for further processing of ISS data:

  - [Level1 - Radiometric Calibration](Level1_ISS)
  - [Level2 - Map projection](Level2_ISS)

<span id="Raw-Camera-Geometry"></span>

## Raw Camera Geometry [¶](#Raw-Camera-Geometry-)

-----

Once spiceinit has been successfully applied to a raw ISIS image cube
and abundance of information can be computed and retrieved for geometry
and photometry.

  - [Camera Geometry Overview and Applications](Camera_Geometry)

</div>

<div style="clear:both;">

</div>

</div>

</div>
