<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Adding-SPICE"></span>

## Adding SPICE [¶](#Adding-SPICE-)

-----

An important capability of ISIS is the ability to geometrically and
photometrically characterize pixels in raw planetary instrument images.
Information such as latitude, longitude, phase angle, incidence angle,
emission angle, local solar time, sun azimuth, and a many other pixel
characteristics can be computed.

To compute this information, the **SPICE** ( **S** pacecraft and **P**
lanetary ephemeredes, **I** nstrument **C** -matrix and **E** vent
kernel) kernels must first be determined for the particular raw
instrument image. These kernels maintain the spacecraft position and
orientation over time as well as the target position and instrument
specific operating modes.

To add SPICE information to your cube, run
[**spiceinit**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)
application on the image so that camera/instrument specific applications
(e.g.,
[**cam2map**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cam2map/cam2map.html)
,
[**campt**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/campt/campt.html)
,
[**qview**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/qview/qview.html)
) will have the information they need to work properly. Generally, you
can simply run spiceinit with your input filename and no other
parameters:

``` 
  spiceinit FROM=my.cub
```

<span id="Related-ISIS-Applications"></span>

### Related ISIS Applications [¶](#Related-ISIS-Applications-)

See the following ISIS documentation for information about the
applications you will need to use to perform this procedure:

  - [**spiceinit**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)
    : adds SPICE information to the input cube

</div>

<div style="clear:both;">

</div>

</div>

</div>
