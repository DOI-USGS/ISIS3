<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the top level index page which redirects to the current public release docs

Author
Deborah Lee Soltesz
04/15/2003

Updated to be just a redirect page
03/15/2022
-->

<xsl:include href="menu.xsl"/>

<xsl:output 
  media-type="text/html" 
  doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
  doctype-system="http://www.w3.org/TR/html4/loose.dtd"
  indent="yes"
  encoding="utf-8"
  omit-xml-declaration="yes">
  <xsl:template match="/">
     <xsl:apply-templates select="//redirectPage" />
  </xsl:template>
</xsl:output>

  <xsl:template match="//redirectPage">
      <html>
      <head>
        <title>USGS Isis: Planetary Image Processing Software</title>
        <meta name="keywords" content="Isis, image processing, software, open source, remote sensing, planetary science, astrogeology"/>
        <!-- need ratings tag -->
        <meta name="description" content="Integrated Software for Imagers and Spectrometers (ISIS), created and managed by the USGS Astrogeology Research Program. ISIS provides a comprehensive, user-friendly, portable tool for processing, analyzing, and displaying remotely sensed image data."/>
        <meta name="publisher" content="USGS - GD - Astrogeology Program"/>
        <meta name="author" content="Deborah Lee Soltesz, webteam@astrogeology.usgs.gov"/>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <meta name="country" content="USA"/>
        <meta name="state" content="AZ"/>
        <meta name="county" content="Coconino"/>
        <meta name="city" content="Flagstaff"/>
        <meta name="zip" content="86001"/>

        <link rel="stylesheet" href="assets/styles/IsisStyleCommon.css"/>
        <link rel="stylesheet" href="assets/styles/main.css"/>
        <link rel="stylesheet" href="assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="assets/styles/print.css"/>

        <script type="text/javascript" src="assets/scripts/homepage.js"></script>
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
      <!-- ** end  PAGE HEADER needs these scripts  ** -->

      </head>

      <body>

        <div class="isisMenu">
          <xsl:call-template  name="writeMenu"/>
         </div>

       <div class="isisContent">
        <!--xsl:copy-of select="."/-->
        <xsl:apply-templates select="* | text()" mode="copyContents"/>
       </div>

      <!-- end of body -->
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

