/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2010/04/08 15:28:20 $
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

#include "Isis.h"

#include <QString>

#include "Cube.h"
#include "CSVReader.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Read the CSV file and get the header
  QString csvFileName = ui.GetFileName("csv");
  CSVReader reader;
  try {
    reader = CSVReader(csvFileName, true);
  }
  catch(IException &e) {
    QString msg = "Failed to read CSV file [" + csvFileName + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
  int numColumns = reader.columns();
  int numRows = reader.rows();
  if (numColumns < 1 || numRows < 1) {
    QString msg = "CSV file does not have data.\nFile has [" + toString(numRows) +
                  "] rows and [" + toString(numColumns) +"] columns.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  CSVReader::CSVAxis header = reader.getHeader();

  // Construct an empty table with the CSV header as field names
  TableRecord tableRow;
  for (int columnIndex = 0; columnIndex < numColumns; columnIndex++) {
    TableField columnField(QString(header[columnIndex]), TableField::Double);
    tableRow += columnField;
  }
  QString tableName = ui.GetString("tablename");
  Table table(tableName, tableRow);

  // Fill the table
  for (int rowIndex = 0; rowIndex < numRows; rowIndex++) {
    CSVReader::CSVAxis csvRow = reader.getRow(rowIndex);
    for (int columnIndex = 0; columnIndex < numColumns; columnIndex++) {
      tableRow[columnIndex] = toDouble(csvRow[columnIndex]);
    }
    table += tableRow;
  }

  // If a set of label keywords was passed add them to the table
  if (ui.WasEntered("label")) {
    QString labelPvlFilename = ui.GetFileName("label");
    Pvl labelPvl;
    try {
      labelPvl.read(labelPvlFilename);
    }
    catch(IException &e) {
      QString msg = "Failed to read PVL label file [" + labelPvlFilename + "].";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    PvlObject &tableLabel = table.Label();
    for (int keyIndex = 0; keyIndex < labelPvl.keywords(); keyIndex++) {
      tableLabel.addKeyword(labelPvl[keyIndex]);
    }
  }

  // Write the table to the cube
  QString outCubeFileName(ui.GetFileName("to"));
  Cube outCube;
  try {
    outCube.open(outCubeFileName, "rw");
  }
  catch(IException &e) {
    QString msg = "Could not open output cube [" + outCubeFileName + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  try {
    outCube.write(table);
  }
  catch(IException &e) {
    QString msg = "Could not write output table [" + tableName +
                  "] to output cube [" + outCubeFileName + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  outCube.close();

}
