#include <iostream>
#include "TableRecord.h"
#include "iException.h"
#include "Buffer.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Testing Isis::TableRecord" << endl;
  Isis::TableField f1("One",Isis::TableField::Integer);
  Isis::TableField f2("Two",Isis::TableField::Double);
  Isis::TableField f3("Three",Isis::TableField::Text,50);
  Isis::TableField f4("Four",Isis::TableField::Real);

  Isis::TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;

  
  cout << "Fields      = " << rec.Fields() << endl;
  cout << "Record size = " << rec.RecordSize() << endl;

  cout << "-----" << endl;
  cout <<"testing pack" << endl;
  
  char buf[66];
  cout << "Packing..." <<endl; 
  rec.Pack(buf);
  cout << "Unpacking..." <<endl; 
  rec.Unpack(buf);
  for (int i=0;i<=3;i++){
    Isis::TableField &f = rec[i];
    Isis::PvlGroup g = f.PvlGroup();
    cout << g << endl;
  }

  cout << "-----" << endl;
  Isis::TableField &f = rec[2];
  Isis::PvlGroup g = f.PvlGroup();
  cout << g << endl;

  cout << "-----" << endl;
  f = rec["TWO"];
  g = f.PvlGroup();
  cout << g << endl;

  cout << "-----" << endl;
  try {
    f = rec["Five"];
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}

