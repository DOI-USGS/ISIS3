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
            USGS Isis: Get Inspired User Documentation
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
                Get Inspired by Other ISIS Users
              </h1>
              <p>
                These papers, talks and 
                demonstrations are examples that you can draw inspiration from and model your own workflows after.
              </p>
              <p>
                If you have examples you would like to add to this page, submit a pull request on
                <a href="https://github.com/DOI-USGS/ISIS3">GitHub</a>!
              </p>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>



<!-- Talks -->
      <hr/>
      <h2>Talks</h2>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://youtu.be/rxb5_Q0xrqI?t=5040">
            A New Approach to Create Image Control Networks in ISIS
            </a>
          </th>
          <td>
            A presentation given by Kris Becker concerning the motivation and work done to create the <i>findfeatures</i> application. The corresponding abstract 
            can be found <a href="https://www.hou.usra.edu/meetings/planetdata2017/pdf/7133.pdf">here</a>.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="https://www.youtube.com/watch?v=IYpe-RJj5tE">
            Using the Community Sensor Model with Planetary Data</a>
          </th>
          <td>
            A talk given by Jesse Mapel at an OpenPlanetary Lunch covering the Community Sensor Model API, the USGS ASC's implementation of it, 
            supported cameras as of October 2021, and how to use them in ISIS.
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

<!-- Papers and Posters -->
    <hr/>
    <h2>Papers, Posters, and Abstracts</h2>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:35%">
            <a href="https://www.hou.usra.edu/meetings/lpsc2018/pdf/2538.pdf">
            Production of New Clementine UVVIS Map Products Tied to the LRO Reference Frame</a>
          </th>
          <td>
          In the production of the new product, the LROC team leveraged ISIS to ingest and calibrate the Clementine image as well as used the <i>findfeatures</i> 
          application to automatically generate control points in their registration process.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:35%">
            <a href="https://www.isprs-ann-photogramm-remote-sens-spatial-inf-sci.net/I-4/203/2012/isprsannals-I-4-203-2012.pdf">
            Jigsaw: The ISIS3 Bundle Adjustment for Extraterrestrial Photogrammetry</a>
          </th>
          <td>
            One of the original authors of the <i>jigsaw</i> application describes the sparse matrix method of the photogrammetric bundle implemented in the 
            <i>jigsaw</i> application.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:35%">
            <a href="https://www.hou.usra.edu/meetings/lpsc2014/eposter/1915.pdf">
            Photometric Control of Oblique Apollo 15 Metric Camera Images</a>
          </th>
          <td>
            Assessing the capabilities of the ISIS software to photometrically and geodetically control oblique images from the Apollo Metric Camera (MC) data set.
          </td>
        </tr>
        <tr valign="top">
          <th style="width:35%">
            <a href="https://www.hou.usra.edu/meetings/lpsc2014/pdf/2892.pdf">
            Automated and Accurate: Making DTMs from LRO-NAC using the Ames Stereo Pipeline</a>
          </th>
          <td>
            Outline of the ISIS and ASP commands used by the LROC team to generate automated Digital Terrain Models (DTMs).
          </td>
        </tr>
        <tr valign="top">
          <th style="width:35%">
            <a href="https://agupubs.onlinelibrary.wiley.com/doi/pdfdirect/10.1029/2007JE003000">
            Ultrahigh resolution topographic mapping of Mars with MRO HiRISE stereo images: Meter-scale slopes of candidate Phoenix landing sites</a>
          </th>
          <td>
            An analysis of candidate landing sites for the Phoenix mission in which ISIS helped calibrate and rectify images from the Mars Global Surveyor 
            (MGS) Mars Orbiter Camera (MOC) and Mars Reconnaissance Orbiter (MRO) High Resolution Imaging Science Experiment (HiRISE) instruments.
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
