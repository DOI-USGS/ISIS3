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
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Contributing-Application-Documentation">
            Contributing ISIS Application Documentation</a>
          </th>
          <td>
          This how-to guide will take you through the steps required to write and modify the ISIS
          application documentation.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/HowToApplicationExamples/index.html">
            ISIS Application Examples Guide</a>
          </th>
          <td>
          This how-to guide covers writing examples for ISIS application documentation.
          Application examples are a specialized section of the ISIS application
          documentation and require additional steps besides editing the application XML.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/HowToGeneralDocumentation/index.html">
            General ISIS Documentation Guide</a>
          </th>
          <td>
          This how-to guide covers writing documentation for this website.
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
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Writing-ISIS3-Tests-Using-Gtest-and-Ctest">
            Writing Tests Using GTest and CTest</a>
          </th>
          <td>
          As of version 3.6, ISIS uses googletest and CTest for testing. This how-to guide will
          walk you through the steps required to write tests using the new frameworks. It also
          covers the steps needed to convert ISIS applications to the new callable format.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Test-Data-Obtaining,-Maintaining,-and-Submitting">
            ISIS Test Data</a>
          </th>
          <td>
          Legacy Makefile based tests in ISIS use a input and truth data to confirm that the
          aplications produce the proper output. This page explains how to download these
          files required to run the ISIS test suite.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/HowToApplicationCategoryTest/index.html">
            Legacy Makefile Tests Guide</a>
          </th>
          <td>
          This documents describes how legacy Makefile tests work. Contributors are required
          to use the new googletest system for new tests, but there are rare situations where
          existing legacy Makefile tests need to be updated and cannot be converted.
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
            <a href="https://github.com/USGS-Astrogeology/ISIS3/wiki/Public-Release-Process">
            Release Process</a>
          </th>
          <td>
          This guide goes through all the steps required to create both release candidates
          and full releases.
          </td>
        </tr>
      </table>


<!-- API Reference​ -->
    <hr/>
    <h2>ISIS API Reference</h2>
      <p>
        The <a href="../Object/Developer/index.html">ISIS API reference</a> is particularly
        useful if you are working on an application or interested in using the ISIS library
        as a dependency.
      </p>


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
            <a href="../Schemas/Application/documentation/index.html">
            ISIS Application XML Schema</a>
          </th>
          <td>
          This schema defines the ISIS application XML format. This includes the applications
          documentation and all of the application arguments.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../Schemas/Documentation/documentation/index.html">
            ISIS Web Documentation XML Schema</a>
          </th>
          <td>
          This schema defines the XML documentation format used to generate this documentation.
          If you want to make a change to the ISIS website documentation, then you will need to
          reference this schema for your changes.
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
