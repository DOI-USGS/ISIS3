<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    exclude-result-prefixes="xmlns fo">

  <!--

  This stylesheet will be used to generate the menu for all pages,
  Include this file in other XSL files, define variable for menuPath,
  and apply the template mode writeMenu .

  Authors:
  Deborah Lee Soltesz - 2/13/2002
  Jacob Cain and Amy Stamile - 11/22/2024

  -->

  <xsl:template mode="writeMenuContents" name="writeMenuContents">
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
        <ul class="usa-sidenav__sublist" id="versions-menu-large">
        </ul>
      </li>
    </ul>
    <script><![CDATA[
      fetch('/versions.json')
      .then(response => response.json())
      .then(versions => {
        const largeMenu = document.getElementById('versions-menu-large');
        const smallMenu = document.getElementById('versions-menu-small');
        
        largeMenu.innerHTML = '';
        smallMenu.innerHTML = '';
        
        const latestLi = document.createElement('li');
        latestLi.className = 'usa-sidenav__item';
        latestLi.innerHTML = `<a href="https://isis.astrogeology.usgs.gov">Latest Release</a>`;
        
        largeMenu.appendChild(latestLi);
        smallMenu.appendChild(latestLi.cloneNode(true));

        const pathParts = window.location.pathname.split('/');
        let currentVersion = pathParts[1];
        if (currentVersion.endsWith('/')) {
          currentVersion = currentVersion.slice(0, -1);
        }

        versions.forEach(version => {
          const li = document.createElement('li');
          li.className = 'usa-sidenav__item';
          if (currentVersion === version) {
            li.classList.add('usa-current');
          }
          li.innerHTML = `<a href="https://isis.astrogeology.usgs.gov/${version}/">${version}</a>`;
          
          largeMenu.appendChild(li);
          smallMenu.appendChild(li.cloneNode(true));  // Clone for the smaller menu
        });
      })
      .catch(error => {
        console.error('Failed to load versions list', error);
      });
    ]]></script>           
  </xsl:template>

  <xsl:template mode="writeMiniMenuContents" name="writeMiniMenuContents">
    <ul class="usa-sidenav">
      <li class="usa-sidenav__item">
        <a href="{$menuPath}index.html" target="_top" id="homeLink">Home/About</a>
      </li>
      <li class="usa-sidenav__item">
        <div class="usa-accordion">
          <h2 class="usa-accordion__heading">
            <button class="usa-accordion__button text-normal bg-white" aria-expanded="false" aria-controls="app-mini-menu">
              Application&#160;Manuals
            </button>
          </h2>
          <div id="app-mini-menu" class="usa-accordion__content usa-prose" hidden="hidden">
            <ul class="usa-sidenav__sublist">
              <li class="usa-sidenav__item">
                <a href="{$menuPath}Application/index.html">By Category</a>
              </li>
              <li class="usa-sidenav__item">
                <a href="{$menuPath}Application/alpha.html">By Alphabetical</a>
              </li>
            </ul>
          </div>
        </div>
      </li>
      <li class="usa-sidenav__item">
        <a href="https://github.com/DOI-USGS/ISIS3">GitHub</a>
      </li>
      <li class="usa-sidenav__item">
        <a href="{$menuPath}Object/Developer/index.html">API</a>
      </li>
      <li class="usa-sidenav__item">
        <div class="usa-accordion">
          <h2 class="usa-accordion__heading">
            <button class="usa-accordion__button text-normal bg-white" aria-expanded="false" aria-controls="versions-menu-small">
              Versions
            </button>
          </h2>
          <div id="versions-menu-small" class="usa-accordion__content usa-prose" hidden="hidden">
            <ul class="usa-sidenav__sublist"></ul>
          </div>
        </div>
      </li>
    </ul>        
  </xsl:template>

  <xsl:template mode="writeFrontPageMenu" name="writeFrontPageMenu">

    <nav aria-label="usa-sidenav" class="sidenav">

      <div id="nav-title">
        <div id="nav-title-text-box">
          <em id="nav-title-text">ISIS Application<br />Manuals</em>
        </div>
      </div>

      <xsl:call-template  name="writeMenuContents"/>

    </nav>

  </xsl:template>

  <xsl:template mode="writeMenu" name="writeMenu">

    <nav aria-label="usa-sidenav" class="sidenav">

      <div id="nav-title">
        <div>
          <img id="nav-title-logo" height="65" src="{$menuPath}assets/img/isis-logo-yellow-notxt.svg"></img>
        </div>
        <div id="nav-title-text-box">
          <em id="nav-title-text">ISIS Application<br />Manuals</em>
        </div>
      </div>

      <xsl:call-template  name="writeMenuContents"/>
      
    </nav>

  </xsl:template>

  <xsl:template mode="writeMiniMenu" name="writeMiniMenu">

  <nav aria-label="usa-sidenav" class="sidenav">

    <xsl:call-template  name="writeMiniMenuContents"/>
    
  </nav>

</xsl:template>

</xsl:stylesheet>
