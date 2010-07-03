<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" xmlns:xs="http://www.w3.org/2001/XMLSchema">

<!--

This stylesheet is used to strip information out of the XML Schema for Isis AML and create a new XML file summarizing the categories

Author
Deborah Lee Soltesz
2006-04-10

-->

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template match="/">
    <!--xs:simpleType name="missionItem_type" final="restriction"-->
    <categories>
     <xsl:apply-templates select="//xs:simpleType[@name='categoryItem_type']"/>
     <xsl:apply-templates select="//xs:simpleType[@name='missionItem_type']"/>
    </categories>
  </xsl:template>
</xsl:output>

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template name="category" match="//xs:simpleType[@name='categoryItem_type']">
    <xsl:for-each select="xs:restriction/xs:enumeration">
    <category><xsl:value-of select="@value"/></category>
    </xsl:for-each>
  </xsl:template>
</xsl:output>

<xsl:output indent="yes" omit-xml-declaration="yes">
  <xsl:template name="mission" match="//xs:simpleType[@name='missionItem_type']">
    <xsl:for-each select="xs:restriction/xs:enumeration">
    <mission><xsl:value-of select="@value"/></mission>
    </xsl:for-each>
  </xsl:template>
</xsl:output>

</xsl:stylesheet>

