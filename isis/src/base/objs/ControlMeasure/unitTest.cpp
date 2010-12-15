#include "ControlMeasure.h"
#include "Preference.h"

#include <iostream>

#include <QList>
#include <QString>
#include <QStringList>

#include <float.h>
using namespace std;
void outit (Isis::ControlMeasure &d);

int main () {
  /**
   * @brief Test ControlMeasure object for accuracy and correct behavior.
   *
   * @history 2010-06-30  Tracie Sucharski, Updated for binary control net and 
   *                         new keywords.
   * @history 2010-08-12  Tracie Sucharski,  Keywords changed AGAIN.. 
   * @history 2010-10-18  Tracie Sucharski,  Set EditLock to false before 
   *                         Test 5 so type can be updated. 
   * @history 2010-11-03  Mackenzie Boyd,  Added test for PrintableClassData() 
   *  
  */
  Isis::Preference::Preferences(true);
  Isis::ControlMeasure d;
  cout << "Test 1 Default values" << endl;
  d.SetChooserName("ManuallySet");
  d.SetDateTime("2001-01-01T00:00:00");
  outit(d);

  d.SetCubeSerialNumber("Test");
  d.SetType(Isis::ControlMeasure::Reference);
  d.SetIgnore(true);
  d.SetCoordinate(1.0,2.0);
  d.SetResidual(-3.0,4.0);
  d.SetDiameter(15.0);
  d.SetAprioriSample(2.0);
  d.SetAprioriLine(5.0);
  d.SetSampleSigma(.01);
  d.SetLineSigma(.21);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 2" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::Candidate);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 3" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::Manual);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  d.SetEditLock(true);
  cout << "Test 4" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::RegisteredPixel);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  d.SetEditLock(false);
  cout << "Test 5" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::RegisteredSubPixel);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 6" << endl;
  outit(d);

  d.SetType(Isis::ControlMeasure::Ground);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 7" << endl;
  outit(d);

  // Dump of all variables
  cout << "Test 7" << endl;
  QList< QStringList > printableMeasureData = d.PrintableClassData();
  QStringList nameValuePair;
  foreach (nameValuePair, printableMeasureData) {
    cout << nameValuePair.at(0).toStdString() << "=" << nameValuePair.at(1).toStdString() << endl;
  }
  cout << DBL_MIN;

}

void outit (Isis::ControlMeasure &d) {
  Isis::Pvl pvl;
  pvl.AddGroup(d.CreatePvlGroup());
  cout << pvl << endl;
}
