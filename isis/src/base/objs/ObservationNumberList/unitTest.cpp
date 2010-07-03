#include <iostream>
#include "ObservationNumberList.h"
#include "SerialNumberList.h"
#include "iException.h"

using namespace std;
int main (int argc, char *argv[]) {

  try {
    Isis::SerialNumberList snl(false);

    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$mgs/testData/m0402852.cub");
    snl.Add("$lo/testData/3133_h1.cub");
    snl.Add("$odyssey/testData/I00824006RDR.lev2.cub");

    Isis::ObservationNumberList onl( &snl );

    cout << "size   = " << onl.Size() << endl;
    cout << "hasXYZ = " << onl.HasObservationNumber("XYZ") << endl;

    for (int i=0; i<onl.Size(); i++) {
      cout << onl.Filename(i) << " = " << onl.ObservationNumber(i)<< endl;
    }

    cout << endl;
    vector<std::string> filenames = onl.PossibleFilenames( onl.ObservationNumber(2) );
    for( unsigned i=0; i<filenames.size(); i++ ) {
      cout << "Possible filename for [" << onl.ObservationNumber(2)
           << "]: " << filenames[i] << endl;
    }
    vector<std::string> serials = onl.PossibleSerialNumbers( onl.ObservationNumber(2) );
    for( unsigned i=0; i<serials.size(); i++ ) {
      cout << "Possible serial number for [" << onl.ObservationNumber(2)
           << "]: " << serials[i] << endl;
    }

    cout << "File->ON:" << onl.ObservationNumber("$mgs/testData/ab102401.cub") << endl;

    cout << endl << "SN->File (0): " << snl.Filename(0) << endl;
    cout << "SN->File (1): " << snl.Filename(1) << endl;
    cout << "SN->File (2): " << snl.Filename(2) << endl << endl;

    if( onl.HasObservationNumber("NotAnObservation") )
      cout << "This line shouldn't be showing!" << endl;
    else
      cout << "[NotAnObservation] is not an existing ObservationNumber" << endl;

  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl << endl;;

}
