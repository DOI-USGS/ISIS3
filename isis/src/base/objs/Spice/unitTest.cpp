#include <iostream>
#include <iomanip>
#include "iException.h"
#include "Spice.h"
#include "Filename.h"
#include "Preference.h"


using namespace std;
/**
 * UnitTest for Spice class.
 * 
 * @internal 
 * @history 2009-03-23  Tracie Sucharski - Removed old keywords 
 *          SpacecraftPosition and SpacecraftPointing with the corrected
 *          InstrumentPosition and InstrumentPointing. 
 */
class MySpice : public Isis::Spice {
  public:
    MySpice (Isis::Pvl &lab) : Isis::Spice (lab) {
      cout << "BodyCode        = " << NaifBodyCode() << endl;
      cout << "SpkCode         = " << NaifSpkCode() << endl;
      cout << "CkCode          = " << NaifCkCode() << endl;
      cout << "IkCode          = " << NaifIkCode() << endl;
      cout << endl;
    }

    int MyInteger (string key) {
      return GetInteger(key,0);
    }

    double MyDouble (string key) {
      return GetDouble(key,0);
    }

    string MyString (string key) {
      return GetString(key,0);
    }

    void MyOutput () {
      cout << "BJ is " << endl;
      vector<double> BJ = BodyRotation()->Matrix();
      for (int i=0; i<(int)BJ.size(); i++) {
        cout << BJ[i] << endl;
      }
      vector<double> IJ = InstrumentRotation()->Matrix();
      vector<double> BI = IJ;
      mxmt_c((SpiceDouble (*)[3])&BJ[0],(SpiceDouble (*)[3])&IJ[0],
             (SpiceDouble (*)[3])&BI[0]);

      cout << "BP is " << endl;
      for (int i=0; i<(int)BI.size(); i++) {
        cout << BI[i] << endl;
      }
    };
};



int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

cout << setprecision(10);
cout << "Unit test for Isis::Spice" << endl;

  Isis::PvlGroup inst("Instrument");
  inst += Isis::PvlKeyword("TargetName","Mard");

  Isis::PvlGroup kern("Kernels");
  Isis::Filename f("$base/testData/kernels");
  string dir = f.Expanded() + "/";
  kern += Isis::PvlKeyword("NaifFrameCode",-94031);
  kern += Isis::PvlKeyword("LeapSecond",dir+"naif0007.tls");
  kern += Isis::PvlKeyword("SpacecraftClock",dir+"MGS_SCLKSCET.00045.tsc");
  kern += Isis::PvlKeyword("TargetPosition",dir+"de405.bsp");
  kern += Isis::PvlKeyword("TargetAttitudeShape",dir+"pck00006.tpc");
  kern += Isis::PvlKeyword("Instrument",dir+"mocSpiceUnitTest.ti");
  kern += Isis::PvlKeyword("InstrumentAddendum",dir+"mocAddendum.ti");
  kern += Isis::PvlKeyword("InstrumentPosition",dir+"moc.bsp");
  kern += Isis::PvlKeyword("InstrumentPointing",dir+"moc.bc");
  kern += Isis::PvlKeyword("Frame","");

  // Time Setup
  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  kern += Isis::PvlKeyword("StartPadding", slope);
  kern += Isis::PvlKeyword("EndPadding", slope);

  Isis::Pvl lab;
  lab.AddGroup(inst);
  lab.AddGroup(kern);

  // Test bad target
  try {
    cout << "Testing unknown target ..." << endl;
    MySpice spi(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  // Test bad getInteger
  Isis::PvlGroup &temp = lab.FindGroup("Instrument");
  temp.AddKeyword(Isis::PvlKeyword("TargetName","Mars"),Isis::Pvl::Replace);
  cout << "Creating Spice object ..." << endl;
  MySpice spi(lab);
  spi.InstrumentRotation()->SetTimeBias(-1.15);
  cout << endl;

  try {
    cout << "Testing unknown integer keyword ... " << endl;
    spi.MyInteger("BadInteger");
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  // Test bad getDouble
  try {
    cout << "Testing unknown double keyword ... " << endl;
    spi.MyDouble("BadDouble");
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  // Test bad getString
  try {
    cout << "Testing unknown string keyword ... " << endl;
    spi.MyString("BadString");
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  // Test good gets
  cout << "Testing convience get methods ... " << endl;
  cout << spi.MyInteger("FRAME_MGS_MOC") << endl;
  cout << spi.MyDouble("INS-94030_NA_FOCAL_LENGTH") << endl;
  cout << spi.MyString("FRAME_-94031_NAME") << endl;
  cout << endl;

  // Testing radius
  cout << "Testing radius ... " << endl;
  double radii[3];
  spi.Radii(radii);
  cout << "Radii[0]:  " << radii[0] << endl;
  cout << "Radii[1]:  " << radii[1] << endl;
  cout << "Radii[2]:  " << radii[2] << endl;
  cout << endl;

  // Normal testing (no cache)
  cout << "Testing without cache ... " << endl;
  for (int i=0; i<10; i++) {
    double t = startTime + (double) i * slope;
    spi.SetEphemerisTime(t);
    cout << "Time           = " << spi.EphemerisTime() << endl;
    double p[3];
    spi.InstrumentPosition(p);
    cout << "Spacecraft (B) = " << p[0] << " " << p[1] << " " << p[2] << endl;
    double v[3];
    spi.InstrumentVelocity(v);
    cout << "Spacecraft Velocity (B) = " << v[0] << " " << v[1] << " " << v[2] << endl;
    spi.SunPosition(p);
    cout << "Sun        (B) = " << p[0] << " " << p[1] << " " << p[2] << endl;
    spi.MyOutput();
    double lat,lon;
    spi.SubSpacecraftPoint (lat,lon);
    cout << "SubSpacecraft  = " << lat << " " << lon << endl;
    spi.SubSolarPoint (lat,lon);
    cout << "SubSolar       = " << lat << " " << lon << endl;
  }
  cout << endl;

  // Testing with cache
  cout << "Testing with cache ... " << endl;
  double tol=.0022; //estimate resolution pixelPitch*alt/fl*1000.
  spi.CreateCache(startTime+slope,endTime-slope,10,tol);
  for (int i=0; i<10; i++) {
    double t = startTime + (double) i * slope;
    spi.SetEphemerisTime(t);
    cout << "Time           = " << spi.EphemerisTime() << endl;
    double p[3];
    spi.InstrumentPosition(p);
    cout << "Spacecraft (B) = " << p[0] << " " << p[1] << " " << p[2] << endl;
    double v[3];
    spi.InstrumentVelocity(v);
    cout << "Spacecraft Velocity (B) = " << v[0] << " " << v[1] << " " << v[2] << endl;
    spi.SunPosition(p);
    cout << "Sun        (B) = " << p[0] << " " << p[1] << " " << p[2] << endl;
    spi.MyOutput();
  }
  cout << endl;

  cout << "Testing Utility methods" << endl;
  cout << "Target Name = " << spi.Target () << endl;
}
