<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="SPICE"></span>

# SPICE [¶](#SPICE-)

-----

  - [SPICE](#SPICE-)
      - [Introduction](#Introduction-)
      - [SPICEINIT](#SPICEINIT-)
          - [Required Mission Keywords](#Required-Mission-Keywords-)
          - [ISIS3 SPICE Labels](#ISIS3-SPICE-Labels-)
      - [Camera Pointing](#Camera-Pointing-)
      - [Spacecraft Position](#Spacecraft-Position-)
      - [Shape Model](#Shape-Model-)
      - [References & Related
        Resources](#References--Related-Resources-)

<span id="Introduction"></span>

## Introduction [¶](#Introduction-)

-----

Once an image has been correctly ingested into ISIS3 (meaning that the
ISIS3 cube label contains all required [mission-specific
keywords](fixit.wr.usgs.gov) ), additional navigational and ancillary
information (SPICE) is required in order to calibrate and
geometrically/photometrically process the data. ISIS3 utilizes software
supplied by the Navigation and Ancillary Information Facility (
[NAIF](fixit.wr.usgs.gov) ).

SPICE ( **S** pacecraft & **P** lanetary ephemerides, **I** nstrument
**C** -matrix and **E** vent kernels) refers to all the information that
is required and computed in order for ISIS3 to map each image onto a
surface with reference to spacecraft position, sun position, instrument
and mission activities.

<span id="SPICEINIT"></span>

## SPICEINIT [¶](#SPICEINIT-)

-----

The ISIS3 application
[**spiceinit**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)
adds the necessary SPICE information to an image cube. It is important
to note that if spiceinit is unsuccessful then further cartographic
processing will not be possible.

<span id="Required-Mission-Keywords"></span>

### Required Mission Keywords [¶](#Required-Mission-Keywords-)

At a minimum 'spiceinit' requires SpacecraftName, InstrumentId,
TargetName, StartTime and StopTime. These keywords are loaded at the
ingestion step ( [Importing Mission
Data](Locating_and_Ingesting_Image_Data) ).

See also: [**Mission Specific
Programs**](http://isis.astrogeology.usgs.gov/Application/index.html)

<span id="ISIS3-SPICE-Labels"></span>

### ISIS3 SPICE Labels [¶](#ISIS3-SPICE-Labels-)

  - The information supplied by spiceinit is placed in the
    **Group=Kernels** and **Group=Instrument** label portion of the
    ISIS3 image cube.  
  - Information on the quality of the kernels is added to keywords
    within the Kernels Group (
    [InstrumentPointingQuality](fixit.wr.usgs.gov) and
    [InstrumentPositionQuality](fixit.wr.usgs.gov)
  - SPICE computation results can also be attached to the **blob**
    portion of the ISIS3 image cube (i.e., parameter **ATTACH=TRUE** -
    the current spiceinit default).
  - See also in the ISIS3 Documentation: [**Label
    Dictionary**](http://isis.astrogeology.usgs.gov/documents/LabelDictionary/LabelDictionary.html)

<span id="Camera-Pointing"></span>

## Camera Pointing [¶](#Camera-Pointing-)

-----

Spiceinit will load the location and filename of the camera pointing
kernel (CK) in the ISIS3 keyword **InstrumentPointing** .

Which file that is loaded depends on a couple of things:

First, the StartTime of the image must be found within any CK kernel

Secondly, The user can specify what type of CK kernel

  - CKSMITHED - This is considered camera pointing that has been through
    a bundle-adjustment and most likely the most accurate; this level of
    pointing is not often available and might not include the entire
    collection of mission image data.
  - CKRECON - The default ck kernel for spiceinit. This kernel is the
    mission actual and is often improved on by NAIF or the mission
    navigation team.
  - CKPREDICTED - The least desired, but "better than nothing" kernel
    that is most available during real-time on newly acquired mission
    data.
  - CKNADIR - Nadir pointing is computed if no CK kernels exist
    (''Requirements...under construction'')

**TIPs** :

  - All parameters above can be set to **=TRUE** and spiceinit will
    search and load in a hierarchical order from c-smithed first to
    predicted last. Be aware if processing multiple files and allowing
    inconsistencies in different ck sources.
  - The **InstrumentPointingQuality** keyword will be added to the
    Kernels Group with a value indicating what quality of camera
    pointing was found and loaded for the input image
    (InstrumentPointingQuality = Reconstructed or Predicted).

<span id="Spacecraft-Position"></span>

## Spacecraft Position [¶](#Spacecraft-Position-)

-----

Spiceinit will load the location and filename of the spacecraft pointing
kernel (SPK) in the ISIS3 keyword **InstrumentPosition** .

Which file that is loaded depends on a couple of things:

First, the StartTime of the image must be found within any SPK kernel

Secondly, The user can specify what type of SPK kernel

  - SPKSMITHED - This is considered camera pointing that has been
    through a bundle-adjustment and most likely the most accurate; this
    level of pointing is not often available and might not include the
    entire collection of mission image data.
  - SPKRECON - The default spk kernel for spiceinit. This kernel is the
    mission actual and is often improved on by NAIF or the mission
    navigation team.
  - SPKPREDICTED - The least desired, but "better than nothing" kernel
    that is most available during real-time on newly acquired mission
    data.

**TIPs** :

  - All parameters above can be set to **=TRUE** and spiceinit will
    search and load in a hierarchical order from c-smithed first to
    predicted last. Be aware if processing multiple files and allowing
    inconsistencies in different spk sources.
  - The **InstrumentPositionQuality** keyword will be added to the
    Kernels Group with a value indicating what quality of camera
    pointing was found and loaded for the input image
    (InstrumentPositionQuality = Reconstructed or Predicted).

<span id="Shape-Model"></span>

## Shape Model [¶](#Shape-Model-)

-----

Shape Models are what ISIS intersects imaging rays with. When mapping a pixel from an image to a ground point, ISIS generates a look vector using the camera model and then intersects that ray with the surface to generate a ground point.

The ISIS data area and spiceinit are set up to automatically select an appropriate shape model for most data sets. If want more control over which shape model you use or are working with a new data set, here's the different options available in ISIS.

The most basic type of shape model is an ellipsoid shape model. When using an ellipsoid shape model, ISIS intersects imaging rays with a tri-axial ellipsoid. The ellipsoid radii are defined in the PCK. To use an ellipsoid shape model, set `SHAPE=ELLIPSOID` when running spiceinit.

ISIS also supports Digital Elevation Models (DEMs) as shape models. A DEM is a raster image that defines the local radius at each pixel in the image. DEMs can cover a local region or the entirety of a body. When working with a DEM, make sure that it covers the entire ground range of your data set. Some applications use DEMs that define the height or elevation above a reference ellipsoid, but ISIS only supports DEMs that define the radius so that there is no inconsistency between reference ellipsoids. Before using a DEM, it must first be ingested into an ISIS cube file and then run through the demprep application. Once that is complete, set `SHAPE=USER MODEL=<path to your DEM cube>` when running spiceinit.

Ray tracing shape models

<span id="References-amp-Related-Resources"></span>

## References & Related Resources [¶](#References--Related-Resources-)

-----

1.  About [NAIF](http://naif.jpl.nasa.gov/naif/index.html)
2.  About [NAIF SPICE](http://naif.jpl.nasa.gov/naif/aboutspice.html)
3.  [NAIF Software Toolkit](http://naif.jpl.nasa.gov/naif/toolkit.html)

</div>

<div style="clear:both;">

</div>

</div>

</div>
