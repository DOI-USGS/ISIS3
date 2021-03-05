/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "IException.h"
#include "Preference.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  try {
    TableField f1("Column1", TableField::Integer);
    TableField f2("Column2", TableField::Double);
    TableField f3("Column3", TableField::Text, 10);
    TableField f4("Column4", TableField::Double);
    TableRecord rec;
    rec += f1;
    rec += f2;
    rec += f3;
    rec += f4;
    cout << "Testing Table(name, record) constructor and Write(filename) method..." << endl << endl;
    Table t("UNITTEST", rec);

    rec[0] = 5;
    rec[1] = 3.14;
    rec[2] = "PI";
    rec[3] = 3.14159;
    t += rec;

    rec[0] = -1;
    rec[1] = 0.5;
    rec[2] = "HI";
    rec[3] = -0.55;
    t += rec;

    // write first table to tTest
    t.Write("tTest");

    // Use constructor that takes existing file name - case insensitive
    cout << "Testing Table(name) constructor and Read(filename) method..." << endl;
    Table t2("UnitTest");
    // Read table from tTest file
    t2.Read("tTest");
    for (int i = 0; i < t2.Records(); i++) {
      for (int j = 0; j < t2.RecordFields(); j++) {
        if (j == 0) {
          cout << (int) t2[i][j] << "\t";
        }
        else if (j == 1 || j == 3) {
          cout << (double) t2[i][j] << "\t";
        }
        else if (j == 2) {
          cout << QString(t2[i][j]) << "\t";
        }
      }
      cout << endl;
    }
    cout << endl;

    cout << "Testing accessor methods..." << endl;
    cout << "Number of Records = " << t2.Records() << endl;
    cout << "Number of Fields  = " << t2.RecordFields() << endl;
    cout << "Record Size = " << t2.RecordSize() << endl;

    rec[0] = 19;
    rec[1] = 2.2;
    rec[2] = "Blob";
    rec[3] = 4.4;
    t2.Update(rec, 0);
    t2.SetAssociation(Table::Lines);
    t2.Write("tTest");

    cout << endl << "Testing Association Checks" << endl;
    cout << "Sample Associated? " << t2.IsSampleAssociated() << endl;
    cout << "Line Associated?   " << t2.IsLineAssociated() << endl;
    cout << "Band Associated?   " << t2.IsBandAssociated() << endl;
    cout << endl;

    // use constructor that takes name and file 
    cout << "Testing Table(name, filename) constructor and Update(record, index) method..." << endl;
    Table t3("UnitTest", "tTest");
    for (int i = 0; i < t3.Records(); i++) {
      for (int j = 0; j < t3.RecordFields(); j++) {
        if (j == 0) {
          cout << (int) t3[i][j] << "\t";
        }
        else if (j == 1 || j == 3) {
          cout << (double) t3[i][j] << "\t";
        }
        else if (j == 2) {
          cout << QString(t3[i][j]) << "\t";
        }
      }
      cout << endl;
    }
    cout << endl;

    cout << "Testing Record Delete method..." << endl;
    cout << "Number of Records Before Delete = " << t3.Records() << endl;
    cout << "Number of Fields  Before Delete = " << t3.RecordFields() << endl;
    t3.Delete(0);
    cout << "Number of Records After Delete = " << t3.Records() << endl;
    cout << "Number of Fields  After Delete = " << t3.RecordFields() << endl << endl;

    // use operator=
    Table t4 = t3;
    cout << "Testing operator= method with empty table..." << endl;
    for (int i = 0; i < t4.Records(); i++) {
      for (int j = 0; j < t4.RecordFields(); j++) {
        if (j == 0) {
          cout << (int) t4[i][j] << "\t";
        }
        else if (j == 1 || j == 3) {
          cout << (double) t4[i][j] << "\t";
        }
        else if (j == 2) {
          cout << QString(t4[i][j]) << "\t";
        }
      }
      cout << endl;
    }
    cout << endl;
    
    Table t5 = t2;
    t5 = t4;
    cout << "Testing operator= method with non empty table..." << endl;
    for (int i = 0; i < t5.Records(); i++) {
      for (int j = 0; j < t5.RecordFields(); j++) {
        if (j == 0) {
          cout << (int) t5[i][j] << "\t";
        }
        else if (j == 1 || j == 3) {
          cout << (double) t5[i][j] << "\t";
        }
        else if (j == 2) {
          cout << QString(t5[i][j]) << "\t";
        }
      }
      cout << endl;
    }
    cout << endl;

    cout << "Testing Clear  method..." << endl;
    t4.Clear();
    cout << "Number of Records = " << t4.Records() << endl;
    cout << "Number of Fields  = " << t4.RecordFields() << endl << endl;
    remove("tTest");

    cout << "InstrumentPointing Table..." << endl;
    QString name1 = "InstrumentPointing";
    Table instPoint(name1, "$ISISTESTDATA/isis/src/base/unitTestData/Table/truth.cub");
    for(int i = 0; i < instPoint.Records(); i++) {
      for(int j = 0; j < instPoint[i].Fields(); j++) {
        if(instPoint[i][j].isText()) {
          cout << (QString)instPoint[i][j] << ",  ";
        }
        else if(instPoint[i][j].isDouble()) {
          cout << (double)instPoint[i][j] << ",  ";
        }
      }
      cout << endl;
    }
    cout << endl;
    cout << "Camera Statistics Table..." << endl;
    QString name2 = "CameraStatistics";
    Table camStats(name2, "$ISISTESTDATA/isis/src/base/unitTestData/Table/truth.cub");
    for(int i = 0; i < camStats.Records(); i++) {
      for(int j = 0; j < camStats[i].Fields(); j++) {
        if(camStats[i][j].isText()) {
          cout << (QString)camStats[i][j] << ",  ";
        }
        else if(camStats[i][j].isDouble()) {
          cout << (double)camStats[i][j] << ",  ";
        }
      }
      cout << endl;
    }

  }
  catch(IException &e) {
    e.print();
  }
}
