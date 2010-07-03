<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the alphabetical TOC for applications

Author
Deborah Lee Soltesz
4/2002

-->


  <xsl:output 
    media-type="text/html" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

  <xsl:include href="../../build/menu.xsl"/>


  <xsl:key name="categoryMatch" match="/tableofcontents/application" use="category/categoryItem"/>


  <xsl:template match="/">
     <xsl:apply-templates select="tableofcontents" />
  </xsl:template>


  <xsl:template match="tableofcontents">
    <html>
      <head>
        <title>
            USGS: ISIS Application Table of Contents (Alphabetical Listing)
        </title>
        <link rel="stylesheet" href="../assets/styles/IsisStyleCommon.css"></link>
        <link rel="stylesheet" href="presentation/PrinterFriendly/styles/IsisApplicationDocStyle.css"></link>
        <link rel="stylesheet" href="../assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="../assets/styles/print.css"/>

        <meta name="keywords" content="Isis, applications, table of contents, image processing"/>

        <meta name="description" content="Isis Applications Table of Contents listed in alphabetical order"/>
        <meta name="publisher" content="USGS - GD - Astrogeology Research Program"/>
        <meta name="author" content="Deborah Lee Soltesz, webteam@astrogeology.usgs.gov"/>

        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <meta name="country" content="USA"/>
        <meta name="state" content="AZ"/>
        <meta name="county" content="Coconino"/>
        <meta name="city" content="Flagstaff"/>
        <meta name="zip" content="86001"/>


       </head>

      <body>

        <div class="isisMenu">
         <xsl:call-template  name="writeMenu"/>
        </div>

        <div class="isisContent">

        <a href="http://www.usgs.gov">
        <img src="../assets/icons/littleVIS.gif" width="80" height="22" border="0" alt="USGS"/></a><br/>


        <p style="margin-top:10px; margin-bottom:0px;">
        Isis 3 Application Documentation</p>

        <hr/>


        <table width="100%" cellpadding="0" border="0" cellspacing="0">
          <tr valign="top">
            <td align="right" class="caption">
              <a href="index.html">Categorical</a> |
              <a href="oldvnew.html">Old vs. New</a> |
              <a href="../index.html">Home</a>
            </td>
          </tr>
          <tr valign="top">
            <td align="left">
              <h1>
                Alphabetical Listing of Applications
              </h1>
            </td>
          </tr>
        </table>




<!-- Alphabetical tables of links to documentation -->

            <table>
            <xsl:for-each select="/tableofcontents/application">
              <xsl:sort order="ascending" select="name"/>
                <tr>
                  <th class="tableCellLevel1_th">
                    <xsl:variable name="appName" select="normalize-space(name)"/>
                    <a href="presentation/Tabbed/{$appName}/{$appName}.html">
                    <xsl:value-of select="$appName"/></a>
                  </th>
                  <td class="tableCellLevel1">
                    <xsl:if test="brief">
                    <xsl:value-of select="brief"/><br/>
                    </xsl:if>
                  </td>
                </tr>
            </xsl:for-each>
            </table>







<!-- FOOTER -->
<script type="text/javascript" language="JavaScript" src="../assets/scripts/footer.js"></script>
</div>


      </body>
    </html>

  </xsl:template>

</xsl:stylesheet>

