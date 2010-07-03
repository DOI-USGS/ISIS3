<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet will be used to strip information out of the XML files in the documents directory
and create a new XML file for generating tables of contents and upper level web pages

Author
Deborah Lee Soltesz
12/04/2002

-->

<xsl:param name="dirParam"/>

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template match="/">
    <xsl:if test="documentation[(@hidden != 'yes' and @hidden != 'true') or not(@hidden)]">
     <xsl:apply-templates select="documentation" />
    </xsl:if>
  </xsl:template>
</xsl:output>

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template name="class" match="documentation">
    <document>

        <title><xsl:value-of select="bibliography/title"/></title>

        <brief><xsl:value-of select="bibliography/brief"/></brief>

        <description><xsl:value-of select="bibliography/description"/></description>

        <category><xsl:for-each select="category/categoryItem"><categoryItem><xsl:value-of select="normalize-space(.)" /></categoryItem></xsl:for-each></category>
        <audience><xsl:for-each select="audience/target"><target><xsl:value-of select="normalize-space(.)" /></target></xsl:for-each></audience>


        <files>
          <xsl:for-each select="files/file">
            <file hidden="{@hidden}" primary="{@primary}">
              <type><xsl:value-of select="type"/></type>

              <xsl:if test="subtitle">
                <subtitle><xsl:value-of select="subtitle"/></subtitle>
              </xsl:if>

              <xsl:if test="size">
                <size><xsl:value-of select="size"/></size>
              </xsl:if>

                <xsl:choose>
                  <xsl:when test="source/URL">
                    <source><xsl:value-of select="normalize-space(source/URL)"/></source>
                  </xsl:when>
                  <xsl:when test="source/filename">
                    <source>../documents/<xsl:value-of select="$dirParam"/>/<xsl:value-of select="normalize-space(source/filename)"/></source>
                  </xsl:when>
                </xsl:choose>

            </file>

          </xsl:for-each>
        </files>

    </document>
  </xsl:template>
</xsl:output>

</xsl:stylesheet>

