#include <iostream>
#include <iomanip>
#include <cmath>
#include "SpiceRotation.h"
#include "FileName.h"
#include "Preference.h"
#include "Table.h"



using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << setprecision(8);
  cout << "Unit test for SpiceRotation" << endl;

  // Test case is taken from moc red wide angle image ab102401
  // sn = MGS/561812335:32/MOC-WA/RED
  Isis::FileName f("$base/testData/kernels");
  string dir = f.expanded() + "/";
  string naif(dir + "naif0007.tls");
  string mgs(dir + "MGS_SCLKSCET.00045.tsc");
  string mocti(dir + "moc13.ti");
  string mocbc(dir + "moc.bc");
  string mocbsp(dir + "moc.bsp");
  string de(dir + "de405.bsp");
  string pck(dir + "pck00006.tpc");
  //string mocadd(dir+"mocAddendum.ti");
  string mocspice(dir + "mocSpiceRotationUnitTest.ti");
  furnsh_c(naif.c_str());
  furnsh_c(mgs.c_str());
  furnsh_c(mocti.c_str());
  furnsh_c(mocbc.c_str());
  furnsh_c(mocbsp.c_str());
  furnsh_c(de.c_str());
  furnsh_c(pck.c_str());
//  furnsh_c(mocadd.c_str());
  furnsh_c(mocspice.c_str());

  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  SpiceInt code;
  namfrm_c("MGS_MOC", &code);
//  namfrm_c ("IAU_MARS", &code);
  cout << "Naif code = " << code << endl;


  Isis::SpiceRotation rot(-94031);

  // Normal testing (no cache)
  cout << "Testing without cache ... " << endl;
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot.SetEphemerisTime(t);
    std::vector<double> CJ = rot.Matrix();

    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if(rot.HasAngularVelocity()) {
      std::vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

  // Testing with cache
  cout << "Testing with cache ... " << endl;
  rot.LoadCache(startTime, endTime, 10);
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
//    vector<double> p = pos.SetEphemerisTime(t);
    rot.SetEphemerisTime(t);
    vector<double> CJ = rot.Matrix();
    cout << "Time           = " << rot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot.HasAngularVelocity()) {
      std::vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

  // Save off cache for polynomial over cache test
  Isis::Table tab = rot.Cache("TestPolyOver");

  // Testing with Functions
  cout << "Testing with functions ... " << endl;
  std::vector<double> abcAng1, abcAng2, abcAng3;
//   rot.SetOverrideBaseTime(0., 1.);
//   rot.ComputeBaseTime();
  rot.SetPolynomial();
  rot.GetPolynomial(abcAng1, abcAng2, abcAng3);
  cout << "Source = " << rot.GetSource() << endl;
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot.SetEphemerisTime(t);
    vector<double> CJ = rot.Matrix();
    cout << "Time           = " << rot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot.HasAngularVelocity()) {
      std::vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Testing polynomial over Spice
  cout << "Testing with polynomial functions over Spice ... " << endl;
  Isis::SpiceRotation rot2(-94031);
  rot2.LoadCache(tab);
  rot2.ComputeBaseTime();
  abcAng1.clear();
  abcAng2.clear();
  abcAng3.clear();
  rot2.SetPolynomialDegree(2);
  abcAng1.push_back(0.0030493533013399013);
  abcAng1.push_back(-0.0027570887651990781);
  abcAng1.push_back(0.0042922079124063069);
  abcAng2.push_back(0.0059563322487913659);
  abcAng2.push_back(0.00050048260885665553);
  abcAng2.push_back(-0.0035838749526626921);
  abcAng3.push_back(0.0057982287753588907);
  abcAng3.push_back(-0.0099666850359987867);
  abcAng3.push_back(-0.0073237560434568881);
  rot2.SetPolynomial(abcAng1, abcAng2, abcAng3,
                     Isis::SpiceRotation::PolyFunctionOverSpice);
  cout << "Source = " << rot2.GetSource() << endl;
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot2.SetEphemerisTime(t);
    vector<double> CJ = rot2.Matrix();
    cout << "Time           = " << rot2.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if(rot2.HasAngularVelocity()) {
      std::vector<double> av = rot2.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;



  // Test polynomial over Cache conversion to reduced cache
  cout << "Test fitting polynomial function over cache to new cache" << endl;

  // Get new cache using existing cache and polynomial
  Isis::Table tab2 = rot2.Cache("Outputcache");
  Isis::SpiceRotation rot3(-94031);


  // Load tab2 into the object
  rot3.LoadCache(tab2);

  cout << "Source = " << rot3.GetSource() << endl;

  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot3.SetEphemerisTime(t);
    vector<double> CJ = rot3.Matrix();
    cout << "Time           = " << rot3.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if(rot3.HasAngularVelocity()) {
      std::vector<double> av = rot3.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Testing ToReferencePartial method
  cout << "Testing ToReferencePartial method" << endl;
  std::vector<double> angles = rot.Angles(3, 1, 3);
  cout << "For angles (ra,dec,twist) = " << angles[0] << " " << angles[1] << " " << angles[2]
       << endl;
  std::vector<double> lookC;
  lookC.push_back(0.);
  lookC.push_back(0.);
  lookC.push_back(1.);
  std::vector<double> lookJ = rot.J2000Vector(lookC);
  cout << " For lookJ = " << lookJ[0] << " " << lookJ[1] << " " << lookJ[2] << endl;
  std::vector<double> dAraLookC(3);
  dAraLookC = rot.ToReferencePartial(lookJ, Isis::SpiceRotation::WRT_RightAscension, 0);
  // Take care of round-off problem.  Look for a better way
  if(abs(dAraLookC[2]) < .00000000001) dAraLookC[2] = 0.;
  cout << "Right ascension partial on A applied to lookJ =:  " << dAraLookC[0] << " "
       << dAraLookC[1] << " " << dAraLookC[2] << endl;

  std::vector<double> dBraLookC(3);
  dBraLookC = rot.ToReferencePartial(lookJ, Isis::SpiceRotation::WRT_RightAscension, 1);
  if(abs(dBraLookC[2]) < .00000000001) dBraLookC[2] = 0.;
  cout << "Right ascension partial on B applied to lookJ =:  " << dBraLookC[0] << " "
       << dBraLookC[1] << " " << dBraLookC[2] << endl;

  std::vector<double> dCraLookC(3);
  dCraLookC = rot.ToReferencePartial(lookJ, Isis::SpiceRotation::WRT_RightAscension, 2);
  if(abs(dCraLookC[2]) < .00000000001) dCraLookC[2] = 0.;
  cout << "Right ascension partial on C applied to lookJ =:  " << dCraLookC[0] << " "
       << dCraLookC[1] << " " << dCraLookC[2] << endl;

  std::vector<double> dAdecLookC(3);
  dAdecLookC = rot.ToReferencePartial(lookJ, Isis::SpiceRotation::WRT_Declination, 0);
  if(abs(dAdecLookC[2]) < .00000000001) dAdecLookC[2] = 0.;
  cout << "Declination partial on A applied to lookJ =:  " << dAdecLookC[0] << " "
       << dAdecLookC[1] << " " << dAdecLookC[2] << endl << endl;
  std::vector<double> dAtwLookC;

  dAtwLookC = rot.ToReferencePartial(lookJ, Isis::SpiceRotation::WRT_Twist, 0);
  for(int i = 0; i < 3; i++) {
    if(abs(dAtwLookC[i]) < .00000000000001) dAtwLookC[i] = 0.;
  }
  cout << "Twist partial on A applied to lookJ =:  " << dAtwLookC[0] << " "
       << dAtwLookC[1] << " " << dAtwLookC[2] << endl << endl;


  cout << "Testing with setting functions ... " << endl;
  Isis::Table tab1 = rot.Cache("Test");
  Isis::SpiceRotation rot4(-94031);
  Isis::SpiceRotation::Source source = Isis::SpiceRotation::Spice;
//   rot4.SetSource(source);
//   rot4.LoadCache(startTime, endTime, 10);
  rot4.LoadCache(tab1);
//   rot4.SetPolynomial(abcAng1, abcAng2, abcAng3);
  source = rot4.GetSource();
  cout << "Source = " << source << endl;
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot4.SetEphemerisTime(t);
    vector<double> CJ = rot4.Matrix();
    cout << "Time           = " << rot4.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if(rot4.HasAngularVelocity()) {
      std::vector<double> av = rot4.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Test LineCache method
  cout << "Testing line cache..." << endl;
  Isis::Table tab4 = rot4.LineCache("Test5");
  Isis::SpiceRotation rot5(-94031);
  rot5.LoadCache(tab4);

  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot5.SetEphemerisTime(t);
    vector<double> CJ = rot5.Matrix();
    cout << "Time           = " << rot5.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if(rot5.HasAngularVelocity()) {
      std::vector<double> av = rot5.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Test table options
  cout << "Testing tables ... " << endl;
  Isis::Table tab3 = rot.Cache("Test");
  Isis::SpiceRotation rot6(-94031);
  rot6.LoadCache(tab3);
  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot6.SetEphemerisTime(t);
    vector<double> CJ = rot6.Matrix();
    cout << "Time           = " << rot6.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
//    cout << "Spacecraft (J) = " << p[0] << " " << p[1] << " " << p[2] << endl;

    if (rot6.HasAngularVelocity()) {
      std::vector<double> av = rot6.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

// Test J2000 and Reference vector methods
  cout << "Testing vector methods" << endl;
  rot6.SetEphemerisTime(startTime);
  std::vector<double> v(3);
  v[0] = 0.;
  v[1] = 0.;
  v[2] = 1.;
  std::vector<double> vout = rot6.J2000Vector(v);
  std::cout << "v = " << v[0] << " " << v[1] << " " << v[2] << endl;
  v = rot6.ReferenceVector(vout);

  // Take care of Solaris round-off problem.  Look for a better way
  if(abs(v[0]) < .00000000000000012) v[0] = 0.;
  if(abs(v[1]) < .00000000000000012) v[1] = 0.;
  std::cout << "v = " << v[0] << " " << v[1] << " " << v[2] << endl;



  // Testing linear Function
  cout << "Testing with linear function ... " << endl;
  Isis::SpiceRotation linrot(-94031);
  linrot.LoadCache(startTime, endTime, 2);
  linrot.SetEphemerisTime(startTime);
  linrot.SetEphemerisTime(endTime);

  linrot.SetPolynomial();
  linrot.GetPolynomial(abcAng1, abcAng2, abcAng3);

  cout << "Source = " << linrot.GetSource() << endl;

  for(int i = 0; i < 2; i++) {
    double t = startTime + (double) i * (endTime - startTime);
    linrot.SetEphemerisTime(t);
    vector<double> CJ = linrot.Matrix();
    cout << "Time           = " << linrot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }
  cout << endl;


// Test Nadir option
  cout << "Testing Nadir rotation ... " << endl;
  Isis::SpiceRotation naRot(-94031, 499);

  for(int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    naRot.SetEphemerisTime(t);
    std::vector<double> CJ = naRot.Matrix();

    cout << "Time           = " << naRot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }
  cout << endl;

  // Test angle wrap routine
  double newangle = naRot.WrapAngle(0.5235987756, 4.188790205);
  cout << "Testing angle wrapping..." << endl;
  cout << "   Using anchor angle of 30, 240 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, -0.1745329252);
  cout << "   Using anchor angle of 30, -10 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, -3.141592654);
  cout << "   Using anchor angle of 30, -180 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, 1.570796327);
  cout << "   Using anchor angle of 30, 90 changes to " << newangle * 180. / pi_c() << endl;


}
