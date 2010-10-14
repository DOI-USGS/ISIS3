#include <iostream>
#include <iomanip>
#include "Constants.h"
#include "iException.h"
#include "Preference.h"
#include "Angle.h"

using namespace std;
using Isis::Angle;

class MyAngle : public Isis::Angle {
public:
  MyAngle(double angle, Angle::Units unit) : Isis::Angle(angle, unit) {}

  void TestUnitWrapValue() {
    cout << "  Degree wrap value = " << UnitWrapValue(Angle::Degrees) << endl;
    cout << "  Radian wrap value = " << UnitWrapValue(Angle::Radians) << endl;
  }

  void SetAngle(const double& angle, const Units& unit) {
    Angle::SetAngle(angle, unit);
    cout << "angle set to " << GetAngle(unit) << endl;
  }

};


int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  cout << setprecision(9);

  cout << endl << "UnitTest for Angle" << endl << endl;
  cout << "    Testing constructors" << endl;

  try {
    Angle angle(30., Angle::Degrees );
    cout <<"  Degree input and radian output:  " << angle.GetRadians() << 
      " radians" << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle(0.523598776, Angle::Radians );
    cout <<"  Radian input and degree output:  " << angle.GetDegrees() <<
      " degrees" <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle(30., Angle::Degrees );
    Angle angleCopy(angle);
    cout <<"  Copy constructor:  " << angleCopy.GetDegrees() << " degrees" <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  cout << endl << "    Testing operators" << endl;

  try {
    Angle angle(30., Angle::Degrees );
    angle = Angle(45., Angle::Degrees);
    cout <<"  Assignment operator:  " << angle.GetDegrees() << " degrees" <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(60., Angle::Degrees );
    angle1 = angle1 + angle2;
    // Begin with 30 degrees and end with 30 degrees
    cout << "  + and - operators..." << endl;
    cout << "    angle + angle: " << angle1.GetDegrees() << " degrees" <<endl;
    angle1 += angle2;
    cout << "    angle += angle: " << angle1.GetDegrees() << " degrees" <<endl;
    angle1 -= angle2;
    cout << "    angle -= angle: " << angle1.GetDegrees() << " degrees" <<endl;
    angle1 = angle1 - angle2;
    cout << "    angle - angle: " << angle1.GetDegrees() << " degrees" <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle(30., Angle::Degrees );
    // Begin with 30 degrees and end with 30 degrees
    cout << "  * and / operators..." << endl;
    angle = 2. * angle;
    cout << "    double * angle: " << angle.GetDegrees() << " degrees" <<endl;
    angle *= 2;
    cout << "    angle *= double: " << angle.GetDegrees() << " degrees" <<endl;
    angle /= 2;
    cout << "    angle /= double: " << angle.GetDegrees() << " degrees" <<endl;
    angle = angle / 2;
    cout << "    angle / double: " << angle.GetDegrees() << " degrees" <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  cout << endl << "    Testing logical operators" << endl;

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 == angle2?  " << (angle1 == angle2) <<endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 < angle2?  " << (angle1 < angle2) << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 <= angle2?  " << (angle1 <= angle2) << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 > angle2?  " << (angle1 > angle2) << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 >= angle2?  " << (angle1 >= angle2) << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  cout << endl << "    Testing protected methods" << endl;

  try {
    MyAngle angle(30., Angle::Degrees);
    angle.TestUnitWrapValue();
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    MyAngle angle(0., Angle::Degrees);
    cout << "  Degree ";
    angle.SetAngle(60., Angle::Degrees);
  }
  catch(Isis::iException &e) {
    e.Report();
  }

  try {
    MyAngle angle(0., Angle::Radians);
    cout << "  Radian ";
    angle.SetAngle(.5, Angle::Degrees);
  }
  catch(Isis::iException &e) {
    e.Report();
  }
//345678901234567890123456789012345678901234567890123456789012345678901234567890


}







