#include "ControlMeasure.h"
#include "Preference.h"

#include <iostream>
using namespace std;
void outit (Isis::ControlMeasure &d);

int main () {
  Isis::Preference::Preferences(true);
  Isis::ControlMeasure d;
  cout << "Test 1" << endl;
  outit(d);

  d.SetCoordinate(1.0,2.0);
  d.SetCubeSerialNumber("Test");
  d.SetDiameter(15.0);
  d.SetIgnore(true);
  d.SetType(Isis::ControlMeasure::Manual);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  d.SetError(-3.0,4.0);
  d.SetGoodnessOfFit(0.5);
  d.SetReference(true);
  cout << "Test 2" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::Estimated);
  d.SetReference(false);
  cout << "Test 3" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::Automatic);
  cout << "Test 4" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::ValidatedManual);
  cout << "Test 5" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::ValidatedAutomatic);
  cout << "Test 6" << endl;
  outit(d);

  d.SetZScores(-1.1, 1.0);
  cout << "Test 7" << endl;
  outit(d);

}

void outit (Isis::ControlMeasure &d) {
  Isis::Pvl pvl;
  pvl.AddGroup(d.CreatePvlGroup());
  cout << pvl << endl;
}
