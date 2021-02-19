<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    exclude-result-prefixes="xmlns fo">

<xsl:param name="dirParam"/>

<xsl:key name="categoryMatch" match="/tableofcontents/document" use="category/categoryItem"/>
<xsl:key name="audienceMatch" match="/tableofcontents/document" use="audience/target"/>

<xsl:template match="/">
  <xsl:apply-templates select="document('../Schemas/Application/application.xsd)/xs:schema')" />
</xsl:template>

<xsl:template name="missions">
  <xsl:for-each select="document('../Schemas/Application/application.xsd')/*[local-name() = 'schema']/*[local-name() = 'simpleType' and @name = 'missionItem_type']/*[local-name() = 'restriction']/*[local-name() = 'enumeration']">
    <mission>
      <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
    </mission>
  </xsl:for-each>
</xsl:template>

<xsl:variable name="root" select="/" />

<xsl:output indent="yes" omit-xml-declaration="no" method="xml">
  <xsl:template match="/">
<documentation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://isis.astrogeology.usgs.gov/Schemas/Documentation/documentation.xsd">

  <category>
    <categoryItem>reference</categoryItem>
  </category>

  <audience>
    <target>intermediate</target>
  </audience>

  <files>
    <file><!--/simpleType[@name = 'missionItem_type']/restriction-->
      <body>
        <h2>Core Programs</h2>
          <ul>
            <xsl:for-each select="isisReleaseNotes/application[normalize-space(category/missionItem) = '' and count(parameters/parameter[@status != 'nochange']) > 0]">
            <xsl:sort select="@name" />
              <li>
                <a>
                  <xsl:attribute name="href">#<xsl:value-of select="@name" /></xsl:attribute>
                  <xsl:value-of select="@name" />
                </a>
              </li>
            </xsl:for-each>
          </ul>

        <h2>Mission-Specific Programs</h2>
        <ul>
          <xsl:for-each select="document('../Schemas/Application/application.xsd')/*[local-name() = 'schema']/*[local-name() = 'simpleType' and @name = 'missionItem_type']/*[local-name() = 'restriction']/*[local-name() = 'enumeration']">
          <xsl:sort select="@value" />
          <xsl:variable name="mission"><xsl:value-of select="@value" /></xsl:variable>
            <li>
              <xsl:choose>
                <xsl:when test="count($root/isisReleaseNotes/application[count(parameters/parameter[@status != 'nochange']) > 0 and normalize-space(category/missionItem) = $mission]) > 0">
                  <a>
                    <xsl:attribute name="href">#<xsl:value-of select="$mission" /></xsl:attribute>
                    <xsl:value-of select="$mission" />
                    <ul>
                      <xsl:for-each select="$root/isisReleaseNotes/application[count(parameters/parameter[@status != 'nochange']) > 0 and normalize-space(category/missionItem) = $mission]">
                        <xsl:sort select="@name" />
                        <li>
                          <a>
                            <xsl:attribute name="href">#<xsl:value-of select="@name" /></xsl:attribute>
                            <xsl:value-of select="@name" />
                          </a>
                        </li>
                      </xsl:for-each>
                    </ul>
                  </a>
                </xsl:when>
                <xsl:otherwise>
                  <b><xsl:value-of select="$mission" /></b>
                </xsl:otherwise>
              </xsl:choose>
            </li>
          </xsl:for-each>
        </ul>

        <h2>Core Programs (detailed)</h2>
          <xsl:for-each select="isisReleaseNotes/application[normalize-space(category/missionItem) = '' and count(parameters/parameter[@status != 'nochange']) > 0]">
          <xsl:sort select="@name" />
          <a>
            <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
          </a>
          <h3><xsl:value-of select="@name" /></h3>
            <font size="2">
              <a>
                <xsl:attribute name="href">../../Application/presentation/Tabbed/<xsl:value-of select="@name" />/<xsl:value-of select="@name" />.html</xsl:attribute>
                View full documentation
              </a>
            </font>
          <ul>
            <xsl:for-each select="parameters/parameter[@status != 'nochange']">
              <xsl:sort select="@status" />
              <xsl:sort select="@name" />
              <li>
                <p>Parameter <xsl:value-of select="normalize-space(@status)"/>: <b><xsl:value-of select="@name" /></b></p>
              </li>
            </xsl:for-each>
          </ul>
        </xsl:for-each>


        <xsl:for-each select="document('../Schemas/Application/application.xsd')/*[local-name() = 'schema']/*[local-name() = 'simpleType' and @name = 'missionItem_type']/*[local-name() = 'restriction']/*[local-name() = 'enumeration']">
          <xsl:sort select="@value" />
          <xsl:variable name="mission"><xsl:value-of select="@value" /></xsl:variable>
          <xsl:if test="count($root/isisReleaseNotes/application[count(parameters/parameter[@status != 'nochange']) > 0 and normalize-space(category/missionItem) = $mission]) > 0">
            <a>
              <xsl:attribute name="name"><xsl:value-of select="$mission" /></xsl:attribute>
            </a>
            <h2><xsl:value-of select="$mission" /> (detailed)</h2>
              <xsl:for-each select="$root/isisReleaseNotes/application[count(parameters/parameter[@status != 'nochange']) > 0 and normalize-space(category/missionItem) = $mission]">
                <xsl:sort select="@name" />
                <a>
                  <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
                </a>
                <h3><xsl:value-of select="@name" /></h3>
                <font size="2">
                  <a>
                    <xsl:attribute name="href">../../Application/presentation/Tabbed/<xsl:value-of select="@name" />/<xsl:value-of select="@name" />.html</xsl:attribute>
                    View full documentation
                  </a>
                </font>
                <ul>
                  <xsl:for-each select="parameters/parameter[@status != 'nochange']">
                    <xsl:sort select="@status" />
                    <xsl:sort select="@name" />
                    <li>
                      <p>Parameter <xsl:value-of select="normalize-space(@status)"/>: <b><xsl:value-of select="@name" /></b></p>
                    </li>
                  </xsl:for-each>
                </ul>
              </xsl:for-each>
          </xsl:if>
        </xsl:for-each>

      </body>
      <type>HTML</type>

      <source>
        <filename>ParameterChanges.html</filename>
      </source>
    </file>
  </files>

  <bibliography>
    <title>Application Parameter Changes</title>
    <brief>Changes since the last ISIS release</brief>
    <description>Listing of the parameters that have changed in ISIS since the last major release.</description>
    <author>Steven Lambright</author>
    <date>2011-10-05</date>
  </bibliography>
</documentation>
  </xsl:template>
</xsl:output>

</xsl:stylesheet>
