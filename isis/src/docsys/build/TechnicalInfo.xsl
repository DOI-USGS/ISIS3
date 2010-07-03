<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the main TOC for the "TechnicalInfo/Developer's Zone" page

Author
Deborah Lee Soltesz
12/05/2002

06/03/2009 - Jeannie Walldren - Removed hardcoded link to ApplicationTest documentation
-->


  <xsl:output 
    media-type="text/html" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

  <xsl:key name="categoryMatch" match="/tableofcontents/document" use="category/categoryItem"/>
  <xsl:key name="audienceMatch" match="/tableofcontents/document" use="audience/target"/>

  <xsl:include href="menu.xsl"/>

  <xsl:template match="/">
     <xsl:apply-templates select="tableofcontents" />
  </xsl:template>


  <xsl:template match="tableofcontents">
    <html>
      <head>
        <title>
            USGS Isis: Technical Documents
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
                Technical Documents
              </h1>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>


<!-- Developer documentation -->
      <hr/>
      <h2>For Application Developers</h2>


<table class="tableTOCmulticol">
  <tr valign="top">
    <td style="width:50%">

        <h4>API</h4>

        <!-- hard coded links -->
          <p>
            <a href="../Object/Developer/index.html" target="_blank">
            Isis Developers Reference</a><br/>
            A handy reference guide to the Isis API and documentation for all Isis libraries and objects
            for developers of new Isis applications
          </p>

      <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'api' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'developer']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>

        <xsl:apply-templates mode="multiColumn" select="."/>

      </xsl:for-each>

    </td>

    <!-- XML -->
    <td style="width:50%">

        <h4>XML</h4>

        <!-- hard coded links -->
          <p>
            <a href="../Schemas/Application/documentation/index.html">
            Isis Application XML Reference</a><br/>
            Reference guide for the Isis Application XML language
          </p>
      
        <!-- hard coded links -->
          <p>
            <a href="../Schemas/Documentation/documentation/index.html">
            Isis Documentation XML Reference</a><br/>
            Reference guide for the Isis Documentation XML language
          </p>

      <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'xml' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'developer']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>

        <xsl:apply-templates mode="multiColumn" select="."/>
      </xsl:for-each>

    </td>
  </tr>
</table>


<!-- Administrator documentation -->
      <hr/>
      <h2>For System Administrators</h2>
      <table class="tableTOC">

      <xsl:for-each select="//document[normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'administrator']">
        <xsl:sort order="ascending" select="normalize-space(.)"/>
        <xsl:apply-templates mode="singleColumn" select="."/>

      </xsl:for-each>
      </table>





<!-- Programmer documentation -->
      <hr/>
      <h2>For Low-Level Programmers</h2>


<table class="tableTOCmulticol">
  <tr valign="top">
    <td style="width:50%">

        <!-- API -->
        <h4>API</h4>

        <!-- hard coded links -->
          <p>
                  <a href="../Object/Programmer/index.html" target="_blank">
                  Isis Programmers Reference</a><br/>
                  An indepth reference guide for programmers who modify the Isis software package
          </p>

      <xsl:for-each select="//document[normalize-space(category/categoryItem) = 'api' and
                                       normalize-space(category/categoryItem) != 'hidden' and
                                       normalize-space(audience/target) = 'programmer']">
        <xsl:sort order="ascending" select="normalize-space(title)"/>
        <xsl:apply-templates mode="multiColumn" select="."/>
      </xsl:for-each>

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
                                  <xsl:for-each select="../../files/file[@primary != 'true']">
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

