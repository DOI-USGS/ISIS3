#include <iostream>
#include "Apollo.h"
#include "IException.h"
#include "FileName.h"
#include "iString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {

    cout << "Unit test for Isis::Apollo" << endl;

    Isis::iString fname("AS15-M-1450");

    Isis::Apollo *apollo;

    apollo = new Isis::Apollo(fname);

    cout << "IsMetric: " << apollo->IsMetric() << endl;
    cout << "IsPanoramic: " << apollo->IsPanoramic() << endl;
    cout << "IsHasselblad: " << apollo->IsHasselblad() << endl;
    cout << "IsApollo15: " << apollo->IsApollo15() << endl;
    cout << "IsApollo16: " << apollo->IsApollo16() << endl;
    cout << "IsApollo17: " << apollo->IsApollo17() << endl;
    cout << "Width: " << apollo->Width() << endl;
    cout << "Height: " << apollo->Height() << endl;
    cout << "Bands: " << apollo->Bands() << endl;
    cout << "ReseauDimension: " << apollo->ReseauDimension() << endl;
    cout << "PixelPitch: " << apollo->PixelPitch() << endl;
    cout << "SpacecraftName: " << apollo->SpacecraftName() << endl;
    cout << "InstrumentId: " << apollo->InstrumentId() << endl;
    cout << "NaifFrameCode: " << apollo->NaifFrameCode() << endl;
    cout << "TargetName: " << apollo->TargetName() << endl;
    string time=(apollo->LaunchDate().UTC());
    cout << "Time: " << time << endl;

  }
  catch(Isis::IException &e) {
    e.print();
  }
}
