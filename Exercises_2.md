<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Exercises-2"></span>

# Exercises 2 [¶](#Exercises-2-)

-----

The goal of this exercise is for the programmer to become familiar with
slightly more advanced features of the ISIS3 API. In this exercise you
will be writing an ISIS3 application that does simple differences
between adjacent pixels in the line or sample directions.

  - [Exercises 2](#Exercises-2-)
      - [Setup](#Setup-)
      - [Task 1 - Rename the copy of mirror to
        diff](#Task-1-Rename-the-copy-of-mirror-to-diff-)
      - [Task 2 - Modify the IsisMain and Processing
        function](#Task-2-Modify-the-IsisMain-and-Processing-function-)
      - [Task 3 - Add the ability to subtract the left
        neighbour](#Task-3-Add-the-ability-to-subtract-the-left-neighbour-)
      - [Task 4 - Add the ability to subtract
        lines](#Task-4-Add-the-ability-to-subtract-lines-)
      - [Task 5 - What about special
        pixels?](#Task-5-What-about-special-pixels-)
      - [Task 6 - Don't duplicate code](#Task-6-Dont-duplicate-code-)
      - [Task 7 - Check your results](#Task-7-Check-your-results-)
      - [Task 8 - Remove Global
        Variables](#Task-8-Remove-Global-Variables-)
      - [Task 9 - Create tests for your new
        software](#Task-9-Create-tests-for-your-new-software-)
      - [Task 10 - Check code coverage](#Task-10-Check-code-coverage-)

<span id="Setup"></span>

## Setup [¶](#Setup-)

-----

**Mirror** is a good starting place, so it is advisable to copy the
**mirror** directory from Exercise 1 to a new directory in base/apps
called diff. The rest of the exercise will be based on this copied
directory.

1.  Setup your environment to run the current public version of ISIS3
2.  Set your current working directory to the "diff" directory created
    above.

<span id="Task-1-Rename-the-copy-of-mirror-to-diff"></span>

## Task 1 - Rename the copy of **mirror** to **diff** [¶](#Task-1-Rename-the-copy-of-mirror-to-diff-)

-----

1.  Make clean
2.  Rename all of the files to match your new application's name:
    **diff** .
3.  Edit the XML file to have only the FROM and TO parameters. Make sure
    they both have appropriate descriptions and add appropriate history
    documentation.
4.  Type **make html** to check the validity of your XML file. Fix any
    errors.

<span id="Task-2-Modify-the-IsisMain-and-Processing-function"></span>

## Task 2 - Modify the IsisMain and Processing function [¶](#Task-2-Modify-the-IsisMain-and-Processing-function-)

-----

1.  Modify the IsisMain and process functions so they produce a new cube
    where each pixel is the subtraction of that pixel and its right
    neighbour. Example: If line 1 sample 1 has a value is 120 and line 1
    sample 2 has a value of 153, the output pixel for line 1 sample 1
    should be -33.

<span id="Task-3-Add-the-ability-to-subtract-the-left-neighbour"></span>

## Task 3 - Add the ability to subtract the left neighbour [¶](#Task-3-Add-the-ability-to-subtract-the-left-neighbour-)

-----

1.  Create a new process function to subtract each pixel from its left
    neighbouring pixel.
2.  Create radio buttons in your XML (you may have to look at other apps
    to see an example) to allow the user to select between the two
    modes.
3.  Set up an if statement so your software can differentiate between
    the two modes. Hint: UserInterface can help you get the string value
    of the radio buttons. It may be useful to look at another
    application.

<span id="Task-4-Add-the-ability-to-subtract-lines"></span>

## Task 4 - Add the ability to subtract lines [¶](#Task-4-Add-the-ability-to-subtract-lines-)

-----

1.  Now create another process function that subtracts lines instead of
    samples.
2.  Edit your XML file to accommodate this new feature.
      - Don't forget **make html** to test your changes
      - Don't forget to append to the if statement to accommodate the
        new mode. Hint: This may require a global variable.

<span id="Task-5-What-about-special-pixels"></span>

## Task 5 - What about special pixels? [¶](#Task-5-What-about-special-pixels-)

-----

1.  Now you need to account for special pixels in the image. If either
    of the input pixels you are dealing with are special, then the
    output pixel should be Isis::Null.
2.  Make sure you account for special pixels in all modes of the
    application.

<span id="Task-6-Dont-duplicate-code"></span>

## Task 6 - Don't duplicate code [¶](#Task-6-Dont-duplicate-code-)

-----

1.  Combine all of the process functions into a single function. Note:
    This also may take some global variables. Hint: Look at
    ProcessByLine and other ProcessByXxxxxx

<span id="Task-7-Check-your-results"></span>

## Task 7 - Check your results [¶](#Task-7-Check-your-results-)

-----

1.  For each mode you have in **diff** , compare the output to the
    original Peaks.cub (available from [Exercises 1](Exercises_1) )
    using **qview** . Make sure your software is processing data
    correctly.
2.  Check your special pixels

<span id="Task-8-Remove-Global-Variables"></span>

## Task 8 - Remove Global Variables [¶](#Task-8-Remove-Global-Variables-)

-----

1.  As much as possible you want to avoid using global variables. In
    this exercise you have likely used a global variable to pass
    information to your line processing function. You should rewrite the
    program so that it does not rely on global variables. Hint: Look at
    Isis::ProcessByBrick

<span id="Task-9-Create-tests-for-your-new-software"></span>

## Task 9 - Create tests for your new software [¶](#Task-9-Create-tests-for-your-new-software-)

-----

1.  Create tests to ensure your new software is functioning correctly.
    Unfortunately, **mirror** 's test data will not do. However, it
    provides a good example.
2.  In *diff* , make sure there is a tsts directory. If is does not
    exist, use the command **make testdir** to create it.
3.  In the tsts directory, make a new test case for one of the modes
    **diff** has with the command **make newtest TEST=testName** . Use a
    meaningful name.
4.  Copy the Makefile from **mirror** 's default test into your new test
    case directory.
5.  Modify the Makefile to fully test one of your cases.
6.  Copy Peaks.cub (available from
    [Exercises 1](http://intraweb.wr.usgs.gov/Projects/IsisWorkshop/index.php/Exercises_1)
    ) in the input directory under your test.
7.  Modify the Makefile to use Peaks.cub as the input.
8.  Run the command **make output** . Check the results in the output
    directory. If they are correct, use **make truthdata** to save those
    results to truth directory.
9.  Repeat the steps above for the other operating modes of **diff** .
10. Now, from the *diff* directory, try running **make test** . All of
    the tests should report OK.
11. (In the production system **make checkin** will copy the truth files
    into the system's truth file directory. Don't do this step for the
    tutorial.)

<span id="Task-10-Check-code-coverage"></span>

## Task 10 - Check code coverage [¶](#Task-10-Check-code-coverage-)

-----

1.  We use a program called Squish Coco to check code coverage. This
    program will generate a report about test coverage in your code.
2.  Remove built files and make your application and tests with the Test
    Coverage mode enabled. On the command line, type **make clean
    appname test MODE=TC**
3.  Coco will print you a summary of your coverage. If any of the
    reports show that your coverage is below 100%, you should check the
    detailed report and make improvements to ensure your code is
    thoroughly tested.
4.  Coco will have made html files in your app directory named
    *functioncoverage.html* , *linecoverage.html* , and
    *scopecoverage.html* .
5.  Explore the features of these detailed reports, find where your code
    is not covered, and make improvements until the Coco coverage
    reports are close to 100% coverage.

</div>

<div style="clear:both;">

</div>

</div>

</div>
