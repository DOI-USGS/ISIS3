#include "Latitude.h"

#include <iostream>

#include "iException.h"
#include "Constants.h"
#include "Distance.h"
#include "Preference.h"

using std::cout;
using std::endl;

using Isis::Angle;
using Isis::Distance;
using Isis::iException;
using Isis::Latitude;
using Isis::Preference;
using Isis::PI;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(14);

  cout << "----- Testing Constructors -----" << endl << endl;

  try {
    cout << "Constructor given a value in degrees" << endl;
    Latitude lat(45.0, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a Planetographic value" << endl;
    Latitude lat(45.0, Distance(1500), Distance(1500),
        Latitude::Planetographic, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a Planetographic value and ellipsoid" << endl;
    Latitude lat(45.0, Distance(1500), Distance(2500),
        Latitude::Planetographic, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a more permissive mode but hard task" << endl;
    Latitude lat(95.0, Distance(1500), Distance(2500),
        Latitude::Planetographic, Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a more permissive mode" << endl;
    Latitude lat(95.0, Distance(1500), Distance(2500),
        Latitude::Planetocentric, Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given disallowed value" << endl;
    Latitude lat(95.0, Distance(1500), Distance(2500),
        Latitude::Planetographic, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Copy constructor" << endl;
    Latitude lat(95.0, Distance(1500), Distance(2500),
        Latitude::Planetocentric, Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.GetDegrees() << " degrees == ";
    cout << Latitude(lat).GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Set Methods -----" << endl << endl;

  try {
    cout << "Set to 45 degrees" << endl;
    Latitude lat(0.0, Angle::Degrees);
    lat.SetPlanetocentric(45, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
    cout << lat.GetRadians() / PI << "*pi radians universal"
        << endl;

    Latitude lat2(0.0, Angle::Degrees);
    lat2 = lat;
    cout << lat.GetDegrees() << " degrees after assignment" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Set to 25 degrees Planetographic" << endl;
    Latitude lat(0.0, Angle::Degrees);
    lat.SetPlanetographic(25, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Set to 25 degrees Planetographic with radii" << endl;
    Latitude lat(0.0, Distance(1400), Distance(1500));
    lat.SetPlanetographic(25, Angle::Degrees);
    cout << lat.GetDegrees() << " degrees" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Get Methods -----" << endl << endl;

  try {
    cout << "-15 degrees with radii (1, 1.1) is" << endl;
    Latitude lat(-15, Distance(1), Distance(1.1), Latitude::Planetocentric,
        Angle::Degrees);
    cout << lat.GetDegrees() << " degrees universal" << endl;
    cout << lat.GetPlanetocentric(Angle::Degrees) << " degrees Planetocentric"
         << endl;
    cout << lat.GetPlanetographic(Angle::Degrees) << " degrees GetPlanetographic"
         << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl;
}
