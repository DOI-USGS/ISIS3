#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "IException.h"
#include "ImportPdsTable.h"
#include "IString.h"
#include "Preference.h"
#include "Table.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  QString inputFile = "data/VIR_IR_1A_1_332974737_1_HK.LBL";
  if (--argc == 1) { inputFile = argv[1]; }

  cout << "\n\nTesting ImportPdsTable class using file " << inputFile << "\n";

  ImportPdsTable myTable(inputFile);
  cout << "\n\nList of Columns found - Total: " << myTable.columns() << "\n";
  QStringList kfiles = myTable.getColumnNames();
  cout << kfiles.join("\n");

  cout << "\n\nNow without Name Conversion: \n";
  kfiles = myTable.getColumnNames(false);
  cout << kfiles.join("\n") << endl;

  // Update/correct column types
  myTable.setType("ShutterStatus", "CHARACTER");
  myTable.setType("ChannelId", "CHARACTER");
  myTable.setType("CompressionMode", "CHARACTER");
  myTable.setType("SpectralRange", "CHARACTER");
  myTable.setType("CurrentMode", "CHARACTER");
  myTable.setType("SubCurrentMode", "CHARACTER");
  myTable.setType("IrExpo", "DOUBLE");
  myTable.setType("IrTemp", "DOUBLE");
  myTable.setType("CcdExpo", "DOUBLE");
  myTable.setType("CcdTemp", "DOUBLE");
  myTable.setType("MirrorSin", "DOUBLE");
  myTable.setType("Mirror", "DOUBLE");
  myTable.setType("SpectTemp", "DOUBLE");
  myTable.setType("TeleTemp", "DOUBLE");
  myTable.setType("COLD TIP TEMP", "DOUBLE");
  myTable.setType("RADIATOR TEMP", "DOUBLE");
  myTable.setType("SU MOTOR CURR", "DOUBLE");
  myTable.setType("LEDGE TEMP", "DOUBLE");
  myTable.setType("FRAME COUNT", "CHARACTER");

  // Create a table from the input
  cout << "Getting ISIS Table...\n";
  Table newTable = myTable.importTable("VIR_DATA");
  for (int i = 0; i < newTable[0].Fields(); i++) {
    cout << newTable[0][i].name() << "\t";
  }
  cout << endl;

  // this test was commented out since the output is not what was expected.
  // See mantis ticket
  // 
  //??? Table newTable2 = myTable.importTable("Apid,PacketsLength", "VIR_DATA_2");
  //??? for (int i = 0; i < newTable2[0].Fields(); i++) {
  //???   cout << newTable[0][i].name() << "\t";
  //??? }
  //??? cout << endl;//??? add mantis ticket -- appears to be failing
  //??? 
  //??? vector<string> cols;
  //??? cols.push_back("Apid");
  //??? cols.push_back("PacketsLength");
  //??? Table newTable3 = myTable.importTable(cols, "VIR_DATA_2");
  //??? for (int i = 0; i < newTable3[0].Fields(); i++) {
  //???   cout << newTable[0][i].name() << "\t";
  //??? }
  //??? cout << endl;

  // The following tests were added when the class was expanded to import binary
  // PDS tables also...

  QString pdsTableDir = "data/";
  QString pdsLabelFile = "";
  QString pdsTableFile = "";

  cout << "\n\n\nImport PDS table from PDS table exported as MSB...\n";
  pdsLabelFile = pdsTableDir + "msb_pds_binary_table.lbl";
  // TEST: Read the table file name from the labels
  ImportPdsTable pdsMsbTable(pdsLabelFile, pdsTableFile, "EXPORTED_ISIS_TABLE");
//  Table isisTableFromMsb = pdsMsbTable.importFromBinaryPds();
  Table isisTableFromMsb = pdsMsbTable.importTable("ReimportedMSB");
  // print the table
  cout << isisTableFromMsb.Name() << endl;
  cout << isisTableFromMsb[0][0].name() << "\t";
  cout << isisTableFromMsb[0][1].name() << "\t";
  cout << isisTableFromMsb[0][2].name() << "\t";
  cout << isisTableFromMsb[0][3].name() << "\n";
  for (int i = 0; i < isisTableFromMsb.Records(); i++) {
    cout << toString((double) isisTableFromMsb[i][0]) << "\t\t\t";
    cout << toString((int)    isisTableFromMsb[i][1]) << "\t\t\t\t";
    cout << QString(          isisTableFromMsb[i][2]) << "\t\t\t";
    cout << toString((float)  isisTableFromMsb[i][3]) << "\n";
  }

  cout << "\n\n\nImport PDS table from PDS table exported as LSB...\n";
  pdsLabelFile = pdsTableDir + "lsb_pds_binary_table.lbl";
  pdsTableFile = pdsTableDir + "lsb_pds_binary_table.dat";
  // TEST: Use default constructor and load() method - default table name is TABLE
  //       The next 2 lines are equivalent to calling 
  //       ImportPdsTable pdsLsbTable(pdsLabelFile, pdsTableFile, "TABLE");
  ImportPdsTable pdsLsbTable;
  pdsLsbTable.load(pdsLabelFile, pdsTableFile);
//  Table isisTableFromLsb = pdsLsbTable.importFromBinaryPds();
  Table isisTableFromLsb = pdsLsbTable.importTable("ReimportedLSB");
  // print the table
  cout << isisTableFromLsb.Name() << endl;
  cout << isisTableFromLsb[0][0].name() << "\t";
  cout << isisTableFromLsb[0][1].name() << "\t";
  cout << isisTableFromLsb[0][2].name() << "\t";
  cout << isisTableFromLsb[0][3].name() << "\n";
  for (int i = 0; i < isisTableFromLsb.Records(); i++) {
    cout << toString((double) isisTableFromLsb[i][0]) << "\t\t\t";
    cout << toString((int)    isisTableFromLsb[i][1]) << "\t\t\t\t";
    cout << QString(          isisTableFromLsb[i][2]) << "\t\t\t";
    cout << toString((float)  isisTableFromLsb[i][3]) << "\n";
  }
  cout << endl;

  // the following currently does not work for binary tables
  // implement this test if the code is fixed...
  // 
  //??? isisTableFromLsb = pdsLsbTable.importTable("Double Value,Real Value", "LsbTable");
  //??? cout << isisTableFromLsb[0][0].name() << "\t";
  //??? cout << isisTableFromLsb[0][1].name() << endl;
  //??? cout << IString((double) isisTableFromLsb[0][0]) << "\t\t";
  //??? cout << IString((float) isisTableFromLsb[0][1]) << endl;
  //??? cout << IString((double) isisTableFromLsb[1][0]) << "\t\t";
  //??? cout << IString((float) isisTableFromLsb[1][1]) << endl;
  //??? cout << endl;

  //  test error throws...
  // 1) load(pdsLabFile, pdsTabFile) -  Unable to import PDS table.  Neither of the following possible table files were found.
  try {
    cout << "Throw error for invalid table file name: " << endl;
    ImportPdsTable error(pdsLabelFile, "INVALID_TABLE_FILE_NAME.DAT", "TABLE");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // 2) getColumnName(index) - Unable to import the binary PDS table into Isis. The requested column index exceeds the number of columns.
  try {
    cout << "Throw error for attempt to access invalid column index: " << endl;
    pdsLsbTable.getColumnName(5);
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // 3) importTable(vector<string> colNames, string tableName) - Unable to import the PDS table into Isis. The requested column name does not exist in the table.
  try {
    cout << "Throw error for attempt to export non-existant columns: " << endl;
    QStringList columnNames;
    columnNames.append("Invalid Column Name");
    myTable.importTable(columnNames, "VIR_DATA");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }
  
  // 4) loadLabel(labFile, tabFile) - The PDS file does not have the required TABLE object. The PDS label file is probably invalid.
  try {
    cout << "Throw error for missing table location in label file:" << endl;
    ImportPdsTable error(pdsLabelFile, pdsTableFile, "MISSING_TABLE");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // 5) loadLabel(labFile, tabFile) - Unable to import the PDS table from the PDS file. The PDS INTERCHANGE_FORMAT is not supported. Valid values are ASCII or BINARY.
  try {
    cout << "Throw error for invalid table format type in label file:" << endl;
    ImportPdsTable error(pdsTableDir + "invalidFormatType.lbl");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // 6) loadLabel(labFile, tabFile) - Number of columns in the COLUMNS label keyword does not match number of COLUMN objects found
  try {
    cout << "Print message when COLUMNS value not matching number of COLUMN objects:" << endl;
    ImportPdsTable error(pdsTableDir + "invalidColumnsValue.lbl");
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;

  // This will cause the following errors to be thrown
  // 7) extract() - Conversion failure of column
  // 8) fillTable() - Failed to convert data in row
  try {
    cout << "Throw error for invalid Text PDS table to be imported: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidTextTable.lbl", "data/invalidTextTable.tab", "TABLE");
    error.importTable("InvalidTable");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // Not sure how to cause this error
  // 9) importFromBinaryPds() - Unable to open file containing PDS table.
  // try {
  //   cout << "Throw error :" << endl;
  //   ImportPdsTable error(pdsLabelFile, "nonExistantFile.dat");
  //   error.importFromBinaryPds();
  // }
  // catch (IException &e) {
  //   e.print();
  //   cout << endl;
  // }

  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 11) makeFieldFromBinaryTable()- Only 4 byte integer values are
  //                     supported in Isis. PDS column has an integer DATA_TYPE
  //                     with N bytes. For MSB_INTEGER
  try {
    cout << "Throw error for invalid MSB_INTEGER bytes in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidMSBIntegerBytes.lbl", "", "EXPORTED_ISIS_TABLE");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 12) makeFieldFromBinaryTable()- Only 4 byte integer values are
  //                     supported in Isis. PDS column has an integer DATA_TYPE
  //                     with N bytes. For LSB_INTEGER
  try {
    cout << "Throw error for invalid INTEGER bytes in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidLSBIntegerBytes.lbl");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 13) makeFieldFromBinaryTable()- Only 4 or 8 byte real values are
  //                     supported in Isis. PDS column has an integer DATA_TYPE
  //                     with N bytes. For IEEE_REAL
  try {
    cout << "Throw error for invalid REAL bytes in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidMSBRealBytes.lbl", "", "EXPORTED_ISIS_TABLE");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 14) makeFieldFromBinaryTable()- Only 4 or 8 byte real values are
  //                     supported in Isis. PDS column has an integer DATA_TYPE
  //                     with N bytes. For PC_REAL
  try {
    cout << "Throw error for invalid REAL bytes in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidLSBRealBytes.lbl");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 15) makeFieldFromBinaryTable()- PDS Column has an unsupported DATA_TYPE.
  try {
    cout << "Throw error for invalid column DATA_TYPE in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidDataType.lbl");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }
 
  // This will cause the following errors to be thrown
  // 10) importTable() - Unable to import the binary PDS table from the PDS file
  //                     into Isis.
  // 16) setByteOrder()- The column DATA_TYPE values indicate differing byte 
  //                     orders.
  try {
    cout << "Throw error for inconsistent byte order in label file: " << endl;
    ImportPdsTable error(pdsTableDir + "invalidByteOrder.lbl");
    error.importTable("ReimportedToIsis");
  }
  catch (IException &e) {
    e.print();
    cout << endl;
  }

  //  Untested methods:
  cout << "cols = " << pdsLsbTable.columns()                 << endl;
  cout << "rows = " << pdsLsbTable.rows()                    << endl;
  cout << "has double = " << pdsLsbTable.hasColumn("Double Value") << endl;
  cout << "col 1 name = " << pdsLsbTable.getColumnName(1)          << endl;
  QStringList names = pdsLsbTable.getColumnNames();
  for (int i = 0; i < names.size(); i++) { 
    cout << names[i] << endl;
  }
  cout << "type Double Value column = " << pdsLsbTable.getType("Double Value") << endl;
  pdsLsbTable.setType("Double Value", "MSB_INTEGER"); 
  cout << "set Double Value column to type MSB_INTEGER " << endl;
  cout << "type Double Value column = " << pdsLsbTable.getType("Double Value") << endl;

  return (0);
}
