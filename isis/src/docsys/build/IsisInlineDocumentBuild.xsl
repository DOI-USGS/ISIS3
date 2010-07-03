<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!-- 

This stylesheet will be used to generate a Makefile for turning documentation inlined in the Documentation XML file
into an HTML page

Author
Deborah Lee Soltesz
12/04/2002

-->

<xsl:param name="dirParam"/>

<xsl:output indent="no" omit-xml-declaration="yes">
  <xsl:template match="/">
     <xsl:apply-templates select="documentation" />
  </xsl:template>
</xsl:output>

<!-- the following template contains very little whitespace to
     avoid adding spaces at the beginnings of the Makefile macro 
     lines:  trying to make this template more "readable" will
     likely break the system -->

<xsl:template name="document" match="documentation">
include $(ISISROOT)/make/isismake.macros
docs:
<xsl:text>	</xsl:text>echo "          Constructing [<xsl:value-of select="$dirParam"/>]"
<xsl:for-each select="files/file"><xsl:if test="body"><xsl:choose>
<xsl:when test="@primary = 'true'"><xsl:text>	</xsl:text>$(XALAN)  -p menuPath "'../../'" -p filenameParam "'<xsl:value-of select="normalize-space(source/filename)"/>'" -o <xsl:value-of select="normalize-space(source/filename)"/><xsl:text> </xsl:text><xsl:value-of select="$dirParam"/>.xml ../../build/IsisPrimaryPageBuild.xsl</xsl:when>
<xsl:otherwise><xsl:text>	</xsl:text>$(XALAN) -p menuPath "'../../'" -p filenameParam "'<xsl:value-of select="normalize-space(source/filename)"/>'" -o <xsl:value-of select="normalize-space(source/filename)"/><xsl:text> </xsl:text><xsl:value-of select="$dirParam"/>.xml ../../build/IsisSubPageBuild.xsl</xsl:otherwise>
</xsl:choose>
<xsl:text>&#xa;</xsl:text>
</xsl:if>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>

