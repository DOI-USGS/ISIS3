<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Exporting-ISIS-Data"></span>

# Exporting ISIS Data [¶](#Exporting-ISIS-Data-)

-----

Naturally, once you're finished processing your image data and you have
your final product, you'll want to use it in reports, papers, posters,
web pages, your favorite Geographic Information System (GIS) or image
analysis package, or simply share it with others. Since many other
software packages can't read ISIS cube format, you'll want to export
your cube to a file type appropriate to your needs.

<span id="General-Purpose-Desktop-Applications-and-Web"></span>

### General Purpose: Desktop Applications and Web [¶](#General-Purpose-Desktop-Applications-and-Web-)

[**isis2std**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2std/isis2std.html)
(ISIS to Standard formats) exports your cubes to a wide variety of
popular image formats, such as PNG (Portable Network Graphics) and TIFF
(Tagged Image File Format). No matter what desktop application you're
using (Word Perfect, Illustrator, or GIMP, just to name a few), or what
you're creating (posters, web pages, or a pretty background for your
desktop), **isis2std** will handle it.

<span id="But-Theres-More-GIS-and-Image-Analysis-Applications"></span>

### But There's More\! GIS and Image Analysis Applications [¶](#But-Theres-More-GIS-and-Image-Analysis-Applications-)

But the icing on the cake is that **isis2std** also exports the
necessary files for taking your image into GIS\! If your cube has
Mapping labels, **isis2std** will write a **world file** that provides
GIS software, and other software which can take advantage of a
geographically registered image, with the necessary information to
properly display your image. Most applications designed to work with
remotely sensed and map data (such as Arc products, Envi, and Global
Mapper) can use TIFF, JPEG, and other images with world files. Several
geographic software applications even have support for planetary data.

<span id="Astronomy-Anyone"></span>

### Astronomy Anyone? [¶](#Astronomy-Anyone-)

[**isis2fits**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2fits/isis2fits.html)
(ISIS to FITS format) exports your cubes to Flexible Image Transport
System (FITS) format, a standard in the astronomical community. There's
lots of software designed to work with FITS (even a few common
applications and software libraries , like ImageMagick and GIMP). In
general, FITS format is useful for mission team members who are working
with cruise data to analyze instrument calibration, camera pointing, and
other factors. Since cruise image data frequently contain stars,
astronomy software comes in quite useful for this type of work.

<span id="Just-the-Data-Maam"></span>

### Just the Data, Ma'am [¶](#Just-the-Data-Maam-)

[**isis2raw**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2raw/isis2raw.html)
(ISIS to Raw format) exports an individual band in your cube to raw
image format, a minimal format that contains just the data in the
bit-type of your choosing. **isis2raw** is a good choice when none of
the other formats ISIS can export to will work for you. In order to
import a raw image into another application, you will need to know the
width (samples), height (rows), bit-type, and endianess of your raw file
in order to import it correctly, so remember to keep track of the
parameters you used to export your cube.

<span id="Related-ISIS-Applications"></span>

### Related ISIS Applications [¶](#Related-ISIS-Applications-)

See the following ISIS documentation for information about the
applications you will need to use to perform this procedure:

  - [**isis2std**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2std/isis2std.html)
    : Export a cube to popular image formats, such as TIFF and PNG, with
    GIS world files
  - [**isis2fits**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2fits/isis2fits.html)
    : Export a cube to FITS format, the standard image format of the
    astronomical community
  - [**isis2raw**](http://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/isis2raw/isis2raw.html)
    : Export a cube to raw format

</div>

<div style="clear:both;">

</div>

</div>

</div>
