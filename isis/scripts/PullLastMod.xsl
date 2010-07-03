<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!--

This stylesheet will be used to yank the latest history change entry from an Application XML file

Author
Deborah Lee Soltesz
12/6/2002

-->



<xsl:output indent="no" omit-xml-declaration="yes">
  <xsl:template match="/">
     <xsl:apply-templates select="//history" />
  </xsl:template>
</xsl:output>



  <xsl:template name="class" match="//history">
      <xsl:for-each select="change">
        <xsl:sort order="ascending" select="@date"/>
        <xsl:if test="position()=last()">(<xsl:value-of select="@date"/>)(<xsl:value-of select="@name"/>)<xsl:value-of select="normalize-space(.)"/></xsl:if>
      </xsl:for-each>

  </xsl:template>


</xsl:stylesheet>

