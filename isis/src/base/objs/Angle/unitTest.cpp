#include <iostream>
#include <iomanip>
#include "Angle.h"
#include "Constants.h"
#include "IException.h"
#include "Preference.h"
#include "SpecialPixel.h"

using namespace std;
using Isis::Angle;
using Isis::PI;

class MyAngle : public Isis::Angle {
public:
  MyAngle(double angle, Angle::Units unit) : Isis::Angle(angle, unit) {}

  void TestUnitWrapValue() {
    cout << "  Degree wrap value = " << unitWrapValue(Angle::Degrees) << endl;
    cout << "  Radian wrap value = " << unitWrapValue(Angle::Radians) << endl;
  }

  void setAngle(const double& angleValue, const Units& unit) {
    Angle::setAngle(angleValue, unit);
    cout << "angle set to " << angle(unit) << endl;
  }

};


int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  cout << setprecision(9);

  cout << "UnitTest for Angle" << endl << endl;
  cout << "Testing constructors" << endl;

  try {
    Angle angle;
    cout << "  Default constructor - valid?:  " << angle.isValid() << 
      " values: " << angle.radians() << " and " << angle.degrees() << 
      endl;
    cout << "  " << angle.toString() << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle(Isis::Null, Angle::Degrees);
    cout << "  Null input and degree output:  " << angle.degrees() <<
      " degrees" <<endl;
    cout << "  Valid? " << angle.isValid() << endl;
    cout << "  " << angle.toString() << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle(30., Angle::Degrees );
    cout << "  Degree input and radian output:  " << angle.radians() << 
      " radians" << endl;
    cout << "  Valid? " << angle.isValid() << endl;
    cout << "  " << angle.toString() << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle(30. * PI / 180., Angle::Radians );
    cout << "  Radian input and degree output:  " << angle.degrees() <<
      " degrees" <<endl;
    cout << "  " << angle.toString(false) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle(30., Angle::Degrees );
    Angle angleCopy(angle);
    cout <<"  Copy constructor:  " << angleCopy.degrees() << " degrees" << 
        endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << "Testing mutators" << endl;

  try {
    Angle angle(30., Angle::Degrees );
    angle.setDegrees(180);
    cout <<"  setDegrees:  " << angle.degrees() << " degrees" << 
        endl;
    angle.setRadians(PI);
    cout <<"  setRadians:  " << angle.radians() << " radians" << 
        endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << "Testing operators" << endl;

  try {
    Angle angle(30., Angle::Degrees );
    angle = Angle(45., Angle::Degrees);
    cout << "  Assignment operator:  " << angle.degrees() << " degrees" <<
        endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(60., Angle::Degrees );
    angle1 = angle1 + angle2;
    // Begin with 30 degrees and end with 30 degrees
    cout << "  + and - operators..." << endl;
    cout << "    angle + angle: " << angle1.degrees() << " degrees" <<endl;
    angle1 += angle2;
    cout << "    angle += angle: " << angle1.degrees() << " degrees" <<endl;
    angle1 -= angle2;
    cout << "    angle -= angle: " << angle1.degrees() << " degrees" <<endl;
    angle1 = angle1 - angle2;
    cout << "    angle - angle: " << angle1.degrees() << " degrees" <<endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle(30., Angle::Degrees );
    // Begin with 30 degrees and end with 30 degrees
    cout << "  * and / operators..." << endl;
    angle = 2. * angle;
    cout << "    double * angle: " << angle.degrees() << " degrees" <<endl;
    angle *= 2;
    cout << "    angle *= double: " << angle.degrees() << " degrees" <<endl;
    angle /= 2;
    cout << "    angle /= double: " << angle.degrees() << " degrees" <<endl;
    angle = angle / 2;
    cout << "    angle / double: " << angle.degrees() << " degrees" <<endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << "Testing logical operators" << endl;

  try {
    Angle angle1(30., Angle::Degrees);
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 == angle2?  " << (angle1 == angle2) << endl;
    cout << "  angle1 == angle2?  " << (Angle() == Angle()) << endl;
    cout << "  angle1 == angle2?  " << (Angle() == angle2) <<endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 <  angle2?  " << (angle1 < angle2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 <= angle2?  " << (angle1 <= angle2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    cout << "  angle1 <  angle2?  " << (angle1 < Angle());
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    cout << "  angle1 <  angle2?  " << (Angle() < angle1);
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    cout << "  angle1 <=  angle2?  " << (Angle() <= angle1);
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 >  angle2?  " << (angle1 > angle2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    cout << "  angle1 >  angle2?  " << (angle1 > Angle()) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    Angle angle2(45., Angle::Degrees);
    cout << "  angle1 >= angle2?  " << (angle1 >= angle2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    Angle angle1(30., Angle::Degrees );
    cout << "  angle1 >= angle2?  " << (angle1 >= Angle()) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << "Testing protected methods" << endl;

  try {
    MyAngle angle(30., Angle::Degrees);
    angle.TestUnitWrapValue();
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    MyAngle angle(0., Angle::Degrees);
    cout << "  Degree ";
    angle.setAngle(60., Angle::Degrees);
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
    MyAngle angle(0., Angle::Radians);
    cout << "  Radian ";
    angle.setAngle(.5, Angle::Degrees);
  }
  catch(Isis::IException &e) {
    e.print();
  }
}







