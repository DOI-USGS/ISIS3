<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Working-with-Cassini-RADAR"></span>

# Working with Cassini RADAR [¶](#Working-with-Cassini-RADAR-)

-----

  - [Working with Cassini RADAR](#Working-with-Cassini-RADAR-)
  - [RADAR](#RADAR-)
      - [Instrument Overview](#Instrument-Overview-)
      - [Technical Details](#Technical-Details-)
  - [ISIS3 Processing of PDS Radar
    Products](#ISIS3-Processing-of-PDS-Radar-Products-)
      - [BIDRs (Basic Image Data
        Records)](#BIDRs-Basic-Image-Data-Records-)
          - [Data Ingestion](#Data-Ingestion-)
          - [Reproject the Oblique
            Cylindrical](#Reproject-the-Oblique-Cylindrical-)
      - [BODPs (Burst Ordered Data
        Products)](#BODPs-Burst-Ordered-Data-Products-)
      - [DMP (Digital Map Products)](#DMP-Digital-Map-Products-)

<span id="RADAR"></span>

# RADAR [¶](#RADAR-)

<span id="Instrument-Overview"></span>

## Instrument Overview [¶](#Instrument-Overview-)

Cassini's microwave radio broadcasts are able to penetrate Titan's
atmosphere. The radar observations help to determine the topography and
surface properties. The specific goals of CSS-Radar is to determine
existence of oceans on Titan and their distribution. Radar is also used
to investigate the geologic features and topography of the solid surface
of Titan.

<span id="Technical-Details"></span>

## Technical Details [¶](#Technical-Details-)

**Radar Modes**

The Cassini Radar instrument operates in both passive (radiometer) and
active (altimeter, SAR imaging, scatterometer) modes.

  - **Synthetic Aperture Radar (SAR):**  
    An active mode, in which Cassini can build up images of the surface
    morphology.  
      
    SAR images may be the sharpest that Cassini can achieve on the
    surface of Titan.

  - **Altimetry:**  
    An active mode, in which Cassini pings a radio signal at Titan's
    surface and waits for the echo to return.  
      
    After the effects of the spacecraft's forward motion and Titan's
    overall spherical shape are removed from the data,  
    scientists can produce an altimetric profile along Cassini's track
    over Titan, showing the shape of the landscape.

  - **Scatterometry:**  
    An active, real-aperature mode that produces regional-scale
    backscatter images across large areas of the  
    surface. Scatterometry data are important complement to radiometry
    data with it's independent constraint  
    on surface roughness.

  - **Radiometry:**  
    A passive mode, in which Cassini points at a target and "listens"
    for radio energy emanating from Titan.  
      
    Radiometry roughly correlates with temperature, and radiometry
    results are often described as "brightness temperatures" of
    surfaces.

<span id="ISIS3-Processing-of-PDS-Radar-Products"></span>

# **ISIS3** Processing of PDS Radar Products [¶](#ISIS3-Processing-of-PDS-Radar-Products-)

-----

<span id="BIDRs-Basic-Image-Data-Records"></span>

## **BIDRs** (Basic Image Data Records) [¶](#BIDRs-Basic-Image-Data-Records-)

-----

  - BIDRs are the Single Pass, calibrated and gridded Synthetic Aperture
    Radar (SAR) data
  - Publically released in PDS/RDR format (Recognized in ISIS3 as a
    Level2 Map Projected product).
  - Produced in an oblique cylindrical map projection (coordinate
    system)

<span id="Data-Ingestion"></span>

### Data Ingestion [¶](#Data-Ingestion-)

  - The PDS BIDR's are compressed and have a .ZIP file extention
  - The PDS/RDR BIDR product contains map projection keyword labels that
    are required by ISIS3.
  - The
    [**pds2isis**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/pds2isis/pds2isis.html)
    application is used to ingest into ISIS3.
      - pds2isis translates the Oblique Cylindrical projection mapping
        parameters to the 'Mapping' Group labels of the ISIS3 output
        file.

**Example**

Download the image and label files:

Search the PDS web site in "CORADR\_0045" folder for the files listed
below:  
Find "BIBQH22N068\_D045\_T003S01\_V02.ZIP" and
"BIBQH22N068\_D045\_T003S01\_V02.LBL"  
on the PDS web site [**PDS IMAGE
NODE**](http://pds-imaging.jpl.nasa.gov/) and copy to local work area

Decompress the PDS File:

    unzip BIBQH22N068_D045_T003S01_V02.ZIP

Ingest the PDS/RDR IMG file into ISIS3:

``` 
 pds2isis from=BIBQH22N068_D045_T003S01_V02.IMG to=BIBQH22N068_D045_T003S01_V02.cub
```

<span id="Reproject-the-Oblique-Cylindrical"></span>

### Reproject the Oblique Cylindrical [¶](#Reproject-the-Oblique-Cylindrical-)

The BIDR's are in oblique cylindrical projection and each one has its
own unique mapping parameters, so it is difficult to visualize where
north is on the images. Converting the file to a different map
projection helps with identifying the image coordinates. If the images
are to be mosaicked they need to be reprojected to a common set of map
parameters (map scale, center latitude, etc). See [Learning About Map
Projections](Learning_About_Map_Projections) .

There are two options, use the current mapping information and only
modify the map projection, or redefine the appropriate mapping
parameters to mosaic the images together.

**Example 1** : Convert to a different projection with **map2map** file
for visual inspection:

``` 
 map2map from=BIBQH51S121_D177_T049S01_V02.cub to=BIBQH51S121_D177_T049S01_V02_simp.cub
 map=\$ISIS3DATA/base/templates/maps/simplecylindrical.map 
```

**Example 2** : Define map parameters for mosaicking:

Refer to [Learning About Map
Projections](Learning_About_Map_Projections) for the fundamentals on
defining a map in ISIS3.

Run **maptemplate** to redefine some mapping parameters in order to
mosaic the reprojected images. The center latitude, center longitude,
and map resolution must match. The user will also need to select the
longitude domain (180 or 360).

Note: ISIS3 defaults to a Positive Longitude East direction, the map
template file can be used to specify Positive Longitude West

``` 
 maptemplate map=my_simp.map projection=simplecylindrical clon=0 
 targopt=user targetname=Titan londir=positivewest londom=180
```

The program produces a PVL file containing the following:

``` 
 Group = Mapping
  ProjectionName     = SimpleCylindrical
  CenterLongitude    = 0.0
  TargetName         = Titan
  EquatorialRadius   = 2575000.0 <meters>
  PolarRadius        = 2575000.0 <meters>
  LatitudeType       = Planetocentric
  LongitudeDirection = PositiveWest
  LongitudeDomain    = 180
 End_Group
 End
```

In this example, reproject the file to a simple cylindrical projection
with **map2map** :

``` 
 map2map from=BIBQH51S121_D177_T049S01_V02.cub to=BIBQH51S121_D177_T049S01_V02_simp.cub 
 map=my_simp.map
```

Occasionally map2map will fail if it cannot resolve the default latitude
and longitude extents of the input file. The error message reported
follows:

``` 
 Error message:
  Group = Error
    Program = map2map
    Code    = 1
    Message = "Unable to determine the correct
               [MinimumLongitude,MaximumLongitude]. Please specify these values
               in the [MINLON,MAXLON] parameters"
    File    = map2map.cpp
    Line    = 304
  End_Group
```

In most cases, the error is due to the longitude range falling outside
of the longitude domain that was selected. For example, the 180
longitude domain was chosen but the longitude range for this file goes
beyond 180. Since the map template does not contain the image boundary,
the values in the labels are used. For this case, the latitude and
longitude extents need to be entered by the user in order to remap the
pixels to the correct locations in a 180 longitude domain. The values
entered must be appropriate for the longitude domain selected.

``` 
  Group = Mapping
    ProjectionName     = ObliqueCylindrical
    TargetName         = Titan
    EquatorialRadius   = 2575000.0 <meters>
    PolarRadius        = 2575000.0 <meters>
    LatitudeType       = Planetocentric
    LongitudeDirection = PositiveWest
    LongitudeDomain    = 360
    **MinimumLatitude    = -89.99653831**
    **MaximumLatitude    = -10.56685759**
    **MinimumLongitude   = 0.0**
    **MaximumLongitude   = 360.0**
    UpperLeftCornerX   = -2067342.51008 <meters>
    UpperLeftCornerY   = 4449280.61952 <meters>
    PixelResolution    = 351.11116 <meters/pixel>
    Scale              = 128.0 <pixels/degree>
    Rotation           = 90.0
    PoleLatitude       = -15.827261 <DEG>
    PoleLongitude      = 342.714697
    PoleRotation       = 43.599373 <DEG>
    XAxisVector        = (-0.39349534, 0.59978022, -0.69672456)
    YAxisVector        = (-0.03558647, 0.74735851, 0.66346730)
    ZAxisVector        = (0.91863759, 0.28586526, -0.27273803)
  End_Group
```

Look at the latitude and longitude extents of the file, asterisk-marked
values above, to help determine what values to enter. If the longitude
range goes from 0 to 360 and the longitude domain selected is 180, then
use minlon=-180 and maxlon=180. The only requirement is there be
sufficient work space available to generate a large file.

``` 
  map2map from=BIBQH51S121_D177_T049S01_V02.cub to=BIBQH51S121_D177_T049S01_V02_simp.cub 
  map=my_simp.map minlat=-90.0 maxlat=-10.5 minlon=-180 maxlon=180
```

Crop out only the portion of valid data (non-NULL) in the file to
minimize disk space usage:

``` 
  cropspecial from=BIBQH51S121_D177_T049S01_V02_simp.cub 
  to=BIBQH51S121_D177_T049S01_V02_simpcrop.cub
```

Display the file to view the result:

``` 
 qview BIBQH22N068_D045_T003S01_V02.cub
```

<span id="BODPs-Burst-Ordered-Data-Products"></span>

## **BODPs** (Burst Ordered Data Products) [¶](#BODPs-Burst-Ordered-Data-Products-)

-----

ISIS3 currently does not support processing of these products

  - Data files include engineering telemetry, radar operational
    parameters, raw echo data, instrument viewing geometry, and
    calibrated science data
  - Level1/EDR PDS type format
  - Three different record formats
      - Short Burst Data Record (SBDR)
      - Long Burst Data Record (LBDR)
      - Altimeter Burst Data Record (ABDR)

<span id="DMP-Digital-Map-Products"></span>

## **DMP** (Digital Map Products) [¶](#DMP-Digital-Map-Products-)

-----

The following maps are currently being generated by Astrogeology (ISIS2)
under the direction of Randy Kirk.

  - PRDR - Pass Radiometry Data Record  
    A global map of Titan in Equirectangular (cylindrical) projection
    containing in gridded form the brightness temperature of the surface
    based on one sequence obtained by one Titan flyby pass. PRDR's are
    provided for the scatterometry, altimetry, and SAR sequences as well
    as for radiometry-only scans.

  - PSDR - Pass Scatterometry Data Record  
    The PSDR resembles the PRDR in format and also contains gridded data
    for a single sequence of one Titan flyby. The primary mapped
    quantity in the PSDR is the backscatter cross-section.

  - GRDR - Global Radiometry Data Record  
    The GRDR is a mosaic of gridded radiometric brightness temperature
    data (corrected to normal emission) assembled from the complete set
    of individual PRDR products.

  - GSDR - Global Scatterometry Data Record  
    The GSDR is a mosaic of backscatter cross-section data (corrected to
    a reference incidence angle) assembled from the PSDR's.

  - GTDR - Global Topography Data Record  
    The GTDR is a mosaic of absolute elevation values obtained on
    multiple Titan flybys. The GTDR contains both altimetric data and
    'SAR topography' elevations derived by monopulse analysis of
    overlapping beams in the SAR images.

  - MIDR - Mosaicked Image Data Record  
    Each MIDR is a mosaic of synthetic aperture radar (SAR) image data
    assembled from the BIDR's obtained on multiple Titan passes. The
    MIDR mosaics will be global coverage divided into separate quad
    boundaries.

  - RIDR - Repeat Image Data Record  
    The RIDR product is designed to facilitate comparison of overlapping
    (repeat) SAR image coverage. A RIDR will be produced for every SAR
    image (BIDR), each image will be mapped into the same quad
    boundaries as defined for the MIDR products.

  - DTM - Digital Topographic Model \[Generated using SOCET Set:
    trademark(BAE Systems)\]  
    Where suitable data are available, DTMs will be generated. Each DTM
    is a gridded data product containing absolute or relative elevation
    values.

</div>

<div style="clear:both;">

</div>

</div>

</div>
