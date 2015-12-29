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

    snl.add("$mgs/testData/ab102401.cub");
    snl.add("$mgs/testData/m0402852.cub");
    snl.add("$lo/testData/3133_h1.cub");
    snl.add("$odyssey/testData/I00824006RDR.lev2.cub");

    ObservationNumberList onl(&snl);

    cout << "size   = " << onl.size() << endl;
    cout << "hasXYZ = " << onl.hasObservationNumber("XYZ") << endl;

    for(int i = 0; i < onl.size(); i++) {
      cout << FileName(onl.fileName(i)).name() << " = " << onl.observationNumber(i) << endl;
    }

    cout << endl;
    vector<QString> filenames = onl.possibleFileNames(onl.observationNumber(2));
    for(unsigned i = 0; i < filenames.size(); i++) {
      cout << "Possible filename for [" << onl.observationNumber(2)
           << "]: " << FileName(filenames[i]).name() << endl;
    }
    vector<QString> serials = onl.possibleSerialNumbers(onl.observationNumber(2));
    for(unsigned i = 0; i < serials.size(); i++) {
      cout << "Possible serial number for [" << onl.observationNumber(2)
           << "]: " << serials[i] << endl;
    }

    cout << "File->ON:" << onl.observationNumber("$mgs/testData/ab102401.cub") << endl;

    cout << endl << "SN->File (0): " << FileName(snl.fileName(0)).name() << endl;
    cout << "SN->File (1): " << FileName(snl.fileName(1)).name() << endl;
    cout << "SN->File (2): " << FileName(snl.fileName(2)).name() << endl << endl;

    if(onl.hasObservationNumber("NotAnObservation"))
      cout << "This line shouldn't be showing!" << endl;
    else
      cout << "[NotAnObservation] is not an existing ObservationNumber" << endl;

  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << endl;

}
