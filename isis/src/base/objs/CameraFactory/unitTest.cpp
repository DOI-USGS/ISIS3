#include <iostream>
#include "IException.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "FileName.h"
#include "Pvl.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

void doit(Cube &cube);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr << "Unit test for CameraFactory" << endl;
  cerr << "Testing missing Instrument Group ..." << endl;
  Cube dummyCube("$base/testData/isisTruth.cub", "r");
  Pvl &lab = *dummyCube.label();
  doit(dummyCube);

  lab.addGroup(PvlGroup("Kernels"));

  cerr << "Testing missing spacecraft name ..." << endl;
  lab.addGroup(PvlGroup("Instrument"));
  doit(dummyCube);

  cerr << "Testing missing instrument id ..." << endl;
  PvlGroup &inst = lab.findGroup("Instrument");
  inst += PvlKeyword("SpacecraftName", "Bogus Spacecraft");
  doit(dummyCube);

  cerr << "Testing unsupported camera mode ..." << endl;
  inst += PvlKeyword("InstrumentId", "Bogus Instrument");
  doit(dummyCube);
}

void doit(Cube &cube) {
  try {
    cerr << "Version: ";
    cerr << CameraFactory::CameraVersion(cube) << endl;
  }
  catch(IException &error) {
    IString errorStr = error.toString();
    errorStr = errorStr.ToQt().replace(QRegExp(" in file.*"), ".");
    cerr << errorStr << endl;
  }

  cerr << endl;

  try {
    CameraFactory::Create(cube);
  }
  catch(IException &error) {
    IString errorStr = error.toString();
    errorStr = errorStr.ToQt().replace(QRegExp(" in file.*"), ".");
    cerr << errorStr << endl;
  }

  cerr << endl;
}



