#ifndef PdsTableCreatorStrategy_h
#define PdsTableCreatorStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: PdsTableCreatorStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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
   * @brief PdsTableCreatorStrategy - provides inline calculations
   *  
   * This strategy is used to create and write a PDS table 
   * descriptor file from a set of Columns and Resources. The 
   * resources in the column are delimited by the delimiter. 
   *  
   * Object=Strategy 
   *   Name = TestCreateTableWriter
   *   Type = PdsTableCreator
   *   PdsFormatFile ="%1/TestGeneralFormat.txt"
   *   PdsFormatFileArgs = "inputdir"
   *   PdsTableFile = "%1/TestCreate.TAB"
   *   PdsTableFileArgs = "outputdir"
   *   Mode = Create
   *   Delimiter = ","
   *   DefaultValue = "NULL"
   * EndObject 
   *  
   * @author 2012-07-15 Kris Becker 
   *  
   * @internal 
   *   @history 2012-07-25 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-27 Makayla Shepherd - Added documentation.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class PdsTableCreatorStrategy : public Strategy {
  
    public:
      PdsTableCreatorStrategy();
      PdsTableCreatorStrategy(const PvlObject &definition, 
                              const ResourceList &globals);
      virtual ~PdsTableCreatorStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
    private:
      int readColumns(PvlObject &pvl);
      int delimiterSize(ResourceList &columns) const;
  
      SharedResource m_parameters; //!< All parameter resources
      ResourceList   m_columns;    //!< List of all columns
      QString        m_delimiter;  //!< The delimiter specified
      QString        m_mode;       //!< The mode in which the app runs.
      QString        m_null;       //!< Default value specified
  
  };

} // Namespace Isis

#endif
