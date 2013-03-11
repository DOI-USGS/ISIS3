#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "ShapeModelFactory.h"
#include "ShapeModel.h"
#include "Camera.h"
#include "Preference.h"
#include "CameraFactory.h"
#include "Target.h"

  /**
   * This application tests the ShapeModelFactory class.
   *
   * @author 2010-10-11 Debbie A. Cook
   *
   * @internal
   *   @history
   */

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::ShapeModel" << endl;

  // Test sky target
  // Build label for sky target test
  PvlGroup inst1("Instrument");
  inst1 += PvlKeyword("TargetName", "Sky");
  PvlGroup inst2("Instrument");
  inst2 += PvlKeyword("TargetName", "Mars");
  PvlGroup kern1("Kernels");
  FileName f("$base/testData/kernels");
  FileName f2("$base/dems");
  FileName f3("$mgs/testData");
  QString dir = f.expanded() + "/";
  QString dir2 = f2.expanded() + "/";
  QString dir3 = f3.expanded() + "/";
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
  kern1 += PvlKeyword("NaifBodyCode", toString(499));
  // Time Setup
  double startTime = -69382819.0;
  double endTime = -69382512.0;
  double slope = (endTime - startTime) / (10 - 1);

  kern1 += PvlKeyword("StartPadding", toString(slope));
  kern1 += PvlKeyword("EndPadding", toString(slope));

  Pvl lab1;
  lab1.addGroup(inst1);
  lab1.addGroup(kern1);

  // Test ShapeModel keyword
  cout << endl << "  Testing ShapeModel keyword..." << endl;
  PvlGroup kern2 = kern1;
  kern2 += PvlKeyword("ShapeModel", dir2  + "molaMarsPlanetaryRadius0005.cub");
  Pvl lab2;
  lab2.addGroup(inst2);
  lab2.addGroup(kern2);
  Spice spiSh(lab2);
  Target targSh(&spiSh, lab2);
  ShapeModel *smSh = ShapeModelFactory::create(&targSh, lab2);
  cout << "    Successfully created shape " << smSh->name() << endl;
  delete smSh;

  // Test ElevationModel keyword with value
  cout << endl << "  Testing ElevationModel keyword..." << endl;
  PvlGroup kern3 = kern1;
  kern3 += PvlKeyword("ElevationModel", dir2  + "molaMarsPlanetaryRadius0005.cub");
  Pvl lab3;
  lab3.addGroup(inst2);
  lab3.addGroup(kern3);
  Spice spiEl(lab3);
  Target targEl(&spiEl, lab3);
  ShapeModel *smEl = ShapeModelFactory::create(&targEl, lab3);
  cout << "    Successfully created shape " << smEl->name() << endl;
  delete smEl;

  // Test ElevationModel keyword with Null value
  cout << endl << "  Testing ElevationModel keyword Null..." << endl;
  PvlGroup kern4 = kern1;;
  kern4 += PvlKeyword("ShapeModel", "Null");
  Pvl lab4;
  lab4.addGroup(inst2);
  lab4.addGroup(kern4);
  Spice spiElNull(lab4);
  Target targElNull(&spiElNull, lab4);
  ShapeModel *smElNull = ShapeModelFactory::create(&targElNull, lab4);
  cout << "    Successfully created shape " << smElNull->name() << endl;
  delete smElNull;

  // Create Spice and Target objects for sky test
  Spice skySpi(lab1);
  Target skyTarget(&skySpi, lab1);
  ShapeModel *skyShape = ShapeModelFactory::create(&skyTarget,  lab1);
  cout << endl << "  Testing Sky target..." << endl << "    Shape model is " << skyShape->name() << endl;

  try {
  // Test ShapeModel file that does not exist
  cout << endl << "  Testing nonexistent file for shape model dem" << endl;
  PvlGroup kern5 = kern1;
  kern5 += PvlKeyword("ShapeModel", "NotAFile");
  Pvl lab5;
  lab5.addGroup(inst2);
  lab5.addGroup(kern5);
  Spice spiBadFile(lab5);
  Target targBadFile(&spiBadFile, lab5);
  ShapeModel *smBadFile = ShapeModelFactory::create(&targBadFile, lab4);
  cout << "    Successfully created shape " << smBadFile->name() << endl;
  delete smBadFile;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
  // Test ShapeModel that's not a valid Isis map projection
  cout << endl << "  Testing Isis cube file for dem that is not map projected" << endl;
  PvlGroup kern5 = kern1;
  kern5 += PvlKeyword("ShapeModel", dir3 + "ab102401.cub");
  Pvl lab5;
  lab5.addGroup(inst2);
  lab5.addGroup(kern5);
  Spice spiBadFile(lab5);
  Target targBadFile(&spiBadFile, lab5);
  ShapeModel *smBadFile = ShapeModelFactory::create(&targBadFile, lab4);
  cout << "    Successfully created shape " << smBadFile->name() << endl;
  delete smBadFile;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try {
  // Test ShapeModel dem that's not Equatorial Cylindrical 
  cout << endl << "  Testing a dem that's not equatorial cylindrical" << endl;
  PvlGroup kern5 = kern1;
  kern5 += PvlKeyword("ShapeModel", dir3 + "ab102402.lev2.cub");
  Pvl lab5;
  lab5.addGroup(inst2);
  lab5.addGroup(kern5);
  Spice spiDem(lab5);
  Target targDem(&spiDem, lab5);
  ShapeModel *smDem = ShapeModelFactory::create(&targDem, lab5);
  cout << "    Successfully created shape " << smDem->name() << endl;
  delete smDem;
  }
  catch(Isis::IException &e) {
    e.print();
  }

 // Test demshape with ShapeModel keyword
  cout << endl << "  Testing dem shape..." << endl;
  QString inputFile = "$ISIS3DATA/mgs/testData/ab102401.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.camera();
  vector<Distance> radii(3,Distance());
  radii = c->target()->radii();
  Pvl pvl = *cube.label();
  Spice spi(pvl);
  Target targ(&spi, pvl);
  targ.setRadii(radii);
  ShapeModel *sm = ShapeModelFactory::create(&targ, pvl);
  cout << "    Successfully created shape " << sm->name() << endl;
  delete sm;
  cube.close();

  // Test ellipsoid shape (ShapeModel = Null)
  cout << endl << "  Testing ellipsoid shape..." << endl;
  inputFile = "$ISIS3DATA/galileo/testData/1213r.cub";
  cube.open(inputFile);
  c = cube.camera();
  radii = c->target()->radii();
  pvl = *cube.label();
  Spice spi2(pvl);
  Target targ2(&spi2, pvl);
  targ2.setRadii(radii);
  sm = ShapeModelFactory::create(&targ2, pvl);
  cout << "    Successfully created shape " << sm->name() << endl;
  delete sm;
  cube.close();

  // Test plane shape  TBD
  // inputFile = "$ISIS3DATA/;
  // cube.open(inputFile);
  // c = cube.camera();
  // radii = c->target()->radii();
  // pvl = *cube.label();
  // Target targ2(pvl);
  // targ3.setRadii(radii);
  // sm = ShapeModelFactory::Create(&targ3, pvl);
  // cout << "Successfully created shape " << sm->name() << endl;
  // delete sm;
  // cube.close();
}
