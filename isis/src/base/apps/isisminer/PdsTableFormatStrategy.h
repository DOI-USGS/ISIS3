#ifndef PdsTableFormatStrategy_h
#define PdsTableFormatStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: PdsTableFormatStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

// parent class
#include "Strategy.h"

// Qt library
#include <QString>

// SharedResource and ResourceList typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief PdsTableFormatStrategy - provides inline calculations
   *  
   * This strategy is used to create and write a PDS table format 
   * descriptor file for a PDS table. When the PDS table format 
   * file is created the columns are created according to the byte
   * size of the resource, then that data is stored in the format 
   * file. The resources in the column are delimited by the 
   * delimiter. 
   *  
   * Object = Strategy 
   *     Name = PdsTableFormat
   *     Type = PdsTableFormat
   *     PdsFormatFile = "POINTCLOUDTAB.FTM"
   *     PdsFormatFileArgs = "outputdir"
   *     Delimiter = ","
   *     Column = ("POINT_ID", "STATUS")
   *     DataType = ("CHARACTER", "CHARACTER")
   *     Unit = ("NONE", "NONE")
   *     Description = ("Unique point identifier.", "Status of point")
   *     POINT_ID_BYTES = 32
   *     STATUS_BYTES = 12
   *   EndObject
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-06 Makayla Shepherd - Added documentation.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-05-12 Makayla Shepherd - Added option to specify a file to
   *                          alter PDS keyword formatting.
   *   @history 2015-07-13 Kris Becker - Renamed column "Description" keyword to
   *                          "DataDescription" due to conflicts with Strategy
   *                          Description keyword.

   */
  class PdsTableFormatStrategy : public Strategy {
  
    public:
      PdsTableFormatStrategy();
      PdsTableFormatStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~PdsTableFormatStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
    private:
      void makeColumns(SharedResource &definition);
      int  validate(const QString &keynam, SharedResource &definition, 
                    const int &maxcols, const int &mincols = 0,
                    const bool &throwOnError = true);
  
      SharedResource m_parameters; //!<  All parameter resources
      ResourceList   m_columns;    //!<  List of all columns
      QString        m_delimiter;  //!< Field delimiter character
  
  };

} // Namespace Isis

#endif
