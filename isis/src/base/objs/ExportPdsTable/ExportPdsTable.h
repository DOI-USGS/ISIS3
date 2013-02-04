#ifndef ExportPdsTable_h
#define ExportPdsTable_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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

#include <fstream>
#include "IString.h"
 
namespace Isis {
  class EndianSwapper;
  class IString;
  class Pvl;
  class PvlObject;
  class Table;
  class TableRecord;
  /**
   * @brief Export a PDS table from an Isis3 Table.
   *  
   * This class ingests an Isis3 Table and converts it to a PDS Table. 
   *
   * @b NOTE: This class exports BINARY format PDS tables. The PDS Standards 
   * Reference document indicates that for files containing multiple tables 
   * with binary data, the records should be FIXED_LENGTH (not STREAM) and 
   * that this value should be the length of the longest record in the file
   * (Appendix A, PDS Standards, last updated February 27, 2009). This 
   * document also indicates that all table rows that are less than the fixed 
   * record length value should be padded (ususally with nulls).
   *
   * @see http://pds.nasa.gov/standards-reference.shtml
   * @see http://pds.nasa.gov/documents/sr/AppendixA.pdf
   *
   * @code
   *   ExportPdsTable pdsTable(isisTable);
   *   pdsTable.exportToPds(buffer, recordBytes, "LSB");
   * @endcode
   * 
   * @ingroup Utility
   *
   * @author 2012-07-21 Jeannie Backer
   *
   * @internal
   *   @history 2012-11-21 Original Version. References #678.
   *
   *   @todo This class is currently only exporting BINARY format PDS tables. 
   *   Implementation may be added later to export ASCII PDS tables.
   *
   */
  class ExportPdsTable {
    public:
      ExportPdsTable(Table isisTable);
      ~ExportPdsTable();
      PvlObject exportTable(char *pdsTableBuffer, int pdsFileRecordBytes,
                            IString pdsByteOrder);
      IString formatPdsTableName(); 
      static IString formatPdsTableName(IString isisTableName); 
    private:
      void Pack(TableRecord record, char *buffer, EndianSwapper *endianSwap);
      PvlObject fillMetaData();
      Table *m_isisTable;      //!< Input Isis3 Table object to be exported.
      int m_numRows;           /**< The number of rows in the exported PDS 
                                    table. This value is the same as the number 
                                    of records the Isis3 Table. **/
      int m_outputRecordBytes; /**< The number of bytes per record in the 
                                    exported PDS file.**/
      int m_rowBytes;          /**< The number of bytes per row in the exported
                                    PDS table. This value is the same as the 
                                    RecordSize (number of bytes per record) of 
                                    the Isis3 Table.**/
      IString m_pdsByteOrder;  /**< A string indicating the byte order of the 
                                    exported PDS file.**/ 
  };
}
#endif
