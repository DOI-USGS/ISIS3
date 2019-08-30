<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Image-Registration"></span>

# Image Registration [¶](#Image-Registration-)

-----

There two alternative paths to automatically creating a tie-point
control network.

1.  **Autoseed Path**
      - This path will distribute and establish tie-points based on
        polygon overlaps of a collection of images.
      - The image coordinates for each point are immediately measured
        with the **autoseed** application.
      - The **footprintinit** and **findimageoverlaps** are required by
        **autoseed**
2.  **Seedgrid Path**
      - A gridded pattern of tie-points are established based on
        specified latitude/longitude values and spacing.
      - The image coordinates for each "seeded" point are measured with
        the **cnetadd** application.

<span id="Creating-a-Tie-Point-Control-Network"></span>

## Creating a Tie-Point Control Network [¶](#Creating-a-Tie-Point-Control-Network-)

-----

| autoseed Path     | seedgrid Path |
| ----------------- | ------------- |
| spiceinit         | spiceinit     |
| footprintinit     |               |
| findimageoverlaps | seedgrid      |
| autoseed          | cnetadd       |
| cnetref           | cnetref       |
| pointreg          | pointreg      |

<span id="Related-ISIS3-Applications"></span>

## Related ISIS3 Applications [¶](#Related-ISIS3-Applications-)

-----

[**spiceinit**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html)  
[**footprintinit**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/footprintinit/footprintinit.html)  
[**findimageoverlaps**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/findimageoverlaps/findimageoverlaps.html)  
[**autoseed**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/autoseed/autoseed.html)  
[**seedgrid**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/seedgrid/seedgrid.html)  
[**cnetadd**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cnetadd/cnetadd.html)  
[**cnetref**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/cnetref/cnetref.html)  
[**pointreg**](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/pointreg/pointreg.html)

</div>

<div style="clear:both;">

</div>

</div>

</div>
