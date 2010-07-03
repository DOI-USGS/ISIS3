<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the Old-vs-New TOC for applications

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
            USGS: ISIS Application Table of Contents (Old vs. New Application Names)
        </title>
        <link rel="stylesheet" href="../assets/styles/IsisStyleCommon.css"></link>
        <link rel="stylesheet" href="presentation/PrinterFriendly/styles/IsisApplicationDocStyle.css"></link>
        <link rel="stylesheet" href="../assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="../assets/styles/print.css"/>

        <meta name="keywords" content="Isis, applications, table of contents, image processing"/>

        <meta name="description" content="Isis Applications Table of Contents - cross reference listing of old Isis application names verses new ones"/>
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
            <a href="alpha.html">Alphabetical</a> |
            <a href="../index.html">Home</a>
            </td>
          </tr>
          <tr valign="top">
            <td align="left">
              <h1>
                Old vs. New Application Names
              </h1>
            </td>
          </tr>
        </table>


<p>
Several applications have been renamed or evolved into multiple
applications in the newest version of Isis. The following table cross-references the current application names in Isis 3 to the names of applications in previous versions of Isis.
</p>




<!-- tables of links to documentation matching old names to new names -->

      <table>
        <tr>
          <th class="tableCellLevel1_th">
            Previous Versions
          </th>
          <th class="tableCellLevel1_th">
            Isis 3
          </th>
        </tr>

        <xsl:for-each select="//application/oldName/item[not(normalize-space(.)=preceding::application/oldName/item)]">
            <xsl:sort order="ascending" select="normalize-space(.)"/>
            <xsl:variable name="oldIsisName" select="normalize-space(.)"/>
                <tr>
                  <td class="tableCellLevel1" valign="top">
                    <xsl:value-of select="."/>
                  </td>
                  <td class="tableCellLevel1" valign="top">
                    <ul>
                    <xsl:for-each select="//application/oldName/item">
                      <xsl:if test="normalize-space(.) = $oldIsisName">
                        <li>
                        <xsl:variable name="appName" select="normalize-space(../../name)"/>
                        <a href="presentation/Tabbed/{$appName}/{$appName}.html">
                        <xsl:value-of select="../../name"/></a>
                        </li>
                      </xsl:if>
                    </xsl:for-each>
                    </ul>
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

