#include <iostream>

#include "IException.h"
#include "Preference.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"
#include "TrackingTable.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  Preference::Preferences(true);

  try{
    cout << "Unit test for TrackingTable\n" << endl;

    cout << endl;


    cout << "Testing default constructor ..." << endl;

    TrackingTable trackingTable1;

    trackingTable1.fileNameToIndex("fileName1.cub", "1");

    Table tableOut1 = trackingTable1.toTable();

    cout << "Record added: " << QString(tableOut1[0][0]) << ", " << QString(tableOut1[0][1]) << endl;

    cout << endl;


    cout << "Testing constructor with a Table object ..." << endl;

    cout << "Constructing Table ..." << endl;

    TableField fileNameField("FileName", TableField::Text, 50);
    TableField serialNumberField("SerialNumber", TableField::Text, 50);

    TableRecord record;
    record += fileNameField;
    record += serialNumberField;
    Table tableIn("TestingTable", record);

    record[0] = "fileName1.cub";
    record[1] = "1234567890";
    tableIn += record;

    record[0] = "fileName2.cub";
    record[1] = "123";
    tableIn += record;

    record[0] = "fileName3.dat";
    record[1] = "456789";
    tableIn += record;

    cout << "First record : " << QString(tableIn[0][0]) << ", " << QString(tableIn[0][1]) << endl;
    cout << "Second record: " << QString(tableIn[1][0]) << ", " << QString(tableIn[1][1]) << endl;
    cout << "Third record : " << QString(tableIn[2][0]) << ", " << QString(tableIn[2][1]) << endl;

    TrackingTable trackingTable2(tableIn);

    cout << "TrackingTable object created" << endl;

    cout << endl;


    cout << "Testing the indexToFileName method ..." << endl;

    for (int i = 3; i < 7; i++) {
      try {
        cout << "FileName at index " << i << ": " << trackingTable2.indexToFileName(i).name() << endl;
      }
      catch (IException e) {
        cout << "FileName at index " << i << " does not exist and an exception is thrown." << endl;
        e.print();
      }
    }

    cout << endl;


    cout << "Testing the fileNameToIndex method ..." << endl;

    cout << "Index of FileName fileName1.cub: "
           << trackingTable2.fileNameToIndex("fileName1.cub", "1234567890") << endl;
    cout << "Index of FileName fileName2.cub: "
           << trackingTable2.fileNameToIndex("fileName2.cub", "123") << endl;
    cout << "Index of FileName fileName3.cub: "
           << trackingTable2.fileNameToIndex("fileName3.dat", "456789") << endl;
    cout << "Index of the non-existent FileName fileName4.cub (demonstrating its addition): "
           << trackingTable2.fileNameToIndex("fileName4.cub", "12345678901234567890") << endl;

    cout << endl;


    cout << "Testing the toTable method ..." << endl;

    Table tableOut2 = trackingTable2.toTable();

    cout << "First record : " << QString(tableOut2[0][0]) << ", "
                              << QString(tableOut2[0][1]) << ", "
                              << int(tableOut2[0][2]) << endl;
    cout << "Second record: " << QString(tableOut2[1][0]) << ", "
                              << QString(tableOut2[1][1]) << ", "
                              << int(tableOut2[1][2]) << endl;
    cout << "Third record : " << QString(tableOut2[2][0]) << ", "
                              << QString(tableOut2[2][1]) << ", "
                              << int(tableOut2[2][2]) << endl;
    cout << "Fourth record: " << QString(tableOut2[3][0]) << ", "
                              << QString(tableOut2[3][1]) << ", "
                              << int(tableOut2[3][2]) << endl;

    cout << endl;


    cout << "Creating a new TrackingTable object with the table returned from toTable method ..." << endl;

    TrackingTable trackingTable3(tableOut2);

    cout << "New TrackingTable object created" << endl;

    cout << endl;


    cout << "Testing that the Table returned from toTable on new TrackingTable matches ..." << endl;

    Table tableOut3 = trackingTable3.toTable();

    cout << "First record : " << QString(tableOut3[0][0]) << ", "
                              << QString(tableOut3[0][1]) << ", "
                              << int(tableOut3[0][2]) << endl;
    cout << "Second record: " << QString(tableOut3[1][0]) << ", "
                              << QString(tableOut3[1][1]) << ", "
                              << int(tableOut3[1][2]) << endl;
    cout << "Third record : " << QString(tableOut3[2][0]) << ", "
                              << QString(tableOut3[2][1]) << ", "
                              << int(tableOut3[2][2]) << endl;
    cout << "Fourth record: " << QString(tableOut3[3][0]) << ", "
                              << QString(tableOut3[3][1]) << ", "
                              << int(tableOut3[3][2]) << endl;

  }
  catch (IException &e) {
    cout << "Unit test failed." << endl;
    e.print();
  }


}
