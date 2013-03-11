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

  /**
   * @file
   *
   * This application tests the Target class.
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   *   @history
   */

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

  // This group should be included in the Target test
  PvlGroup inst1("Instrument");
  inst1 += PvlKeyword("TargetName", "Mars");
  PvlGroup inst2("Instrument");
  inst2 += PvlKeyword("TargetName", "Sky");
  PvlGroup inst3("Instrument");
  inst3 += PvlKeyword("TargetName", "Mard");

  // These are not part of the Target class funtionality except to test missing body code
  PvlGroup kern1("Kernels");
  FileName f("$base/testData/kernels");
  FileName f2("$base/dems");
  QString dir = f.expanded() + "/";
  QString dir2 = f2.expanded() + "/";
  kern1 += PvlKeyword("NaifFrameCode", toString(-94031));
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
  kern2 += PvlKeyword("NaifIkCode", toString(-94031));
  kern2 += PvlKeyword("LeapSecond", dir + "naif0007.tls");
  kern2 += PvlKeyword("SpacecraftClock", dir + "MGS_SCLKSCET.00045.tsc");
  kern2 += PvlKeyword("TargetPosition", dir + "de405.bsp");
  kern2 += PvlKeyword("TargetAttitudeShape", dir + "pck00006.tpc");
  kern2 += PvlKeyword("Instrument", dir + "mocSpiceUnitTest.ti");
  kern2 += PvlKeyword("InstrumentAddendum", dir + "mocAddendum.ti");
  kern2 += PvlKeyword("InstrumentPosition", dir + "moc.bsp");
  kern2 += PvlKeyword("InstrumentPointing", dir + "moc.bc");
  kern2 += PvlKeyword("Frame", "");
  kern2 += PvlKeyword("NaifBodyCode", toString(499));

  PvlGroup kern3 = kern2;
  kern3 += PvlKeyword("ShapeModel", dir2  + "molaMarsPlanetaryRadius0005.cub");

  // Time Setup
  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  kern1 += PvlKeyword("StartPadding", toString(slope));
  kern1 += PvlKeyword("EndPadding", toString(slope));

  Pvl lab1;
  lab1.addGroup(inst1);
  lab1.addGroup(kern1);

  // Create a Spice object to test radii
  Spice spi(lab1);

  // Test good target
  Target tGood(&spi, lab1);
  cout << endl;
  cout << "  Good target test..." << endl;
  cout << "     NaifBodyCode = " << tGood.naifBodyCode() << endl;
  cout << "     TargetName = " << tGood.name() << endl;
  cout << "     IsSky = " << tGood.isSky() << endl;

  // Use a Spice object to test radii
  vector<Distance> r(3);
  r = spi.target()->radii();
  cout << "     Target radii = " << r[0].kilometers() << "/" << r[1].kilometers() << "/" << r[2].kilometers();
  cout << endl;

  // Test Sky
  Pvl lab2;
  lab2.addGroup(inst2);
  lab2.addGroup(kern1);
  Target tSky(&spi, lab2);
  cout << endl;
  cout << "  Testing Sky..." << endl;
  cout << "     IsSky = " << tSky.isSky() << endl;
  r = tSky.radii();
  cout << "     Sky Target radii = " << r[0].kilometers() << "/" << r[1].kilometers() << "/" << r[2].kilometers() << endl;
  cout << "     NaifBodyCode = " << tSky.naifBodyCode() << endl;

  // Test Sky with NaifSpkCode override
  PvlGroup kern4 = kern1;
  kern4 += PvlKeyword("NaifSpkCode", "-93");
  Pvl lab3;
  lab3.addGroup(inst2);
  lab3.addGroup(kern4);
  Target tSky2(&spi, lab3);
  cout << endl;
  cout << "  Testing Sky with NaifSpkCode..." << endl;
  cout << "     IsSky = " << tSky.isSky() << endl;
  cout << "     NaifBodyCode = " << tSky2.naifBodyCode() << endl;
  r = tSky2.radii();
  cout << "     Sky Target radii = " << r[0].kilometers() << "/" << r[1].kilometers() << "/" << r[2].kilometers() << endl;

  Pvl lab4;
  // Test without instrument group
  try {
    cout << endl;
    cout << "  Testing no instrument group ..." << endl;
    lab4.addGroup(kern2);
    Target tNoInstrument(&spi, lab4);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }
  
  Pvl lab5;
  lab4.addGroup(inst3);

  // Test without kernels group
  try {
    cout << endl;
    cout << "  Testing no kernels group ..." << endl;
    Target tNoKernels(&spi, lab5);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }

  // Test bad target
  try {
    cout << endl;
    cout << "  Testing unknown target ..." << endl;
    Target tUnknownTarget(&spi, lab4);
  }
  catch(IException &e) {
    e.print();
    cout << endl;
  }

  // Test case with override of body code
  // lab5.addGroup(inst2);
  // lab5.addGroup(kern4);
  // Target tOverrideBodyCode(&spi, lab5);
  // cout << endl;
  // cout << "  Testing case with bodycode override" << endl;
  // cout << "     NaifBodyCode = " << tOverrideBodyCode.naifBodyCode() << endl;

  // Test methods setShapeEllipsoid and restoreShape
  Pvl lab6;
  lab6.addGroup(inst1);
  lab6.addGroup(kern3);
  Spice spi3(lab6);
  cout << endl << "  Testing methods setShapeEllipsoid and restoreShape..." << endl;
  cout << "    Original shape is " << spi3.target()->shape()->name() << endl;
  spi3.target()->setShapeEllipsoid();
  cout << "    Shape changed to  " << spi3.target()->shape()->name() << endl;
  spi3.target()->restoreShape();
  cout << "    Shape restored to  " << spi3.target()->shape()->name() << endl;

  //Test the default constructor
  Target defaultTarget;
  cout << endl << "  Testing default constructor..." << endl << "    Is it Sky? "  << defaultTarget.isSky() << endl;
  cout << "    Number of radii = " << defaultTarget.radii().size() << endl;

}
