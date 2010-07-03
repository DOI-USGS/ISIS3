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

  <xsl:include href="menu.xsl"/>
  
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

        <link rel="stylesheet" href="../../assets/styles/IsisStyleCommon.css"/>
        <link rel="stylesheet" href="../../assets/styles/main.css"/>
        <link rel="stylesheet" href="../../assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="../../assets/styles/print.css"/>
      </head>

      <body>
        <div class="isisMenu">
         <xsl:call-template  name="writeMenu"/>
        </div>

        <div class="isisContent">

         <!-- HEADING -->
         <a href="http://www.usgs.gov"><img src="../../assets/icons/littleVIS.gif" style="width: 80px; height: 22px;" border="0" alt="USGS"/></a>
         <p style="margin-top:10px; margin-bottom:0px;">
           <xsl:choose>
             <xsl:when test="category[categoryItem = 'isis2']">
             Isis 2 Documentation
             </xsl:when>
             <xsl:otherwise>
             Isis 3 Documentation
             </xsl:otherwise>
           </xsl:choose>
         </p>
         <xsl:if test="files/file/subtitle and normalize-space(files/file) = normalize-space($filenameParam)">
           <h1 class="subtitle"><xsl:value-of select="files/file/subtitle"/></h1>
         </xsl:if>
         <hr/>

         <table style="width: 100%;">
           <tr valign="top">
             <td align="left">
               <h1><xsl:value-of select="bibliography/title"/></h1>
               <h1 class="subtitle">
                 <xsl:value-of select="bibliography/brief"/>
               </h1>
             </td>

             <td align="right" class="caption" nowrap="nowrap">


              <script language="javascript" type="text/javascript">
                //<xsl:comment><![CDATA[
                  // create back link if javascript is available
                  if (history.length > 1) {
                    document.write ("<a" + " href='javascript:history.back();'>Back</" + "a> | ") ;
                  }
                //]]></xsl:comment>
              </script>

              <a href="../../index.html">Home</a>
             </td>
           </tr>
         </table>


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


<!-- FOOTER -->
<script type="text/javascript" language="JavaScript" src="../../assets/scripts/footer.js">
          //<![CDATA[<!--
          //-->]]>
</script>
</div>



      </body>
    </html>

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

