#include <iostream>
#include <float.h>

#include <QList>
#include <QString>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlCubeGraphNode.h"

#include "ControlMeasureLogData.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;


int main() {
  Preference::Preferences(true);

  ControlCubeGraphNode graphNode("Image1");

  ControlPoint point1("Point1");

  ControlMeasure *measure1 = new ControlMeasure();
  measure1->SetCubeSerialNumber("Image1");
  measure1->SetCoordinate(1,2);
  ControlMeasure *measure2 = new ControlMeasure();
  measure2->SetCubeSerialNumber("Image2");

  point1.Add(measure1);
  point1.Add(measure2);

  cout << "Test adding first measure ..." << endl;
  cout << "Key = " << measure1->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
      measure1->GetCubeSerialNumber() << endl;
  try {
    graphNode.addMeasure(measure1);
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
    graphNode.addMeasure(measure2);
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
  measure3->SetCoordinate(3,4);
  ControlMeasure *measure4 = new ControlMeasure();
  measure4->SetCubeSerialNumber("Image2");

  point2.Add(measure3);
  point2.Add(measure4);

  cout << "Test adding third measure ..." << endl;
  cout << "Key = " << measure3->GetPointId().toStdString() << endl;
  cout << "Value = measure with cube serial number " <<
      measure3->GetCubeSerialNumber() << endl;
  try {
    graphNode.addMeasure(measure3);
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
    graphNode.addMeasure(measure4);
    cout << "Successfully added measure" << endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }
  cout << endl;
  
  cout << "Testing getMeasures method...\n";
  QList< ControlMeasure * > measures = graphNode.getMeasures();
  foreach (ControlMeasure * measure, measures) {
    cout << "   (" << measure->GetCubeSerialNumber() << ")\n";
  }
  
  cout << "\nTesting getValidMeasures method...\n";
  measures[0]->SetIgnored(true);
  measures = graphNode.getValidMeasures();
  foreach (ControlMeasure * measure, measures) {
    cout << "   (" << measure->GetCubeSerialNumber() << ")\n";
  }

  return 0;
}

