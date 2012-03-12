#include <iostream>
#include "Table.h"
#include "IException.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    Isis::TableField f1("Column1", Isis::TableField::Integer);
    Isis::TableField f2("Column2", Isis::TableField::Double);
    Isis::TableField f3("Column3", Isis::TableField::Text, 10);
    Isis::TableField f4("Column2", Isis::TableField::Double);
    Isis::TableRecord rec;
    rec += f1;
    rec += f2;
    rec += f3;
    rec += f4;
    Isis::Table t("UNITTEST", rec);

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

    t.Write("tTest");

    Isis::Table t2("UnitTest", "tTest");
    cout << (int) t2[0][0] << endl;
    cout << (double) t2[0][1] << endl;
    cout << (string) t2[0][2] << endl;
    cout << (double) t2[0][3] << endl;
    cout << (int) t2[1][0] << endl;
    cout << (double) t2[1][1] << endl;
    cout << (string) t2[1][2] << endl;
    cout << (double) t2[1][3] << endl;
    cout << endl << "Number of Records = " << t2.Records() << endl;
    cout << "Record Size = " << t2.RecordSize() << endl;

    rec[0] = 19;
    rec[1] = 2.2;
    rec[2] = "Blob";
    rec[3] = 4.4;
    t2.Update(rec, 0);
    t2.SetAssociation(Isis::Table::Lines);
    t2.Write("tTest");

    cout << endl << "Testing Association Checks" << endl;
    cout << "Sample Associated? " << t2.IsSampleAssociated() << endl;
    cout << "Line Associated?   " << t2.IsLineAssociated() << endl;
    cout << "Band Associated?   " << t2.IsBandAssociated() << endl;
    cout << endl;

    Isis::Table t3("UnitTest", "tTest");
    cout << (int) t3[0][0] << endl;
    cout << (double) t3[0][1] << endl;
    cout << (string) t3[0][2] << endl;
    cout << (double) t3[0][3] << endl;
    cout << (int) t3[1][0] << endl;
    cout << (double) t3[1][1] << endl;
    cout << (string) t3[1][2] << endl;
    cout << (double) t3[0][3] << endl << endl;

    cout << "Testing Record Delete method..." << endl;
    cout << "Number of Records = " << t3.Records() << endl;
    cout << "Deleted Record at Index 0" << endl;
    t3.Delete(0);
    cout << "Number of Records = " << t3.Records() << endl << endl;

    cout << "Testing Clear  method..." << endl;
    t3.Clear();
    cout << "Number of Records = " << t3.Records() << endl << endl;
    remove("tTest");

    string name1 = "InstrumentPointing";
    Isis::Table table1(name1, "truth.cub");
    for(int i = 0; i < table1.Records(); i++) {
      for(int j = 0; j < table1[i].Fields(); j++) {
        if(table1[i][j].IsText()) {
          cout << (string)table1[i][j] << ",";
        }
        else if(table1[i][j].IsDouble()) {
          cout << (double)table1[i][j] << ",";
        }
      }
      cout << endl;
    }
    string name2 = "CameraStatistics";
    Isis::Table table2(name2, "truth.cub");
    for(int i = 0; i < table2.Records(); i++) {
      for(int j = 0; j < table2[i].Fields(); j++) {
        if(table2[i][j].IsText()) {
          cout << (string)table2[i][j] << ",";
        }
        else if(table2[i][j].IsDouble()) {
          cout << (double)table2[i][j] << ",";
        }
      }
      cout << endl;
    }

  }
  catch(Isis::IException &e) {
    e.print();
  }
}

