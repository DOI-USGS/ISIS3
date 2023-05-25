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
            USGS Isis: Explore in Detail User Documentation
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
                Explore Using ISIS in Detail 
              </h1>
              <p>
                Now that you have some experience running applications and working with data, understanding the finer details
                about how ISIS works can make you a more efficient and powerful ISIS user.
                On this page you will find resources explaining individual components of ISIS.
                
              </p>
            </td>
            <td align="right" class="caption">
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>



<!-- General Purpose -->
      <hr/>
      <h2>General Purpose</h2>
      <p>
        These resources provide details about fundamental components of ISIS that 
        are used across applications.
      </p>
      <table class="tableTOCmulticol">

        <!-- hardcoded links -->
        <tr valign="top">
          <td style="width:50%">
            <dl>
              <dt><h4>About ISIS Cubes</h4></dt>
              <dd><a href="https://doi-usgs.github.io/ISIS3/ISIS_Cube_Format.html">
              ISIS Cube Format</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Understanding_Bit_Types">
              Understanding Bit Types</a></dd>
              <dd><a href="https://doi-usgs.github.io/ISIS3/Core_Base_and_Multiplier.html">
              Core Base and Multipliers</a></dd>
              <dd><a href="https://doi-usgs.github.io/ISIS3/Special_Pixels.html">
              Special Pixels</a></dd>
              <dd><a href="../documents/LabelDictionary/LabelDictionary.html">
              Label Dictionary</a></dd>
              <dd><a href="../documents/LogicalCubeFormatGuide/LogicalCubeFormatGuide.html">
              Logical Cube Format Guide</a></dd>
            </dl>
          </td>
          <td style="width:50%">
            <dl>
              <dt><h4>About ISIS Applications</h4></dt>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Introduction_to_ISIS#Reserved-Command-Line-Arguments-">
              Command Line Reference</a></dd>
              <dd><a href="../documents/Glossary/Glossary.html">
              Glossary</a></dd>
              <dd><a href="../documents/ErrorHandlingFacility/ErrorHandlingFacility.html">
              Error Handling Facility</a></dd>
              <dd><a href="../documents/ErrorDictionary/ErrorDictionary.html">
              Error Dictionary</a></dd>
              <dd><a href="../documents/PreferenceDictionary/PreferenceDictionary.html">
              Preference Dictionary</a></dd>
              <dd><a href="../documents/SessionLogs/SessionLogs.html">
              Session Log Explanation</a></dd>
            </dl>
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

<!-- Task Specific Guides -->
    <hr/>
    <h2>Task Specific Guides</h2>
      <p>
        These resources apply to specific applications or specialized processing methods.
        Over the years, ISIS has evolved in response to the needs of new datasets; so, there are a wide variety of 
        processing paths and customizations that you can leverage when creating your workflow.
      </p>
      <table class="tableTOCmulticol">

        <!-- hardcoded links -->
        <tr valign="top">
          <td style="width:33%">
            <dl>
              <dt><h4>About Specific Applications</h4></dt>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/General_Utility">
              fx Guide</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Autoseed">
              autoseed Guide</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/SPICE">
              SPICE in ISIS</a></dd>
              <dd><a href="../documents/PatternMatch/PatternMatch.html">
              Pattern Matching Guide</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Automatic_Registration">
              autoreg Guide</a></dd>
            </dl>
          </td>

          <td style="width:33%">
            <dl>
              <dt><h4>About Specialized Methods</h4></dt>
              <dd><a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/971/IsisDemoIntro.pdf">
              Photogrammetry in ISIS</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Image_Registration">
              Image Registration</a></dd>
              <dd><a href="https://github.com/DOI-USGS/ISIS3/wiki/Multi-Instrument_Registration">
              Multi-Instrument Registration</a></dd>
              <dd><a href="https://raw.githubusercontent.com/wiki/DOI-USGS/ISIS3/attachments/download/973/Jigsaw.pdf">
              Jigsaw Overview</a></dd>
            </dl>
          </td>

          <td style="width:33%">
            <dl>
              <dt><h4>Specialized Method References</h4></dt>
              <dd><a href="https://isis.astrogeology.usgs.gov/documents/ControlNetworks/index.html">
              Control Network Reference</a></dd>
              <dd><a href="">
              Map Definition Reference</a></dd>
            </dl>
          </td>
        </tr>
      </table>


<!-- Policies -->
      <hr/>
      <h2>Policies</h2>
      <p>
        These documents describe various policies about what users can expect when using ISIS.
      </p>

      <table class="tableTOC">
        <!-- hardcoded links -->
        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/DOI-USGS/ISIS3/wiki/Release-Schedule">
            Release Schedule</a>
          </th>
          <td>
          A description of ISIS's release cadence and expected dates of future releases.
          </td>
        </tr>

        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/BackwardCompatibility/BackwardCompatibility.html">
            Backward Compatibility Policy</a>
          </th>
          <td>
          What users can expect to still work after updating to a new version of ISIS.
          </td>
        </tr>

        <tr valign="top">
          <th style="width:25%">
            <a href="https://github.com/DOI-USGS/ISIS3/wiki/Deprecation">
            Deprecation Policy</a>
          </th>
          <td>
          ISIS functionality deprecation procedure.
          </td>
        </tr>

        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/UserRights/UserRights.html">
            User Rights</a>
          </th>
          <td>
          Licensing, copyright, distribution, and warranty information.
          </td>
        </tr>

        <tr valign="top">
          <th style="width:25%">
            <a href="../documents/Disclaimers/Disclaimers.html">
            Disclaimers</a>
          </th>
          <td>
          Standard legalese regarding usage, trademarks, and privacy.
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
