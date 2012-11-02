#include <iostream>
#include <iomanip>
#include "IException.h"
#include "Planar.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR Planar Projection" << endl << endl;

  Pvl lab;
  lab.AddGroup(PvlGroup("Mapping"));
  PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += PvlKeyword("ProjectionName", "Planar");
  mapGroup += PvlKeyword("TargetName", "Saturn");
  //mapGroup += PvlKeyword("EquatorialRadius", 1.0);
  //mapGroup += PvlKeyword("PolarRadius", 1.0);
  mapGroup += PvlKeyword("LatitudeType", "Planetocentric");
  mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGroup += PvlKeyword("LongitudeDomain", 180);
  mapGroup += PvlKeyword("MinimumRadius", 0.0);
  mapGroup += PvlKeyword("MaximumRadius", 2000000.0);
  mapGroup += PvlKeyword("MinimumLongitude", -20.0);
  mapGroup += PvlKeyword("MaximumLongitude", 130.0);

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Planar p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLongitude", 0.0);

  cout << "Test missing center radius  keyword ..." << endl;
  try {
    Planar p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterRadius", 200000.0);

  Projection &p = *ProjectionFactory::Create(lab);

  // SetGround(const double radius, const double lon)
  cout << "Test SetGround method ... " << endl;
  cout << std::setprecision(16);
  cout << "Setting ground to (1000.0,45.0)" << endl;
  p.SetGround(1000.0, 45.0);
  cout << "Latitude:               " << p.Latitude() << endl;
  cout << "Longitude:              " << p.Longitude() << endl;
  cout << "XCoord:                 " << p.XCoord() << endl;
  cout << "YCoord:                 " << p.YCoord() << endl;
  cout << endl;

  cout << "Test SetCoordinate method ... " << endl;
  cout << "Setting coordinate to (0.2617993877991494,-0.8726646259971648)" << endl;
  p.SetCoordinate(0.2617993877991494, -0.8726646259971648);
  cout << "Latitude:               " << p.Latitude() << endl;
  cout << "Longitude:              " << p.Longitude() << endl;
  cout << "XCoord:                 " << p.XCoord() << endl;
  cout << "YCoord:                 " << p.YCoord() << endl;
  cout << endl;

/*
  // XYRange method not implemented yet. Do we need it?
  cout << "Test XYRange method ... " << endl;
  double minX, maxX, minY, maxY;
  p.XYRange(minX, maxX, minY, maxY);
  cout << "Minimum X:  " << minX << endl;
  cout << "Maximum X:  " << maxX << endl;
  cout << "Minimum Y:  " << minY << endl;
  cout << "Maximum Y:  " << maxY << endl;
  cout << endl;

  // TrueScaleLatitude  method not implemented yet. Do we need it?
  cout << "Test TrueScaleLatitude method..." << endl;
  cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
  cout << endl;
*/

  Projection *s = &p;
  cout << "Test Name and comparision methods ... " << endl;
  cout << "Name:       " << s->Name() << endl;
  cout << "operator==  " << (*s == *s) << endl;
  cout << endl;

  cout << "Testing default option ... " << endl;
  mapGroup.DeleteKeyword("CenterLongitude");
  mapGroup.DeleteKeyword("CenterRadius");
  Planar p2(lab, true);
  cout << lab << endl;
  cout << endl;

  cout << "Testing Mapping() methods ... " << endl;

  Pvl tmp1;
  //Pvl tmp2;
  //Pvl tmp3;
  tmp1.AddGroup(p.Mapping());
  //tmp2.AddGroup(p.MappingRadii());
  //tmp3.AddGroup(p.MappingLongitudes());

  cout << "Mapping() = " << endl;
  cout << tmp1 << endl;
 // cout << "MappingRadii() = " << endl;
 // cout << tmp2 << endl;
  //cout << "MappingLongitudes() = " << endl;
  //cout << tmp3 << endl;
  //cout << endl;
/*
  std::cout << "Check Invalid Radius" << std::endl;
  mapGroup.AddKeyword(PvlKeyword("CenterLatitude", 90.0), Pvl::Replace);
  std::cout << mapGroup << std::endl;
  try {
    Equirectangular p2(lab);
  }
  catch(IException &e) {
    e.print();
  }
*/  
}



