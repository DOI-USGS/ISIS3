<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="HiRISE-Anaglyphs"></span>

# HiRISE Anaglyphs [¶](#HiRISE-Anaglyphs-)

-----

Using Stereo Observations: RED CCD\[0-9\]

**Note** : This anaglyph procedure is recommended for lowerlatitude
coverage; Polar region stereo coverage is more complicated and has not
been ‘stream-  
lined’ yet.

  - [HiRISE Anaglyphs](#HiRISE-Anaglyphs-)
      - [Preparation - SPICE](#Preparation-SPICE-)
          - [Run spicefit on all Red CCD’s 0-9 for both image
            observations](#Run-spicefit-on-all-Red-CCDs-0-9-for-both-image-observations-)
          - [Retrieve the “local radius”](#Retrieve-the-local-radius-)
          - [Modify Labels](#Modify-Labels-)
          - [Kernels Group with modified
            keywords](#Kernels-Group-with-modified-keywords-)
      - [Map Projection - Template](#Map-Projection-Template-)
      - [Map Projection](#Map-Projection-)
          - [Create Two Red\[0-9\] Mosaics](#Create-Two-Red0-9-Mosaics-)
          - [Assumptions](#Assumptions-)
      - [Stack Left/Right Observations](#Stack-LeftRight-Observations-)

<span id="Preparation-SPICE"></span>

## Preparation - SPICE [¶](#Preparation-SPICE-)

-----

  - Select two stereo observations
  - Begin with calibrated images
  - Geometric control is an option (ccd-to-ccd)
  - Run spiceinit on all RED CCD’s0-9 for both observations
      - Default to system shape model
      - Kernels Group with loaded SPICE keywords

<!-- end list -->

``` 
 Group = Kernels 
  NaifIkCode = -74699
  LeapSecond = $base/kernels/lsk/naif0008.tls  
  TargetAttitudeShape = $base/kernels/pck/pck00008.tpc
  TargetPosition = Table 
  InstrumentPointing = Table
  Instrument = Null 
  SpacecraftClock = $mro/kernels/sclk/MRO_SCLKSCET.00021.65536.tsc
  InstrumentPosition = Table 
  Frame = $mro/kernels/fk/mro_v08.tf
  InstrumentAddendum = $mro/kernels/iak/hiriseAddendum003.ti 
  ShapeModel = $base/dems/molaMarsPlanetaryRadius0001.cub
 End_Group
```

<span id="Run-spicefit-on-all-Red-CCDs-0-9-for-both-image-observations"></span>

### Run spicefit on all Red CCD’s 0-9 for both image observations [¶](#Run-spicefit-on-all-Red-CCDs-0-9-for-both-image-observations-)

  - [spicefit](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spicefit/spicefit.html)
    from=psp\_red0.cub...
  - Take ‘an’ approach to figure out the map information

<!-- end list -->

``` 
 maptemplate fromlist=img1_0-9.lis map=img1.map proj=EQUIRECTANGULAR clat=0.0 clon=0.0 rngopt=CALC resop=CALC 
 maptemplate fromlist=img2_0-9.lis map=img2.map proj=EQUIRECTANGULAR clat=0.0 clon=0.0 rngopt=CALC resop=CALC 
```

  - Evaluate the information within img1.map and img2.map
  - Figure out the map values that will satisfy the coverage of both
    observations
      - latitude range, longitude rangecenter latitude, center longitude
      - average pixel resolution

<span id="Retrieve-the-local-radius"></span>

### Retrieve the “local radius” [¶](#Retrieve-the-local-radius-)

  - The default system shape model will be referenced (spiceinit)
  - Specify the center of each observation

<!-- end list -->

``` 
 campt from=psp_red5_img1.cub samp=1 line=(nlines/2) to=red5_pt1.dat 

 getkey from=red5_pt1.dat grpnname=GroundPointkeyword=LocalRadius 

 campt from=psp_red5_img2.cub samp=1 line=(nlines/2) to=red5_pt2.dat  

 getkey from=red5_pt2.dat grpname=GroundPointkeyword=LocalRadius 
```

  - Calculate the average “local radius” value
  - Convert the average value from meters to kilometers
  - Build a NAIF-format text file with the average local radius (km)
  - *“LocalRad3391.97.tpc”contents*

<!-- end list -->

``` 
 \begindata 
 BODY499_RADII       = ( 3391.97   3391.97 3391.97)
```

<span id="Modify-Labels"></span>

### Modify Labels [¶](#Modify-Labels-)

  - For both observations; all Red CCD’s0-9
  - Remove ShapeModel reference from labels

<!-- end list -->

``` 
 editlab from=psp_red0_img1.cub options=modkeygrpname=Kernels keyword=ShapeModel value=Null 
```

  - Now add the “new” NAIF .tpcfile with the local radius to the Kernels
    group

<!-- end list -->

``` 
 editlab from=psp_red0_img1.cub options=modkeygrpname=Kernels keyword=Instrument keyvalue= LocalRad3391.97.tpc 
```

  - **Note** : We are temporarily using an ‘unused’ keyword (Instrument)
    for the local radius .tpcfile; this keyword follows the main
    “TargetAttitudeShape”keyword which MUST retain reference to the
    original NAIF .tpcfile (we are mimicking a ‘search’ list for radius
    value).

<span id="Kernels-Group-with-modified-keywords"></span>

### Kernels Group with modified keywords [¶](#Kernels-Group-with-modified-keywords-)

``` 
 Group = Kernels
  NaifIkCode = -74699 
  LeapSecond = $base/kernels/lsk/naif0008.tls
  TargetAttitudeShape = $base/kernels/pck/pck00008.tpc  
  TargetPosition = Table
  InstrumentPointing = Table  
  Instrument          = LocalRad3386.70.tpc
  SpacecraftClock = $mro/kernels/sclk/MRO_SCLKSCET.00021.65536.tsc 
  InstrumentPosition = Table
  Frame               = $mro/kernels/fk/mro_v08.tf 
  InstrumentAddendum = $mro/kernels/iak/hiriseAddendum003.ti
  ShapeModel = Null 
 End_Group
```

  - [![Hirise\_local\_radius\_both\_high\_and\_low.png](attachments/thumbnail/981/300.png)](attachments/download/981/Hirise_local_radius_both_high_and_low.png "Hirise_local_radius_both_high_and_low.png")  
    Local Radius retains both high and low frequency stereo data

  - [![Local\_Radius\_bad\_example.png](attachments/thumbnail/978/300.png)](attachments/download/978/Local_Radius_bad_example.png "Local_Radius_bad_example.png")  
    The mapping results for left/right views are too far apart

  - [![Hirise\_stereo\_information\_removed.png](attachments/thumbnail/979/300.png)](attachments/download/979/Hirise_stereo_information_removed.png "Hirise_stereo_information_removed.png")  
    All stereo information removed

  - [![Hirise\_low\_freq\_removed\_gentle\_slopes.png](attachments/thumbnail/980/300.png)](attachments/download/980/Hirise_low_freq_removed_gentle_slopes.png "Hirise_low_freq_removed_gentle_slopes.png")  
    Low frequency stereo information is removed; (e.g., gentle slopes
    across the scene)

![](attached_Gullies_MOLA_Stereo_Profile.png)

<span id="Map-Projection-Template"></span>

## Map Projection - Template [¶](#Map-Projection-Template-)

-----

  - Build a Map Template for
    [cam2map](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cam2map/cam2map.html)
  - Specify the Ellipsoid values for Mars Contents:

<!-- end list -->

``` 
 :::::::::::::: 
 equi.map
 :::::::::::::: 
 Group = Mapping
  ProjectionName = Equirectangular 
  CenterLongitude = 353.18
  CenterLatitude = 8.06 
  TargetName = mars
  EquatorialRadius = 3396190.0 <meters> 
  PolarRadius = 3376200.0 <meters>
  LatitudeType = Planetocentric 
  LongitudeDirection = PositiveEast
  LongitudeDomain = 360 
  PixelResolution = 0.28 <meters/pixel>
 End_Group 
 End
```

<span id="Map-Projection"></span>

## Map Projection [¶](#Map-Projection-)

-----

  - Run
    [cam2map](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cam2map/cam2map.html)
    on both observations
      - all Red ccd’s\[0-9\]

<!-- end list -->

``` 
 cam2map from=psp_red0_img1.cub map=equi.mapto=psp_red0_eq.cub pixres=map defaultrange=camera 
```

  - **OR** Use the -batchlist option

<!-- end list -->

``` 
 cam2map –batchlist=all_reds.lis from=\$1.balance.cub to=\$1.eq.cub pixres=map defaultrange=camera 
```

  - all\_reds.lis should not include file extensions
  - Allow cam2map to figure out the lat/lon boundaries of each
    individual ccd

<span id="Create-Two-Red0-9-Mosaics"></span>

### Create Two Red\[0-9\] Mosaics [¶](#Create-Two-Red0-9-Mosaics-)

  - Create a list of cam2map output files for each observation
  - Mosaic the projected Red CCD’s
  - Specify the same latitude/longitude boundaries for the output
    mosaics

<!-- end list -->

``` 
 automos fromlist=img1_eq.lis mosaic=img1_RedMos.cub grange=user minlat=  maxlat=  minlon=  maxlon= 
 automos fromlist=img2_eq.lis mosaic=img2_RedMos.cub grange=user minlat=  maxlat=  minlon=  maxlon=
```

  - Refer to the camptoutput files that was executed previously

<!-- end list -->

``` 
 getkey from=red5_pt1.dat grpnname=GroundPointkeyword=SubSpacecraftLongitude 
 getkey from=red5_pt2.dat grpnname=GroundPointkeyword=SubSpacecraftLongitude 
```

  - The observation image whose SubspacecraftLongitude is the farthest
    West, is the “Left Look”
      - (subspc1 \< subspc2) or (subspc2 \< subspc1)

<span id="Assumptions"></span>

### Assumptions [¶](#Assumptions-)

  - Observation pair has a longitude range within 0 –360 longitude
  - LongitudeDomain= 360
  - Neither image pair crosses the 0 and/or the 360 longitude boundary
  - [cam2map](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cam2map/cam2map.html)
    defaults to reassigning the LongitudeDomain if the image crosses a
    0/360/180 boundary

<span id="Stack-LeftRight-Observations"></span>

## Stack Left/Right Observations [¶](#Stack-LeftRight-Observations-)

-----

  - Create a text file for
    [cubeit](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cubeit/cubeit.html)

<!-- end list -->

``` 
 ls left_redmos_img.cub > c.lis
 ls right_redmos_img.cub >> c.lis 
```

  - cubeit list=c.lis to=anag.cub
  - Display anag.cub with qview

<!-- end list -->

``` 
 Band1 = Red
 Band2 = Green 
 Band2 = Blue
```

</div>

<div class="attachments">

<div class="contextual">

</div>

[Local\_Radius\_bad\_example.png](attachments/download/978/Local_Radius_bad_example.png)
[View](attachments/download/978/Local_Radius_bad_example.png "View")
<span class="size"> (436 KB) </span> <span class="author"> Ian Humphrey,
2016-05-31 04:22 PM </span>

[Hirise\_stereo\_information\_removed.png](attachments/download/979/Hirise_stereo_information_removed.png)
[View](attachments/download/979/Hirise_stereo_information_removed.png "View")
<span class="size"> (258 KB) </span> <span class="author"> Ian Humphrey,
2016-05-31 04:22 PM </span>

[Hirise\_low\_freq\_removed\_gentle\_slopes.png](attachments/download/980/Hirise_low_freq_removed_gentle_slopes.png)
[View](attachments/download/980/Hirise_low_freq_removed_gentle_slopes.png "View")
<span class="size"> (377 KB) </span> <span class="author"> Ian Humphrey,
2016-05-31 04:22 PM </span>

[Hirise\_local\_radius\_both\_high\_and\_low.png](attachments/download/981/Hirise_local_radius_both_high_and_low.png)
[View](attachments/download/981/Hirise_local_radius_both_high_and_low.png "View")
<span class="size"> (829 KB) </span> <span class="author"> Ian Humphrey,
2016-05-31 04:22 PM </span>

[Gullies\_MOLA\_Stereo\_Profile.png](attachments/download/982/Gullies_MOLA_Stereo_Profile.png)
[View](attachments/download/982/Gullies_MOLA_Stereo_Profile.png "View")
<span class="size"> (123 KB) </span> <span class="author"> Ian Humphrey,
2016-05-31 04:23 PM </span>

</div>

<div style="clear:both;">

</div>

</div>

</div>
