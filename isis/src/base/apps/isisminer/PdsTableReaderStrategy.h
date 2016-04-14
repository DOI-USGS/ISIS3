#ifndef PdsTableReaderStrategy_h
#define PdsTableReaderStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: PdsTableReaderStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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

#include "ImportPdsTable.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief PdsTableReaderStrategy - provides inline calculations
   *  
   * This strategy is used to read a PDS table that are compliant with PDS 
   * standards. The typically use is to read index or cumindex files. 
   *  
   * Object=Strategy 
   *   Name = TestPdsReader
   *   Type = PdsTableReader
   *   PdsFormatFile ="%1/myindextable.lbl"
   *   PdsFormatFileArgs = "inputdir"
   *   PdsTableFile = "%1/myindextable.TAB"
   *   PdsTableFileArgs = "outputdir"
   *   Delimiter = ","
   * EndObject 
   *  
   * @author 2012-07-15 Kris Becker 
   *  
   * @internal 
   *   @history 2015-07-15 Kris Becker - Original version
   */
  class PdsTableReaderStrategy : public Strategy, public ImportPdsTable {
  
    public:
      PdsTableReaderStrategy();
      PdsTableReaderStrategy(const PvlObject &definition, 
                              const ResourceList &globals);
      virtual ~PdsTableReaderStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
    private:
      ResourceList   m_resources;  //!< Transient list of Resources
      ResourceList   m_globals;    //!< Trasient list of Globals
      QString        m_identity;   //!< Identity value
      QString        m_delimiter;  //!< The delimiter specified
      QString        m_table;      //!< Name of table
      bool           m_useFormatted; //!< Use converted column name
  
      virtual bool processRow(const int &row, const QString &rowdata);
  };

} // Namespace Isis

#endif
