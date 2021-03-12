/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <float.h>

#include <QDebug>
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
  qDebug() << "Test 1 Default values";
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
  qDebug() << "Test 2";
  outit(m);

  m.SetType(ControlMeasure::Candidate);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  qDebug() << "Test 3";
  outit(m);

  m.SetType(ControlMeasure::Manual);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  m.SetEditLock(true);
  qDebug() << "Test 4";
  outit(m);

  m.SetType(ControlMeasure::RegisteredPixel);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  m.SetEditLock(false);
  qDebug() << "Test 5";
  outit(m);

  m.SetType(ControlMeasure::RegisteredSubPixel);
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");
  qDebug() << "Test 6";
  outit(m);

  m.SetLogData(ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit, 5.0));
  m.SetChooserName("Bob");
  m.SetDateTime("2005-05-03T00:00:00");

  // Dump of all variables
  qDebug() << "Test 7";
  QList< QStringList > printableMeasureData = m.PrintableClassData();
  QStringList nameValuePair;
  foreach(nameValuePair, printableMeasureData) {
    // qDebug adds uneccessary spacing after each << so we use cout to display this data.
    std::cout << nameValuePair.at(0).toLatin1().data() << "=" <<
        nameValuePair.at(1).toLatin1().data() <<std::endl;
  }

  qDebug() << "Test 8";
  qDebug() << m.GetLogData(ControlMeasureLogData::GoodnessOfFit).
      GetNumericalValue();
  qDebug() << m.GetLogValue(ControlMeasureLogData::GoodnessOfFit).
      toDouble();

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
  qDebug() << "";
  qDebug() << "Test 9";
  qDebug() << "Testing point editLock on reference measure";
  if (m2.IsEditLocked())
    qDebug() << "Reference point ok";
  else
    qDebug() << "Reference point failed";
  if (m.IsEditLocked())
    qDebug() << "Nonreference point failed";
  else
    qDebug() << "Nonreference point ok";

  qDebug() << "";
  qDebug() << "Test 10";
  qDebug() << "Testing == operator on the same measures";
  if (m == m) {
    qDebug() << "Measure1 == Measure1   TRUE";
  }
  else {
    qDebug() << "Measure1 == Measure1   FALSE";
  }

  qDebug() << "";
  qDebug() << "Test 11";
  qDebug() << "Testing == operator on two different measures";
  if (m == m2) {
    qDebug() << "Measure1 == Measure2   TRUE";
  }
  else {
    qDebug() << "Measure1 == Measure2   FALSE";
  }

  qDebug() << "";
  qDebug() << "Test 12";
  qDebug() << "Testing != operator on the same measures";
  if (m != m) {
    qDebug() << "Measure1 != Measure1   TRUE";
  }
  else {
    qDebug() << "Measure1 != Measure1   FALSE";
  }

  qDebug() << "";
  qDebug() << "Test 13";
  qDebug() << "Testing != operator on two different measures";
  if (m != m2) {
    qDebug() << "Measure1 != Measure2   TRUE";
  }
  else {
    qDebug() << "Measure1 != Measure2   FALSE";
  }

  try {
    m.SetLogData(ControlMeasureLogData());
  }
  catch (IException &e) {
    e.print();
  }

  qDebug() << "";
  qDebug() << "Test 14: Testing accessor methods";

  if (m.IsRejected()) {
      qDebug() << "Measure was rejected.";
  }
  else {
      qDebug() << "Measure was not rejected.";
  }
  qDebug() << "Measure HasChooserName(): " << m.HasChooserName();
  qDebug() << "Measure HasDateTime(): " << m.HasDateTime();
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
  std::cout << "Printing measure:\n" << tmp << "\nDone printing measure.\n" << std::endl;
  remove("./tmp.net");
}
