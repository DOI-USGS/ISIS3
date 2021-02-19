/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Latitude.h"

#include <iomanip>
#include <iostream>

#include "IException.h"
#include "Constants.h"
#include "Distance.h"
#include "Preference.h"

using std::cout;
using std::endl;

using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(14);

  cout << "----- Testing Constructors -----" << endl << endl;

  try {
    cout << "Default constructor" << endl;
    Latitude lat;
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a value in degrees" << endl;
    Latitude lat(45.0, Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a Planetographic value" << endl;
    Latitude lat(45.0,
                 Distance(1500, Distance::Meters),
                 Distance(1500, Distance::Meters),
                 Latitude::Planetographic,
                 Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a Planetographic value and ellipsoid" << endl;
    Latitude lat(45.0, Distance(1500, Distance::Meters),
                 Distance(2500, Distance::Meters), Latitude::Planetographic,
                 Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a more permissive mode but hard task" << endl;
    Latitude lat(95.0, Distance(1500, Distance::Meters),
                 Distance(2500, Distance::Meters), Latitude::Planetographic,
                 Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a more permissive mode" << endl;
    Latitude lat(95.0, Distance(1500, Distance::Meters),
                 Distance(2500, Distance::Meters), Latitude::Planetocentric,
                 Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given disallowed value" << endl;
    Latitude lat(95.0, Distance(1500, Distance::Meters),
                 Distance(2500, Distance::Meters), Latitude::Planetographic,
                 Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Copy constructor" << endl;
    Latitude lat(95.0, Distance(1500, Distance::Meters),
        Distance(2500, Distance::Meters), Latitude::Planetocentric,
        Angle::Degrees, Latitude::AllowPastPole);
    cout << lat.degrees() << " degrees == ";
    cout << Latitude(lat).degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Set Methods -----" << endl << endl;

  try {
    cout << "Set to 45 degrees" << endl;
    Latitude lat(0.0, Angle::Degrees);
    lat.setPlanetocentric(45, Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
    cout << lat.radians() / PI << "*pi radians universal"
        << endl;

    Latitude lat2(0.0, Angle::Degrees);
    lat2 = lat;
    cout << lat.degrees() << " degrees after assignment" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Set to 25 degrees Planetographic" << endl;
    Latitude lat(0.0, Angle::Degrees);
    lat.setPlanetographic(25, Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Set to 25 degrees Planetographic with radii" << endl;
    Latitude lat(0.0, Distance(1400, Distance::Meters),
                 Distance(1500, Distance::Meters));
    lat.setPlanetographic(25, Angle::Degrees);
    cout << lat.degrees() << " degrees" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Get Methods -----" << endl << endl;

  try {
    cout << "-15 degrees with radii (1, 1.1) is" << endl;
    Latitude lat(-15, Distance(1, Distance::Meters),
                 Distance(1.1, Distance::Meters), Latitude::Planetocentric,
                 Angle::Degrees);
    cout << lat.degrees() << " degrees universal" << endl;
    cout << lat.planetocentric(Angle::Degrees) << " degrees Planetocentric"
         << endl;
    cout << lat.planetographic(Angle::Degrees) << " degrees planetographic"
         << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Add Methods -----" << endl << endl;

  try {
    Isis::Pvl latRangeTest;
    latRangeTest.addGroup(Isis::PvlGroup("Mapping"));
    Isis::PvlGroup &latTestGroup = latRangeTest.findGroup("Mapping");
    latTestGroup += Isis::PvlKeyword("ProjectionName", "Equirectangular");
    latTestGroup += Isis::PvlKeyword("EquatorialRadius", "5000.0");
    latTestGroup += Isis::PvlKeyword("PolarRadius", "1000.0");
    latTestGroup += Isis::PvlKeyword("LatitudeType", "Planetographic");
    latTestGroup += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
    latTestGroup += Isis::PvlKeyword("LongitudeDomain", "360");
    latTestGroup += Isis::PvlKeyword("Scale", "5.0");
    latTestGroup += Isis::PvlKeyword("MinimumLatitude", "-90.0");
    latTestGroup += Isis::PvlKeyword("MaximumLatitude", "90.0");
    latTestGroup += Isis::PvlKeyword("MinimumLongitude", "0.0");
    latTestGroup += Isis::PvlKeyword("MaximumLongitude", "360.0");
    latTestGroup += Isis::PvlKeyword("CenterLatitude", "0.0");
    latTestGroup += Isis::PvlKeyword("CenterLongitude", "0.0");
  
    Latitude ographicLat(25, latTestGroup, Angle::Degrees);
    Angle ographicAngle(30, Angle::Degrees);
    cout << "Adding an angle to a planetographic latitude with the add methods." << endl
         << ographicLat.planetographic(Angle::Degrees) << " + "
         << ographicAngle.degrees()
         << " = " << ographicLat.add(ographicAngle, latTestGroup).planetographic(Angle::Degrees)
         << endl;
    cout << "Adding an angle to a planetographic latitude with the + operator." << endl
         << ographicLat.planetographic(Angle::Degrees) << " + "
         << ographicAngle.degrees()
         << " = " << std::setprecision(5) << (ographicLat + ographicAngle).degrees()
         << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl;
  PvlGroup mappingGroup("Mapping");
  mappingGroup += PvlKeyword("TargetName", "Yoda");
  try {
    Latitude( Angle(PI, Angle::Radians), mappingGroup);
  }
  catch (IException &e) {
    cout << "-------------------------------------------------------" << endl;
    cout << "FAILED TO CONSTRUCT LATITUDE OBJECT FROM MAPPING GROUP: " << endl;
    cout << endl << mappingGroup << endl << endl;
    cout << "THROWS: " << endl << endl;
    e.print();
    cout << "-------------------------------------------------------" << endl << endl;
  }
}
