#include <iostream>
#include <iomanip>
#include "Sensor.h"
#include "iException.h"
#include "Filename.h"

#include "Preference.h"

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

    cout << setprecision(9);
    cout << "Unit test for Isis::Sensor" << endl;

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

    // Testing Set Look Direction
    cout << "Test SetLookDirection using ShapeModel=Null" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi.SetEphemerisTime(t);
      cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;

      spi.SetLookDirection(v);
      cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
      cout << "Latitude            = " << spi.UniversalLatitude() << endl;
      cout << "Longitude           = " << spi.UniversalLongitude() << endl;
      spi.Coordinate(p);
      cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
      cout << "Local Radius        = " << spi.LocalRadius() << endl;
      cout << "Phase               = " << spi.PhaseAngle() << endl;
      cout << "Emission            = " << spi.EmissionAngle() << endl;
      cout << "Incidence           = " << spi.IncidenceAngle() << endl;
      spi.LookDirection(p);
      cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
      cout << "Slant Distance      = " << spi.SlantDistance() << endl;
      cout << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
      cout << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
      cout << "Solar Distance      = " << spi.SolarDistance() << endl;
    }
    cout << endl;

    // Test bad look direction
    cout << "Test bad look direction using ShapeModel=Null" << endl;
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = -1.0;
    spi.SetLookDirection(p);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cout << endl;

    // Test SetUniversalGround
    cout << "Test SetUniversalGround (lat/lon only) using ShapeModel=Null" << endl;
    spi.SetUniversalGround(11.57143551329, 223.328646604);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cout << "Latitude            = " << spi.UniversalLatitude() << endl;
    cout << "Longitude           = " << spi.UniversalLongitude() << endl;
    spi.Coordinate(p);
    cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Local Radius        = " << spi.LocalRadius() << endl;
    cout << "Phase               = " << spi.PhaseAngle() << endl;
    cout << "Emission            = " << spi.EmissionAngle() << endl;
    cout << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Slant Distance      = " << spi.SlantDistance() << endl;
    cout << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
    cout << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
    cout << "Solar Distance      = " << spi.SolarDistance() << endl;
    cout << endl;

    // Test SetUniversalGround
    cout << "Test SetUniversalGround (lat/lon/radius) using ShapeModel=Null" << endl;
    spi.SetUniversalGround(11.57143551329, 223.328646604, 3400.);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cout << "Latitude            = " << spi.UniversalLatitude() << endl;
    cout << "Longitude           = " << spi.UniversalLongitude() << endl;
    cout << "Radius              = " << spi.LocalRadius() << endl;
    spi.Coordinate(p);
    cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Local Radius        = " << spi.LocalRadius() << endl;
    cout << "Phase               = " << spi.PhaseAngle() << endl;
    cout << "Emission            = " << spi.EmissionAngle() << endl;
    cout << "Incidence           = " << spi.IncidenceAngle() << endl;
    spi.LookDirection(p);
    cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Slant Distance      = " << spi.SlantDistance() << endl;
    cout << "Local Solar Time    = " << spi.LocalSolarTime() << endl;
    cout << "Spacecraft Altitude = " << spi.SpacecraftAltitude() << endl;
    cout << "Solar Distance      = " << spi.SolarDistance() << endl;
    cout << endl;

    // Test bad ground point
    cout << "Test Bad ground point using ShapeModel=Null" << endl;
    spi.SetUniversalGround(11.57143551329, 43.328646604);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl << endl << endl;

    Isis::Pvl lab2;
    lab2.AddGroup(inst);

    Isis::PvlGroup kern2("Kernels");

    kern2 += Isis::PvlKeyword("NaifFrameCode", -94031);
    kern2 += Isis::PvlKeyword("LeapSecond", dir + "naif0007.tls");
    kern2 += Isis::PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
    kern2 += Isis::PvlKeyword("TargetPosition", dir + "de405.bsp");
    kern2 += Isis::PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
    kern2 += Isis::PvlKeyword("Instrument", dir + "mocSensorUnitTest.ti");
    kern2 += Isis::PvlKeyword("InstrumentAddendum", "");
    kern2 += Isis::PvlKeyword("InstrumentPosition", dir + "moc.bsp");
    kern2 += Isis::PvlKeyword("InstrumentPointing", dir + "moc.bc");
    kern2 += Isis::PvlKeyword("Frame", "");
    kern2 += Isis::PvlKeyword("ShapeModel","$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub");
    lab2.AddGroup(kern2);

    // Setup
    Isis::Sensor spi2(lab2);
    spi2.InstrumentRotation()->SetTimeBias(-1.15);

    // Testing Set Look Direction
    cout << "Test SetLookDirection using ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    for(int i = 0; i < 10; i++) {
      double t = startTime + (double) i * slope;
      spi2.SetEphemerisTime(t);
      cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;

      spi2.SetLookDirection(v);
      cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
      cout << "Latitude            = " << spi2.UniversalLatitude() << endl;
      cout << "Longitude           = " << spi2.UniversalLongitude() << endl;
      spi2.Coordinate(p);
      cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
      cout << "Local Radius        = " << spi2.LocalRadius() << endl;
      cout << "Phase               = " << spi2.PhaseAngle() << endl;
      cout << "Emission            = " << spi2.EmissionAngle() << endl;
      cout << "Incidence           = " << spi2.IncidenceAngle() << endl;
      spi2.LookDirection(p);
      cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
      cout << "Slant Distance      = " << spi2.SlantDistance() << endl;
      cout << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
      cout << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
      cout << "Solar Distance      = " << spi2.SolarDistance() << endl;
    }
    cout << endl;

    // Test bad look direction
    cout << "Test bad look direction using ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = -1.0;
    spi2.SetLookDirection(p);
    cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cout << endl;

    // Test SetUniversalGround
    cout << "Test SetUniversalGround (lat/lon only) using ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    spi2.SetUniversalGround(11.57143551329, 223.328646604);
    cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cout << "Latitude            = " << spi2.UniversalLatitude() << endl;
    cout << "Longitude           = " << spi2.UniversalLongitude() << endl;
    spi2.Coordinate(p);
    cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Local Radius        = " << spi2.LocalRadius() << endl;
    cout << "Phase               = " << spi2.PhaseAngle() << endl;
    cout << "Emission            = " << spi2.EmissionAngle() << endl;
    cout << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Slant Distance      = " << spi2.SlantDistance() << endl;
    cout << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
    cout << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
    cout << "Solar Distance      = " << spi2.SolarDistance() << endl;
    cout << endl;

    // Test SetUniversalGround
    cout << "Test SetUniversalGround (lat/lon/radius) using ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    spi2.SetUniversalGround(11.57143551329, 223.328646604, 3400.);
    cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
    cout << "Latitude            = " << spi2.UniversalLatitude() << endl;
    cout << "Longitude           = " << spi2.UniversalLongitude() << endl;
    cout << "Radius              = " << spi2.LocalRadius() << endl;
    spi2.Coordinate(p);
    cout << "Point               = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Local Radius        = " << spi2.LocalRadius() << endl;
    cout << "Phase               = " << spi2.PhaseAngle() << endl;
    cout << "Emission            = " << spi2.EmissionAngle() << endl;
    cout << "Incidence           = " << spi2.IncidenceAngle() << endl;
    spi2.LookDirection(p);
    cout << "Look Direction      = " << p[0] << " " << p[1] << " " << p[2] << endl;
    cout << "Slant Distance      = " << spi2.SlantDistance() << endl;
    cout << "Local Solar Time    = " << spi2.LocalSolarTime() << endl;
    cout << "Spacecraft Altitude = " << spi2.SpacecraftAltitude() << endl;
    cout << "Solar Distance      = " << spi2.SolarDistance() << endl;
    cout << endl;

    // Test bad ground point
    cout << "Test Bad ground point using ShapeModel=$ISIS3DATA/base/dems/molaMarsPlanetaryRadius0004.cub" << endl;
    spi2.SetUniversalGround(11.57143551329, 43.328646604);
    cout << "Has Intersection    = " << spi2.HasSurfaceIntersection() << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}
