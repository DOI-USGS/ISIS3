#include <iostream>
#include <iomanip>
#include "LambertConformal.h"
#include "iException.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR LambertConformal" << endl << endl;

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += Isis::PvlKeyword("EquatorialRadius",1.0);
  mapGroup += Isis::PvlKeyword("PolarRadius",1.0);
  mapGroup += Isis::PvlKeyword("LatitudeType","Planetographic");
  mapGroup += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup += Isis::PvlKeyword("LongitudeDomain",180);
  mapGroup += Isis::PvlKeyword("MinimumLatitude",20.0);
  mapGroup += Isis::PvlKeyword("MaximumLatitude",80.0);
  mapGroup += Isis::PvlKeyword("MinimumLongitude",-180.0);
  mapGroup += Isis::PvlKeyword("MaximumLongitude",180.0);
  mapGroup += Isis::PvlKeyword("ProjectionName","LambertConformal");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Isis::LambertConformal p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLongitude",-96.0);

  cout << "Test missing center latitude keyword..." << endl;
  try {
    Isis::LambertConformal p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLatitude", 23.0);

    cout << "Test missing first standard parallel keyword..." << endl;
  try {
    Isis::LambertConformal p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("FirstStandardParallel",33);

    cout << "Test missing second standard parallel keyword..." << endl;
  try {
    Isis::LambertConformal p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("SecondStandardParallel",45);

  try {
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);
  //  Isis::LambertConformal p(lab);
  
    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (35,-75)" << endl;
    p.SetGround(35.0,-75.0);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
  
    
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (0.29667846, 0.246211229)" << endl;
    p.SetCoordinate(0.29667846, 0.246211229);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
    p.SetCoordinate(0.0,0.0);
  
    cout << "Test XYRange method ... " << endl;
    double minX,maxX,minY,maxY;
    p.XYRange(minX,maxX,minY,maxY);
    cout << "Minimum X:  " << minX << endl;
    cout << "Maximum X:  " << maxX << endl;
    cout << "Minimum Y:  " << minY << endl;
    cout << "Maximum Y:  " << maxY << endl;
    cout << endl;
  
    Isis::Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;
  
    cout << "Test default computation ... " << endl;
    mapGroup.DeleteKeyword("CenterLongitude");
    mapGroup.DeleteKeyword("CenterLatitude");
    Isis::LambertConformal p2(lab,true); 
    cout << lab << endl;
    cout << endl;
  
    cout << "Test TrueScaleLatitude method... " << endl;
    cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
    cout << endl;

    cout << "Testing Mapping() methods ... " << endl;

    Isis::Pvl tmp1;
    Isis::Pvl tmp2;
    Isis::Pvl tmp3;
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

    cout << "Test invalid combinations of mapping parameters ..." << endl;

    mapGroup.DeleteKeyword("CenterLatitude");
    mapGroup += Isis::PvlKeyword("CenterLatitude", -90.0);
    try {
      Isis::LambertConformal p(lab);
    }
    catch (Isis::iException &e) {
      e.Report(false);
    }
    cout << endl;

    mapGroup.DeleteKeyword("CenterLatitude");
    mapGroup += Isis::PvlKeyword("CenterLatitude", 90.0);
    mapGroup.DeleteKeyword("FirstStandardParallel");
    mapGroup += Isis::PvlKeyword("FirstStandardParallel",-60);

    try {
      Isis::LambertConformal p(lab);
    }
    catch (Isis::iException &e) {
      e.Report(false);
    }
    cout << endl;





    cout << "Unit test was obtained from:" << endl << endl;
    cout << "  Map Projections - A Working Manual" << endl;
    cout << "  USGS Professional Paper 1395 by John P. Snyder" << endl;
    cout << "  Pages 295-297" << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}

