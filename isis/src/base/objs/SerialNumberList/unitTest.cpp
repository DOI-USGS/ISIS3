#include <iostream>
#include "SerialNumberList.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    Isis::SerialNumberList snl(false);

    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$mgs/testData/m0402852.cub");
    snl.Add("$lo/testData/3133_h1.cub");

    cout << "size   = " << snl.Size() << endl;
    cout << "hasXYZ = " << snl.HasSerialNumber("XYZ") << endl;

    for (int i=0; i<snl.Size(); i++) {
      cout << snl.Filename(i) << " = " << snl.SerialNumber(i)<< endl;
    }

    cout << endl << "SN->File: " << snl.Filename("MGS/561812335:32/MOC-WA/RED") << endl
         << "File->SN:" << snl.SerialNumber("$mgs/testData/ab102401.cub") << endl;

    for (int i=0; i<snl.Size(); i++) {
      cout << snl.SerialNumber(i) << " = " << snl.SerialNumberIndex(snl.SerialNumber(i)) << endl;
    }

    cout << endl << "SN->File (0): " << snl.Filename(0) << endl;
    cout << endl << "SN->File (1): " << snl.Filename(1) << endl;
    cout << endl << "SN->File (2): " << snl.Filename(1) << endl;

    cout<< endl << "Index->observationNumber (2):  " << snl.ObservationNumber(2) << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl << endl;;

  // Test to make sure all targets being the same works
  try {
    Isis::SerialNumberList snl;
    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$base/testData/blobTruth.cub");
    snl.Add("$lo/testData/3133_h1.cub");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

}
