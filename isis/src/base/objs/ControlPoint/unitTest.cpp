#include "ControlPoint.h"
#include "Preference.h"
#include "iException.h"

#include <string>
#include <iostream>

using namespace std;
void outit(Isis::ControlPoint &p);

int main() {
  Isis::Preference::Preferences(true);
  cout << "ControlPoint unitTest" << endl;

  Isis::ControlPoint c("C151");
  Isis::ControlMeasure d;
  d.SetCoordinate(1.0, 2.0);
  d.SetCubeSerialNumber("Test1");
  d.SetDiameter(15.0);
  d.SetError(-1.0, 1.0);
  c.Add(d);

  c.SetHeld(true);
  c.SetIgnore(true);
  c.SetUniversalGround(10.0, 15.0, 20.0);
  c.SetType(Isis::ControlPoint::Ground);
  cout << "test PointTypeToString(): " << c.PointTypeToString() << "\n";
  outit(c);

  d.SetCubeSerialNumber("Test2");
  d.SetCoordinate(100.0, 200.0);
  d.SetType(Isis::ControlMeasure::Manual);
  d.SetError(-2.0, 2.0);
  d.SetReference(true);
  c.Add(d);
  outit(c);
  cout << "ReferenceIndex = " << c.ReferenceIndex() << endl;

  c.Delete(0);
  outit(c);
  cout << "ReferenceIndex = " << c.ReferenceIndex() << endl;

  cout << endl << "Test adding control measures with identical serial numbers ..." << endl;
  try {
    c.Add(d);
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
}

void outit(Isis::ControlPoint &p) {
  Isis::Pvl pvl;
  pvl.AddObject(p.CreatePvlObject());
  cout << pvl << endl;
}
