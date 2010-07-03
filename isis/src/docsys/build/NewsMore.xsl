<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the "More News" page

Author
Deborah Lee Soltesz
04/04/2003

-->

  <xsl:output 
    media-type="text/html" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

  <xsl:include href="menu.xsl"/>
  
  <xsl:param name="dateNow"/>
  <xsl:param name="IsisVer"/>

  <xsl:template match="/">
      <xsl:apply-templates select="news" />
  </xsl:template>

  <xsl:template match="news">


    <html>
      <head>
        <title>
            USGS Isis: Isis News
        </title>
        <meta name="keywords" content="news, Isis, image processing, software, open source, remote sensing, planetary science, astrogeology"/>
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
        Isis News</p>

        <hr/>


        <table width="100%" cellpadding="0" border="0" cellspacing="0">
          <tr valign="top">
            <td align="left">
              <h1>
                Headlines: Isis
                <xsl:choose>
                  <xsl:when test="$IsisVer = '3'">3</xsl:when>
                  <xsl:when test="$IsisVer = '2'">2.1</xsl:when>
                  <xsl:otherwise></xsl:otherwise>
                </xsl:choose>
              </h1>
            </td>
            <td align="right" class="caption">
              <script language="javascript" type="text/javascript">
                //<xsl:comment><![CDATA[
                  // create back link if javascript is available
                  if (history.length > 1) {
                    document.write ("<a" + " href='javascript:history.back();'>Back</" + "a> | ") ;
                  }
                //]]></xsl:comment>
              </script>
            <a href="../index.html">Home</a>
            </td>
          </tr>
        </table>

      <hr/>

           <!-- NEWS -->

              <xsl:for-each select="newsItem[not(newsDate = preceding::newsItem/newsDate)]">
                <xsl:sort select="newsDate" order="descending"/>
                <xsl:variable name="newsDateReporting" select="normalize-space(newsDate)"/>
                <h3><xsl:value-of select="$newsDateReporting"/></h3>
                <xsl:for-each select="../newsItem[(info/item = $IsisVer or info/item = 'both') and
                                               (expireDate &gt;  $dateNow or not(expireDate)) and
                                               (activeDate &lt;= $dateNow or not(activeDate)) and
                                               (normalize-space(newsDate) = $newsDateReporting)]">
                    <p style="margin-left:30px;">
                      <a href="Isis{$IsisVer}-{normalize-space(newsDate)}_{position()}.html">
                      <xsl:value-of select="headline"/></a>
                      <xsl:if test="brief">
                        <br/>
                        <xsl:value-of select="brief"/>
                      </xsl:if>
                    </p>
                </xsl:for-each>
              </xsl:for-each>

            <!-- END NEWS -->



      <!-- FOOTER -->
      <script type="text/javascript" language="JavaScript" src="../assets/scripts/footer.js"></script>
</div>


      </body>
    </html>

  </xsl:template>
</xsl:stylesheet>















