#include <iostream>
#include <iomanip>
#include "iException.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "Projection.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  void doit (Isis::Pvl &lab);
  void doit2 (Isis::Pvl &lab);

  try {
  cout << "Unit test for Isis::ProjectionFactory" << endl;
  
  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += Isis::PvlKeyword("EquatorialRadius",3396190.0);
  mapGroup += Isis::PvlKeyword("PolarRadius",3376200.0);
  
  mapGroup += Isis::PvlKeyword("LatitudeType","Planetographic");
  mapGroup += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup += Isis::PvlKeyword("LongitudeDomain",360);

  mapGroup += Isis::PvlKeyword("ProjectionName","SimpleCylindrical");
  mapGroup += Isis::PvlKeyword("CenterLongitude",220.0);

  cout << "Test for missing pixel resolution ... " << endl;
  doit(lab);
  doit2(lab);
  
  mapGroup += Isis::PvlKeyword("PixelResolution",2000.0);
  cout << "Test for missing upper left X ... " << endl;
  doit(lab);

  mapGroup += Isis::PvlKeyword("UpperLeftCornerX",-18000.0);
  cout << "Test for missing upper left Y ... " << endl;
  doit(lab);
  
  mapGroup += Isis::PvlKeyword("UpperLeftCornerY",2062000.0);

  cout << "Testing conversion from image to ground ... " << endl;
  Isis::Projection *proj = Isis::ProjectionFactory::CreateFromCube(lab);
  proj->SetWorld(245.0,355.0);
  cout << setprecision(14);
  cout << "Latitude:  " << proj->Latitude() << endl;
  cout << "Longitude: " << proj->Longitude() << endl;
  cout << endl;
  
  cout << "Testing conversion from ground to image ... " << endl;
  proj->SetGround(22.84279897788801,227.9291842833142);
  cout << "Sample:    " << proj->WorldX() << endl;
  cout << "Line:      " << proj->WorldY() << endl;
  cout << endl;

  cout << "Testing missing ground range on create method ... " << endl;
  doit2(lab);

  mapGroup += Isis::PvlKeyword("MinimumLatitude",10.8920539924144);
  mapGroup += Isis::PvlKeyword("MaximumLatitude",34.7603960060206);
  mapGroup += Isis::PvlKeyword("MinimumLongitude",219.72432466275);
  mapGroup += Isis::PvlKeyword("MaximumLongitude",236.186050244411);
  mapGroup.DeleteKeyword("UpperLeftCornerX");
  mapGroup.DeleteKeyword("UpperLeftCornerY");

  cout << "Testing create method ... " << endl;
  int lines, samples;
  proj = Isis::ProjectionFactory::CreateForCube(lab,samples, lines);
  cout << "Lines:       " << lines << endl;
  cout << "Samples:     " << samples << endl;
  cout << "UpperLeftX:  " << (double) mapGroup["UpperLeftCornerX"] << endl;
  cout << "UpperLeftY:  " << (double) mapGroup["UpperLeftCornerY"] << endl;
  cout << endl;

  cout << "Testing create method with existing cube labels" << endl;
  mapGroup.AddKeyword(Isis::PvlKeyword("UpperLeftCornerX",-16000.0),Isis::Pvl::Replace);
  mapGroup.AddKeyword(Isis::PvlKeyword("UpperLeftCornerY",2060000.0),Isis::Pvl::Replace);

  Isis::Pvl lab2;
  Isis::PvlObject icube("IsisCube");
  Isis::PvlObject core("Core");
  Isis::PvlGroup dims("Dimensions");
  dims += Isis::PvlKeyword("Lines",400);
  dims += Isis::PvlKeyword("Samples",600);
  core.AddGroup(dims);
  icube.AddObject(core);
  icube.AddGroup(mapGroup);
  lab2.AddObject(icube);

  proj = Isis::ProjectionFactory::CreateForCube(lab2, samples, lines);
  cout << "Lines:       " << lines << endl;
  cout << "Samples:     " << samples << endl;
  mapGroup = lab2.FindGroup("Mapping",Isis::Pvl::Traverse);
  cout << "UpperLeftX:  " << (double) mapGroup["UpperLeftCornerX"] << endl;
  cout << "UpperLeftY:  " << (double) mapGroup["UpperLeftCornerY"] << endl;
  cout << endl;

  cout << "Label results" << endl;
  cout << lab2 << endl;
  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

void doit(Isis::Pvl &lab) {
  try {
    Isis::ProjectionFactory::CreateFromCube(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
}

void doit2(Isis::Pvl &lab) {
  try {
    int lines, samples;
    Isis::ProjectionFactory::CreateForCube(lab, samples, lines);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
}


