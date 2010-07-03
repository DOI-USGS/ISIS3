#include "ControlNet.h"
#include "SpecialPixel.h"
#include "TextFile.h"
#include "iException.h"
#include "Preference.h"

#include <string>
#include <iostream>
using namespace std;

int main () {
  Isis::Preference::Preferences(true);
  cout << "UnitTest for ControlNet ...." << endl << endl;
  Isis::ControlMeasure cm;
  cm.SetType(Isis::ControlMeasure::Unmeasured);
  cm.SetCubeSerialNumber("Id1");

  Isis::ControlPoint cp1("T0001");
  cp1.SetType(Isis::ControlPoint::Tie);
  cp1.SetIgnore(true);
  cp1.Add(cm);

  Isis::ControlMeasure cm1;
  cm1.SetCoordinate(15.5, 23.2, Isis::ControlMeasure::Manual);
  cm1.SetError(1.0,1.0);
  cm1.SetCubeSerialNumber("Id1");
  cm1.SetDiameter(7900.4);
  cm1.SetChooserName("janeDoe");
  cm1.SetDateTime("2004-12-20T10:12:05");

  Isis::ControlMeasure cm2;
  cm2.SetCoordinate(13.5, 168.3, Isis::ControlMeasure::Automatic);
  cm2.SetError(0.5,0.5);
  cm2.SetCubeSerialNumber("Id2");

  Isis::ControlPoint cp2("G0001");
  cp2.SetType(Isis::ControlPoint::Ground);
  cp2.SetUniversalGround(30.5, 175.0, 3950.2);
  cp2.SetHeld(true);
  cp2.Add(cm1);
  cp2.Add(cm2);

  Isis::ControlMeasure cm3;
  cm3.SetCoordinate(45.2, 135.4, Isis::ControlMeasure::ValidatedManual);
  cm3.SetError(0.25,0.25);
  cm3.SetCubeSerialNumber("Id1");

  Isis::ControlMeasure cm4;
  cm4.SetCoordinate(53.8, 110.5, Isis::ControlMeasure::ValidatedAutomatic);
  cm4.SetError(0.1,0.1);
  cm4.SetCubeSerialNumber("Id2");

  Isis::ControlMeasure cm5;
  cm5.SetCoordinate(70.1, 118.7, Isis::ControlMeasure::Estimated);
  cm5.SetError(0.75,0.75);
  cm5.SetCubeSerialNumber("Id3");
  cm5.SetIgnore(true);

  Isis::ControlMeasure cm6;
  cm6.SetCoordinate(84.1, 168.7, Isis::ControlMeasure::Estimated);
  cm6.SetError(0.75,0.75);
  cm6.SetCubeSerialNumber("Id3");
    
  Isis::ControlPoint cp3("G0002");
  cp3.SetType(Isis::ControlPoint::Ground);
  cp3.SetUniversalGround(63.5, 168.2, 3950.2);
  cp3.Add(cm3);
  cp3.Add(cm4);
  cp3.Add(cm5);

  Isis::ControlPoint cp4("G0002");
  cp4.SetType(Isis::ControlPoint::Ground);
  cp4.SetUniversalGround(65.1, 102.2, 3950.2);
  cp4.Add(cm5);

  Isis::ControlPoint cp5("T0002");
  cp5.SetType((Isis::ControlPoint::PointType)999);
  cp5.Add(cm);

  Isis::ControlNet cn1;
  cn1.SetType(Isis::ControlNet::ImageToGround);
  cn1.SetTarget("Mars");
  cn1.SetNetworkId("Test");
  cn1.SetUserName("jdoe");
  cn1.SetCreatedDate( "2009-02-05T14:20:15" );
  cn1.SetModifiedDate( "2009-02-05T14:20:55" );
  cn1.SetDescription("UnitTest of ControlNetwork");
  cn1.Add(cp1);
  cn1.Add(cp2); 
  cn1.Add(cp3);
  cn1.Add(cp5);

  cout << "Test adding control points with identical id numbers ..." << endl;
  try {
    cn1.Add(cp4);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Test adding invalid control point ..." << endl;
  try {
    cn1.Write("temp.txt");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cn1.Delete(cp5.Id());

  cn1.Write("temp.txt");
  cout << "Test deleting nonexistant control point id ..." << endl;
  try {
    cn1.Delete(cp5.Id());
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Test deleting nonexistant control point index ..." << endl;
  try {
    cn1.Delete(7);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  Isis::ControlNet cn2("temp.txt");

  cn2.Write("temp2.txt");

  string f1 = "temp.txt";
  string f2 = "temp2.txt";

  Isis::TextFile t1;
  Isis::TextFile t2;
  t1.Open(f1);
  t2.Open(f2);

  if (t1.LineCount() != t2.LineCount()) {
    cout << "ERROR: Text Files are not the same!" << endl;
  }
  else {
    for (int l=0; l<t1.LineCount(); l++) {
      string line1, line2;
      t1.GetLine(line1);
      t2.GetLine(line2);
      if (!(line1 == line2)) {
        cout <<  "ERROR: Text Files are not the same!" << endl;
      }
    }
  }

  Isis::Pvl p1("temp.txt");
  cout << p1 << endl;

  remove("temp.txt");
  remove("temp2.txt");

}
