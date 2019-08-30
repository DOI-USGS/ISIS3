<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Level0-VIMS"></span>

# Level0 VIMS [¶](#Level0-VIMS-)

-----

  - [Level0 VIMS](#Level0-VIMS-)
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
**Planetary Data System** ( [PDS](PDS) ).

  - [VIMS](CSS)

For Level 0 processing of VIMS image data, ISIS3 expects PDS raw
**Experiment Data Record** ( [EDR](EDR) ) images. EDR data has not been
radiometrically calibrated and contains the "mission-specific" label
keyword information that is required by ISIS3 for cartographic
processing.

The PDS VIMS EDR data will have one file to download for ISIS ingestion,
the image data file (v *img\_number* .qub).  
  
The IR and VIS data are archived together within the same 'cube' file.

<span id="Ingestion"></span>

## Ingestion [¶](#Ingestion-)

-----

In order to cartographically process VIMS image data within ISIS3, the
raw PDS EDR data must be converted to ISIS with the appropriate
application that correctly imports the required mission specific
keywords. The application is
[vims2isis](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/vims2isis/vims2isis.html)
.

**Notes:**

  - The vims2isis application creates two separate output files
    separating the VIS and IR data.
  - PDS releases a detached .lbl with the .qub, ingest the **.qub only**

**Example (includes the specified output cube attribute for band
sequential):**

``` 
  vims2isis from=v1514302573_1.qub  ir=v1514302573_1_ir.cub  vis=v1514302573_1_vis.cub
```

Sample image (v1514302573\_1) is a VIMS observation of Titan
\[Sequence\_Id=S17; Observation\_Id=VIMS\_019TI\_HDAC001\]

Input PDS cube: Samples=64 x Lines=64 x Bands=352 (IR=256 bands; VIS=96
bands)

<span id="SPICE-Spacecraft-amp-Planetary-ephemeredes-Instrument-C-matrix-and-Event-kernels"></span>

## [SPICE ( **S** pacecraft & **P** lanetary ephemeredes, **I** nstrument **C** -matrix and **E** vent kernels)](SPICE_Information) [¶](#SPICE-Spacecraft--Planetary-ephemeredes-Instrument-C-matrix-and-Event-kernels-)

-----

The application
[spiceinit](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)
will add the appropriate SPICE information to the ISIS image cube.
Spiceinit must be applied to both the VIS and IR cubes.

Example:

    spiceinit from=vxxxxx_ir.cub
    spiceinit from=vxxxxx_vis.cub

A successful spiceinit allows further processing:

<span id="Raw-Camera-Geometry"></span>

## Raw Camera Geometry [¶](#Raw-Camera-Geometry-)

-----

Once spiceinit has been successfully applied to a raw ISIS image cube an
abundance of information can be computed and retrieved for geometry and
photometry.

  - [**Camera Geometry Overview and Applications**](Camera_Geometry)

  - [Level1 - Radiometric Calibration](Level1_VIMS)

  - [Level2 - Map Projection](Level2_VIMS)

</div>

<div style="clear:both;">

</div>

</div>

</div>
