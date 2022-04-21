<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

# Working with Lunar Reconnaissance Orbiter (LRO) Data [¶](#Working-with-Lunar-Reconnaissance-Orbiter-LRO-Data-)

-----

<span id="LRO"></span>

# LRO [¶](#LRO-)

-----

<span id="LRO"></span>

  - [LRO](#LRO-)
      -   - [Brief Mission Summary](#Brief-Mission-Summary-)
          - [Science Goals](#Science-Goals-)
          - [Science Instruments](#Science-Instruments-)
          - [Data Processing with ISIS](#Data-Processing-with-ISIS-)         
      - [References & Related
        Resources](#References--Related-Resources-)
          - [Planetary Data System (PDS) Information and Data Search
            Tools](#Planetary-Data-System-PDS-Information-and-Data-Search-Tools-)

<span id="Brief-Mission-Summary"></span>

### Brief Mission Summary [¶](#Brief-Mission-Summary-)

The Lunar Reconnaissance Orbiter (LRO) is a NASA robotic spacecraft currently orbiting the Moon in an eccentric polar mapping orbit. Data collected by LRO have been described as essential for planning NASA's future human and robotic missions to the Moon. Its detailed mapping program is identifying safe landing sites, locating potential resources on the Moon, characterizing the radiation environment, and demonstrating new technologies.

Launched on June 18, 2009, in conjunction with the Lunar Crater Observation and Sensing Satellite (LCROSS), as the vanguard of NASA's Lunar Precursor Robotic Program, LRO was the first United States mission to the Moon in over ten years. LRO and LCROSS were launched as part of the United States's Vision for Space Exploration program.

See https://en.wikipedia.org/wiki/Lunar_Reconnaissance_Orbiter for further information.

<span id="Science-Goals"></span>

### Science Goals [¶](#Science-Goals-)

Areas of investigation include selenodetic global topography; the lunar polar regions, including possible water ice deposits and the lighting environment; characterization of deep space radiation in lunar orbit; and high-resolution mapping, at a maximum resolution of 50 cm/pixel (20 in/pixel), to assist in the selection and characterization of future landing sites.

<span id="Science-Instruments"></span>

### Science Instruments [¶](#Science-Instruments-)

See https://en.wikipedia.org/wiki/Lunar_Reconnaissance_Orbiter#Payload.

### Data Processing with ISIS [¶](#Data-Processing-with-ISIS-)

#### LRO Narrow Angle Camera (NAC)

TBD

#### LRO Wide Angle Camera (WAC)

We will focus on the monochromatic images for this sensor. Visit:

   https://ode.rsl.wustl.edu/moon/indexproductsearch.aspx

Find the *Lunar Reconnaissance Orbiter -> Experiment Data Record Wide
Angle Camera - Mono (EDRWAM)* option.

Search either based on a longitude-latitude window, or near a
notable feature, such as a named crater.  Here are a couple of images
having the Tycho crater::

    http://pds.lroc.asu.edu/data/LRO-L-LROC-2-EDR-V1.0/LROLRC_0002/DATA/MAP/2010035/WAC/M119923055ME.IMG
    http://pds.lroc.asu.edu/data/LRO-L-LROC-2-EDR-V1.0/LROLRC_0002/DATA/MAP/2010035/WAC/M119929852ME.IMG

Fetch these with ``wget``. For a dataset called ``image.IMG``, do:

    lrowac2isis from = image.IMG to = image.cub

This will create so-called *even* and *odd* datasets, with names like
``image.vis.even.cub`` and ``image.vis.odd.cub``.

Run ``spiceinit`` on them to set up the SPICE kernels:

    spiceinit from = image.vis.even.cub
    spiceinit from = image.vis.odd.cub

followed by ``lrowaccal`` to adjust the image intensity:

    lrowaccal from = image.vis.even.cub to = image.vis.even.cal.cub
    lrowaccal from = image.vis.odd.cub  to = image.vis.odd.cal.cub

If these are inspected, such as with ``qview``, it can be
seen that instead of a single contiguous image we have a set of narrow
horizontal bands, with some bands in the *even* and some in the *odd*
cub file. The pixel rows in each band may also be recorded in reverse.

The only way to fix these artifacts currently is to mapprojected these
images and fuse them. This happens as:

    cam2map from = image.vis.even.cal.cub to = image.vis.even.cal.map.cub
    cam2map from = image.vis.odd.cal.cub  to = image.vis.odd.cal.map.cub  \
      map = image.vis.even.cal.map.cub matchmap = true

Note how in the second ``cam2map`` call we used the ``map`` and
``matchmap`` arguments. This is to ensure that both of these output
images have the same resolution and projection. In particular, if more
datasets are present, it is suggested for all of them to use the same
previously created .cub file as a map reference. That makes terrain
creation with photogrammetry work more reliably. 

The fusion happens as:

    ls image.vis.even.cal.map.cub image.vis.odd.cal.map.cub  > image.txt
    noseam fromlist = image.txt to = image.noseam.cub SAMPLES=73 LINES=73

The obtained file ``image.noseam.cub`` may still have some small artifacts
but should be overall reasonably good. 

<span id="References-amp-Related-Resources"></span>

## References & Related Resources [¶](#References--Related-Resources-)

-----

<span id="Planetary-Data-System-PDS-Information-and-Data-Search-Tools"></span>

### Planetary Data System (PDS) Information and Data Search Tools [¶](#Planetary-Data-System-PDS-Information-and-Data-Search-Tools-)

  - PDS Geosciences Node, Lead Node for LRO:
    <http://geo.pds.nasa.gov/missions/lro/default.htm>
      - Lunar Orbital Data Explorer: <http://ode.rsl.wustl.edu/moon/>
  - PDS Imaging Node:
      - Data Release Calendar:
        <http://pds-imaging.jpl.nasa.gov/schedules/lro_release.html>
      - LRO Mission:
        <http://pds-imaging.jpl.nasa.gov/portal/lro_mission.html>
      - Photojournal: <http://photojournal.jpl.nasa.gov/mission/LRO>
      - Planetary Image Locator Tool (PILOT):
        <http://pilot.wr.usgs.gov/index.php?view=map&target=moon>
  - PDS Engineering Node:
      - Global data search engine: <http://pds.nasa.gov/>
  - PDS Navigation and Ancillary Information Node:
      - Spice Kernels:
        <https://naif.jpl.nasa.gov/pub/naif/pds/data/lro-l-spice-6-v1.0/>
      - Toolkit: <ftp://naif.jpl.nasa.gov/pub/naif/toolkit/>

</div>

<div style="clear:both;">

</div>

</div>

</div>
