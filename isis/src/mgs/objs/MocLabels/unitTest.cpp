#include "iException.h"
#include "Pvl.h"
#include "MocLabels.h"
#include "Preference.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);

  try {
    cout << "Unit test for MocLabels" << endl;
    cout << "MocWideAngleCamera cub test..." << endl;
    Isis::Pvl p1("$mgs/testData/ab102401.cub");
    Isis::Mgs::MocLabels lab1(p1);

    cout << "NarrowAngle?  " << lab1.NarrowAngle() << endl;
    cout << "WideAngle?  " << lab1.WideAngle() << endl;
    cout << "WideAngleRed?  " << lab1.WideAngleRed() << endl;
    cout << "WideAngleBlue?  " << lab1.WideAngleBlue() << endl;
    cout << "Crosstrack Summing = " << lab1.CrosstrackSumming() << endl;
    cout << "Downtrack Summing = " << lab1.DowntrackSumming() << endl;
    cout << "First Line Sample = " << lab1.FirstLineSample() << endl;
    cout << "Focal Plane Temperature = " << lab1.FocalPlaneTemperature() << endl;
    cout << "Line Rate = " << lab1.LineRate() << endl;
    cout << "Exposure Duration = " << lab1.ExposureDuration() << endl;
    cout << "Start Time = " << lab1.StartTime() << endl;
    cout << "Detectors = " << lab1.Detectors() << endl;
    cout << "StartDetector(1) = " << lab1.StartDetector(1) << endl;
    cout << "EndDetector(1) = " << lab1.EndDetector(1) << endl;
    cout << "Sample = " << lab1.Sample(lab1.StartDetector(1)) << endl;
    cout << "EphemerisTime = " << lab1.EphemerisTime(1) << endl;
    cout << "Gain = " << lab1.Gain() << endl;
    cout << "Offset = " << lab1.Offset() << endl << endl;

    cout << "MocNarrowAngleCamera cub test..." << endl;
    Isis::Pvl p2("$mgs/testData/fha00491.lev1.cub");
    Isis::Mgs::MocLabels lab2(p2);

    cout << "NarrowAngle?  " << lab2.NarrowAngle() << endl;
    cout << "WideAngle?  " << lab2.WideAngle() << endl;
    cout << "WideAngleRed?  " << lab2.WideAngleRed() << endl;
    cout << "WideAngleBlue?  " << lab2.WideAngleBlue() << endl;
    cout << "Crosstrack Summing = " << lab2.CrosstrackSumming() << endl;
    cout << "Downtrack Summing = " << lab2.DowntrackSumming() << endl;
    cout << "First Line Sample = " << lab2.FirstLineSample() << endl;
    cout << "Focal Plane Temperature = " << lab2.FocalPlaneTemperature() << endl;
    cout << "Line Rate = " << lab2.LineRate() << endl;
    cout << "Exposure Duration = " << lab2.ExposureDuration() << endl;
    cout << "Start Time = " << lab2.StartTime() << endl;
    cout << "Detectors = " << lab2.Detectors() << endl;
    cout << "StartDetector(1) = " << lab2.StartDetector(1) << endl;
    cout << "EndDetector(1) = " << lab2.EndDetector(1) << endl;
    cout << "Sample = " << lab2.Sample(lab2.StartDetector(1)) << endl;
    cout << "EphemerisTime = " << lab2.EphemerisTime(1) << endl;
    cout << "Gain = " << lab2.Gain() << endl;
    cout << "Offset = " << lab2.Offset() << endl << endl;

  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

