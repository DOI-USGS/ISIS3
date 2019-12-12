#include <iostream>
#include <iomanip>
#include <cmath>

#include <nlohmann/json.hpp>

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "SpiceRotation.h"
#include "Table.h"

// Declarations for bindings for Naif Spicelib routines that do not have
// a wrapper
extern int bodeul_(integer *body, doublereal *et, doublereal *ra,
                   doublereal *dec, doublereal *w, doublereal *lamda);

using json = nlohmann::json;
using namespace std;
using namespace Isis;

//TODO test loadPCFromSpice() and loadPCFromTable() methods
//TODO see end of unit test for exceptions that need to be tested

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << setprecision(8);
  cout << "Unit test for SpiceRotation" << endl;

  // Test case is taken from moc red wide angle image ab102401
  // sn = MGS/561812335:32/MOC-WA/RED
  // Load kernels to allow testing of various SpiceRotation sources
  FileName f("$base/testData/kernels");
  QString dir = f.expanded() + "/";
  QString naif (dir + "naif0007.tls");
  QString mgs(dir + "MGS_SCLKSCET.00045.tsc");
  QString mocti(dir + "moc13.ti");
  QString mocbc(dir + "moc.bc");
  QString mocbsp(dir + "moc.bsp");
  QString de(dir + "de405.bsp");
  QString pck(dir + "../../kernels/pck/pck00009.tpc");
  QString cgFK(dir + "ROS_V29.TF");
  QString cgCK(dir + "CATT_DV_145_02_______00216.BC");
  //QString mocadd(dir+"mocAddendum.ti");
  QString mocspice(dir + "mocSpiceRotationUnitTest.ti");
  furnsh_c(naif.toLatin1().data());
  furnsh_c(mgs.toLatin1().data());
  furnsh_c(mocti.toLatin1().data());
  furnsh_c(mocbc.toLatin1().data());
  furnsh_c(mocbsp.toLatin1().data());
  furnsh_c(de.toLatin1().data());
  furnsh_c(pck.toLatin1().data());
  furnsh_c(mocspice.toLatin1().data());
  furnsh_c(cgFK.toLatin1().data());
  furnsh_c(cgCK.toLatin1().data());

  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  SpiceInt code;
  namfrm_c("MGS_MOC", &code);
//  namfrm_c ("IAU_MARS", &code);
  cout << "Naif code = " << code << endl;


  SpiceRotation rot(-94031); // MGS_MOC

  // Normal testing of SetEphemerisTime ie. source=SPICE(no cache)
  cout << "Testing without cache (from SPICE)... " << endl;
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot.SetEphemerisTime(t);
    vector<double> CJ = rot.Matrix();

    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot.HasAngularVelocity()) {
      vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

  // Testing with cache
  cout << "Testing with cache ... " << endl;
  rot.LoadCache(startTime, endTime, 10);
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot.SetEphemerisTime(t);
    vector<double> CJ = rot.Matrix();
    cout << "Time           = " << rot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot.HasAngularVelocity()) {
      vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

  // Save off cache for polynomial over SPICE test
  Table tab = rot.Cache("TestPolyOver");

  // Testing with Functions
  cout << "Testing with functions ... " << endl;
  vector<double> abcAng1, abcAng2, abcAng3;
  rot.SetPolynomial();
  rot.GetPolynomial(abcAng1, abcAng2, abcAng3);
  cout << "Source = " << rot.GetSource() << endl;

  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot.SetEphemerisTime(t);
    vector<double> CJ = rot.Matrix();
    cout << "Time           = " << rot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot.HasAngularVelocity()) {
      vector<double> av = rot.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Testing polynomial over Spice
  cout << "Testing with polynomial functions over Spice ... " << endl;
  SpiceRotation rot2(-94031);
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
  abcAng3.push_back(-0.009966680359987867);
  abcAng3.push_back(-0.0073237560434568881);
  rot2.SetPolynomial(abcAng1, abcAng2, abcAng3, SpiceRotation::PolyFunctionOverSpice);
  cout << "Source = " << rot2.GetSource() << endl;

  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot2.SetEphemerisTime(t);
    vector<double> CJ = rot2.Matrix();
    cout << "Time           = " << rot2.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot2.HasAngularVelocity()) {
      vector<double> av = rot2.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

  // Test polynomial over Cache conversion to reduced cache
  cout << "Test fitting polynomial function over cache to new cache" << endl;

  // Get new cache using existing cache and polynomial
  Table tab2 = rot2.Cache("Outputcache");
  SpiceRotation rot3(-94031);


  // Load tab2 into the object
  rot3.LoadCache(tab2);

  cout << "Source = " << rot3.GetSource() << endl;

  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot3.SetEphemerisTime(t);
    vector<double> CJ = rot3.Matrix();
    cout << "Time           = " << rot3.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot3.HasAngularVelocity()) {
      vector<double> av = rot3.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Testing ToReferencePartial method
  cout << "Testing ToReferencePartial method" << endl;
  vector<double> angles = rot.Angles(3, 1, 3);
  cout << "For angles (ra,dec,twist) = " << angles[0] << " " << angles[1] << " " << angles[2]
       << endl;
  vector<double> lookC;
  lookC.push_back(0.);
  lookC.push_back(0.);
  lookC.push_back(1.);
  vector<double> lookJ = rot.J2000Vector(lookC);
  // Save a J2000 vector for testing target body partial methods later.
  vector<double> testLookJ(lookJ);
  cout << " For lookJ = " << lookJ[0] << " " << lookJ[1] << " " << lookJ[2] << endl;
  vector<double> dAraLookC(3);
  dAraLookC = rot.ToReferencePartial(lookJ, SpiceRotation::WRT_RightAscension, 0);
  // Take care of round-off problem.  Look for a better way
  if (abs(dAraLookC[2]) < .00000000001) dAraLookC[2] = 0.;
  cout << "Right ascension partial on A applied to lookJ =:  " << dAraLookC[0] << " "
       << dAraLookC[1] << " " << dAraLookC[2] << endl;

  vector<double> dBraLookC(3);
  dBraLookC = rot.ToReferencePartial(lookJ, SpiceRotation::WRT_RightAscension, 1);
  if (abs(dBraLookC[2]) < .00000000001) dBraLookC[2] = 0.;
  cout << "Right ascension partial on B applied to lookJ =:  " << dBraLookC[0] << " "
       << dBraLookC[1] << " " << dBraLookC[2] << endl;

  vector<double> dCraLookC(3);
  dCraLookC = rot.ToReferencePartial(lookJ, SpiceRotation::WRT_RightAscension, 2);
  if (abs(dCraLookC[2]) < .00000000001) dCraLookC[2] = 0.;
  cout << "Right ascension partial on C applied to lookJ =:  " << dCraLookC[0] << " "
       << dCraLookC[1] << " " << dCraLookC[2] << endl;

  vector<double> dAdecLookC(3);
  dAdecLookC = rot.ToReferencePartial(lookJ, SpiceRotation::WRT_Declination, 0);
  if (abs(dAdecLookC[2]) < .00000000001) dAdecLookC[2] = 0.;
  cout << "Declination partial on A applied to lookJ =:  " << dAdecLookC[0] << " "
       << dAdecLookC[1] << " " << dAdecLookC[2] << endl << endl;

  vector<double> dAtwLookC;
  dAtwLookC = rot.ToReferencePartial(lookJ, SpiceRotation::WRT_Twist, 0);
  for (int i = 0; i < 3; i++) {
    if (abs(dAtwLookC[i]) < .00000000000001) dAtwLookC[i] = 0.;
  }
  cout << "Twist partial on A applied to lookJ =:  " << dAtwLookC[0] << " "
       << dAtwLookC[1] << " " << dAtwLookC[2] << endl << endl;

  cout << "Testing with setting functions ... " << endl;
  Table tab1 = rot.Cache("Test");
  SpiceRotation rot4(-94031);
  SpiceRotation::Source source = SpiceRotation::Spice;
//   rot4.SetSource(source);
//   rot4.LoadCache(startTime, endTime, 10);
  rot4.LoadCache(tab1);
//   rot4.SetPolynomial(abcAng1, abcAng2, abcAng3);
  source = rot4.GetSource();
  cout << "Source = " << source << endl;
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot4.SetEphemerisTime(t);
    vector<double> CJ = rot4.Matrix();
    cout << "Time           = " << rot4.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot4.HasAngularVelocity()) {
      vector<double> av = rot4.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Test LineCache method
  cout << "Testing line cache..." << endl;
  Table tab4 = rot4.LineCache("Test5");
  SpiceRotation rot5(-94031);
  rot5.LoadCache(tab4);

  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot5.SetEphemerisTime(t);
    vector<double> CJ = rot5.Matrix();
    cout << "Time           = " << rot5.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;

    if (rot5.HasAngularVelocity()) {
      vector<double> av = rot5.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;


  // Test table options
  cout << "Testing tables ... " << endl;
  Table tab3 = rot.Cache("Test");
  SpiceRotation rot6(-94031);
  rot6.LoadCache(tab3);
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    rot6.SetEphemerisTime(t);
    vector<double> CJ = rot6.Matrix();
    cout << "Time           = " << rot6.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
//    cout << "Spacecraft (J) = " << p[0] << " " << p[1] << " " << p[2] << endl;

    if (rot6.HasAngularVelocity()) {
      vector<double> av = rot6.AngularVelocity();
      cout << "av(" << i << ") = " << av[0] << " " << av[1] << " " << av[2] << endl;
    }
  }
  cout << endl;

// Test J2000 and Reference vector methods
  cout << "Testing vector methods" << endl;
  rot6.SetEphemerisTime(startTime);
  vector<double> v(3);
  v[0] = 0.;
  v[1] = 0.;
  v[2] = 1.;
  vector<double> vout = rot6.J2000Vector(v);
  cout << "v = " << v[0] << " " << v[1] << " " << v[2] << endl;
  v = rot6.ReferenceVector(vout);

  // Take care of Solaris round-off problem.  Look for a better way
  if (abs(v[0]) < .00000000000000012) v[0] = 0.;
  if (abs(v[1]) < .00000000000000012) v[1] = 0.;
  cout << "v = " << v[0] << " " << v[1] << " " << v[2] << endl;



  // Testing linear Function
  cout << "Testing with linear function ... " << endl;
  SpiceRotation linrot(-94031);
  linrot.LoadCache(startTime, endTime, 2);
  linrot.SetEphemerisTime(startTime);
  linrot.SetEphemerisTime(endTime);

  linrot.SetPolynomial();
  linrot.GetPolynomial(abcAng1, abcAng2, abcAng3);

  cout << "Source = " << linrot.GetSource() << endl;

  for (int i = 0; i < 2; i++) {
    double t = startTime + (double) i * (endTime - startTime);
    linrot.SetEphemerisTime(t);
    vector<double> CJ = linrot.Matrix();
    cout << "Time           = " << linrot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }
  cout << endl;


// Test Nadir source option
  cout << "Testing Nadir rotation ... " << endl;
  SpiceRotation naRot(-94031, 499);

  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    naRot.SetEphemerisTime(t);
    vector<double> CJ = naRot.Matrix();

    cout << "Time           = " << naRot.EphemerisTime() << endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }
  cout << endl;

  // Test angle wrap method
  double newangle = naRot.WrapAngle(0.5235987756, 4.188790205);
  cout << "Testing angle wrapping..." << endl;
  cout << "   Using anchor angle of 30, 240 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, -0.1745329252);
  cout << "   Using anchor angle of 30, -10 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, -3.141592654);
  cout << "   Using anchor angle of 30, -180 changes to " << newangle * 180. / pi_c() << endl;
  newangle = naRot.WrapAngle(0.5235987756, 1.570796327);
  cout << "   Using anchor angle of 30, 90 changes to " << newangle * 180. / pi_c() << endl <<endl << endl;


  cout << "Begin tests for PCK data..." << endl << endl;
  cout << "Test LoadPCFromSpice and all the coefficient accessors..." << endl;
  // Use Mars for testing PCK constants to compute target orientation with cache sizes > 1
  // Use Galileo Io image with product id = 21I0165 for testing nutation/precession terms.  Mars has none.
  //  tet = -15839262.24291
  // body frame code for Io = 10023
  // Use Europa for exercising the code using nutation/precession terms.  Mars has none.
  SpiceRotation targrot1(10014);   //Frame code for Mars
  // SpiceRotation targrotV1(10024);   //Frame code for Europa
  // targrotV1.LoadCache(-646009153.46723, -646009153.46723, 1); // This calls LoadPcFromSpice for Europa
  SpiceRotation targrotV1(10023);   //Frame code for Io
  targrotV1.LoadCache(-15839262.24291, -15839262.24291, 1); // This calls LoadPcFromSpice for Io
  targrot1.LoadCache(startTime, endTime, 2); // This calls LoadPcFromSpice for Mars
  cout << "Test CacheLabel for PCK data..." << endl;
  Table pcktab = targrot1.Cache("Planetary constants test table"); // This calls CacheLabel
  Table pcktabV = targrotV1.Cache("Planetary constants test table"); // This calls CacheLabel
  SpiceRotation targrot(10014);  // Mars
  // SpiceRotation targrotV(10024);  // Europa  --  The results for pm will differ slightly from TrigBasis because of the older PCK
  SpiceRotation targrotV(10023);  // Io  --
  cout << "Test LoadPCFromTable..." << endl;
  targrot.LoadCache(pcktab);  // This calls LoadPcFromTable
  targrotV.LoadCache(pcktabV);  // This calls LoadPcFromTable
  // Now get the values
  vector<Angle> poleRa = targrotV.poleRaCoefs();
  vector<Angle> poleDec = targrotV.poleDecCoefs();
  vector<Angle> prMer = targrotV.pmCoefs();
  vector<double> raNutPrec = targrotV.poleRaNutPrecCoefs();
  vector<double> decNutPrec = targrotV.poleDecNutPrecCoefs();
  vector<double> pmNutPrec = targrotV.pmNutPrecCoefs();
  vector<Angle> sysNutPrec0 = targrotV.sysNutPrecConstants();
  vector<Angle> sysNutPrec1 = targrotV.sysNutPrecCoefs();
  cout << "Io Pole RA coefficients = " << poleRa[0].degrees() << "," << poleRa[1].degrees() << ","
       << poleRa[2].degrees() << endl;
  cout << "Io Pole DEC coefficients = " << poleDec[0].degrees() << "," << poleDec[1].degrees()
       << "," << poleDec[2].degrees() << endl;
  cout << "Io PM coefficients = " << prMer[0].degrees() << "," << prMer[1].degrees() << ","
       << prMer[2].degrees() << endl;
  int numcoef = (int) sysNutPrec0.size();

  if (raNutPrec.size() > 0) {
    cout << "Io Pole RA Nutation/Precession coefficients = ";
    for (int ic = 0; ic < numcoef; ic++)  cout << raNutPrec[ic] << ",";
    cout << endl;
  }
  if (decNutPrec.size() > 0) {
  cout << "Io Pole DEC Nutation/Precession coefficients = ";
    for (int ic = 0; ic < numcoef; ic++)  cout << decNutPrec[ic] << ",";
    cout << endl;
  }
  if (pmNutPrec.size() > 0) {
  cout << "Io PM Nutation/Precession coefficients = ";
  for (int ic = 0; ic < numcoef; ic++)  cout << pmNutPrec[ic] << ",";
    cout << endl;
  }
  if (sysNutPrec0.size() > 0) {
  cout << "Io System Nutation/Precession constants = ";
  for (int ic = 0; ic < numcoef; ic++)  cout << sysNutPrec0[ic].degrees() << ",";
    cout << endl;
  }
  if (sysNutPrec0.size() > 0) {
  cout << "Io System Nutation/Precession coefficients = ";
  for (int ic = 0; ic < numcoef; ic++)  cout << sysNutPrec1[ic].degrees() << ",";
    cout << endl;
  }

  // Test SetPckPolynomial methods
  cout << endl << "Testing with PCK polynomial ... " << endl;

  SpiceInt ibod;
  doublereal tet, tra, tdec, tomega, tlambda;

  // Only save one set
  // For testing Europa with the nutation/precession terms and a cache size of 1
  //  tet = -646009153.46723; // time et for Europa
  //  ibod = 502; // Europa
  // bodeul_(&ibod, &tet, &tra,&tdec, &tomega, &tlambda);
  // targrotV.SetEphemerisTime(tet);
  // vector<double> pckanglesV = targrotV.Angles(3, 1, 3);
  // cout << "    Angles = " << pckanglesV[0]*dpr_c() <<","<< pckanglesV[1]*dpr_c() <<","
  //      << pckanglesV[2]*dpr_c() <<endl <<endl;
  // end Europa test

  // For testing Io with the nutation/precession terms and a cache size of 1
  tet = -15839262.24291;  // time et for Io
  ibod = 501; // Io
  bodeul_(&ibod, &tet, &tra,&tdec, &tomega, &tlambda);
  targrotV.SetEphemerisTime(tet);
  vector<double> pckanglesV = targrotV.Angles(3, 1, 3);
  cout << "Io    Angles = " << pckanglesV[0]*dpr_c() <<","<< pckanglesV[1]*dpr_c() <<","
       << pckanglesV[2]*dpr_c() <<endl <<endl;
  // end Io test

  // For testing Mars with more than one value in the cache
  cout << endl << "  Mars original SPICE values for target body orientation unadjusted"
       << endl;
  cout << "  Source = " << targrot.GetSource() << endl;
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    targrot.SetEphemerisTime(t);
    vector<double> CJ = targrot.Matrix();
    //temp debug lines to be removed...
    //end temp debug lines   Note: uncomment the next 4 lines
    cout << "    Time           = " << targrot.EphemerisTime() << endl;
    // vector<double> pckangles = targrot.Angles(3, 1, 3);
    // cout << "    Angles = " << pckangles[0]*dpr_c() <<","<< pckangles[1]*dpr_c() <<","<< pckangles[2]*dpr_c() <<endl;
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }

  cout << endl << endl << "Now PCK polynomial values for angles unadjusted ..." << endl;
  targrotV.usePckPolynomial();
  targrot.usePckPolynomial();
  cout << "  Io PCK polynomial output" << endl;
  targrotV.SetEphemerisTime(0.0);
  targrotV.SetEphemerisTime(-15839262.24291);
  pckanglesV.clear();
  cout << "  Source = " << targrotV.GetSource() << endl;
  pckanglesV = targrotV.Angles(3, 1, 3);
  cout << "    Angles = " << pckanglesV[0]*dpr_c() <<","<< pckanglesV[1]*dpr_c() <<","
       << pckanglesV[2]*dpr_c() << endl << endl;

   cout << "  Mars PCK polynomial output" << endl;
   cout << "  Source = " << targrot.GetSource() << endl;
  // For testing Mars with more than one value in the cache
  for (int i = 0; i < 10; i++) {
    double t = startTime + (double) i * slope;
    targrot.SetEphemerisTime(t);
    vector<double> CJ = targrot.Matrix();
    //temp debug lines to be removed...
    // vector<double> pckangles = targrot.Angles(3, 1, 3);
    cout << "    Time           = " << targrot.EphemerisTime() << endl;

    // cout << "    Angles = " << pckangles[0]*dpr_c() <<","<< pckangles[1]*dpr_c() <<","<< pckangles[2]*dpr_c() <<endl;
    //    cout << "    Angles = " << pckangles[0]*dpr_c() + 90. <<","<< 90. - pckangles[1]*dpr_c() <<","<< pckangles[2]*dpr_c() <<endl;
    //end temp debug lines   Note: uncomment the next 4 lines
    cout << "CJ(" << i << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "         " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "         " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }

    // Test angular velocities
  cout << endl << endl << "Testing angular velocity with Io data ..." << endl;
  if (targrotV.HasAngularVelocity()) {
    vector<double> av = targrotV.AngularVelocity();
    cout << "SpiceRotation av = " << av[0] << " " << av[1] << " " << av[2] << endl;
    SpiceDouble tsipm[6][6];
    sxform_c ( "J2000", "IAU_IO", -15839262.24291, tsipm);
    // sxform_c ( "J2000", "IAU_EUROPA", -646009153.46723, tsipm);
    SpiceDouble tipm[3][3];
    vector<SpiceDouble> nav(3,0.);
    xf2rav_c (tsipm, tipm, &(nav[0]) );
    cout << "J2000 to body-fixed Naif av = " << nav[0] << " " << nav[1] << " " << nav[2] << endl;
  }
  cout << endl;

  cout << endl << endl << "Testing partials for target body parameters..." << endl;
  targrot.SetEphemerisTime(startTime);
  cout << "For angles (ra,dec,rotation) = " << angles[0] << " " << angles[1] << " " << angles[2]
       << endl;
  //  vector<double> dLookB(3);
  cout << "Beginning with J2000 vector " << testLookJ[0] << " " << testLookJ[1] << " " << testLookJ[2] << endl;
  vector<double> lookB = targrot.ReferenceVector(testLookJ);
  cout << "lookB = " << lookB[0] << " " << lookB[1] << " " << lookB[2] << endl;

  vector<double> dLookB = targrot.ToReferencePartial(testLookJ, SpiceRotation::WRT_RightAscension, 0);
  cout << endl << " dLookB with respect to ra = " << dLookB[0] << " " << dLookB[1] << " " << dLookB[2] << endl;
  vector<double> matchLookJ(3);
  matchLookJ = targrot.toJ2000Partial(dLookB, SpiceRotation::WRT_RightAscension, 0);
  cout << "  Right ascension partial on A applied to dlookB =:  " << matchLookJ[0] << " "
       << matchLookJ[1] << " " << matchLookJ[2] << endl;

  dLookB = targrot.ToReferencePartial(testLookJ, SpiceRotation::WRT_Declination, 0);
  cout << endl << " dLookB with respect to dec = " << dLookB[0] << " " << dLookB[1] << " "
       << dLookB[2] << endl;
  matchLookJ = targrot.toJ2000Partial(dLookB, SpiceRotation::WRT_Declination, 0);
  cout << "  Declination partial on A applied to dlookB =:  " << matchLookJ[0] << " "
       << matchLookJ[1] << " " << matchLookJ[2] << endl;

  dLookB = targrot.ToReferencePartial(testLookJ, SpiceRotation::WRT_Twist, 1);
  cout << endl << " dLookB with respect to rotation rate = " << dLookB[0] << " " <<
          dLookB[1] << " " << dLookB[2] << endl;
  //If I apply toJ2000Partial to dLookB, I get back lookJ(x,y,0) with roundoff  -- 05-12-2015 DAC
  matchLookJ = targrot.toJ2000Partial(dLookB, SpiceRotation::WRT_Twist, 1);
  cout << "  Rotation rate partial on A applied to dlookB =:  " << matchLookJ[0] << " "
       << matchLookJ[1] << " " << matchLookJ[2] << endl;

  dLookB = targrot.ToReferencePartial(testLookJ, SpiceRotation::WRT_Twist, 0);
  cout << endl << " dLookB with respect to rotation = " << dLookB[0] << " " <<
          dLookB[1] << " " << dLookB[2] << endl;
  //If I apply toJ2000Partial to dLookB, I get back lookJ(x,y,0) with roundoff  -- 05-12-2015 DAC
  matchLookJ = targrot.toJ2000Partial(dLookB, SpiceRotation::WRT_Twist, 0);
  cout << "  Rotation partial on A applied to dlookB =:  " << matchLookJ[0] << " "
       << matchLookJ[1] << " " << matchLookJ[2] << endl;

  cout << endl << endl << "... Testing failure of body rotation with binary PCK" << endl;
  FileName fb("$base/kernels/");
  QString dirb = fb.expanded() + "/";
  QString bpck(dirb + "pck/lunar_de403_1950-2199_pa.bpc");
  QString fk(dirb + "fk/lunarMeanEarth001.tf");
  furnsh_c(bpck.toLatin1().data());
  furnsh_c(fk.toLatin1().data());
  SpiceRotation targrotbin(310001);   //Frame code for Moon
  cout << " Source = " << targrotbin.GetSource() << endl;
  targrotbin.LoadCache(startTime, startTime, 1); // This calls LoadPcFromSpice
  SpiceRotation::FrameType frameType = targrotbin.getFrameType();

  if (frameType == SpiceRotation::BPC)
    cout << "Frame type is binary PCK and cannot be updated" << endl;


  cout << "End of PCK testing" << endl;

  // Test CK based body rotation
  cout << endl << endl << "Testing CK based body rotation with 67P/Churyumov–Gerasimenko data ..." << endl;

  SpiceRotation cgRotation(-1000012000);
  // Test time from Rosetta OSIRIS NAC image n20140901t144253568id30f22
  double cgTestTime = 462854709.88606;
  cgRotation.SetEphemerisTime(cgTestTime);
  vector<double> cgCJ = cgRotation.Matrix();
  cout << "Time = " << cgRotation.EphemerisTime() << endl;
  cout << "CJ = " << cgCJ[0] << " " << cgCJ[1] << " " << cgCJ[2] << endl;
  cout << "     " << cgCJ[3] << " " << cgCJ[4] << " " << cgCJ[5] << endl;
  cout << "     " << cgCJ[6] << " " << cgCJ[7] << " " << cgCJ[8] << endl;

  // Test loading cache from ALE ISD with only time dependent quaternions
  cout << endl << endl << "Testing loading cache from ALE ISD with only time dependent quaternions ..." << endl;
  SpiceRotation aleQuatRot(-94031);
  // Test Rotations are 'xyz' euler angle rotations:
  // [0, 0, 0],
  // [-90, 0, 0],
  // [-90, 180, 0],
  // [-90, 180, 90]
  json aleQuatIsd = {{"CkTableStartTime"    , 0.0},
                     {"CkTableEndTime"      , 3.0},
                     {"CkTableOriginalSize" , 4},
                     {"EphemerisTimes"      , {0.0, 1.0, 2.0, 3.0}},
                     {"TimeDependentFrames" , {-94031, 10014, 1}},
                     {"Quaternions"         , {{0.0, 0.0, 0.0, 1.0},
                                               {-1.0 / sqrt(2), 0.0, 0.0, 1.0 / sqrt(2)},
                                               {0.0, 1.0 / sqrt(2), 1.0 / sqrt(2), 0.0},
                                               {-0.5, -0.5, 0.5, 0.5}}}};
  aleQuatRot.LoadCache(aleQuatIsd);
  cout << "Frame type = " << aleQuatRot.getFrameType() << endl;
  cout << "Is cached? " << (aleQuatRot.IsCached() ? "Yes" : "No") << endl;
  cout << "Has AV? " << (aleQuatRot.HasAngularVelocity() ? "Yes" : "No") << endl;
  vector<int> timeDepChain = aleQuatRot.TimeFrameChain();
  cout << "Time dependent frame chain = { ";
  for (int i = 0; i < timeDepChain.size(); i++) {
    if (i > 0) {
      cout << ", ";
    }
    cout << timeDepChain[i];
  }
  cout << " }" << endl;
  vector<int> constChain = aleQuatRot.ConstantFrameChain();
  cout << "Time dependent frame chain = { ";
  for (int i = 0; i < constChain.size(); i++) {
    if (i > 0) {
      cout << ", ";
    }
    cout << constChain[i];
  }
  cout << " }" << endl;
  for (int t = 0; t <= 3; t++) {
    aleQuatRot.SetEphemerisTime(t);
    vector<double> CJ = aleQuatRot.Matrix();
    cout << "Time = " << aleQuatRot.EphemerisTime() << endl;
    cout << "CJ(" << t << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "        " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "        " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }

  // Test loading cache from ALE ISD with time dependent quaternions and AV
  cout << endl << endl << "Testing loading cache from ALE ISD with time dependent quaternions and AV ..." << endl;
  SpiceRotation aleQuatAVRot(-94031);
  json aleQuatAVIsd(aleQuatIsd);
  aleQuatAVIsd["AngularVelocity"] = {{-Isis::PI / 2, 0.0, 0.0},
                                     {0.0, Isis::PI, 0.0},
                                     {0.0, 0.0, Isis::PI / 2},
                                     {0.0, 0.0, Isis::PI / 2}};
  aleQuatAVRot.LoadCache(aleQuatAVIsd);
  cout << "Has AV? " << (aleQuatAVRot.HasAngularVelocity() ? "Yes" : "No") << endl;

  // Test loading cache from ALE ISD with time dependent quaternions and constant rotation
  cout << endl << endl << "Testing loading cache from ALE ISD with time dependent quaternions and constant rotation ..." << endl;
  SpiceRotation aleQuatConstRot(-94031);
  json aleQuatConstIsd(aleQuatIsd);
  aleQuatConstIsd["TimeDependentFrames"] = {-94030, 10014, 1};
  aleQuatConstIsd["ConstantFrames"] = {-94031, -94030};
  aleQuatConstIsd["ConstantRotation"] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0};
  aleQuatConstRot.LoadCache(aleQuatConstIsd);
  timeDepChain = aleQuatConstRot.TimeFrameChain();
  cout << "Time dependent frame chain = { ";
  for (int i = 0; i < timeDepChain.size(); i++) {
    if (i > 0) {
      cout << ", ";
    }
    cout << timeDepChain[i];
  }
  cout << " }" << endl;
  constChain = aleQuatConstRot.ConstantFrameChain();
  cout << "Time dependent frame chain = { ";
  for (int i = 0; i < constChain.size(); i++) {
    if (i > 0) {
      cout << ", ";
    }
    cout << constChain[i];
  }
  cout << " }" << endl;
  for (int t = 0; t <= 3; t++) {
    aleQuatConstRot.SetEphemerisTime(t);
    vector<double> CJ = aleQuatConstRot.Matrix();
    cout << "Time = " << aleQuatConstRot.EphemerisTime() << endl;
    cout << "CJ(" << t << ") = " << CJ[0] << " " << CJ[1] << " " << CJ[2] << endl;
    cout << "        " << CJ[3] << " " << CJ[4] << " " << CJ[5] << endl;
    cout << "        " << CJ[6] << " " << CJ[7] << " " << CJ[8] << endl;
  }

  //Test exceptions
  cout << endl << endl << "Testing exceptions..." << endl;
  SpiceRotation testRot(-94031); // MGS_MOC


  // SpiceRotation(frameCode, targetCode)
  //     "Cannot find [key] in text kernels
  try {
    cout << endl;
    SpiceRotation sr(-99999, 499); // will not have a INS<framecode>_TRANSX key
  }
  catch (IException &e) {
    e.print();
  }

  // LoadCache(startTime, endTime, size)
  //     "Argument cacheSize must not be less or equal to zero"
  try {
    cout << endl;
    testRot.LoadCache(10, 20, -1);
  }
  catch (IException &e) {
    e.print();
  }

  //     "Argument startTime must be less than or equal to endTime"
  try {
    cout << endl;
    testRot.LoadCache(20, 10, 1);
  }
  catch (IException &e) {
    e.print();
  }

  //     "Cache size must be more than 1 if startTime and endTime differ"
  try {
    cout << endl;
    testRot.LoadCache(10, 20, 1);
  }
  catch (IException &e) {
    e.print();
  }

  //     "A SpiceRotation cache has already been created"
  try {
    cout << endl;
    testRot.LoadCache(startTime, endTime, 2);
    testRot.LoadCache(startTime, endTime - 1, 2);
  }
  catch (IException &e) {
    e.print();
  }

  // ReloadCache()
  //      "The SpiceRotation has not yet been fit to a function"
  try {
    cout << endl;
    testRot.ReloadCache();
  }
  catch (IException &e) {
    e.print();
  }

  // LineCache(tableName)
  //     "Only cached rotations can be returned as a line cache of quaternions and time"
  try {
    cout << endl;
    SpiceRotation sr(-94031);
    sr.LineCache("TableTest");
  }
  catch (IException &e) {
    e.print();
  }

  // Cache(tableName)
  //     "To create table source of data must be either Memcache or PolyFunction"
  try {
    cout << endl;
    SpiceRotation sr(-94031);
    sr.Cache("TableTest");
  }
  catch (IException &e) {
    e.print();
  }

  // toJ2000Partial()
  //TODO test its 3 exceptions

  // usePckPolynomial()
  //     "Target body orientation information not available.  Rerun spiceinit."
  try {
    cout << endl;
    testRot.usePckPolynomial();
  }
  catch (IException &e) {
    e.print();
  }

  // DPolynomial(coeffIndex)
  //     "Unable to evaluate the derivative of the SPCIE rotation fit
  //      polynomial for the given coefficient index. Index is negative
  //      or exceeds degree of polynomial"
  try {
    cout << endl;
    testRot.DPolynomial(-1);
  }
  catch (IException &e) {
    e.print();
  }

  // DPckPolynomial(partialVar, coeffIndex)
  //     "Unable to evaluate the derivative of the SPCIE rotation fit
  //      polynomial for the given coefficient index. Index is negative
  //      or exceeds degree of polynomial"
  try {
    cout << endl;
    testRot.DPckPolynomial(SpiceRotation::WRT_Twist, 100);
  }
  catch (IException &e) {
    e.print();
  }

  // ToReferencePartial(lookJ, partialVar, coeffIndex)
  //     "Only CK and PCK partials can be calculated"
  //TODO (need to use a frame type that isn't UNKNOWN, CK, or PCK (try BPC?)


  // SetAxes(axis1, axis2, axis3)
  //     "A rotation axis is outside the valid range of 1 to 3"
  try {
    cout << endl;
    testRot.SetAxes(0,2,3);
  }
  catch (IException &e) {
    e.print();
  }

  // LoadTimeCache()
  //TODO test its 3 exceptions

  // GetFullCacheTime()
  //    "Time cache not availabe -- rerun spiceinit"
  try {
    cout << endl;
    SpiceRotation sr(-94031);
    sr.GetFullCacheTime();
  }
  catch (IException &e) {
    e.print();
  }

  // FrameTrace()
  //TODO test its 3 exceptions

  // ComputeAv()
  //     "The SpiceRotation pointing angles must be fit to polynomials in order to
  //      compute angular velocity."
  try {
    cout << endl;
    testRot.ComputeAv();
  }
  catch (IException &e) {
    e.print();
  }

  // LoadCache(json)
  //     "SpiceRotation::LoadCache(json) only support Spice source
  try {
    cout << endl;
    json errorTestIsd = {"Invalid"};
    linrot.LoadCache(errorTestIsd);
  }
  catch (IException &e) {
    e.print();
  }

  //TODO
  //     "Planetary angular velocity must be fit computed with PCK polynomials "

  // SetEphemerisTimeSpice()
  //TODO test its 3 exceptions
}
