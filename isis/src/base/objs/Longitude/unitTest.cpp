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
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a value in degrees" << endl;
    Longitude lon(180.0, Angle::Degrees);
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a positive west value in degrees" << endl;
    Longitude lon(180.0, Angle::Degrees, Longitude::PositiveWest);
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a positive west, -90 value in degrees" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain180);
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given -90 degrees PW & 360 domain" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain360);
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Copy constructor" << endl;
    Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain360);
    cout << lon.degrees() << " degrees == " << Longitude(lon).degrees() <<
      " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Set Methods -----" << endl << endl;

  try {
    cout << "Set to 90 degrees" << endl;
    Longitude lon(270.0, Angle::Degrees);
    lon.setPositiveEast(90, Angle::Degrees);
    cout << lon.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Set to 90 degrees PW" << endl;
    Longitude lon(270.0, Angle::Degrees);
    Longitude lonCopy(lon);
    lon.setPositiveWest(90, Angle::Degrees);
    cout << lon.degrees() << " degrees" << endl;

    lonCopy = lon;
    cout << "After assignment: " << lonCopy.degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Get Methods -----" << endl << endl;

  try {
    cout << "90 degrees is" << endl;
    Longitude lon(90.0, Angle::Degrees);
    cout << lon.degrees() << " degrees universal" << endl;
    cout << lon.positiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.positiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.positiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.positiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "450 degrees is" << endl;
    Longitude lon(450.0, Angle::Degrees);
    cout << lon.degrees() << " degrees universal" << endl;
    cout << lon.positiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.positiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.positiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.positiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "-450 degrees is" << endl;
    Longitude lon(-450.0, Angle::Degrees);
    cout << lon.degrees() << " degrees universal" << endl;
    cout << lon.positiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.positiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.positiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.positiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;

  try {
    cout << "-450 degrees PW is" << endl;
    Longitude lon(-450.0, Angle::Degrees, Longitude::PositiveWest);
    cout << lon.degrees() << " degrees universal" << endl;
    cout << lon.positiveEast(Angle::Degrees) << " degrees positive east"
        << endl;
    cout << lon.positiveEast(Angle::Radians) / PI << "*pi radians positive "
        "east" << endl;
    cout << lon.positiveWest(Angle::Degrees) << " degrees positive west"
      << endl;
    cout << lon.positiveWest(Angle::Radians) / PI << "*pi radians positive "
        "west" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Domain Methods -----" << endl << endl;

  try {
    cout << "Test force180Domain" << endl;
    Longitude lon(270.0, Angle::Degrees);
    cout << lon.force180Domain().degrees() << " degrees" << endl;

    cout << "Test force360Domain" << endl;
    cout << lon.force360Domain().degrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }
}
