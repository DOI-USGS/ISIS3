#ifndef TrackingTable_h
#define TrackingTable_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/14 19:17:09 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "FileName.h"
#include "Pvl.h"
#include "Table.h"

#include <QList>
#include <QString>


namespace Isis {
  /**
   * @brief 
   * @author 2018-07-19 Jesse Mapel & Summer Stapleton
   *
   * @internal
   */
   
  const QString tableName = "InputImages";

  class TrackingTable{
    
    public:
            
      TrackingTable();
      
      TrackingTable(Table table);
      
      ~TrackingTable();
      
      Table toTable();
      
      // oldDnToNewDn(); TODO (maybe?)
      
      FileName indexToFileName(unsigned int index);
      
      unsigned int fileNameToIndex(FileName file, QString serialNumber);
      
    private:
      
      QList< QPair< FileName, QString > > m_fileList;   //!< The list to keep track of images
  };
};

#endif