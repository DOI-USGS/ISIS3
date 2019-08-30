<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Exercises-1"></span>

# Exercises 1 [¶](#Exercises-1-)

-----

The purpose behind these exercises is to help programmers become
familiar with the ISIS3 environment, the ISIS3 API, and ISIS3 standards.

  - [Exercises 1](#Exercises-1-)
      - [Before We Start](#Before-We-Start-)
      - [Setup](#Setup-)
      - [Task 1 - Get Familiar with
        ISIS3](#Task-1-Get-Familiar-with-ISIS3-)
      - [Task 2 - Examine the ISIS3 mirror Application and Modify
        it](#Task-2-Examine-the-ISIS3-mirror-Application-and-Modify-it-)
      - [Task 3 - Modify the mirror
        Application](#Task-3-Modify-the-mirror-Application-)
      - [Task 4 - Add a Parameter to the mirror
        Application](#Task-4-Add-a-Parameter-to-the-mirror-Application-)
      - [Task 5 - Allow User to Decide if Alternating Lines or Samples
        are Set to 0 or
        NULL](#Task-5-Allow-User-to-Decide-if-Alternating-Lines-or-Sles-are-Set-to-0-or-NULL-)
      - [Task 6 - Provide Information to the
        User](#Task-6-Provide-Information-to-the-User-)
      - [Task 7 - Modify the Cube
        Labels](#Task-7-Modify-the-Cube-Labels-)
      - [Task 8 - Create Documentation](#Task-8-Create-Documentation-)

<span id="Before-We-Start"></span>

## Before We Start [¶](#Before-We-Start-)

-----

This workshop does not attempt to teach basic Linux command line usage.
If you do not have a basic knowledge of the Linux command line please
search the internet for tutorials about "Linux commands" and learn them
before attempting this workshop.

Download a test image,
[Peaks.cub.gz](attachments/download/1042/Peaks.cub.gz) (you must
uncompress this image). This image is 1024x1024 with 7 bands and it has
19 [LRS](LRS) special pixels.

<span id="Setup"></span>

## Setup [¶](#Setup-)

-----

Initialize your environment to run the ISIS3 applications. Set the
ISISROOT environment variable using a command like: setenv ISISROOT
**THE/LOCATION/OF/THE/MAIN/ISIS/DIRECTORY**

Example:

``` 
 setenv ISISROOT /usgs/pkgs/isis3/isis
```

**NOTE** : The last directory in the path must be "isis"

Run the ISIS3 initialization script:

``` 
 source $ISISROOT/scripts/isis3Startup.csh
```

**NOTE** : Astrogeology ISIS3 users should set an alias:

``` 
 alias setisis source /home/isis3mgr/bin/initIsis.csh 
 setisis /usgs/pkgs/isis3/isis
```

Verify your ISIS3 environment is working by examining the test cube
(downloaded above) using the ISIS3 application
[qview](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/qview/qview.html)

    qview Peaks.cub

<span id="Task-1-Get-Familiar-with-ISIS3"></span>

## Task 1 - Get Familiar with ISIS3 [¶](#Task-1-Get-Familiar-with-ISIS3-)

-----

1.  Browse the [ISIS Website](https://isis.astrogeology.usgs.gov/)
      - Examine the
        [user](https://isis.astrogeology.usgs.gov/UserDocs/index.html)
        documentation
      - Examine the
        [programmer](https://isis.astrogeology.usgs.gov/TechnicalInfo/index.html)
        documentation (Technical documents)
2.  Look at the ISIS3 directory structure
      - Look at $ISISROOT/src
      - Look at $ISISROOT/src/base
      - Look at $ISISROOT/src/base/apps
      - Look at $ISISROOT/src/base/objs
      - Look at $ISISROOT/src/mgs ...
      - Look at $ISISROOT/bin

Questions:  
What is a "Cube Attribute?"  
What are the two pixel storage orders ISIS3 supports?  
Name one reserved ISIS3 command line parameter.  
What is the general indenting scheme for ISIS3?  
What is the naming convention for local variables?  
What is an include guard?  
What are the names of the three dimensions of an ISIS3 cube?  
How would you specify the string "Hello World" for a parameter named
"comment" in an application called **diff** on the command line?

<span id="Task-2-Examine-the-ISIS3-mirror-Application-and-Modify-it"></span>

## Task 2 - Examine the ISIS3 **mirror** Application and Modify it [¶](#Task-2-Examine-the-ISIS3-mirror-Application-and-Modify-it-)

-----

1.  Create the following directory path under your home area:
    isis3/isis/src/base/apps/mirror
2.  Copy the **mirror** application(mirror.cpp, mirror.xml, Makefile)
    from **$ISISROOT/src/base/apps/mirror** into the new directory
3.  Type ' **make** ' to build the program
4.  Run **mirror** using the input file Peaks.cub downloaded above (the
    image must be uncompressed prior to this step).
5.  Look at the results with **qview**
6.  Modify the mirror program so that instead of mirroring, every other
    sample in the image is set to zero. You may want to save the
    mirroring code for later in a comment instead of deleting it.
7.  Verify the results with **qview**

<span id="Task-3-Modify-the-mirror-Application"></span>

## Task 3 - Modify the **mirror** Application [¶](#Task-3-Modify-the-mirror-Application-)

-----

1.  Modify the your copy of the **mirror** program so instead of
    mirroring and/or setting every other sample to zero, have it set
    every other line to zero. You may want to save previous code in a
    comment instead of deleting it.
2.  Verify the results with **qview**

<span id="Task-4-Add-a-Parameter-to-the-mirror-Application"></span>

## Task 4 - Add a Parameter to the **mirror** Application [¶](#Task-4-Add-a-Parameter-to-the-mirror-Application-)

-----

1.  Add a parameter to the **mirror** application so the user can pick
    either zero or Isis::Null as the output DN for the blank lines
    and/or samples. You will need to modify the xml file to accomplish
    this **Hint: Look at other ISIS3 applications for examples or the
    [ISIS XML
    documentation](http://isis.astrogeology.usgs.gov/Schemas/Application/documentation/index.html)**
2.  Verify the results with **qview**

<span id="Task-5-Allow-User-to-Decide-if-Alternating-Lines-or-Samples-are-Set-to-0-or-NULL"></span>

## Task 5 - Allow User to Decide if Alternating Lines or Sles are Set to 0 or NULL [¶](#Task-5-Allow-User-to-Decide-if-Alternating-Lines-or-Sles-are-Set-to-0-or-NULL-)

-----

1.  Combining what you did in Task 3 and Task 4, modify the **mirror**
    application again to allow the user to decide both if the program
    changes the value of every other line or every other sample and if
    the value should be changed to 0 or Isis::NULL.
2.  You will need to again modify the xml file.
3.  *Hint* : you may need to use C++ polymorphism and class inheritance.
    If you are unfamiliar with these concepts, now would be a great time
    to learn\!

<span id="Task-6-Provide-Information-to-the-User"></span>

## Task 6 - Provide Information to the User [¶](#Task-6-Provide-Information-to-the-User-)

-----

1.  Modify the program to compute the average and standard deviation of
    the output image. To start with just write the values using the C++
    cout stream. *\[Hint: See the
    [Statistics](https://isis.astrogeology.usgs.gov/Object/Developer/class_isis_1_1_statistics.html)
    class\]*
2.  Next put the average and standard deviation in a PvlGroup and write
    it to the log file(print.prt) *\[Hint: See the
    [Application](https://isis.astrogeology.usgs.gov/Object/Developer/class_isis_1_1_application.html)
    class\]*

<span id="Task-7-Modify-the-Cube-Labels"></span>

## Task 7 - Modify the Cube Labels [¶](#Task-7-Modify-the-Cube-Labels-)

-----

1.  Use the ' **more** ' utility to look at the labels in the beginning
    of the cube
2.  Modify the program to add the average and standard deviation to the
    labels *\[Hint: Look at the
    [Process](https://isis.astrogeology.usgs.gov/Object/Developer/class_isis_1_1_process.html)
    and
    [Cube](https://isis.astrogeology.usgs.gov/Object/Developer/class_isis_1_1_cube.html)
    classes\]*

<span id="Task-8-Create-Documentation"></span>

## Task 8 - Create Documentation [¶](#Task-8-Create-Documentation-)

-----

1.  You may have noticed some xml tags in mirror.xml look like comments.
    Tags like `  <brief>  ` , `  <description>  ` , and `  <example>  `
    help programmers get contextual information when reading or editing
    source code. They also serve to generate documentation. To see what
    your application's documentation looks like, go to the application's
    directory and type ' **make html** ' to build the documentation.
    Open the .html file it creates in a web browser to see what it looks
    like.
2.  Look at other applications in ISIS3 for examples of documentation
    and read the [ISIS Application Examples
    How-to](http://isis.astrogeology.usgs.gov/documents/HowToApplicationExamples/index.html)
3.  Modify the xml for your application and use ' **make html** ' to
    verify the results.

</div>

<div class="attachments">

<div class="contextual">

</div>

[Peaks.cub.gz](attachments/download/1042/Peaks.cub.gz)
<span class="size"> (5.76 MB) </span> <span class="author"> Makayla
Shepherd, 2016-06-01 10:30 AM </span>

</div>

<div style="clear:both;">

</div>

</div>

</div>
