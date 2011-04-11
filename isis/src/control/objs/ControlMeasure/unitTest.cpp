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
#include "iException.h"
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
   *                         Test 5 so type can be updated.
   * @history 2010-11-03  Mackenzie Boyd,  Added test for PrintableClassData()
   *
  */
  Preference::Preferences(true);
  ControlMeasure d;
  cout << "Test 1 Default values" << endl;
  d.SetChooserName("ManuallySet");
  d.SetDateTime("2001-01-01T00:00:00");
  outit(d);

  d.SetCubeSerialNumber("Test");
  d.SetType(ControlMeasure::Candidate);
  d.SetIgnored(true);
  d.SetCoordinate(1.0, 2.0);
  d.SetResidual(-3.0, 4.0);
  d.SetDiameter(15.0);
  d.SetAprioriSample(2.0);
  d.SetAprioriLine(5.0);
  d.SetSampleSigma(.01);
  d.SetLineSigma(.21);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 2" << endl;
  outit(d);

  d.SetType(ControlMeasure::Candidate);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 3" << endl;
  outit(d);

  d.SetType(ControlMeasure::Manual);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  d.SetEditLock(true);
  cout << "Test 4" << endl;
  outit(d);

  d.SetType(ControlMeasure::RegisteredPixel);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  d.SetEditLock(false);
  cout << "Test 5" << endl;
  outit(d);

  d.SetType(ControlMeasure::RegisteredSubPixel);
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");
  cout << "Test 6" << endl;
  outit(d);

  d.SetLogData(
    ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit, 5.0)
  );
  d.SetChooserName("Bob");
  d.SetDateTime("2005-05-03T00:00:00");

  // Dump of all variables
  cout << "Test 7" << endl;
  QList< QStringList > printableMeasureData = d.PrintableClassData();
  QStringList nameValuePair;
  foreach(nameValuePair, printableMeasureData) {
    cout << nameValuePair.at(0).toStdString() << "=" <<
        nameValuePair.at(1).toStdString() << endl;
  }

  cout << "Test 8" << endl;
  cout << d.GetLogData(ControlMeasureLogData::GoodnessOfFit).
      GetNumericalValue() << endl;
  cout << d.GetLogValue(ControlMeasureLogData::GoodnessOfFit).
      toDouble() << endl;

  try {
    d.SetLogData(ControlMeasureLogData());
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
}

void outit(ControlMeasure &d) {
  ControlNet net;
  ControlPoint *pt = new ControlPoint;
  pt->Add(new ControlMeasure(d));
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
  remove("./tmp.net");
}
