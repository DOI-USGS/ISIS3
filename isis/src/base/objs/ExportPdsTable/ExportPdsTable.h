#ifndef ExportPdsTable_h
#define ExportPdsTable_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <fstream>

#include <QString>

namespace Isis {
  class EndianSwapper;
  class Pvl;
  class PvlObject;
  class Table;
  class TableRecord;
  /**
   * @brief Export a PDS table from an ISIS Table.
   *
   * This class ingests an ISIS Table and converts it to a PDS Table.
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
                            QString pdsByteOrder);
      QString formatPdsTableName();
      static QString formatPdsTableName(QString isisTableName);
    private:
      void Pack(TableRecord record, char *buffer, EndianSwapper *endianSwap);
      PvlObject fillMetaData();
      Table *m_isisTable;      //!< Input ISIS Table object to be exported.
      int m_numRows;           /**< The number of rows in the exported PDS
                                    table. This value is the same as the number
                                    of records the ISIS Table. **/
      int m_outputRecordBytes; /**< The number of bytes per record in the
                                    exported PDS file.**/
      int m_rowBytes;          /**< The number of bytes per row in the exported
                                    PDS table. This value is the same as the
                                    RecordSize (number of bytes per record) of
                                    the ISIS Table.**/
      QString m_pdsByteOrder;  /**< A string indicating the byte order of the
                                    exported PDS file.**/
  };
}
#endif
