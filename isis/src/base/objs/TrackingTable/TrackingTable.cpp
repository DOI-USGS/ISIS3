/**
 * @file
 * $Revision: 1.4 $
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

#include "TrackingTable.h"

#include <QList>
#include <QString>

#include "FileName.h"
#include "Pvl.h"
#include "Table.h"

using namespace std;
namespace Isis {


  TrackingTable::TrackingTable() {
    
  }
  
  
  TrackingTable::TrackingTable(Table table) {
    
    for (int i=0; i < table.Records(); i++) {
      TableRecord record = table[i];
      QString nameField = QString(record["FileName"]).toLatin1().data());
      int found = nameField.lastIndexOf(".cub");
      if (found != -1) {
        // clear the packing characters - get only the file name
        nameField.remove(found + 4);
      }
      FileName fileName = new FileName(nameField);
      QString serialNumber = QString(record["SerialNumber"]);
      m_fileList.append(QPair<FileName, QString>(fileName, serialNumber));
    }
  }
  
  
  TrackingTable::~TrackingTable() {
    
  }
  
  
  Table TrackingTable::toTable() {
    
    int fieldLength = 0;
    
    for (int i=0; i < m_fileList.size(); i++) {
      if (m_fileList[i].first().name().length() > fieldLength) {
        fieldLength = m_fileList[i].first().name().length();
      }
      if (m_fileList[i].second().length() > fieldLength) {
        fieldLength = m_fileList[i].second().length();
      }
    }
    
    Table table(tableName);
    
    for (int i=0; i < m_fileList.size(); i++) {
      TableField fileNameField("FileName", TableField::Text, fieldLength);
      fileNameField = m_fileList[i].first().name();
      TableField serialNumberField("SerialNumber", TableField::Text, fieldLength);
      serialNumberField = fieldLength = m_fileList[i].second();
        TableField indexField("Index", TableField::Integer);
        indexField = i;
      
      TableRecord record;
      record += fileNameField;
      record += serialNumberField;
      record += indexField;
      
      table += record;
    }
    
    return table;
  }
  
  
  FileName TrackingTable::indexToFileName(unsigned int index) {
    if (index > m_fileList.size()) {
      QString msg = "Cannot convert index [" + toString(index)
                  + "] to a filename, index is out of bounds.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    
    return m_fileList[index].first();
  }
  
  
  unsigned int fileNameToIndex(FileName file, QString serialNumber) {
    for (int i = 0; i < m_fileList.size(); i++) {
      if (m_fileList[i].first() == file) {
        return i;
      }
    }
    
    // At this point, the file is not in the internal file list so append it
    // and return its new index.
    m_fileList.append(QPair<FileName, QString>(file, serialNumber));
    return m_fileList.size() - 1;
  }

}
