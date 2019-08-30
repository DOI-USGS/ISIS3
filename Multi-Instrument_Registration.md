<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Multi-Instrument-Registration"></span>

# Multi-Instrument Registration [¶](#Multi-Instrument-Registration-)

-----

  - [Multi-Instrument Registration](#Multi-Instrument-Registration-)
      - [Reasons for merging datasets](#Reasons-for-merging-datasets-)
      - [Choosing the "truth"](#Choosing-the-truth-)
          - [Registration Tools](#Registration-Tools-)
          - [Initial Datasets](#Initial-Datasets-)
      - [Merging THEMIS / CTX / HiRISE (controlled to
        MOLA)](#Merging-THEMIS-CTX-HiRISE-controlled-to-MOLA-)
          - [Controlling THEMIS to MOLA](#Controlling-THEMIS-to-MOLA-)
          - [Controlling CTX to THEMIS](#Controlling-CTX-to-THEMIS-)
          - [Controlling HiRISE to CTX](#Controlling-HiRISE-to-CTX-)
      - [Projecting all](#Projecting-all-)
      - [Problems and Issues](#Problems-and-Issues-)

<span id="Reasons-for-merging-datasets"></span>

## Reasons for merging datasets [¶](#Reasons-for-merging-datasets-)

-----

List of useful reasons:

  - Change detection
  - Spatial / Spectral merges
  - Understanding instrument calibration (optical distortion,
    radiometric similarities)
  - Multivariate statistical analysis
  - Principal component analysis
  - Etc., etc., etc.

<span id="Choosing-the-truth"></span>

## Choosing the "truth" [¶](#Choosing-the-truth-)

-----

Datasets from different instruments often do not register using the raw,
native pointing. In order to merge these datasets, they must be brought
together by selecting one dataset as having the correct pointing or
“truth”. The pointing for the other datasets is then adjusted to match
the truth.

Choosing the truth is dependent on the datasets being merged. First, the
data with the known best pointing could be chosen, such as the Mars
Global Surveyor’s MOLA. Or if simply merging HiRISE and CTX, HiRISE
pointing is considered to be more accurate than CTX pointing.

The differing resolution of the datasets is something to keep in mind.
You will not want to attempt to register HiRISE directly to MOLA as the
difference in resolution is too large to find a common point. In
general, you will want to build up in resolution.

  - MOLA ( *Lowest Resolution* )
  - MOC WA
  - THEMIS
  - MOC NA
  - CTX
  - HiRISE ( *Highest Resolution* )

Registering to MOLA can be difficult. Using a shaded relief version of
MOLA (program – **shade** ), and running either
[map2map](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/map2map/map2map.html)
or
[map2cam](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/map2cam/map2cam.html)
to bring MOLA into the local ground area will help.

| CTX                                                                                                                      | MOLA                                                                                                                     |
| ------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------ |
| [![CTX\_High\_Res.png](attachments/thumbnail/925/200.png)](attachments/download/925/CTX_High_Res.png "CTX_High_Res.png") | [![MOLA\_Low\_Res.png](attachments/thumbnail/924/200.png)](attachments/download/924/MOLA_Low_Res.png "MOLA_Low_Res.png") |

<span id="Registration-Tools"></span>

### Registration Tools [¶](#Registration-Tools-)

[**deltack**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/deltack/deltack.html)

  - Updates camera pointing for a single image
  - Tie one image to another
  - THEMIS to MOLA, HiRISE to MOC NA, HiRISE to CTX, etc.

[**jigsaw**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/jigsaw/jigsaw.html)

  - Simultaneously updates camera pointing for multiple images
  - Typically used for regional or global mosaics

<span id="Initial-Datasets"></span>

### Initial Datasets [¶](#Initial-Datasets-)

Start with Experiment Data Records (EDRs) if possible to ensure
consistency among the datasets for SPICE and mapping parameters such as
planetocentric vs planetographic and positive longitude east vs west.

<span id="Merging-THEMIS-CTX-HiRISE-controlled-to-MOLA"></span>

## Merging THEMIS / CTX / HiRISE (controlled to MOLA) [¶](#Merging-THEMIS-CTX-HiRISE-controlled-to-MOLA-)

-----

<span id="Controlling-THEMIS-to-MOLA"></span>

### Controlling THEMIS to MOLA [¶](#Controlling-THEMIS-to-MOLA-)

Start with the lowest resolution dataset, which in this case is THEMIS.
Pick a matching point between THEMIS and MOLA using qview. (Hint: It is
easier to use a crater as a match point.)  
Write down the latitude / longitude of the point on MOLA and the sample
/ line of the point on the THEMIS image. Update the THEMIS pointing by
running the ISIS3 program **deltack** .

    deltack from=themis.cub lat1=molaLat lon1=molaLon samp1=themisSamp line1=themisLine

This will update the themis pointing to the MOLA.

<span id="Controlling-CTX-to-THEMIS"></span>

### Controlling CTX to THEMIS [¶](#Controlling-CTX-to-THEMIS-)

Next, control the CTX image to the THEMIS image by picking a match point
the same as was done between the MOLA and THEMIS. Use the THEMIS image
that has the updated pointing (after
[deltack](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/deltack/deltack.html)
).

    deltack from=ctx.cub lat1=themisLat lon1=themisLon samp1=ctxSamp line1=ctxLine

<span id="Controlling-HiRISE-to-CTX"></span>

### Controlling HiRISE to CTX [¶](#Controlling-HiRISE-to-CTX-)

| ![](attachments/download/932/Ctx_hirise_blink.gif) |
| -------------------------------------------------- |
| **CTX/HiRISE Misregistration**                     |
| Registration Errors: \~ 40 samples, \~ 20 lines    |

First, control the HiRISE red ccds locally by running the ISIS3 program,
[autoseed](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/autoseed/autoseed.html)
, which will automatically pick match points among the red ccd’s. Then
run the ISIS3 program,
[pointreg](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/pointreg/pointreg.html)
, which will sub-pixel register the points that autoseed picked.

Next, pick a matching point between HiRISE RED5 and the CTX image with
the updated pointing (after deltack). Record the CTX latitude /
longitude and the red5 sample / line. This will be used as a ground
point when running jigsaw on the red CCDs to adjust the pointing.  
In order to use this point as a ground point, the radius is needed. Run
the ISIS3 program,
[campt](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/campt/campt.html)
to get this radius.

    campt from=ctx.cub lat=cxtLat lon=ctxLon type=ground

This ground point can now be added to the control net file that was
output from pointreg using any editor.

    Object              = ControlPoint
       PointType        = Ground
       PointId          = Ground_1
       Latitude         = -5.3320739040534
       Longitude        = 213.71209674872
       Radius           = 3394090.9408245
    
       Group            = ControlMeasure
         SerialNumber   = MRO/HIRISE/RED5/848376111:51904/2
         MeasureType    = Automatic
         Sample         = 1131.92
         Line           = 18555.3
         ErrorLine      = 0
         ErrorSample    = 0
         ErrorMagnitude = 0
         DateTime       = 2007-05-25T09:24:50
         ChooserName    = "TLS"
         GoodnessOfFit  = 0
         Reference      = True
       End_Group
     End_Object

Finally, run
[jigsaw](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/jigsaw/jigsaw.html)
on the HiRISE red cubes.

See Also:

  - controlled mosaics
  - [jigsaw](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/jigsaw/jigsaw.html)

| MOC Registered with HiRISE Red CCDs                | MOC Registered with HiRISE Red CCDs (zoomed)                |
| -------------------------------------------------- | ----------------------------------------------------------- |
| ![](attachments/download/938/MOC_HiRISE_blink.gif) | ![](attachments/download/936/MOC_HiRISE_ZoomedIn_blink.gif) |

<span id="Projecting-all"></span>

## Projecting all [¶](#Projecting-all-)

-----

Project the THEMIS, CTX and HiRISE red images using the CTX resolution
and latitude / longitude range.

    cam2map from=ctx.cub to=ctx.equi.cub pixres=mpp resolution=5.0 
      map=$base/templates/maps/equirectangular.map
    
    cam2map from=themis.cub to=themis.equi.cub map=ctx.equi.cub pixres=map defaultrange=map
    
    /bin/ls *RED* | sed s/.balance.cub// > red.lis
    
    cam2map from=\$1.balance.cub to=\$1.equi.cub map=ctx.equi.cub pixres=map defaultrange=map
      -batch=res.lis
    
    /bin/ls *RED*.equi.cub > redEqui.lis
    
    automos fromlist=redEqui.lis mosaic=redMos.cub match=no

More Information: [Learning About Map
Projections](Learning_About_Map_Projections)

<span id="Problems-and-Issues"></span>

## Problems and Issues [¶](#Problems-and-Issues-)

-----

  - Registering to MOLA can be difficult:
    
      - Use shaded relief if possible (
        [shade](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/shade/shade.html)
        )
      - Use
        [map2cam](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/map2cam/map2cam.html)
        or
        [map2map](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/map2map/map2map.html)
        to bring MOLA image into local ground area to help with the
        smaller MOLA image size

  - Simultaneous CTX and HiRISE observations do not register
    
      - Inconsistent sample/line offsets
      - Probably have timing issues
      - Maybe some boresight misalignment
      - Will research this problem as more CTX data becomes available

  - MOC Narrow Angle does not align with HiRISE or CTX
    
      - Update the MOC NA focal length
      - MOC NA camera model has no distortion model
      - More research needed in this area

  - MRO CRISM
    
      - Unable to fully decipher the PDS IMAGE\_PROJECTION\_OBJECT
      - No CRISM camera model in ISIS3
      - Unable to compare HiRISE, MOC, and THEMIS to CRISM

</div>

<div class="attachments">

<div class="contextual">

</div>

[MOLA\_Low\_Res.png](attachments/download/924/MOLA_Low_Res.png)
[View](attachments/download/924/MOLA_Low_Res.png "View")
<span class="size"> (31.9 KB) </span> <span class="author"> Jesse Mapel,
2016-05-31 02:59 PM </span>

[CTX\_High\_Res.png](attachments/download/925/CTX_High_Res.png)
[View](attachments/download/925/CTX_High_Res.png "View")
<span class="size"> (243 KB) </span> <span class="author"> Jesse Mapel,
2016-05-31 02:59 PM </span>

[Ctx\_hirise\_blink.gif](attachments/download/932/Ctx_hirise_blink.gif)
[View](attachments/download/932/Ctx_hirise_blink.gif "View")
<span class="size"> (213 KB) </span> <span class="author"> Jesse Mapel,
2016-05-31 03:06 PM </span>

[MOC\_HiRISE\_ZoomedIn\_blink.gif](attachments/download/936/MOC_HiRISE_ZoomedIn_blink.gif)
[View](attachments/download/936/MOC_HiRISE_ZoomedIn_blink.gif "View")
<span class="size"> (118 KB) </span> <span class="author"> Jesse Mapel,
2016-05-31 03:12 PM </span>

[MOC\_HiRISE\_blink.gif](attachments/download/938/MOC_HiRISE_blink.gif)
[View](attachments/download/938/MOC_HiRISE_blink.gif "View")
<span class="size"> (39.9 KB) </span> <span class="author"> Jesse Mapel,
2016-05-31 03:12 PM </span>

</div>

<div style="clear:both;">

</div>

</div>

</div>
