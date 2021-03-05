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

    <div>
      <a href="{$menuPath}AboutIsis/index.html" target="_top">
      About ISIS</a>
    </div>

    <div>
      <a href="https://github.com/USGS-Astrogeology/ISIS3/issues" target="_blank" title="Launch the ISIS Support Center in a new window">
      Support</a>
    </div>

    <div>
      <a href="{$menuPath}Installation/index.html" target="_top">
      Download</a>
    </div>

    <hr/>
    <h2>
      ISIS
    </h2>

    <div>
      <a href="{$menuPath}UserDocs/index.html" target="_top">
      Documentation</a>
    </div>

    <!-- Isis Workshop is only available on the website  -->
    <div>
      <a href="https://isis.astrogeology.usgs.gov/fixit/projects/isis/wiki/ISIS_Online_Workshops" target="_blank" title="Launch ISIS Workshop in a new window">
      Tutorials</a>
    </div>

    <div>
      <a href="{$menuPath}TechnicalInfo/index.html" target="_top">
      Technical Documents</a>
    </div>


    <hr/>
    <h2>
      ISIS 2
    </h2>

    <div>
      <a href="{$menuPath}documents/Isis2UserDocs/index.html" target="_top">
      Documentation</a>
    </div>

    <div>
      <a href="{$menuPath}documents/Isis2Tutorials/index.html" target="_top">
      Tutorials</a>
    </div>

    <div>
      <a href="{$menuPath}documents/Isis2TechnicalDocs/index.html" target="_top">
      Technical Documents</a>
    </div>


    <!-- <hr/>
        <h2>Search</h2> -->

        <!-- search -->
        <!-- <form name="seek1" method="get" action="http://search.usgs.gov/query.html" target="_top" style="padding-top:0px;margin-top:0px;">
          <table  style="width:150px;" align="center">
          <tr valign="top">
          <td>
            <table class="AstroIsisSearch">
              <tr valign="top">
                <td>
                <input type="hidden" name="col" value="usgs"/>
                <input type="hidden" name="qp" value=""/>
                <input type="hidden" name="qs" value=""/>
                <input type="hidden" name="qc" value=""/>
                <input type="hidden" name="pw" value="100%"/>
                <input type="hidden" name="ws" value="1"/>
                <input type="hidden" name="la" value=""/>
                <input type="hidden" name="qm" value="1"/>
                <input type="hidden" name="st" value="1"/>
                <input type="hidden" name="nh" value="10"/>
                <input type="hidden" name="lk" value="1"/>
                <input type="hidden" name="rf" value="0"/>
                <input type="hidden" name="oq" value="site:isis.astrogeology.usgs.gov"/>
                <input type="hidden" name="rq" value="1"/>
                <div align="center">
                <input type="text" alt="Search term" name="qt" size="12" maxlength="2033"/><input alt="Search" type="submit" value="Go"/><br/>
                <a href="{$menuPath}search.html" target="_top">Advanced</a> |
                <a href="http://search.usgs.gov/searchhelp.html"
                   target="_top">Help</a>
                </div>
                </td>
              </tr>
            </table>
          </td>
          </tr>
          </table>
        </form> -->

  </xsl:template>

</xsl:stylesheet>
