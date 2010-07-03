<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!-- 

This stylesheet will be used to generate a Makefile for turning News XML into HTML pages

Author
Deborah Lee Soltesz
04/04/2003

-->

<xsl:output indent="no" omit-xml-declaration="yes">
  <xsl:template match="/">
     <xsl:apply-templates select="news" />
  </xsl:template>
</xsl:output>

<!-- the following template contains very little whitespace to
     avoid adding spaces at the beginnings of the Makefile macro 
     lines:  trying to make this template more "readable" will
     likely break the system -->

<xsl:template match="news">
include $(ISISROOT)/make/isismake.macros
all: <xsl:for-each select="newsItem[info/item = '3' or info/item = 'both']"><xsl:sort select="newsDate" order="descending"/>Isis3-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html </xsl:for-each> <xsl:for-each select="newsItem[info/item = '2' or info/item = 'both']"><xsl:sort select="newsDate" order="descending"/>Isis2-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html </xsl:for-each>
<xsl:text>&#xa;</xsl:text>


<xsl:for-each select="newsItem[info/item = '3' or info/item = 'both']">
<xsl:sort select="newsDate" order="descending"/>
Isis3-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html: ../build/News.xsl isisnews.xml
<xsl:text>	</xsl:text>echo "          Constructing News [Isis3-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>]"
<xsl:text>	</xsl:text>$(XALAN) -p filenameParam "'Isis3-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>'" -o Isis3-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html<xsl:text> </xsl:text>isisnews.xml ../build/News.xsl
<xsl:text>&#xa;</xsl:text>
</xsl:for-each>

<xsl:for-each select="newsItem[info/item = '2' or info/item = 'both']">
<xsl:sort select="newsDate" order="descending"/>
Isis2-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html: ../build/News.xsl isisnews.xml
<xsl:text>	</xsl:text>echo "          Constructing News [Isis2-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>]"
<xsl:text>	</xsl:text>$(XALAN) -p filenameParam "'Isis2-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>'" -o Isis2-<xsl:value-of select="normalize-space(newsDate)"/>_<xsl:value-of select="position()"/>.html<xsl:text> </xsl:text>isisnews.xml ../build/News.xsl
<xsl:text>&#xa;</xsl:text>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>















