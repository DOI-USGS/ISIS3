<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format" 
    exclude-result-prefixes="xmlns fo">

<!--

This stylesheet generates the PRINTER FRIENDLY HTML version of the application documentation

Author
Deborah Lee Soltesz
4/2002

-->

  <xsl:output 
    media-type="text/html" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"
    indent="yes"
    encoding="utf-8"
    omit-xml-declaration="yes"/>

<xsl:include href="../../../../build/menu.xsl"/>


  <xsl:template match="/">
     <xsl:apply-templates select="application" />
  </xsl:template>


  <xsl:template name="class" match="application">
    <html>
      <head>
        <title>
            USGS: ISIS <xsl:value-of select="@name"/> Application Documentation
        </title>
        <link rel="stylesheet" href="../../../../assets/styles/IsisStyleCommon.css"></link>
        <link rel="stylesheet" href="../styles/IsisApplicationDocStyle.css"></link>
        <link rel="stylesheet" href="../../../../assets/styles/menu.css"/>
        <link rel="stylesheet" media="print" href="../../../../assets/styles/print.css"/>

        <xsl:variable name="keywordList">
          Isis, image processing,
          <xsl:value-of select="normalize-space(//application/@name)"/>
          <xsl:for-each select="//application/category/item">
            , <xsl:value-of select="normalize-space(.)"/>
          </xsl:for-each>
        </xsl:variable>
        <meta name="keywords" content="{normalize-space($keywordList)}"/>


        <!-- 'author' is the person who originally wrote this program - see history for detailed list of authors-->
        <xsl:for-each select="history/change">
          <xsl:sort order="ascending" select="@date"/>
          <xsl:if test="position() = 1">
            <meta name="author" content="{@name}"/>
          </xsl:if>
        </xsl:for-each>

        <meta name="description" content="{normalize-space(brief)}"/>
        <meta name="publisher" content="USGS - GD - Astrogeology Research Program"/>

        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <meta name="country" content="USA"/>
        <meta name="state" content="AZ"/>
        <meta name="county" content="Coconino"/>
        <meta name="city" content="Flagstaff"/>
        <meta name="zip" content="86001"/>

        <script language="javascript" type="text/javascript" src="../../../../assets/scripts/utility.js"></script>

       </head>

      <body>

        <div class="isisMenu">
         <xsl:call-template  name="writeMenu"/>
        </div>

        <div class="isisContent">

        <a href="http://www.usgs.gov">
        <img src="../../../../assets/icons/littleVIS.gif" width="80" height="22" border="0" alt="USGS"/></a><br/>


        <p style="margin-top:10px; margin-bottom:0px;">
        Isis 3 Application Documentation</p>

        <hr/>

        <table width="100%" cellpadding="0" border="0" cellspacing="0">
          <tr valign="top">
            <td align="left">
              <h1>
                <xsl:value-of select="@name"/>
              </h1>
            </td>
            <td align="right" class="caption">
            <a href="../../Tabbed/{@name}/{@name}.html">Standard View</a> |
            <a href="../../../index.html">TOC</a> |
            <a href="../../../../index.html">Home</a>
            </td>
          </tr>
        </table>

        <p style="margin-top:0px; font-style:italic;">
          <xsl:value-of select="brief"/>
        </p>

        <!-- table of contents -->
        <p class="TOCanchors">

          <a href="#Description">
            Description</a><br/>

          <xsl:if test="category">
            <a href="#Categories">
              Categories</a><br/>
          </xsl:if>

          <xsl:if test="groups">
            <a href="#Groups">
              Groups</a><br/>
          </xsl:if>

          <xsl:if test="examples">
            <a href="#Examples">
              Examples</a><br/>
          </xsl:if>

          <xsl:if test="history">
            <a href="#History">
              History</a><br/>
          </xsl:if>

          <xsl:if test="liens">
            <a href="#ThingsToDo">
              Things To Do</a><br/>
          </xsl:if>

        </p>


        <!-- Description  -->
        <a name="Description"></a>
        <hr/>
        <h2>
          Description
        </h2>

        <div style="font-weight: normal;">
          <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
        </div>

        <!-- categories -->
        <a name="Categories"></a>
        <hr/>
        <h2>
          Categories
        </h2>

        <ul>
          <xsl:for-each select="category/categoryItem">
            <li>
              <a href="../../../index.html#{translate(normalize-space(.), ' ', '_')}">
              <xsl:value-of select="." /></a>
            </li>
          </xsl:for-each>
          <xsl:for-each select="category/missionItem">
            <li>
              <a href="../../../index.html#{translate(normalize-space(.), ' ', '_')}">
              <xsl:value-of select="." /></a>
            </li>
          </xsl:for-each>
        </ul>

       <!-- oldName -->
        <xsl:if test="oldName">
          <hr/>
          <h2>
            Related Applications in Previous Versions of Isis
          </h2>

          This application replaces the following
          <xsl:choose>
            <xsl:when test="count(oldName/item) > 1">
            applications existing in previous versions of Isis, which have been deprecated
            </xsl:when>
            <xsl:otherwise>
            application existing in previous versions of Isis, which has been deprecated
            </xsl:otherwise>
          </xsl:choose>
          from the current version of Isis:

          <ul>
            <xsl:for-each select="oldName/item">
              <li><xsl:value-of select="."/></li>
            </xsl:for-each>
          </ul>
        </xsl:if>



       <!-- SeeAlso -->
         <xsl:choose>
           <xsl:when test="seeAlso">
            <hr/>
              <h2>
                <a name="SeeAlso">
                  Related Objects and Documents</a>
              </h2>

              <!-- seeAlso -->
              <xsl:for-each select="seeAlso">

                <xsl:if test="applications">
                <h3>Applications</h3>
                  <ul>
                    <xsl:for-each select="applications">
                      <xsl:for-each select="item">
                        <li><a href="../{.}/{.}.html">
                        <xsl:value-of select="."/></a></li>
                      </xsl:for-each>
                    </xsl:for-each>
                  </ul>
                </xsl:if>

                <xsl:if test="documents">
                <h3>Documents</h3>
                  <ul>
                    <xsl:for-each select="documents">
                      <xsl:for-each select="document">
                        <xsl:choose>

                          <xsl:when test="source/path">
                            <li><a href="{source/path}{source/filename}">
                            <xsl:value-of select="title"/></a></li>
                          </xsl:when>

                          <xsl:when test="source/filename">
                            <li><a href="../documents/{source/filename}">
                            <xsl:value-of select="title"/></a></li>
                          </xsl:when>

                          <xsl:otherwise>
                            <li>
                              <xsl:value-of select="title"/>

                              <xsl:if test="author">
                              ,  <xsl:value-of select="title"/>
                              </xsl:if>

                              <xsl:if test="date">
                              ,  <xsl:value-of select="date"/>
                              </xsl:if>

                              <xsl:if test="publisher">
                              ;  <xsl:value-of select="publisher"/>
                              </xsl:if>

                              <xsl:if test="pages">
                              ;  <xsl:value-of select="pages"/>
                              </xsl:if>
                            </li>
                          </xsl:otherwise>

                        </xsl:choose>
                      </xsl:for-each>
                    </xsl:for-each>
                  </ul>
                </xsl:if>

              </xsl:for-each>
           </xsl:when>
         </xsl:choose>



        <!-- Groups -->
        <xsl:if test="groups">

          <a name="Groups"></a>
          <hr/>
            <h2>
                Parameter Groups
            </h2>
            <!-- table of groups links -->
            <xsl:for-each select="groups">
              <xsl:for-each select="group">
                <xsl:variable name="groupName" select="@name"/>
                <h3><xsl:value-of select="@name"/></h3>
                <table>
                  <tr>
                    <th class="tableCellLevel1_th">
                    Name
                    </th>
                    <th class="tableCellLevel1_th">
                    Description
                    </th>
                  </tr>
                  <xsl:for-each select="parameter">
                    <tr>
                      <td class="tableCellLevel1_name">
                        <a href="#{translate(normalize-space($groupName), ' ()', '_')}{@name}">
                        <xsl:value-of select="@name"/></a>
                      </td>
                      <td class="tableCellLevel1">
                        <xsl:value-of select="brief"/>
                      </td>
                    </tr>
                  </xsl:for-each>
                </table>
              </xsl:for-each>
           </xsl:for-each>

            <!-- groups information -->
            <xsl:for-each select="groups">
              <xsl:for-each select="group">
                <xsl:variable name="groupName" select="@name"/>

                  <xsl:for-each select="parameter">
                    <a name="{translate(normalize-space($groupName), ' ()', '_')}{@name}"></a>
                    <hr/>
                    <h3>
                        <span style="font-style:italic; text-decoration:underline;">
                        <xsl:value-of select="$groupName"/>
                        </span>:
                        <xsl:value-of select="@name"/>
                    </h3>
                    <div style="margin-left:20px;">

                       <!-- description -->
                       <h4>
                         Description
                       </h4>
                       <p>
                         <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
                       </p>

                       <!-- type -->
                       <table>
                         <tr>
                           <th class="tableCellLevel1_th" align="right">
                             Type
                           </th>
                           <td class="tableCellLevel1_description">
                             <xsl:value-of select="type"/>
                           </td>
                         </tr>

                       <!-- fileMode -->
                         <xsl:if test="fileMode">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               File Mode
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="fileMode"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- pixelType -->
                         <xsl:if test="pixelType">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Pixel Type
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="pixelType"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- default path -->
                         <xsl:if test="defaultPath">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Default Path
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="defaultPath"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- count -->
                         <xsl:if test="count">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Count
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="count"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- default -->
                         <xsl:if test="default">
                           <tr>
                             <th class="tableCellLevel1_th">
                               Default
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="default"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- internal default -->
                         <xsl:if test="internalDefault">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Internal Default
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="internalDefault"/>
                             </td>
                           </tr>
                         </xsl:if>

                       <!-- option list -->
                         <xsl:if test="list">
                           <tr>
                             <th class="tableCellLevel1_th" align="right" valign="top">
                               Option List:
                             </th>
                             <td class="tableCellLevel1_description">
                               <table>
                                 <tr>
                                   <th class="tableCellLevel2" valign="top">
                                   Option</th>
                                   <th class="tableCellLevel2" valign="top">
                                   Brief</th>
                                   <th class="tableCellLevel2" valign="top">
                                   Description</th>
                                 </tr>
                                 <xsl:for-each select="list/option">
                                   <tr>
                                     <td class="tableCellLevel2_name" valign="top">
                                       <xsl:value-of select="@value"/>
                                     </td>
                                     <td class="tableCellLevel2_type" valign="top">
                                       <xsl:value-of select="brief"/>
                                     </td>
                                     <td class="tableCellLevel2_description" valign="top">
                                       <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>

                                       <xsl:if test="exclusions">
                                         <h4>Exclusions</h4>
                                         <ul>
                                         <xsl:for-each select="exclusions/item">
                                           <li><xsl:value-of select="."/></li>
                                         </xsl:for-each>
                                         </ul>
                                       </xsl:if>
                                       <xsl:if test="inclusions">
                                         <h4>Inclusions</h4>
                                         <ul>
                                         <xsl:for-each select="inclusions/item">
                                           <li><xsl:value-of select="."/></li>
                                         </xsl:for-each>
                                         </ul>
                                       </xsl:if>

                                     </td>
                                   </tr>
                                 </xsl:for-each>
                               </table>
                             </td>
                           </tr>
                         </xsl:if>


                         <xsl:if test="minimum">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Minimum
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="minimum"/>
                               <xsl:choose>
                                 <xsl:when test="minimum/@inclusive = 'yes'">
                                   (inclusive)
                                 </xsl:when>
                                 <xsl:when test="minimum/@inclusive = 'true'">
                                   (inclusive)
                                 </xsl:when>
                                 <xsl:otherwise>
                                   (exclusive)
                                 </xsl:otherwise>
                               </xsl:choose>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="maximum">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Maximum
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="maximum"/>
                               <xsl:choose>
                                 <xsl:when test="maximum/@inclusive = 'yes'">
                                   (inclusive)
                                 </xsl:when>
                                 <xsl:when test="maximum/@inclusive = 'true'">
                                   (inclusive)
                                 </xsl:when>
                                 <xsl:otherwise>
                                   (exclusive)
                                 </xsl:otherwise>
                               </xsl:choose>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="greaterThan">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Greater Than
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="greaterThan/item">
                                   <li>
                                     <xsl:value-of select="."/>
                                   </li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="greaterThanOrEqual">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Greater Than or Equal
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="greaterThanOrEqual/item">
                                   <li>
                                     <xsl:value-of select="."/>
                                   </li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="lessThan">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Less Than
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="lessThan/item">
                                   <li><xsl:value-of select="."/></li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="lessThanOrEqual">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Less Than or Equal
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="lessThanOrEqual/item">
                                   <li><xsl:value-of select="."/></li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="notEqual">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Not Equal
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="notEqual/item">
                                   <li><xsl:value-of select="."/></li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="odd">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Odd
                             </th>
                             <td class="tableCellLevel1_description">
                               This value must be an odd number
                             </td>
                           </tr>
                         </xsl:if>


                         <xsl:if test="exclusions">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Exclusions
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="exclusions/item">
                                   <li><xsl:value-of select="."/></li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="inclusions">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Inclusions
                             </th>
                             <td class="tableCellLevel1_description">
                               <ul>
                                 <xsl:for-each select="inclusions/item">
                                   <li><xsl:value-of select="."/></li>
                                 </xsl:for-each>
                               </ul>
                             </td>
                           </tr>
                         </xsl:if>

                         <xsl:if test="filter">
                           <tr>
                             <th class="tableCellLevel1_th" align="right">
                               Filter
                             </th>
                             <td class="tableCellLevel1_description">
                               <xsl:value-of select="filter"/>
                             </td>
                           </tr>
                         </xsl:if>



                       </table>
                     </div>

                  </xsl:for-each>

              </xsl:for-each>
           </xsl:for-each>



         </xsl:if>


        <!-- Examples -->
        <xsl:if test="examples">
          <a name="Examples"></a>
          <hr/>
          <h2>
            Examples
          </h2>

            <xsl:for-each select="examples">
              <xsl:for-each select="example">
                <hr />
                <h3>
                Example <xsl:number/>
                </h3>
                <p style="margin-top:0px;">
                  <xsl:value-of select="brief"/>
                </p>

                <h4>
                Description
                </h4>

                <div style="margin-left:20px;">
                  <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
                </div>

                <!-- Command Line -->

                <xsl:if test="terminalInterface">
                  <h4>
                  Command Line
                  </h4>

                  <div style="margin-left:20px;">
                      <xsl:for-each select="terminalInterface">
                            <tt style="font-weight:bold;">
                              <xsl:value-of select="/application/@name"/>
                              <xsl:text> </xsl:text>
                              <xsl:value-of select="commandLine"/>
                            </tt>
                            <br/>
                            <div style="font-style:italic; font-size:X-SMALL;margin-left:20px; width:400px;">
                              <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
                            </div>
                      </xsl:for-each>
                  </div>

                </xsl:if>


                <!-- GUI Screenshots -->

                <xsl:if test="guiInterfaces">
                  <h4>
                  GUI Screenshot
                  </h4>

                  <div style="margin-left:20px;">
                    <table cellpadding="5" width="600">
                      <xsl:for-each select="guiInterfaces/guiInterface/image">
                      <xsl:apply-templates mode="tabledImages" select="."/>
                      </xsl:for-each>
                    </table>
                  </div>

                </xsl:if>

                <!-- Input Images -->

                <xsl:if test="inputImages">
                  <h4>
                    <xsl:choose>
                 <xsl:when test="count(inputImages/image) > 1">
                        Input Images
                 </xsl:when>
                 <xsl:otherwise>
                   Input Image
                 </xsl:otherwise>
                    </xsl:choose>
                  </h4>

                  <div style="margin-left:20px;">
                    <table cellpadding="5" width="600">
                      <xsl:for-each select="inputImages/image">
                        <xsl:apply-templates mode="tabledImages" select="."/>
                      </xsl:for-each>
                    </table>
                  </div>

                </xsl:if>


                <!-- Data Files -->
                <xsl:if test="dataFiles">
                  <h4>
                    <xsl:choose>
                 <xsl:when test="count(dataFiles/dataFile) > 1">
                        Data Files
                 </xsl:when>
                 <xsl:otherwise>
                   Data File
                 </xsl:otherwise>
                    </xsl:choose>
                  </h4>

                  <div style="margin-left:20px;">
                    <table cellpadding="5" width="600">
                      <xsl:for-each select="dataFiles/dataFile">
                        <tr>
                          <th class="tableCellLevel1_th">
                            <a href="{@path}"><xsl:value-of select="brief"/></a>
                          </th>
                          <td class="tableCellLevel1">
                            <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
                          </td>
                        </tr>
                      </xsl:for-each>
                    </table>
                  </div>

                </xsl:if>


                <!-- Output Images -->

                <xsl:if test="outputImages">
                  <h4>
                    <xsl:choose>
                 <xsl:when test="count(outputImages/image) > 1">
                        Output Images
                 </xsl:when>
                 <xsl:otherwise>
                   Output Image
                 </xsl:otherwise>
                    </xsl:choose>
                  </h4>

                  <div style="margin-left:20px;">
                    <table cellpadding="5" width="600">
                      <xsl:for-each select="outputImages/image">
                        <xsl:apply-templates mode="tabledImages" select="."/>
                      </xsl:for-each>
                    </table>
                  </div>

                </xsl:if>



              </xsl:for-each>
            </xsl:for-each>

         </xsl:if>


       <!-- History  -->
       <xsl:if test="history">
        <a name="History"></a>
        <hr/>
          <h2>
              History
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

       <!-- Liens  -->
       <xsl:if test="Liens">
        <a name="ThingsToDo"></a>
        <hr/>
          <h2>
              Things To Do
          </h2>

         <ul>
           <xsl:for-each select="liens/item">
             <li><xsl:value-of select="item"/></li>
           </xsl:for-each>
         </ul>
       </xsl:if>




<!-- FOOTER -->
<script type="text/javascript" language="JavaScript" src="../../../../assets/scripts/footer.js"></script>
</div>


      </body>
    </html>
  </xsl:template>

  <xsl:template match="image" mode="tabledImages">

                        <tr valign="top">
                          <td class="tableCellLevel1_th" width="{thumbnail/@width}" align="center">

<!-- start IMAGE LINK TAG: pop up window if javascript, otherwise normal link -->
<script type="text/javascript">
    <xsl:comment>
        <![CDATA[
        //<!--
document.write("<a title='Click to view larger image' href='javascript:popUpNewWindow (\"]]><xsl:value-of select="@src"/><![CDATA[\","  + ]]>
<xsl:value-of select="@width"/><![CDATA[+ ", " + ]]><xsl:value-of select="@height"/><![CDATA[+ ")'>") ;

document.write("<img src=\"]]><xsl:value-of select="normalize-space(thumbnail/@src)"/><![CDATA[\""
+ " width=" + ]]><xsl:value-of select="normalize-space(thumbnail/@width)"/><![CDATA[
+ " height=" + ]]><xsl:value-of select="normalize-space(thumbnail/@height)"/><![CDATA[
+ " alt=\"]]><xsl:value-of select="normalize-space(thumbnail/@caption)"/><![CDATA[\" class='blackBorderedImage'><" + "/a><br>") ;
        //-->
        ]]>
    </xsl:comment>
</script>


<noscript>
  <a title='Click to view larger image' href="{normalize-space(@src)}" target="_new">
  <img src="{normalize-space(thumbnail/@src)}" width="{normalize-space(thumbnail/@width)}" height="{normalize-space(thumbnail/@height)}" alt="{normalize-space(thumbnail/@caption)}" class="blackBorderedImage"/></a><br/>
</noscript>
<!-- end IMAGE LINK TAG: pop up window if javascript, otherwise normal link -->

                           <div class="caption">
                             <xsl:value-of select="thumbnail/@caption"/>
                           </div>

                          </td>
                          <td class="tableCellLevel1">
                            <p style="font-weight:bold;">
                              <xsl:value-of select="brief"/>
                            </p>

                            <xsl:if test="parameterName">
                              <p>
                                <span style="font-weight:bold;">
                                Parameter Name:
                                </span>
                                <xsl:value-of select="parameterName"/>
                                <br/>
                                <xsl:value-of select="parameterName/description"/>
                              </p>
                            </xsl:if>

                            <p>
                              <xsl:apply-templates select="description/* | description/text()" mode="copyContents"/>
                            </p>

                          </td>
                        </tr>

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

