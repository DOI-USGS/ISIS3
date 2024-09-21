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
  <xsl:template mode="writeMenu" name="writeMenu">

    <nav aria-label="usa-sidenav" class="sidenav">

      <div id="nav-title">
        <div>
          <img id="nav-title-logo" height="65" src="{$menuPath}assets/img/isis-logo-yellow-notxt.svg"></img>
        </div>
        <div id="nav-title-text-box">
          <em id="nav-title-text">ISIS<br />Documentation</em>
        </div>
      </div>

      <ul class="usa-sidenav">
        <li class="usa-sidenav__item">
          <a href="{$menuPath}index.html" target="_top" id="homeLink">Home/About</a>
        </li>
        <li class="usa-sidenav__item">
          <a href="{$menuPath}Application/index.html" target="_top" id="homeLink">Application&#160;Manuals</a>
          <ul class="usa-sidenav__sublist">
            <li class="usa-sidenav__item">
              <a href="{$menuPath}Application/index.html">By Category</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="{$menuPath}Application/alpha.html">By Alphabetical</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="{$menuPath}Application/oldvnew.html">Old vs. New</a>
            </li>
          </ul>
        </li>
        <li class="usa-sidenav__item">
          <a href="https://github.com/DOI-USGS/ISIS3">GitHub</a>
        </li>
        <li class="usa-sidenav__item">
          <a href="{$menuPath}Object/Developer/index.html">API</a>
        </li>
        <li class="usa-sidenav__item">
          <a href="https://isis.astrogeology.usgs.gov">Versions</a>
          <ul class="usa-sidenav__sublist">
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov">Public&#160;Release</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/8.1.0/">8.1.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/8.0.0/">8.0.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/7.2.0/">7.2.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/7.1.0/">7.1.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/7.0.0/">7.0.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/6.0.0/">6.0.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/3.9.0/">3.9.0</a>
            </li>
            <li class="usa-sidenav__item">
              <a href="https://isis.astrogeology.usgs.gov/3.5.0/">3.5.0</a>
            </li>
          </ul>
        </li>
      </ul>
    </nav>

  </xsl:template>

</xsl:stylesheet>
