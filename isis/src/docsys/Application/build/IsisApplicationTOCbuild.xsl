<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!--

This stylesheet will be used to strip information out of the XML files in the data directory and create a new XML file for generating tables of contents for Applications

Author
Deborah Lee Soltesz
4/25/2002

-->

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template match="/">
     <xsl:apply-templates select="application" />
  </xsl:template>
</xsl:output>

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template name="class" match="application">
    <application>

        <name><xsl:value-of select="@name"/></name>

        <brief><xsl:value-of select="brief"/></brief>

        <category>
          <xsl:for-each select="category/categoryItem">
            <categoryItem><xsl:value-of select="normalize-space(.)" /></categoryItem>
          </xsl:for-each>
          <xsl:for-each select="category/missionItem">
            <missionItem><xsl:value-of select="normalize-space(.)" /></missionItem>
          </xsl:for-each>
        </category>

        <xsl:if test="oldName">
          <oldName><xsl:for-each select="oldName/item"><item><xsl:value-of select="normalize-space(.)" /></item></xsl:for-each></oldName>
        </xsl:if>

        <xsl:if test="groups/group/parameter/type = 'filename'">
          <groups>
            <xsl:for-each select="groups/group/parameter">
              <xsl:if test="filemode">
                <parameter>
                  <name><xsl:value-of select="normalize-space(@name)"/></name>
                  <xsl:if test="filemode">
                    <filemode><xsl:value-of select="normalize-space(filemode)"/></filemode>
                  </xsl:if>
                </parameter>
              </xsl:if>
            </xsl:for-each>
          </groups>
        </xsl:if>


    </application>
  </xsl:template>
</xsl:output>

</xsl:stylesheet>

