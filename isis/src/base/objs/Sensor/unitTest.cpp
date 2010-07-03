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
    cout << "Test SetLookDirection" << endl;
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
    cout << "Test bad look direction" << endl;
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = -1.0;
    spi.SetLookDirection(p);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;
    cout << endl;

    // Test SetUniversalGround
    cout << "Test SetUniversalGround (lat/lon only)" << endl;
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
    cout << "Test SetUniversalGround (lat/lon/radius)" << endl;
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
    cout << "Test Bad ground point" << endl;
    spi.SetUniversalGround(11.57143551329, 43.328646604);
    cout << "Has Intersection    = " << spi.HasSurfaceIntersection() << endl;

  }
  catch(Isis::iException &e) {
    e.Report();
  }
}
