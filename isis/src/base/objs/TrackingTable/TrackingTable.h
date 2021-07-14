#ifndef TrackingTable_h
#define TrackingTable_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "FileName.h"
#include "Pvl.h"
#include "Table.h"

#include <QList>
#include <QString>


namespace Isis {

  const QString trackingTableName = "InputImages";

  /**
   * Table to store tracking information for a mosaic.
   *
   * This table will currently be stored in the label of a separate cube. This tracking cube will
   * also contain a single tracking band. The DN values stored in this band will correlate to the
   * indices in this table. Each record within this table will contain the filename of an
   * associated cube, that cube's serial number, and the DN value associated with this cube within
   * the tracking band (which should also be its index in the table).
   *
   * @author 2018-07-19 Jesse Mapel & Summer Stapleton
   *
   * @internal
   *   @history 2018-07-26 Jesse Mapel - Added offset based on minimum unsigned integer value.
   *                           Renamed methods to better convey output/input meaning.
   *   @history 2018-07-30 Kaitlyn Lee - Added pixelToSN() and fileNameToIndex().
   */
  class TrackingTable{

    public:

      TrackingTable();

      TrackingTable(Table table);

      ~TrackingTable();

      Table toTable();

      FileName pixelToFileName(unsigned int pixel);

      unsigned int fileNameToPixel(FileName file, QString serialNumber);

      int fileNameToIndex(FileName file, QString serialNumber);

      QString pixelToSN(unsigned int pixel);

    private:

      QList< QPair< FileName, QString > > m_fileList;   //!< The list to keep track of images
  };
};

#endif
