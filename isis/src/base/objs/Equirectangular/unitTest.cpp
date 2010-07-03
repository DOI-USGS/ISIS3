#include <iostream>
#include <iomanip>
#include "iException.h"
#include "Equirectangular.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main (int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR Equirectangular" << endl << endl;

  Pvl lab;
  lab.AddGroup(PvlGroup("Mapping"));
  PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += PvlKeyword("EquatorialRadius",1.0);
  mapGroup += PvlKeyword("PolarRadius",1.0);
  mapGroup += PvlKeyword("LatitudeType","Planetocentric");
  mapGroup += PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup += PvlKeyword("LongitudeDomain",180);
  mapGroup += PvlKeyword("MinimumLatitude",-90.0);
  mapGroup += PvlKeyword("MaximumLatitude",90.0);
  mapGroup += PvlKeyword("MinimumLongitude",-180.0);
  mapGroup += PvlKeyword("MaximumLongitude",180.0);
  mapGroup += PvlKeyword("ProjectionName","Equirectangular");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Equirectangular p(lab);
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLongitude",-90.0);

  cout << "Test missing center latitude keyword ..." << endl;
  try {
    Equirectangular p(lab);
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLatitude",0.0);

  Projection &p = *ProjectionFactory::Create(lab);

  cout << "Test SetGround method ... " << endl;
  cout << std::setprecision(16);
  cout << "Setting ground to (-50,-75)" << endl;
  p.SetGround(-50.0,-75.0);
  cout << "Latitude:               " << p.Latitude() << endl;
  cout << "Longitude:              " << p.Longitude() << endl;
  cout << "XCoord:                 " << p.XCoord() << endl;
  cout << "YCoord:                 " << p.YCoord() << endl;
  cout << endl;

  
  cout << "Test SetCoordinate method ... " << endl;
  cout << "Setting coordinate to (0.2617993877991494,-0.8726646259971648)" << endl;
  p.SetCoordinate(0.2617993877991494,-0.8726646259971648);
  cout << "Latitude:               " << p.Latitude() << endl;
  cout << "Longitude:              " << p.Longitude() << endl;
  cout << "XCoord:                 " << p.XCoord() << endl;
  cout << "YCoord:                 " << p.YCoord() << endl;
  cout << endl;
  
  cout << "Test XYRange method ... " << endl;
  double minX,maxX,minY,maxY;
  p.XYRange(minX,maxX,minY,maxY);
  cout << "Minimum X:  " << minX << endl;
  cout << "Maximum X:  " << maxX << endl;
  cout << "Minimum Y:  " << minY << endl;
  cout << "Maximum Y:  " << maxY << endl;
  cout << endl;

  cout << "Test TrueScaleLatitude method..." << endl; 
  cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
  cout << endl;

  Projection *s = &p;
  cout << "Test Name and comparision methods ... " << endl;
  cout << "Name:       " << s->Name() << endl;
  cout << "operator==  " << (*s == *s) << endl;
  cout << endl;

  cout << "Testing default option ... " << endl;
  mapGroup.DeleteKeyword("CenterLongitude");
  mapGroup.DeleteKeyword("CenterLatitude");
  Equirectangular p2(lab,true);
  cout << lab << endl;
  cout << endl;

  cout << "Testing Mapping() methods ... " << endl;

  Pvl tmp1;
  Pvl tmp2;
  Pvl tmp3;
  tmp1.AddGroup(p.Mapping());
  tmp2.AddGroup(p.MappingLatitudes());
  tmp3.AddGroup(p.MappingLongitudes());
  
  cout << "Mapping() = " << endl;
  cout << tmp1 << endl;
  cout << "MappingLatitudes() = " << endl;
  cout << tmp2 << endl;
  cout << "MappingLongitudes() = " << endl;
  cout << tmp3 << endl;
  cout << endl;

  std::cout << "Check Invalid Latitude" << std::endl;
  mapGroup.AddKeyword(PvlKeyword("CenterLatitude", 90.0), Pvl::Replace);
  std::cout << mapGroup << std::endl;
  try {
    Equirectangular p2(lab);
  }
  catch (iException &e) {
    e.Report(false);
  }
}



