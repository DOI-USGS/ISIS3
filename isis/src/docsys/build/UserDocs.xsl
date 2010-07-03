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
            USGS Isis: User Documentation
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
      </head>

      <body>

        <div class="isisMenu">
         <xsl:call-template  name="writeMenu"/>
        </div>

        <div class="isisContent">

        <a href="http://www.usgs.gov">
        <img src="../assets/icons/littleVIS.gif" width="80" height="22" border="0" alt="USGS"/></a><br/>


        <p style="margin-top:10px; padding-bottom:0px;">
        Isis 3 Documentation</p>

        <hr/>


        <table width="100%" cellpadding="0" border="0" cellspacing="0">
          <tr valign="top">
            <td align="left">
              <h1>
                User Documentation
              </h1>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>



<!-- Getting Started -->
      <hr/>
      <h2>Getting Started</h2>
      <table class="tableTOC">

        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="../Application/index.html">
            Software Manual</a>
          </th>
          <td>
          Handy reference guide to all Isis applications
          </td>
        </tr>
        <tr valign="top">
          <th style="width:25%">
            <a href="http://isis.astrogeology.usgs.gov/IsisWorkshop" target="_blank" title="Launch Isis Workshop in a new window">
            Isis Workshop</a>
          </th>
          <td>
          Interactive and hands on tutorials designed to guide users through the basics of using Isis to advanced processing techniques for creating mosaics from mission data,
          correcting and enhancing problem data, and other techniques.
          </td>
        </tr>

      <!-- User -->
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
      </table>



<!-- Guides -->
    <hr/>
    <h2>Guides</h2>
      <!-- Intermediate -->
      <h3>Intermediate</h3>
      <table class="tableTOC">
      <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'guide' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'intermediate']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>
      </xsl:for-each>
      </table>

      <!-- Advanced -->
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



<!-- References -->
      <hr/>
      <h2>References</h2>

      <table class="tableTOC">
        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="../Application/index.html">
            Software Manual</a>
          </th>
          <td>
          Handy reference guide to all Isis applications
          </td>
        </tr>

        <tr valign="top">
          <th style="width:25%">
            <a href="../Application/oldvnew.html">
            Old vs. New Program Names</a>
          </th>
          <td>
          A useful table cross-referencing the current application names in Isis 3 to the names of applications in previous versions of Isis.

          </td>
        </tr>

      <!-- Intermediate -->
      <xsl:for-each select="//document[(normalize-space(category/categoryItem) = 'information' or
                                        normalize-space(category/categoryItem) = 'reference'or
                                        normalize-space(category/categoryItem) = 'technicaldoc') and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'intermediate']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>
      </xsl:for-each>

      <!-- Advanced -->
      <xsl:for-each select="//document[(normalize-space(category/categoryItem) = 'information' or
                                        normalize-space(category/categoryItem) = 'reference'or
                                        normalize-space(category/categoryItem) = 'technicaldoc') and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'advanced']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>
      </xsl:for-each>
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

