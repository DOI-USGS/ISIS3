<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="ISIS-Training-2016"></span>

# ISIS Training 2016 [¶](#ISIS-Training-2016-)

  - [ISIS Training 2016](#ISIS-Training-2016-)
      - [Description](#Description-)
      - [Agenda](#Agenda-)
          - [Lesson1](#Lesson1-)
          - [Lesson2](#Lesson2-)
          - [Lesson3](#Lesson3-)
          - [Lesson4](#Lesson4-)
          - [Lesson5](#Lesson5-)
          - [Lesson6](#Lesson6-)
      - [Experience Requirements](#Experience-Requirements-)
      - [Prerequisites](#Prerequisites-)
      - [Resources](#Resources-)
      - [Downloads](#Downloads-)

<span id="Description"></span>

## Description [¶](#Description-)

The hands-on ISIS Workshop was an introductory level workshop with focus
on the end-to-end work flow of cartographic processing of planetary
image data held in NASA's PDS archives.  
Instructors presented lessons where participants will interactively
exercised through the fundamentals of ISIS.

<span id="Agenda"></span>

## Agenda [¶](#Agenda-)

Training was held 2 days, **Thursday through Friday, July 28-29, 2016 in
Flagstaff, AZ** .

<span id="Lesson1"></span>

### Lesson1 [¶](#Lesson1-)

This first lesson demonstrates an end-to-end processing of a Cassini-WA
image (Enceladus-hemisphere view). We ingest the raw PDS image into
ISIS, attach SPICE labels, examine the ISIS cube labels, apply
radiometric calibration, and project the cube. We examine the cube
visually throughout the process by using the interactive *qview*
application.

<span id="Lesson2"></span>

### Lesson2 [¶](#Lesson2-)

In this lesson, we relate two data (image) sets, comprised of Themis-IR
(color) and Dawn-FC images, that overlap the same region of Mars. We
initialize footprints for these ingested images and view the footprints
with the interactive *qmos* application.

<span id="Lesson3"></span>

### Lesson3 [¶](#Lesson3-)

In this lesson, we create a stereo anaglyph using MRO-CTX images of the
Kasei Valles region on Mars. An extra set of instructions is provided at
the end of the lesson that demonstrate the *shade* application.

<span id="Lesson4"></span>

### Lesson4 [¶](#Lesson4-)

Using the previous lesson's CTX anaglyph as a map template, we project a
color Themis-IR image to relate it and CTX anaglyph on the Kasei Valles
region on Mars.

<span id="Lesson5"></span>

### Lesson5 [¶](#Lesson5-)

In this lesson, we create a Red 0-9 ccd mosaic of MRO-HiRISE images of
the Kasei Valles region on Mars. This lesson introduces the *equalizer*
application for tone matching a set of images and introduces the
*automos* application for creating a mosaic from a list of input images.

<span id="Lesson6"></span>

### Lesson6 [¶](#Lesson6-)

In this lesson, we process Messenger MDIS-WAC color-set images to create
a color mosaic of the Raditladi Basin on Mercury. This lesson introduces
the *coreg* application, which is used to register images of two of the
color filters to images of the third color filter. This ensures that the
features in the images will all line-up properly when mosaicking. After
registration, we then mosaic the images together.

<span id="Experience-Requirements"></span>

## Experience Requirements [¶](#Experience-Requirements-)

  - Basic knowledge and comfortability with Unix commands\! (See [this
    pdf](https://ubuntudanmark.dk/filer/fwunixref.pdf) or [this
    webpage](http://mally.stanford.edu/~sr/computing/basic-unix.html)
    for basic Unix help.)
  - If no prior ISIS experience, please review [online ISIS
    workshops](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/ISIS_Online_Workshops)
    .
  - Intermediate ISIS experienced participants are welcome; please
    attend with an open mind to learn new tips, arrive with questions
    and allow yourself to explore with the prepared interactive lessons.

<span id="Prerequisites"></span>

## Prerequisites [¶](#Prerequisites-)

  - **Participants were required to bring their own laptop computers.
    See [Laptop
    Requirements](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/VirtualBox_Installation_Guide#Laptop-Requirements-for-ISIS-VM)
    .**
  - The ISIS workshop was conducted using VirtualBox with a slimmed-down
    version of ISIS (ISIS3.4.12). It is *highly-recommended* to
    **[install
    VirtualBox](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/VirtualBox_Installation_Guide)**
    before arriving at the workshop.
      - There is NO preference for one OS versus another, any host OS
        that is supported by VirtualBox can be used. See the [supported
        host OSes
        here](https://www.virtualbox.org/manual/ch01.html#hostossupport)
        .
      - We strongly encourage participants to use the VM for the
        workshop and avoid using your own ISIS installation\!
      - (The VM eliminates issues with any particular OS and simplifies
        support during the workshop. For example, some GUI-based ISIS
        applications do not perform as they should on Mac OSX 10.10 and
        10.11.)
  - ***On the first morning (8-10am) of July 28, the ISIS VM was
    supplied and installed on-site from a provided USB drive***
    (participants took the USB home).

<span id="Resources"></span>

## Resources [¶](#Resources-)

Here are some useful resources for setting up and customizing your ISIS
VM:

  - [VirtualBox Installation
    Guide](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/VirtualBox_Installation_Guide)
  - [ISIS3 Workshop VM Installation
    Guide](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/ISIS3_Workshop_VM_Installation_Guide)
  - [Customizing the ISIS Workshop
    VM](https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/Customizing_the_ISIS_Workshop_VM)

<span id="Downloads"></span>

## Downloads [¶](#Downloads-)

Please see read the installation guides under [Resources](#Resources-) on this page before downloading.

The workshop VM and lesson materials are downloadable through the
following [Astropedia](http://astrogeology.usgs.gov/search) links:

  - [ISIS3VM](http://astropedia.astrogeology.usgs.gov/downloadBig/Docs/IsisWorkshop/FY16_Beginner/Isis3VM.tar.gz)
    (isis3.4.12)
  - [ISIS3WorkshopTutorials\_Beginner\_FY16](http://astropedia.astrogeology.usgs.gov/downloadBig/Docs/IsisWorkshop/FY16_Beginner/ISISWorkshopTutorials_Beginner_FY16.tar.gz)

Note that these are compressed files, you will need to extract them.

  - **Windows** - You will need to download a tool, such as 7-Zip, to
    extract the file.
  - **OSX** - Double-click the downloaded file to automatically extract
    it.
  - **Linux** - Open a terminal, then navigate to the directory that
    contains the downloaded file (e.g. \~/Downloads). Then, run the
    following command (replace *file* .tar.gz with the name of your
    downloaded file):

<!-- end list -->

    tar xzf file.tar.gz

</div>

<div style="clear:both;">

</div>

</div>

</div>
