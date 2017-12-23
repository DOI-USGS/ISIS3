#include <iostream>
#include <float.h>

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasureLogData.h"
#include "IException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;
void outit(ControlMeasure &d);

int main() {
  /**
   * @brief Test ControlMeasure object for accuracy and correct behavior.
   *
   * @history 2010-06-30  Tracie Sucharski, Updated for binary control net and
   *                         new keywords.
   * @history 2010-08-12  Tracie Sucharski,  Keywords changed AGAIN..
   * @history 2010-10-18  Tracie Sucharski,  Set EditLock to false before
   *                         Test 5 so type can be updatem.
   * @history 2010-11-03  Mackenzie Boyd,  Added test for PrintableClassData() 
   * @history 2012-07-26  Tracie Sucharski,  Added test for == and != operators. 
   * @history 2017-12-21  Kristin Berry - Added tests for accessor methods. 
   *
  */
  Preference::Preferences(true);
  ControlMeasure m;
  cout << "Test 1 Default values" << endl;
  m.SetChooserName("ManuallySet");
  m.SetDateTime("2001-01-01T00:00:00");
  outit(m);

  m.SetCubeSerialNumber("Test");
  m.SetType(ControlMeasure::Candidate);
  m.SetIgnored(true);
  m.SetCoordinate(1.0, 2.0);
  m.SetResidual(-3.0, 4.0);
  m.SetDiameter(15.0);
  m.SetAprioriSample(2.0);
  m.SetAprioriLine(5.0);
  m.SetSampleSigma(.01);
  m.SetLineSigma(.21);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  return 0; 
  cout << "Test 2" << endl;
  outit(m);

  m.SetType(ControlMeasure::Candidate);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 3" << endl;
  outit(m);

  m.SetType(ControlMeasure::Manual);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  m.SetEditLock(true);
  cout << "Test 4" << endl;
  outit(m);

  m.SetType(ControlMeasure::RegisteredPixel);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  m.SetEditLock(false);
  cout << "Test 5" << endl;
  outit(m);

  m.SetType(ControlMeasure::RegisteredSubPixel);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 6" << endl;
  outit(m);

  m.SetLogData(
    ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit, 5.0)
  );
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");

  // Dump of all variables
  cout << "Test 7" << endl;
  QList< QStringList > printableMeasureData = m.PrintableClassData();
  QStringList nameValuePair;
  foreach(nameValuePair, printableMeasureData) {
    cout << nameValuePair.at(0).toStdString() << "=" <<
        nameValuePair.at(1).toStdString() << endl;
  }

  cout << "Test 8" << endl;
  cout << m.GetLogData(ControlMeasureLogData::GoodnessOfFit).
      GetNumericalValue() << endl;
  cout << m.GetLogValue(ControlMeasureLogData::GoodnessOfFit).
      toDouble() << endl;

  // Test parent editLock on reference measure
  ControlPoint *cp = new ControlPoint("Parent1");
  cp->SetType(ControlPoint::Free);
  cp->Add(&m);
  cp->SetChooserName("Me");
  cp->SetDateTime("Yesterday");
  ControlMeasure m2;
  m2.SetCubeSerialNumber("ReferenceMeasure");
  m2.SetCoordinate(200.,100.);
  m2.SetDateTime("2011-07-04T00:00:00");
  cp->Add(&m2);
  cp->SetRefMeasure(&m2);
  cp->SetEditLock(true);
  cout << endl << "Test 9" << endl;
  cout << "Testing point editLock on reference measure" << endl;
  if (m2.IsEditLocked())
    cout << "Reference point ok" << endl;
  else
    cout << "Reference point failed" << endl;
  if (m.IsEditLocked())
    cout << "Nonreference point failed" << endl;
  else
    cout << "Nonreference point ok" << endl;

  cout << endl << "Test 10" << endl;
  cout << "Testing == operator on the same measures" << endl;
  if (m == m) {
    cout << "Measure1 == Measure1   TRUE" << endl;
  }
  else {
    cout << "Measure1 == Measure1   FALSE" << endl;
  }

  cout << endl << "Test 11" << endl;
  cout << "Testing == operator on two different measures" << endl;
  if (m == m2) {
    cout << "Measure1 == Measure2   TRUE" << endl;
  }
  else {
    cout << "Measure1 == Measure2   FALSE" << endl;
  }

  cout << endl << "Test 12" << endl;
  cout << "Testing != operator on the same measures" << endl;
  if (m != m) {
    cout << "Measure1 != Measure1   TRUE" << endl;
  }
  else {
    cout << "Measure1 != Measure1   FALSE" << endl;
  }

  cout << endl << "Test 13" << endl;
  cout << "Testing != operator on two different measures" << endl;
  if (m != m2) {
    cout << "Measure1 != Measure2   TRUE" << endl;
  }
  else {
    cout << "Measure1 != Measure2   FALSE" << endl;
  }


  try {
    m.SetLogData(ControlMeasureLogData());
  }
  catch (IException &e) {
    e.print();
  }

  cout << endl; 
  cout << "Test 14: Testing accessor methods" << endl; 

  if (m.HasChooserName()) {
    cout << "Chooser Name: " << m.GetChooserName() << endl;
  }

  if (m.HasDateTime()) {
    cout << "DateTime: " << m.GetDateTime() << endl; 
  }

  if (m.HasSample()) {
    cout << "Sample: " << m.GetSample() << endl; 
  }

  if (m.HasLine()) {
    cout << "Line: " << m.GetLine() << endl; 
  }

  if (m.HasDiameter()) {
    cout << "Diameter: " << m.GetDiameter() << endl; 
  }

  if (m.HasAprioriSample()) {
    cout << "AprioriSample: " << m.GetAprioriSample() << endl; 
  }

  if (m.HasAprioriLine()) {
    cout << "AprioriLine: " << m.GetAprioriLine() << endl; 
  }

  if (m.HasSampleSigma()) {
    cout << "SampleSigma: " << m.GetSampleSigma() << endl; 
  }

  if (m.HasLineSigma()) {
    cout << "LineSigma: " << m.GetLineSigma() << endl; 
  }

  if (m.HasSampleResidual()) {
    cout << "SampleResidual: " << m.GetSampleResidual() << endl; 
  }

  if (m.HasLineResidual()) {
    cout << "LineResidual: " << m.GetLineResidual() << endl; 
  }

  if (m.HasJigsawRejected()) {
    if (m.JigsawRejected()) {
      cout << "Measure was rejected by Jigsaw." << endl; 
    }
    else {
      cout << "Measure was not rejected by Jigsaw." << endl; 
    }
  }

  cout << "Log Size: " << m.LogSize() << endl; 
}

void outit(ControlMeasure &m) {
  ControlNet net;
  ControlPoint *pt = new ControlPoint;
  pt->Add(new ControlMeasure(m));
  pt->SetId("CP01");
  pt->SetChooserName("Me");
  pt->SetDateTime("Yesterday");
  net.AddPoint(pt);
  net.SetNetworkId("Identifier");
  net.SetCreatedDate("Yesterday");
  net.SetModifiedDate("Yesterday");
  net.Write("./tmp.net", true);
  Pvl tmp("./tmp.net");
  cout << "Printing measure:\n" << tmp << "\nDone printing measure." << endl
       << endl;
 // remove("./tmp.net");
}
