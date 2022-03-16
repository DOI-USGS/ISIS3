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
                Contributor Explore in Detail 
              </h1>
              <p>
                Creating and maintaining quality tools is not simple. ISIS contributors have
                worked hard to create accurate references and guides to support future
                contributions. If you encounter a specific case that could help future
                contributors, feel free to submit documentation for this page.
              </p>
              <p>
                On this page you will find detailed references and specialized guides for contributing to ISIS.
              </p>
              <p>
              </p>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>


<!-- API Reference​ -->
    <hr/>
    <h2>ISIS API Reference</h2>
      <p>
        The ISIS API reference is particularly useful if you are working on an application
        or interested in using the ISIS library as a dependency.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://astrodiscuss.usgs.gov/">
            astrodiscuss</a>
          </th>
          <td>
          Ask or answer questions on out community discussion board.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/issues">
            GitHub Issues</a>
          </th>
          <td>
          Comment on bug reports or feature requests from other users.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/discussions?discussions_q=label%3ARFC">
            Requests for Comment</a>
          </th>
          <td>
          Provide feedback on major changes proposed for ISIS.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS_TC">
            ISIS Technical Committee</a>
          </th>
          <td>
          Get involved in project governance.
          </td>
        </tr>
      </table>


<!-- XML Schemas -->
      <hr/>
      <h2>XML Schemas</h2>
      <p>
        The interface for ISIS applications and much of the ISIS documentation (including this
        very page) are generated from XML files. These schemas are a helpful reference if you
        are creating a new application, modifying application arguments, or writing documentation.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#getting-started-with-github">
            Downloading the Source Code</a>
          </th>
          <td>
          How to pull the source code from GitHub.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#anaconda-and-isis3-dependencies">
            Dependency Management</a>
          </th>
          <td>
          How to download the dependencies required to build ISIS.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3">
            Building</a>
          </th>
          <td>
          How to build ISIS once you have the source code and dependencies downloaded.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Developing-ISIS3-with-cmake#running-tests">
            Running the Test Suite</a>
          </th>
          <td>
          How to run the ISIS test suite once ISIS is built.
          </td>
        </tr>
      </table>


<!-- Documentation Guides​ -->
    <hr/>
    <h2>Documentation Guides</h2>
      <p>
        If you're not familiar with working on the ISIS documentation,
        these guides can help get you started.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Exercises_1">
            Basic ISIS3 Programming</a>
          </th>
          <td>
          hese exercises will walk you through the basics of ISIS applications.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Exercises_2">
            Advanced ISIS Programming Exercises</a>
          </th>
          <td>
          These exercises give you the chance to explore more yourself and interact with more
          complex components of the ISIS API.
          </td>
        </tr>
      </table>


<!-- Testing Guides -->
    <hr/>
    <h2>Testing Guides</h2>
      <p>
        Testing is a crucial component of creating sustainable software and a requirement for all
        code contributions to ISIS. These guides will help you run and modify the ISIS test suite.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="">
            Adding a New Feature</a>
          </th>
          <td>
          This document will walk you through what's needed when adding a new feature to ISIS.
          If there is something you think is missing in ISIS this will walk you through the
          steps needed to add it.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="">
            Modifying an Existing Feature</a>
          </th>
          <td>
          Modifying an existing feature is slightly more complex than adding a new feature to ISIS.
          This document will walk you through what is different when you are building from an
          existing feature instead of something new.
          </td>
        </tr>
      </table>


<!-- Maintainer Guides​ -->
    <hr/>
    <h2>Maintainer Guides</h2>
      <p>
        These resources are useful for maintainers and outline the processes that
        support the broader user and contributor community.
      </p>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="">
            Adding a New Feature</a>
          </th>
          <td>
          This document will walk you through what's needed when adding a new feature to ISIS.
          If there is something you think is missing in ISIS this will walk you through the
          steps needed to add it.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="">
            Modifying an Existing Feature</a>
          </th>
          <td>
          Modifying an existing feature is slightly more complex than adding a new feature to ISIS.
          This document will walk you through what is different when you are building from an
          existing feature instead of something new.
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
