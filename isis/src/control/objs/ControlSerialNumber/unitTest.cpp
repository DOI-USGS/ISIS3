#include <iostream>
#include <float.h>

#include <QList>
#include <QString>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlSerialNumber.h"

#include "ControlMeasureLogData.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;


int main() {
  Preference::Preferences(true);

  ControlSerialNumber controlSN("Image1");

  ControlPoint point1("Point1");

  ControlMeasure *measure1 = new ControlMeasure();
  measure1->SetCubeSerialNumber("Image1");
  ControlMeasure *measure2 = new ControlMeasure();
  measure2->SetCubeSerialNumber("Image2");

  point1.Add(measure1);
  point1.Add(measure2);

  cout << "Test adding first measure ..." << endl;
  cout << "Key = " << measure1->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
    measure1->GetCubeSerialNumber() << endl;
  try {
    controlSN.AddMeasure(measure1->GetPointId(), measure1);
    cout << "Successfully added measure" << endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
  cout << endl;

  cout << "Test adding second measure ..." << endl;
  cout << "Key = " << measure2->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
    measure2->GetCubeSerialNumber() << endl;
  try {
    controlSN.AddMeasure(measure2->GetPointId(), measure2);
    cout << "Successfully added measure" << endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
  cout << endl;

  ControlPoint point2("Point2");

  ControlMeasure *measure3 = new ControlMeasure();
  measure3->SetCubeSerialNumber("Image1");
  ControlMeasure *measure4 = new ControlMeasure();
  measure4->SetCubeSerialNumber("Image2");

  point2.Add(measure3);
  point2.Add(measure4);

  cout << "Test adding third measure ..." << endl;
  cout << "Key = " << measure3->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
    measure3->GetCubeSerialNumber() << endl;
  try {
    controlSN.AddMeasure(measure3->GetPointId(), measure3);
    cout << "Successfully added measure" << endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
  cout << endl;

  cout << "Test adding fourth measure ..." << endl;
  cout << "Key = " << measure4->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
    measure4->GetCubeSerialNumber() << endl;
  try {
    controlSN.AddMeasure(measure4->GetPointId(), measure4);
    cout << "Successfully added measure" << endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
  cout << endl;

  return 0;
}

