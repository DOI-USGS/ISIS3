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
#include "FileName.h"
#include "IException.h"
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

  Preference::Preferences(true);

  try {

    cerr << setprecision(9);
    cerr << "Unit test for Sensor" << endl;

    Cube dummyCube("$base/testData/isisTruth.cub", "r");

    Pvl &lab = *dummyCube.label();
    PvlGroup inst("INSTRUMENT");
    inst += PvlKeyword("TargetName", "Mars");
    lab.addGroup(inst);

    PvlGroup kern("Kernels");
    FileName f("$base/testData/kernels");
    QString dir = f.expanded() + "/";

    kern += PvlKeyword("NaifFrameCode", "-94031");
    kern += PvlKeyword("LeapSecond", dir + "naif0007.tls");
    kern += PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
    kern += PvlKeyword("TargetPosition", dir + "de405.bsp");
    kern += PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
    kern += PvlKeyword("Instrument", dir + "mocSensorUnitTest.ti");
//  kern += PvlKeyword("InstrumentAddendum",dir+"mocAddendum.ti");
    kern += PvlKeyword("InstrumentAddendum", "");
    kern += PvlKeyword("InstrumentPosition", dir + "moc.bsp");
    kern += PvlKeyword("InstrumentPointing", dir + "moc.bc");
    kern += PvlKeyword("Frame", "");
    lab.addGroup(kern);

    // Setup
    double startTime = -69382819.0;
    double endTime = -69382512.0;
    double slope = (endTime - startTime) / (10 - 1);
    Sensor spi(dummyCube);
    spi.instrumentRotation()->SetTimeBias(-1.15);

    double v[3] = { 0.0, 0.0, 1.0 };
    double p[3];
    double scSurf[3];

    cerr << setprecision(8);

    // Testing Set Look Direction
    cerr << "Test SetLookDirection using ShapeModel=Null" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi.setTime(iTime(t));
      cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;

      spi.SetLookDirection(v);

      cerr << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
      cerr << "Latitude            = " << spi.UniversalLatitude() << endl;
      cerr << "Longitude           = " << spi.UniversalLongitude() << endl;
      spi.Coordinate(p);
      cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Local Radius        = " //< setprecision(6)
           << spi.LocalRadius().meters() << endl;
      cerr << "Phase               = " << spi.PhaseAngle() << endl;
      cerr << "Emission            = " << spi.EmissionAngle() << endl;
      cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
      spi.LookDirection(p);
      cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      spi.SpacecraftSurfaceVector(scSurf);
      cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
           << " " << scSurf[2] <<endl;
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
    cerr << "Local Radius        = " << spi.LocalRadius().meters() << endl;
    cerr << "Phase               = " << spi.PhaseAngle() << endl;
    cerr << "Emission            = " << spi.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    spi.SpacecraftSurfaceVector(scSurf);
    cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
         << " " << scSurf[2] <<endl;
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
    cerr << "Radius              = " << spi.LocalRadius().meters() << endl;
    spi.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi.LocalRadius().meters() << endl;
    cerr << "Phase               = " << spi.PhaseAngle() << endl;
    cerr << "Emission            = " << spi.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    spi.SpacecraftSurfaceVector(scSurf);
    cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
         << " " << scSurf[2] <<endl;
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

    kern.clear();

    kern += PvlKeyword("NaifFrameCode", "-94031");
    kern += PvlKeyword("LeapSecond", dir + "naif0007.tls");
    kern += PvlKeyword("SpacecraftClock",
                              dir + "MGS_SCLKSCET.00045.tsc");
    kern += PvlKeyword("TargetPosition", dir + "de405.bsp");
    kern += PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
    kern += PvlKeyword("Instrument", dir + "mocSensorUnitTest.ti");
    kern += PvlKeyword("InstrumentAddendum", "");
    kern += PvlKeyword("InstrumentPosition", dir + "moc.bsp");
    kern += PvlKeyword("InstrumentPointing", dir + "moc.bc");
    kern += PvlKeyword("Frame", "");
    kern += PvlKeyword("ShapeModel",
                        "$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub");
    lab.deleteGroup("Kernels");
    lab.addGroup(kern);

    // Setup
    Sensor spi2(dummyCube);
    spi2.instrumentRotation()->SetTimeBias(-1.15);

    // Testing Set Look Direction
    cerr << "Test SetLookDirection using ShapeModel="
            "$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi2.setTime(iTime(t));
      cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;

      spi2.SetLookDirection(v);

      cerr << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
      cerr << "Latitude            = " << spi2.UniversalLatitude() << endl;
      cerr << "Longitude           = " << spi2.UniversalLongitude() << endl;
      spi2.Coordinate(p);
      cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      cerr << "Local Radius        = " << spi2.LocalRadius().meters() << endl;
      cerr << "Phase               = " << spi2.PhaseAngle() << endl;
      cerr << "Emission            = " << spi2.EmissionAngle() << endl;
      cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
      spi2.LookDirection(p);
      cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
           << endl;
      spi2.SpacecraftSurfaceVector(scSurf);
      cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
           << " " << scSurf[2] <<endl;
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
    cerr << "Local Radius        = " << spi2.LocalRadius().meters() << endl;
    cerr << "Phase               = " << spi2.PhaseAngle() << endl;
    cerr << "Emission            = " << spi2.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    spi2.SpacecraftSurfaceVector(scSurf);
    cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
         << " " << scSurf[2] <<endl;
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
    cerr << "Radius              = " << spi2.LocalRadius().meters() << endl;
    spi2.Coordinate(p);
    cerr << "Point               = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    cerr << "Local Radius        = " << spi2.LocalRadius().meters() << endl;
    cerr << "Phase               = " << spi2.PhaseAngle() << endl;
    cerr << "Emission            = " << spi2.EmissionAngle() << endl;
    cerr << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cerr << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2]
         << endl;
    spi2.SpacecraftSurfaceVector(scSurf);
    cerr << "Spacecraft Surface Vector = " << scSurf[0] << " " << scSurf[1]
         << " " << scSurf[2] <<endl;
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
  catch(IException &e) {
    e.print();
  }
}
