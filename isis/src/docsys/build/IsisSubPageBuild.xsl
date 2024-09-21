<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    exclude-result-prefixes="xmlns fo">

<!--
This stylesheet will be used to transform a Documentation XML file into an HTML page.
If there are multiple pages in the document, this stylesheet is used to generate the sub-page
of the set, and IsisPrimaryPageBuild.xsl is used to generate the main page

Author
Deborah Lee Soltesz
12/04/2002
-->


  <xsl:output
    media-type="text/html"
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

  <xsl:param name="menuPath"/>

  <xsl:include href="header.xsl"/>
  <xsl:include href="menu.xsl"/>
  <xsl:include href="footer.xsl"/>

  <xsl:param name="filenameParam"/>


  <xsl:output
    media-type="text/html"
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>


  <xsl:template match="/">
     <xsl:apply-templates select="documentation" />
  </xsl:template>

  <xsl:template name="document" match="documentation">
    <html>

      <head>
        <title>
          USGS Isis: <xsl:value-of select="bibliography/title"/>
          <xsl:if test="files/file/subtitle and normalize-space(files/file) = normalize-space($filenameParam)">
          - <xsl:value-of select="files/file/subtitle"/>
          </xsl:if>
        </title>

        <meta name="author" content="{normalize-space(bibliography/author)}"/>
        <meta name="description" content="{normalize-space(bibliography/description)}"/>
        <xsl:choose>
          <xsl:when test="bibliography/publisher">
            <meta name="publisher" content="{normalize-space(bibliography/publisher)}"/>
          </xsl:when>
          <xsl:otherwise>
            <meta name="publisher" content="USGS-GD-Astrogeology"/>
          </xsl:otherwise>
        </xsl:choose>

        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <meta name="country" content="USA"/>
        <meta name="state" content="AZ"/>
        <meta name="county" content="Coconino"/>
        <meta name="city" content="Flagstaff"/>
        <meta name="zip" content="86001"/>

        <!-- Govt -->
        <link rel="stylesheet" href="../../assets/styles/uswds.css"/>
        <script src="../../../../assets/scripts/uswds-init.min.js"></script>
        
        <!-- USGS -->
        <link rel="stylesheet" href="../../assets/styles/usgs/common.css" />
        <link rel="stylesheet" href="../../assets/styles/usgs/custom.css" />

        <!-- ISIS Docs -->
        <link rel="stylesheet" href="../../assets/styles/IsisStyleCommon.css"></link>
        <link rel="stylesheet" href="../styles/IsisApplicationDocStyle.css"></link>
        <link rel="stylesheet" media="print" href="../../assets/styles/print.css"/>
        
        <noscript> <!-- Use Print stylesheet, unhide all sections if no script -->
          <link rel="stylesheet" href="../../assets/styles/print.css"/>
        </noscript> <!-- Note: currently hides header/menu -->
        
      </head>

      <body>

        <script src="../../assets/scripts/uswds.min.js"></script>

        <xsl:call-template  name="writeHeader"/>

        <div id="page">

          <div class="isisMenu">
            <xsl:call-template  name="writeMenu"/>
          </div>

          <main class="isisContent">
            
            <xsl:if test="files/file/subtitle and normalize-space(files/file) = normalize-space($filenameParam)">
              <h1 class="subtitle"><xsl:value-of select="files/file/subtitle"/></h1>
            </xsl:if>
            <hr/>

            <h1><xsl:value-of select="bibliography/title"/></h1>
            <h2 class="subtitle">
              <xsl:value-of select="bibliography/brief"/>
            </h2>

            <!-- links to other chapters/sections -->
            <xsl:if test="count(files/file) > 1">
              <p class="TOCanchors">
                <xsl:if test="count(files/file[type = 'HTML']) > 1">
                  <xsl:for-each select="files/file[type = 'HTML']">
                    <xsl:choose>
                      <xsl:when test="normalize-space(source/filename) != normalize-space($filenameParam)">
                        <xsl:choose>
                          <xsl:when test="subtitle">
                            <a href="{normalize-space(source/filename)}"><xsl:value-of select="normalize-space(subtitle)"/></a>
                          </xsl:when>
                          <xsl:otherwise>
                            <a href="{normalize-space(source/filename)}"><xsl:value-of select="position()"/></a>
                          </xsl:otherwise>
                        </xsl:choose>
                      </xsl:when>

                      <xsl:otherwise>
                        <span style="font-style:italic; font-weight:bold;">
                        <xsl:choose>
                          <xsl:when test="subtitle">
                            <xsl:value-of select="normalize-space(subtitle)"/>
                          </xsl:when>
                          <xsl:otherwise>
                            <xsl:value-of select="position()"/>
                          </xsl:otherwise>
                        </xsl:choose>
                        </span>
                      </xsl:otherwise>
                    </xsl:choose>
                    <xsl:if test="position() != last()"> | </xsl:if>
                  </xsl:for-each>
                </xsl:if>

                <xsl:for-each select="files/file[type != 'HTML']">
                  <br/>
                  <xsl:choose>
                    <xsl:when test="normalize-space(source/filename) != normalize-space($filenameParam)">
                      <xsl:choose>
                        <xsl:when test="subtitle">
                          <a href="{normalize-space(source/filename)}"><xsl:value-of select="normalize-space(subtitle)"/>
                          (<xsl:value-of select="type"/><xsl:if test="size">, <xsl:value-of select="size"/></xsl:if>)</a>
                        </xsl:when>
                        <xsl:otherwise>
                          <a href="{normalize-space(source/filename)}"><xsl:value-of select="type"/><xsl:if test="size"> (<xsl:value-of select="size"/>)</xsl:if></a>
                        </xsl:otherwise>
                      </xsl:choose>
                    </xsl:when>

                    <xsl:otherwise>
                      <span style="font-style:italic; font-weight:bold;">
                      <xsl:choose>
                        <xsl:when test="subtitle">
                          <xsl:value-of select="normalize-space(subtitle)"/>
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:value-of select="position()"/>
                        </xsl:otherwise>
                      </xsl:choose>
                      </span>
                    </xsl:otherwise>
                  </xsl:choose>
                  <xsl:if test="position() != last()"> | </xsl:if>
                </xsl:for-each>
              </p>
            </xsl:if>

            <hr/>
            <!-- END HEADING -->


            <!-- INLINE BODY CONTENT -->
            <xsl:for-each select="files/file[normalize-space(source/filename) = normalize-space($filenameParam)]">
                  <xsl:if test="body">
                    <xsl:choose>
                      <xsl:when test="body/src">
                        <!-- Output body content from source file -->
                        <!--xsl:copy-of select="document(body/src)"/-->
                        <xsl:apply-templates select="document(body/src)/* | document(body/src)/text()" mode="copyContents"/>
                      </xsl:when>
                      <xsl:otherwise>
                        <!--output body content inlined in this file -->
                          <xsl:apply-templates select="body/*" mode="copyContents"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:if>
            </xsl:for-each>
            <!-- END INLINE BODY CONTENT -->



            <!-- History  -->
            <xsl:if test="history">
            <a name="History"></a>
            <hr/>
              <h2>
                  Document History
              </h2>

              <table>
                <xsl:for-each select="history/change[(@hidden != 'yes' and @hidden != 'true') or not(@hidden)]">
                  <tr>
                    <td class="tableCellHistory_name" nowrap="nowrap">
                      <xsl:value-of select="@name"/>
                    </td>

                    <td class="tableCellHistory_date" nowrap="nowrap">
                      <xsl:value-of select="@date"/>
                    </td>

                    <td class="tableCellHistory_description">
                      <xsl:value-of select="."/>
                    </td>
                  </tr>
                </xsl:for-each>
              </table>
            </xsl:if>

          </main>

        </div>

        <xsl:call-template  name="writeFooter"/>

      </body>
    </html>

  </xsl:template>

  <xsl:template match="definitions" mode="copyContents">
    <h3>Table of Contents</h3>
    <ul>
      <xsl:for-each select="definition">
        <xsl:sort order="ascending" select="@name" />
        <li><a href="#{translate(normalize-space(@name), ' ', '')}"><xsl:value-of select="@name"/></a></li>
      </xsl:for-each>
    </ul>

    <hr />

    <xsl:for-each select="definition">
      <xsl:sort order="ascending" select="@name" />
      <h2><a name="{translate(normalize-space(@name), ' ', '')}"><xsl:value-of select="@name"/></a></h2>
      <p>
        <xsl:apply-templates mode="copyContents"/>
      </p>
    </xsl:for-each>
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
