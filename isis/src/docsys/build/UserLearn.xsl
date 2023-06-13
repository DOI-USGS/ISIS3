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
            USGS Isis: Learn More User Documentation
        </title>
        <meta name="keywords" content="about, overview, introduction, Isis, image processing, software, open source, remote sensing, planetary science, astrogeology"/>
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
                Learn More About Using ISIS
              </h1>
              <p>
                At this point you have installed ISIS and executed your first applications, meaning you are officially an ISIS user, congratulations!
              </p>
              <p>
                On this page you will find information and resources to help you further your skills as an ISIS user.
                The tutorials and explanations here will walk you through the basic steps of working with data in ISIS
                and provide information about how to customize your ISIS install.
              </p>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>



<!-- An example of how categoryItem and audience/target tags in the documentation resources
     can be used to auto populate their ToC reference.
      <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'guide' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       (normalize-space(audience/target) = 'user' or
                                        normalize-space(audience/target) = 'all')]">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>
      </xsl:for-each>

      <xsl:for-each select="//document[(normalize-space(category/categoryItem) = 'information' or
                                        normalize-space(category/categoryItem) = 'reference'or
                                        normalize-space(category/categoryItem) = 'technicaldoc') and
                                        normalize-space(category/categoryItem) != 'hidden' and
                                       (normalize-space(audience/target) = 'user' or
                                        normalize-space(audience/target) = 'all')]">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>
      </xsl:for-each>
-->
<!-- An example of putting the category / audience documents into a table structure
      <xsl:if test="//document[normalize-space(category/categoryItem) = 'guide' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'advanced']">
        <h3>Advanced</h3>
        <table class="tableTOC">
        <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'guide' and
                                         normalize-space(category/categoryItem) != 'hidden' and
                                         normalize-space(audience/target) = 'advanced']">
          <xsl:sort order="ascending" select="normalize-space(title)"/>
          <xsl:apply-templates mode="singleColumn" select="."/>
        </xsl:for-each>
      </table>
      </xsl:if>
-->

<!-- Intermediate Resources -->
    <hr/>
    <h2>Intermediate Resources</h2>
      <p>
        These resources are more complex than the introductory materials and are useful across workflows in ISIS.
      </p>
      <table class="tableTOCmulticol">
        <tbody>
          <tr valign="top">
            <td style="width:33%">
              <h4>General Image Processing Tutorials</h4>
              <p>
                <a href="https://doi-usgs.github.io/ISIS3/The_Power_of_Spatial_Filters.html">
                The Power of Spatial Filters</a><br/>
                A description of spatial filter applications in ISIS and common use cases.<br/>
                <a href="https://doi-usgs.github.io/ISIS3/Removing_Striping_Noise_from_Image_Data.html">
                Removing Striping Noise from Image Data</a><br/>
                Procedures for removing horizontal or vertical noise in an image.
              </p>
            </td>

            <td style="width:33%">
              <h4>Interactive Tool Demos</h4>
              <p>
                <a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/972/Qnet.pdf">
                Qnet</a><br/>
                An interactive tool to create and edit control networks.<br/>
                <a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/974/Qtie.pdf">
                Qtie</a><br/>
                An interactive tool to update the camera geometry of a single cube.<br/>
                <a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/975/Qview.pdf">
                Qview</a><br/>
                An interactive tool to display and analyze cubes.<br/>
                <a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/976/Qmos.pdf">
                Qmos</a><br/>
                An interactive tool to display and analyze cube footprints.
              </p>
            </td>

            <td style="width:33%">
              <h4>Cartography Concepts</h4>
              <p>
                <a href="https://github.com/DOI-USGS/ISIS3/wiki/Learning_About_Map_Projections">
                Learning About Map Projection</a><br/>
                An introduction to map projections and utilizing them in ISIS.<br/>
                <a href="https://github.com/DOI-USGS/ISIS3/wiki/Camera_Geometry">
                Camera Geometry</a><br/>
                A description of camera geometry within ISIS and some of the ISIS applications that have access to it.
              </p>
            </td>

          </tr>
        </tbody>
      </table>

      <br/>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/EnvironmentAndPreferencesSetup/EnvironmentAndPreferencesSetup.html">
            Environment and Preference Setup</a>
          </th>
          <td>
          This document describes how to setup your Unix environment to run ISIS and manage ISIS preference files for multiple users or installations.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="../Application/index.html"> 
            Software Manual</a>
          </th>
          <td>
          This document provides detailed documentation for every application in ISIS. If you are looking for an application to perform a specific task, 
          this is a great place to start.
          </td>
        </tr>
      </table>


<!-- Mission Specific ISIS Processing -->
      <hr/>
      <h2>Mission Specific ISIS Processing</h2>
      <p>
        These tutorials take you through all the steps required to process raw image data to map projected products for specific datasets.
        Each tutorial is accompanied by an overview and references for the mission and instrument that created the dataset.
      </p>

      <table class="tableTOCmulticol">
        <!-- hardcoded links -->
        <tr valign="top">
          <td style="width:33%">
            <h4>Moon</h4>
            <dl>
              <dt>Lunar Reconnaissance Orbiter</dt>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Lunar_Reconnaissance_Orbiter_MiniRF_Data">
              MiniRF</a></dd>
            </dl>
          </td>

          <td style="width:33%">
            <h4>Mars</h4>
            <dl>
              <dt><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Mars_Reconnaissance_Orbiter_(MRO)_Data">
              Mars Reconnaissance Orbiter</a></dt>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Mars_Reconnaissance_Orbiter_HiRISE_Data">
                High Resolution Imaging Science Experiment (HiRise)</a></dd>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Mars_Reconnaissance_Orbiter_CTX_Data">
                Context Imager (CTX)</a></dd>
              <dt><a href="https://github.com/DOI-USGS/ISIS3/wiki/Mars_Global_Surveyor_Mission">
              Mars Global Surveyor</a></dt>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Mars_Orbiter_Camera_Data">
                Mars Orbiter Camera (MOC)</a></dd>
              <dt><a href="https://github.com/DOI-USGS/ISIS3/wiki/Viking_Orbiter_Mission">
              Mars Viking Orbiter</a></dt>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Mars_Viking_Orbiter_Data">
                Narrow Angle Camera (NAC) and Wide Angle Camera (WAC)</a></dd>
            </dl>
          </td>

          <td style="width:33%">
            <h4>Other</h4>
            <dl>
              <dt><a href="https://github.com/DOI-USGS/ISIS3/wiki/CSS">
              Cassini-Huygens</a></dt>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Cassini_ISS_Data">
                Imaging Science Subsystem (ISS)</a></dd>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Cassini_RADAR">
                Radar</a></dd>
                <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Working_with_Cassini_VIMS">
                Visual and Infrared Mapping Spectrometer (VIMS)</a></dd>
            </dl>
          </td>

        </tr>
      </table>




<!-- FOOTER -->
<script type="text/javascript" language="JavaScript" src="../assets/scripts/footer.js"></script>
</div>


      </body>
    </html>

  </xsl:template>





<!-- ***************************-->
<!-- SINGLE COL TABLE DOC ENTRY -->

  <xsl:template match="document" mode="singleColumn">
                    <xsl:choose>

                      <!-- if there is only one file in the set -->
                      <xsl:when test="count(files/file) = 1">
                        <tr valign="top">
                          <th style="width:25%">
                            <xsl:variable name="docName" select="normalize-space(title)"/>
                            <xsl:variable name="docSrc"  select="normalize-space(files/file/source)"/>
                            <a href="{$docSrc}">
                            <xsl:value-of select="$docName"/></a>
                          </th>
                          <td>
                            <xsl:if test="files/file/size">
                            (<xsl:value-of select="files/file/type"/>, <xsl:value-of select="files/file/size"/>)
                            </xsl:if>
                            <xsl:value-of select="brief"/><br/>
                          </td>
                        </tr>
                      </xsl:when>

                      <!-- if there are multiple files in the set -->
                      <xsl:otherwise>
                        <xsl:for-each select="files/file">

                          <!-- look for the primary file and print table row -->
                          <xsl:if test="@primary = 'true'">

                            <tr valign="top">
                              <th style="width:25%">
                                <!-- link to the primary file -->
                                <xsl:variable name="docName" select="normalize-space(../../title)"/>
                                <xsl:variable name="docSrc"  select="normalize-space(source)"/>
                                <a href="{$docSrc}">
                                <xsl:value-of select="$docName"/></a>
                              </th>
                              <td>
                                <xsl:if test="size">
                                (<xsl:value-of select="type"/>, <xsl:value-of select="size"/>)
                                </xsl:if>

                                <xsl:value-of select="../../brief"/><br/>

                                  <!-- add links to secondary documents (e.g. chapters) -->
                                  <xsl:for-each select="../../files/file">
                                    <xsl:if test="@primary != 'true'">
                                      <xsl:choose>
                                        <xsl:when test="subtitle">
                                          <xsl:variable name="docName" select="normalize-space(subtitle)"/>
                                          <xsl:variable name="docSrc"  select="normalize-space(source)"/>
                                          <a href="{$docSrc}" style="font-size:80%; font-style:italic;">
                                          <xsl:value-of select="$docName"/></a>
                                        </xsl:when>
                                        <xsl:otherwise>
                                          <xsl:variable name="docName" select="type"/>
                                          <xsl:variable name="docSrc"  select="normalize-space(source)"/>
                                          <a href="{$docSrc}" style="font-size:80%; font-style:italic;">
                                          <xsl:value-of select="$docName"/></a>
                                        </xsl:otherwise>
                                      </xsl:choose>
                                      <xsl:if test="position() != last()"> | </xsl:if>
                                    </xsl:if>
                                  </xsl:for-each>
                              </td>
                            </tr>

                          </xsl:if>
                        </xsl:for-each>
                      </xsl:otherwise>

                    </xsl:choose>

  </xsl:template>



<!-- ***************************-->
<!-- MULTI COL TABLE DOC ENTRY -->

  <xsl:template match="document" mode="multiColumn">
        <!-- developer documents -->
                    <xsl:choose>

                      <!-- if there is only one file in the set -->
                      <xsl:when test="count(files/file) = 1">
                          <p>
                            <xsl:variable name="docName" select="normalize-space(title)"/>
                            <xsl:variable name="docSrc"  select="normalize-space(files/file/source)"/>
                            <a href="{$docSrc}">
                            <xsl:value-of select="$docName"/></a><br/>

                            <xsl:value-of select="brief"/><br/>
                            <xsl:if test="files/file/size">
                              (<xsl:value-of select="files/file/type"/>, <xsl:value-of select="files/file/size"/>)
                            </xsl:if>
                          </p>
                      </xsl:when>

                      <!-- if there are multiple files in the set -->
                      <xsl:otherwise>
                        <xsl:for-each select="files/file">

                          <!-- look for the primary file and print table row -->
                          <xsl:if test="@primary = 'true'">

                              <p>
                                <!-- link to the primary file -->
                                <xsl:variable name="docName" select="normalize-space(../../title)"/>
                                <xsl:variable name="docSrc"  select="normalize-space(source)"/>
                                <a href="{$docSrc}">
                                <xsl:value-of select="$docName"/></a>

                                <xsl:if test="size">
                                  (<xsl:value-of select="type"/>, <xsl:value-of select="size"/>)<br/>
                                </xsl:if>

                                <xsl:value-of select="../../brief"/><br/>

                                  <!-- add links to secondary documents (e.g. chapters) -->
                                  <xsl:for-each select="../../files/file">
                                    <xsl:if test="@primary != 'true'">
                                      <xsl:choose>
                                        <xsl:when test="subtitle">
                                          <xsl:variable name="docName_Subtitle" select="normalize-space(subtitle)"/>
                                          <xsl:variable name="docSrc_Subtitle"  select="normalize-space(source)"/>
                                          <a href="{$docSrc_Subtitle}" style="font-size:80%; font-style:italic;">
                                          <xsl:value-of select="$docName_Subtitle"/></a>
                                        </xsl:when>
                                        <xsl:otherwise>
                                          <xsl:variable name="docName_Type" select="type"/>
                                          <xsl:variable name="docSrc_Type"  select="normalize-space(source)"/>
                                          <a href="{$docSrc_Type}" style="font-size:80%; font-style:italic;">
                                          <xsl:value-of select="$docName_Type"/></a>
                                        </xsl:otherwise>
                                      </xsl:choose>
                                      <xsl:if test="position() != last()"> | </xsl:if>
                                    </xsl:if>
                                  </xsl:for-each>
                              </p>

                          </xsl:if>
                        </xsl:for-each>
                      </xsl:otherwise>

                    </xsl:choose>

  </xsl:template>


</xsl:stylesheet>
