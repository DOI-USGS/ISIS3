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

#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Spice.h"
#include "Target.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

/**
 * UnitTest for Target class.
 *
 * @internal
 * @history 2009-03-23  Tracie Sucharski - Removed old keywords
 *          SpacecraftPosition and SpacecraftPointing with the corrected
 *          InstrumentPosition and InstrumentPointing.
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << setprecision(10);
  cout << "Unit test for Isis::Target" << endl;

  PvlGroup inst1("Instrument");
  inst1 += PvlKeyword("TargetName", "Mars");
  PvlGroup inst2("Instrument");
  inst2 += PvlKeyword("TargetName", "Sky");
  PvlGroup inst3("Instrument");
  inst3 += PvlKeyword("TargetName", "Mard");

  PvlGroup kern1("Kernels");
  FileName f("$base/testData/kernels");
  string dir = f.expanded() + "/";
  kern1 += PvlKeyword("NaifFrameCode", -94031);
  kern1 += PvlKeyword("LeapSecond", dir + "naif0007.tls");
  kern1 += PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
  kern1 += PvlKeyword("TargetPosition", dir + "de405.bsp");
  kern1 += PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
  kern1 += PvlKeyword("Instrument", dir + "mocSpiceUnitTest.ti");
  kern1 += PvlKeyword("InstrumentAddendum", dir + "mocAddendum.ti");
  kern1 += PvlKeyword("InstrumentPosition", dir + "moc.bsp");
  kern1 += PvlKeyword("InstrumentPointing", dir + "moc.bc");
  kern1 += PvlKeyword("Frame", "");

  PvlGroup kern2("Kernels");
  kern2 += PvlKeyword("NaifFrameCode", -94031);
  kern2 += PvlKeyword("LeapSecond", dir + "naif0007.tls");
  kern2 += PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
  kern2 += PvlKeyword("TargetPosition", dir + "de405.bsp");
  kern2 += PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
  kern2 += PvlKeyword("Instrument", dir + "mocSpiceUnitTest.ti");
  kern2 += PvlKeyword("InstrumentAddendum", dir + "mocAddendum.ti");
  kern2 += PvlKeyword("InstrumentPosition", dir + "moc.bsp");
  kern2 += PvlKeyword("InstrumentPointing", dir + "moc.bc");
  kern2 += PvlKeyword("Frame", "");
  kern2 += PvlKeyword("NaifBodyCode", 499);

  // Time Setup
  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  kern1 += PvlKeyword("StartPadding", slope);
  kern1 += PvlKeyword("EndPadding", slope);

  Pvl lab1;
  lab1.AddGroup(inst1);
  lab1.AddGroup(kern1);

  // Test good target
  Target tGood(lab1);
  cout << "Good target test..." << endl;
  cout << "NaifBodyCode = " << tGood.naifBodyCode() << endl;
  cout << "TargetName = " << tGood.name() << endl;
  cout << "IsSky = " << tGood.isSky() << endl;

  // Create a Spice object to test radii
  Spice spi(lab1);
  vector <Distance> r(3);
  spi.target()->radii(r);
  cout << "Target radii = " << r[0] << "/" << r[1] << "/" << r[2] << endl;

  // Test Sky
  Pvl lab2;
  lab2.AddGroup(inst2);
  lab2.AddGroup(kern1);
  Target tSky(lab2);
  cout << "Testing Sky..." << endl;
  cout << "IsSky = " << tSky->isSky() << endl;
  cout << "Target radii = " << r[0] << "/" << r[1] << "/" << r[2] << endl;

  // Create a Spice object to test setSky
  Spice spi2(lab1);
  cout << "NaifBodyCode = " spi2->target()->naifBodyCode() << endl;

  Pvl lab3;
  // Test without instrument group
  try {
    cout << "Testing no instrument group ..." << endl;
    Target tNoInstrument(lab2);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }
  
  lab3.AddGroup(inst3);

  // Test without kernels group
  try {
    cout << "Testing no kernels group ..." << endl;
    Target tNoKernels(lab3);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }

  lab.AddGroup(kern2);

  // Test bad target
  try {
    cout << "Testing unknown target ..." << endl;
    Target tUnknownTarget(lab3);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }

  // Test case with override of body code
  Pvl lab3;
  lab2.AddGroup(inst2);
  lab2.AddGroup(kern1);
  cout << "Testing case with bodycode override" << endl;
  cout << "NaifBodyCode = " spi2->target()->naifBodyCode() << endl;
}
