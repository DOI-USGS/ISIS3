#include <iostream>

#include "ObservationNumberList.h"
#include "SerialNumberList.h"
#include "IException.h"
#include "IString.h"
#include "FileName.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  try {
    SerialNumberList snl(false);

    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$mgs/testData/m0402852.cub");
    snl.Add("$lo/testData/3133_h1.cub");
    snl.Add("$odyssey/testData/I00824006RDR.lev2.cub");

    ObservationNumberList onl(&snl);

    cout << "size   = " << onl.Size() << endl;
    cout << "hasXYZ = " << onl.HasObservationNumber("XYZ") << endl;

    for(int i = 0; i < onl.Size(); i++) {
      cout << FileName(onl.FileName(i)).name() << " = " << onl.ObservationNumber(i) << endl;
    }

    cout << endl;
    vector<std::string> filenames = onl.PossibleFileNames(onl.ObservationNumber(2));
    for(unsigned i = 0; i < filenames.size(); i++) {
      cout << "Possible filename for [" << onl.ObservationNumber(2)
           << "]: " << FileName(filenames[i]).name() << endl;
    }
    vector<std::string> serials = onl.PossibleSerialNumbers(onl.ObservationNumber(2));
    for(unsigned i = 0; i < serials.size(); i++) {
      cout << "Possible serial number for [" << onl.ObservationNumber(2)
           << "]: " << serials[i] << endl;
    }

    cout << "File->ON:" << onl.ObservationNumber("$mgs/testData/ab102401.cub") << endl;

    cout << endl << "SN->File (0): " << FileName(snl.FileName(0)).name() << endl;
    cout << "SN->File (1): " << FileName(snl.FileName(1)).name() << endl;
    cout << "SN->File (2): " << FileName(snl.FileName(2)).name() << endl << endl;

    if(onl.HasObservationNumber("NotAnObservation"))
      cout << "This line shouldn't be showing!" << endl;
    else
      cout << "[NotAnObservation] is not an existing ObservationNumber" << endl;

  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << endl;;

}
