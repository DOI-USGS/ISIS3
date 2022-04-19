<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the main TOC for the "User Docs" page

Author
Deborah Lee Soltesz
12/05/2002

-->


  <xsl:output
    media-type="text/html"
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

  <xsl:include href="menu.xsl"/>


  <xsl:key name="categoryMatch" match="/tableofcontents/document" use="category/categoryItem"/>
  <xsl:key name="audienceMatch" match="/tableofcontents/document" use="audience/target"/>


  <xsl:template match="/">
     <xsl:apply-templates select="tableofcontents" />
  </xsl:template>


  <xsl:template match="tableofcontents">
    <html>
      <head>
        <title>
            USGS Isis: Getting Started Contributor Documentation
        </title>
        <meta name="keywords" content="about, overview, introduction, Isis, image processing, contributor, software, open source, remote sensing, planetary science, astrogeology"/>
        <meta name="description" content="About the Integrated Software for Imagers and Spectrometers (ISIS), created and managed by the USGS Astrogeology Research Program. ISIS provides a comprehensive, user-friendly, portable tool for processing, analyzing, and displaying remotely sensed image data."/>
        <meta name="publisher" content="USGS - GD - Astrogeology Program"/>
        <meta name="author" content="Deborah Lee Soltesz, webteam@astrogeology.usgs.gov"/>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <meta name="country" content="USA"/>
        <meta name="state" content="AZ"/>
        <meta name="county" content="Coconino"/>
        <meta name="city" content="Flagstaff"/>
        <meta name="zip" content="86001"/>

        <link rel="stylesheet" href="../assets/styles/IsisStyleCommon.css"/>
        <link rel="stylesheet" href="../assets/styles/main.css"/>
        <link rel="stylesheet" href="../assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="../assets/styles/print.css"/>
        <!-- Dynamic analytics insertion to prevent running on local URLs -->
        <xsl:text>&#xa;</xsl:text>
        <script type="text/javascript">
          //<xsl:comment><![CDATA[
          (function() {
            var usgsAnalytics = document.createElement('script');
            usgsAnalytics.type = 'text/javascript';
            usgsAnalytics.async = true;
            usgsAnalytics.src = 'http://www.usgs.gov/scripts/analytics/usgs-analytics.js';
            if('http:' == document.location.protocol) {
              var s = document.getElementsByTagName('script')[0];
              s.parentNode.insertBefore(usgsAnalytics, s);
            }
          })();
          ]]></xsl:comment>
        <xsl:text>&#xa;</xsl:text>
        </script>
      </head>

      <body>

        <div class="isisMenu">
         <xsl:call-template  name="writeMenu"/>
        </div>

        <div class="isisContent">

        <a href="http://www.usgs.gov">
        <img src="../assets/icons/littleVIS.gif" width="80" height="22" border="0" alt="USGS"/></a><br/>


        <p style="margin-top:10px; padding-bottom:0px;">
        ISIS Documentation</p>

        <hr/>


        <table width="100%" cellpadding="0" border="0" cellspacing="0">
          <tr valign="top">
            <td align="left">
              <h1>
                Learn More About Contributing Code
              </h1>
              <p>
                ISIS is a large package with many places to contribute and
                improve the code base; however, the size and history of ISIS can
                be intimidating.
                On this page you will find resources to learn more about ISIS development
                and help with your first code contribution.
              </p>
              <p>
              </p>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>

<!-- Exercises -->
    <hr/>
    <h2>Programming Exercises</h2>
      <p>
        These self-guided exercises will have you explore the ISIS code base and
        learn about the ISIS API.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Exercises_1">
            Basic ISIS Programming</a>
          </th>
          <td>
          An exercise introduces you to the ISIS API by
          examining and altering an existing ISIS application.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Exercises_2">
            Advanced ISIS Programming</a>
          </th>
          <td>
          An exercise grows your understanding of the ISIS API by
          building on and refining the work you did in the previous exercise.
          </td>
        </tr>
      </table>


<!-- Contributing Guides -->
    <hr/>
    <h2>Contributing Guides</h2>
      <p>
        The previous exercises showed you how to write code in ISIS.
        These guides will show you how to contribute your code change back into
        the ISIS code base.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/DeveloperAddFeature/DeveloperAddFeature.html">
            Adding a New Feature</a>
          </th>
          <td>
          A guide to the requirements when adding a new feature to ISIS.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/DeveloperModifyIsis/DeveloperModifyIsis.html">
            Modifying an Existing Feature</a>
          </th>
          <td>
          A guide to the requirements when modifying an existing feature in ISIS.
          </td>
        </tr>
      </table>


<!-- Testing Guides -->
    <hr/>
    <h2>Testing Guides</h2>
      <p>
        Testing is a crucial component of creating sustainable software and a requirement for all
        code contributions to ISIS.
      </p>
      <p>
        As of version 3.6, ISIS uses googletest and CTest for new tests. A majority of existing tests 
        have been converted to this new framework, however, some tests are still using the legacy Makefile framework.
      </p>
      <p>
        These guides will help you create, run, and modify ISIS tests within both frameworks.
      </p>
      <h3>googletest and CTest</h3>
        <p>
          Contributors are required to use the googletest system for new tests and convert legacy Makefile tests wherever
          possible.
        </p>
      <table class="tableTOC">
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Writing-ISIS3-Tests-Using-Gtest-and-Ctest">
            Writing Tests Using googletest and CTest</a>
          </th>
          <td>
          A how-to guide for writing new tests and converting ISIS applications to the new callable format required
          by the googletest framework.
          </td>
        </tr>
      </table>

      <h3>Makefile</h3>
        <p>
          There are rare situations where existing legacy Makefile tests have not yet been converted and need to be updated. 
          These legacy Makefile tests use an input and truth data to confirm applications produce the proper output
          under the testing conditions. The following guides describe how to work within the legacy testing system
          and with the legacy testing data.
        </p>
      <table class="tableTOC">
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/HowToApplicationCategoryTest/index.html">
            Legacy Makefile Tests Guide</a>
          </th>
          <td>
          This document describes how legacy Makefile tests work.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Test-Data-Obtaining,-Maintaining,-and-Submitting">
            ISIS Test Data</a>
          </th>
          <td>
          This page explains how to download the test input/truth data required to run the legacy ISIS test suite.
          </td>
        </tr>
      </table>


<!-- FOOTER -->
<script type="text/javascript" language="JavaScript" src="../assets/scripts/footer.js"></script>
</div>


      </body>
    </html>

  </xsl:template>


</xsl:stylesheet>
