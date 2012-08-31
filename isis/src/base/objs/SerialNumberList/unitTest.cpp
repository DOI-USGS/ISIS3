#include <iostream>
#include "SerialNumberList.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    Isis::SerialNumberList snl(false);

    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$mgs/testData/m0402852.cub");
    snl.Add("$lo/testData/3133_h1.cub");
    // Test adding level 2 using filename
    snl.Add("$mgs/testData/ab102402.lev2.cub",true);
    // Test adding by giving a serial number
    snl.Add("m0402852.cub", "$mgs/testData/m0402852.cub");

    cout << "size   = " << snl.Size() << endl;
    cout << "hasXYZ = " << snl.HasSerialNumber(QString("XYZ")) << endl;

    for(int i = 0; i < snl.Size(); i++) {
      cout << FileName(snl.FileName(i)).name() << " = " << snl.SerialNumber(i) << endl;
    }

    cout << endl << "SN->File: " << FileName(snl.FileName("MGS/561812335:32/MOC-WA/RED")).name()
         << endl
         << "File->SN:" << snl.SerialNumber("$mgs/testData/ab102401.cub") << endl;

    for(int i = 0; i < snl.Size(); i++) {
      cout << snl.SerialNumber(i) << " = " << snl.SerialNumberIndex(snl.SerialNumber(i)) << endl;
    }

    for(int i = 0; i < snl.Size(); i++) {
      cout << endl <<"SN->File ("<< i <<"): " << FileName(snl.FileName(i)).name() << endl;
    }

    cout << endl << "Index->observationNumber (2):  " << snl.ObservationNumber(2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << endl;;

  // Test to make sure all targets being the same works
  try {
    Isis::SerialNumberList snl;
    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$base/testData/blobTruth.cub");
    snl.Add("$lo/testData/3133_h1.cub");
  }
  catch(Isis::IException &e) {
    QString error = e.toString().ToQt().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }

}
