/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iostream>
#include <iomanip>

#include "Angle.h"
#include "Distance.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Sensor.h"
#include "SurfacePoint.h"

#include "Preference.h"

using namespace Isis;
using namespace std;
/**
 * UnitTest for Spice class.
 *
 * @internal
 * @history 2009-03-24  Tracie Sucharski - Replace obsolete keywords
 *          SpacecraftPosition and SpacecraftPointing with InstrumentPosition
 *          and InstrumentPointing.
 */
int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {

    cerr << setprecision(9);
    cerr << "Unit test for Isis::Sensor" << endl;

    Isis::Pvl lab;
    Isis::PvlGroup inst("INSTRUMENT");
    inst += Isis::PvlKeyword("TargetName", "Mars");
    lab.AddGroup(inst);

    Isis::PvlGroup kern("Kernels");
    Isis::Filename f("$base/testData/kernels");
    string dir = f.Expanded() + "/";

    kern += Isis::PvlKeyword("NaifFrameCode", -94031);
    kern += Isis::PvlKeyword("LeapSecond", dir + "naif0007.tls");
    kern += Isis::PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
    kern += Isis::PvlKeyword("TargetPosition", dir + "de405.bsp");
    kern += Isis::PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
    kern += Isis::PvlKeyword("Instrument", dir + "mocSensorUnitTest.ti");
//  kern += Isis::PvlKeyword("InstrumentAddendum",dir+"mocAddendum.ti");
    kern += Isis::PvlKeyword("InstrumentAddendum", "");
    kern += Isis::PvlKeyword("InstrumentPosition", dir + "moc.bsp");
    kern += Isis::PvlKeyword("InstrumentPointing", dir + "moc.bc");
    kern += Isis::PvlKeyword("Frame", "");
    lab.AddGroup(kern);

    // Setup
    double startTime = -69382819.0;
    double endTime = -69382512.0;
    double slope = (endTime - startTime) / (10 - 1);
    Isis::Sensor spi(lab);
    spi.InstrumentRotation()->SetTimeBias(-1.15);

    double v[3] = { 0.0, 0.0, 1.0 };
    double p[3];

    cerr << setprecision(8);

    // Testing Set Look Direction
    cerr << "Test SetLookDirection using ShapeModel=Null" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi.SetTime(iTime(t));
      cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;

      spi.SetLookDirection(v);

      cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
      cerr << "Latitude            = " << spi.UniversalLatitude() << endl;
      cerr << "Longitude           = " << spi.UniversalLongitude() << endl;
      spi.Coordinate(p);
      cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Local Radius        = " //< setprecision(6)
           << spi.LocalRadius().GetMeters() << endl;
      cerr << "Phase               = " << spi.PhaseAngle() << endl;
      cerr << "Emission            = " << spi.EmissionAngle() << endl;
      cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
      spi.LookDirection(p);
      cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Slant Distance      = " << spi.SlantDistance() << endl;
      cerr << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
      cerr << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
      cerr << "Solar Distance      = " << spi.SolarDistance() << endl;
    }
    cerr << endl;

    // Test bad look direction
    cerr << "Test bad look direction using ShapeModel=Null" << endl;
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = -1.0;
    spi.SetLookDirection(p);
    cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cerr << endl;

    // Test SetUniversalGround
    cerr << "Test SetUniversalGround (lat/lon only) using ShapeModel=Null"
         << endl;
    spi.SetUniversalGround(11.57143551329, 223.328646604);
    cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cerr << "Latitude            = " << spi.UniversalLatitude() << endl;
    cerr << "Longitude           = " << spi.UniversalLongitude() << endl;
    spi.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi.LocalRadius().GetMeters() << endl;
    cerr << "Phase               = " << spi.PhaseAngle() << endl;
    cerr << "Emission            = " << spi.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Slant Distance      = " << spi.SlantDistance() << endl;
    cerr << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
    cerr << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
    cerr << "Solar Distance      = " << spi.SolarDistance() << endl;
    cerr << endl;

    // Test SetUniversalGround
    cerr << "Test SetUniversalGround (lat/lon/radius) using ShapeModel=Null"
         << endl;
    Latitude lat(11.57143551329, Angle::Degrees);
    Longitude lon(223.328646604, Angle::Degrees);
    Distance radius(3400., Distance::Meters);
    SurfacePoint tmp(lat, lon, radius);
    spi.SetGround(tmp);

    cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cerr << "Latitude            = " << spi.UniversalLatitude() << endl;
    cerr << "Longitude           = " << spi.UniversalLongitude() << endl;
    cerr << "Radius              = " << spi.LocalRadius().GetMeters() << endl;
    spi.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi.LocalRadius().GetMeters() << endl;
    cerr << "Phase               = " << spi.PhaseAngle() << endl;
    cerr << "Emission            = " << spi.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Slant Distance      = " << spi.SlantDistance() << endl;
    cerr << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
    cerr << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
    cerr << "Solar Distance      = " << spi.SolarDistance() << endl;
    cerr << endl;

    // Test bad ground point
    cerr << "Test Bad ground point using ShapeModel=Null" << endl;
    spi.SetUniversalGround(11.57143551329, 43.328646604);
    cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl
         << endl << endl;

    Isis::Pvl lab2;
    lab2.AddGroup(inst);

    Isis::PvlGroup kern2("Kernels");

    kern2 += Isis::PvlKeyword("NaifFrameCode", -94031);
    kern2 += Isis::PvlKeyword("LeapSecond", dir + "naif0007.tls");
    kern2 += Isis::PvlKeyword("SpacecraftClock",
                              dir + "MGS_SCLKSCET.00045.tsc");
    kern2 += Isis::PvlKeyword("TargetPosition", dir + "de405.bsp");
    kern2 += Isis::PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
    kern2 += Isis::PvlKeyword("Instrument", dir + "mocSensorUnitTest.ti");
    kern2 += Isis::PvlKeyword("InstrumentAddendum", "");
    kern2 += Isis::PvlKeyword("InstrumentPosition", dir + "moc.bsp");
    kern2 += Isis::PvlKeyword("InstrumentPointing", dir + "moc.bc");
    kern2 += Isis::PvlKeyword("Frame", "");
    kern2 += Isis::PvlKeyword("ShapeModel",
                        "$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub");
    lab2.AddGroup(kern2);

    // Setup
    Isis::Sensor spi2(lab2);
    spi2.InstrumentRotation()->SetTimeBias(-1.15);

    // Testing Set Look Direction
    cerr << "Test SetLookDirection using ShapeModel="
            "$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi2.SetTime(iTime(t));
      cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;

      spi2.SetLookDirection(v);

      cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
      cerr << "Latitude            = " << spi2.UniversalLatitude() << endl;
      cerr << "Longitude           = " << spi2.UniversalLongitude() << endl;
      spi2.Coordinate(p);
      cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Local Radius        = " << spi2.LocalRadius().GetMeters() << endl;
      cerr << "Phase               = " << spi2.PhaseAngle() << endl;
      cerr << "Emission            = " << spi2.EmissionAngle() << endl;
      cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
      spi2.LookDirection(p);
      cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Slant Distance      = " << spi2.SlantDistance() << endl;
      cerr << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
      cerr << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
      cerr << "Solar Distance      = " << spi2.SolarDistance() << endl;
    }
    cerr << endl;

    // Test bad look direction
    cerr << "Test bad look direction using "
            "ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub"
         << endl;
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = -1.0;
    spi2.SetLookDirection(p);
    cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cerr << endl;

    // Test SetUniversalGround
    cerr << "Test SetUniversalGround (lat/lon only) using "
            "ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub"
         << endl;
    spi2.SetUniversalGround(11.57143551329, 223.328646604);
    cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cerr << "Latitude            = " << spi2.UniversalLatitude() << endl;
    cerr << "Longitude           = " << spi2.UniversalLongitude() << endl;
    spi2.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi2.LocalRadius().GetMeters() << endl;
    cerr << "Phase               = " << spi2.PhaseAngle() << endl;
    cerr << "Emission            = " << spi2.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Slant Distance      = " << spi2.SlantDistance() << endl;
    cerr << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
    cerr << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
    cerr << "Solar Distance      = " << spi2.SolarDistance() << endl;
    cerr << endl;

    // Test SetUniversalGround
    cerr << "Test SetUniversalGround (lat/lon/radius) using "
            "ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub"
         << endl;
    spi2.SetUniversalGround(11.57143551329, 223.328646604, 3400.);
    cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cerr << "Latitude            = " << spi2.UniversalLatitude() << endl;
    cerr << "Longitude           = " << spi2.UniversalLongitude() << endl;
    cerr << "Radius              = " << spi2.LocalRadius().GetMeters() << endl;
    spi2.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi2.LocalRadius().GetMeters() << endl;
    cerr << "Phase               = " << spi2.PhaseAngle() << endl;
    cerr << "Emission            = " << spi2.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Slant Distance      = " << spi2.SlantDistance() << endl;
    cerr << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
    cerr << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
    cerr << "Solar Distance      = " << spi2.SolarDistance() << endl;
    cerr << endl;

    // Test bad ground point
    cerr << "Test Bad ground point using "
            "ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub"
         << endl;
    spi2.SetUniversalGround(11.57143551329, 43.328646604);
    cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}
