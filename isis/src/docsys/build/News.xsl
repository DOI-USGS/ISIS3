<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate a page for a single news article, containing the
complete content of the article

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
  
  <xsl:param name="filenameParam"/>

  <xsl:template match="/">
    <xsl:for-each select="news/newsItem[info/item = '3' or info/item = 'both']">
      <xsl:sort select="newsDate" order="descending"/>
      <xsl:if test="concat('Isis3-', newsDate, '_', position()) = $filenameParam">
        <xsl:apply-templates select="." />
      </xsl:if>
    </xsl:for-each>

    <xsl:for-each select="news/newsItem[info/item = '2' or info/item = 'both']">
      <xsl:sort select="newsDate" order="descending"/>
      <xsl:if test="concat('Isis2-', newsDate, '_', position()) = $filenameParam">
        <xsl:apply-templates select="." />
      </xsl:if>

    </xsl:for-each>
  </xsl:template>

  <xsl:template match="news/newsItem">


    <html>
      <head>
        <title>
            USGS Isis: User Documentation
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
                <xsl:value-of select="headline"/>
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

      <p><em><xsl:value-of select="newsDate"/></em></p>

      <xsl:if test="article/body">
        <xsl:copy-of select="article/body"/>
      </xsl:if>

      <xsl:if test="article/links/link[@linkType = 'article']">
        <hr/>
        <h3>Read More!</h3>
        <ul>
          <xsl:for-each select="article/links/link[@linkType = 'article']">
            <li>
              <a href="{@src}"><xsl:value-of select="title"/></a>:
              <xsl:value-of select="brief"/>
            </li>
          </xsl:for-each>
        </ul>
      </xsl:if>

      <xsl:if test="article/links/link[@linkType = 'related']">
        <h3>Related Links</h3>
        <ul>
          <xsl:for-each select="article/links/link[@linkType = 'related']">
            <li>
              <a href="{@src}"><xsl:value-of select="title"/></a>:
              <xsl:value-of select="brief"/>
            </li>
          </xsl:for-each>
        </ul>
      </xsl:if>

      <!-- FOOTER -->
      <script type="text/javascript" language="JavaScript" src="../assets/scripts/footer.js"></script>
</div>

      </body>
    </html>

  </xsl:template>

  <xsl:template match="*" mode="copyContents">
    <xsl:element name="{name()}" namespace="{namespace-uri()}">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates mode="copyContents"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="text()" mode="copyContents">
      <xsl:value-of select="."/>
      <xsl:apply-templates mode="copyContents"/>
  </xsl:template>

</xsl:stylesheet>















