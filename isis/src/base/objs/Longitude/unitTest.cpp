#include "Longitude.h"

#include <iostream>

#include "iException.h"
#include "Constants.h"
#include "Preference.h"

using std::cout;
using std::endl;

using Isis::Angle;
using Isis::Longitude;
using Isis::iException;
using Isis::Preference;
using Isis::PI;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(14);

  cout << "----- Testing Constructors -----" << endl << endl;

  try {
    cout << "Default constructor" << endl;
    Longitude lon;
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a value in degrees" << endl;
    Longitude lon(180.0, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a positive west value in degrees" << endl;
    Longitude lon(180.0, Angle::Degrees, Longitude::PositiveWest);
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a positive west, -90 value in degrees" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain180);
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given -90 degrees PW & 360 domain" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain360);
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Copy constructor" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain360);
    cout << lon.GetDegrees() << " degrees == " << Longitude(lon).GetDegrees() <<
      " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Set Methods -----" << endl << endl;

  try {
    cout << "Set to 90 degrees" << endl;
    Longitude lon(270.0, Angle::Degrees);
    lon.SetPositiveEast(90, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Set to 90 degrees PW" << endl;
    Longitude lon(270.0, Angle::Degrees);
    Longitude lonCopy(lon);
    lon.SetPositiveWest(90, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees" << endl;

    lonCopy = lon;
    cout << "After assignment: " << lonCopy.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Get Methods -----" << endl << endl;

  try {
    cout << "90 degrees is" << endl;
    Longitude lon(90.0, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees universal" << endl;
    cout << lon.GetPositiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.GetPositiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.GetPositiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.GetPositiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "450 degrees is" << endl;
    Longitude lon(450.0, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees universal" << endl;
    cout << lon.GetPositiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.GetPositiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.GetPositiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.GetPositiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "-450 degrees is" << endl;
    Longitude lon(-450.0, Angle::Degrees);
    cout << lon.GetDegrees() << " degrees universal" << endl;
    cout << lon.GetPositiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.GetPositiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.GetPositiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.GetPositiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "-450 degrees PW is" << endl;
    Longitude lon(-450.0, Angle::Degrees, Longitude::PositiveWest);
    cout << lon.GetDegrees() << " degrees universal" << endl;
    cout << lon.GetPositiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.GetPositiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.GetPositiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.GetPositiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Domain Methods -----" << endl << endl;

  try {
    cout << "Test Force180Domain" << endl;
    Longitude lon(270.0, Angle::Degrees);
    cout << lon.Force180Domain().GetDegrees() << " degrees" << endl;

    cout << "Test Force360Domain" << endl;
    cout << lon.Force360Domain().GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }
}
