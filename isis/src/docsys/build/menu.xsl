<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to generate the menu for all pages,
Include this file in other XSL files, define variable for menuPath,
and apply the template mode writeMenu .

Author
Deborah Lee Soltesz
12/13/2002

-->
  <xsl:param name="menuPath"/>

  <xsl:template mode="writeMenu" name="writeMenu">
    <div>
      <a href="{$menuPath}index.html" target="_top" id="homeLink">
      Home</a>
    </div>

    <hr/>
    <h2>
      Quick Links
    </h2>

    <div>
      <a href="{$menuPath}Application/index.html">
      Software Manual</a>
    </div>

    <div>
      <a href="https://github.com/DOI-USGS/ISIS3">
      GitHub</a>
    </div>

    <div>
      <a href="{$menuPath}Object/Developer/index.html">
      API Reference</a>
    </div>

    <hr/>
    <h2>
      Documentation Versions
    </h2>

    <div>
      <a href="https://isis.astrogeology.usgs.gov">Public Release</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/8.1.0/">8.1.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/8.0.0/">8.0.0</a>
    </div>  
    <div>
      <a href="https://isis.astrogeology.usgs.gov/7.2.0/">7.2.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/7.1.0/">7.1.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/7.0.0/">7.0.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/6.0.0/">6.0.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/3.9.0/">3.9.0</a>
    </div>
    <div>
      <a href="https://isis.astrogeology.usgs.gov/3.5.0/">3.5.0</a>
    </div>

  </xsl:template>

</xsl:stylesheet>
