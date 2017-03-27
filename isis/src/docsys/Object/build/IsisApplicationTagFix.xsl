<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!--

This stylesheet will be used to strip information out of the XML files in the data directory and create a new XML file for generating tables of contents for Applications

Author
Deborah Lee Soltesz
4/25/2005

-->

<xsl:output indent="yes" omit-xml-declaration="no">
  <xsl:template match="/tagfile">
    <tagfile>
    <!-- 9/29/2005 dls
         does this need to modified to ignore "class" in the tagfile? other doxy runs
         seem to be upset about an unexpected tag 'class' -->
    <xsl:for-each select="compound[@kind != 'namespace' and name != 'QObject' and name != 'QScrollView']">
      <xsl:apply-templates select="." mode="copy"/>
    </xsl:for-each>
    </tagfile>
  </xsl:template>

  <xsl:template match="@* | node() [not(self::namespace) and not(. = 'class')]" mode="copy">
    <xsl:copy>
      <xsl:apply-templates select="@*" mode="copy"/>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:template>

</xsl:output>
</xsl:stylesheet>

