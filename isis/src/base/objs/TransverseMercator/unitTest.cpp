#include <iostream>
#include <iomanip>
#include "TransverseMercator.h"
#include "iException.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR TransverseMercator" << endl << endl;
  cout << "Part 1: Sphere..." << endl;

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += Isis::PvlKeyword("EquatorialRadius",1.0);
  mapGroup += Isis::PvlKeyword("PolarRadius",1.0);
  mapGroup += Isis::PvlKeyword("LatitudeType","Planetographic");
  mapGroup += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup += Isis::PvlKeyword("LongitudeDomain",180);
  mapGroup += Isis::PvlKeyword("MinimumLatitude",-70.0);
  mapGroup += Isis::PvlKeyword("MaximumLatitude",70.0);
  mapGroup += Isis::PvlKeyword("MinimumLongitude",-90.0);
  mapGroup += Isis::PvlKeyword("MaximumLongitude",-60.0);
  mapGroup += Isis::PvlKeyword("ProjectionName","TransverseMercator");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Isis::TransverseMercator p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLongitude",-75.0);

  cout << "Test missing center latitude keyword..." << endl;
  try {
    Isis::TransverseMercator p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLatitude", 0.0);

  cout << "Test missing scale factor keyword..." << endl;
  try {
    Isis::TransverseMercator p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("ScaleFactor", 1.0);

  try {
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);
  //  Isis::TransMercator p(lab);
  
    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (40.5,-73.5)" << endl;
    p.SetGround(40.5,-73.5);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
  
    
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (0.0199077372, 0.707027609)" << endl;
    p.SetCoordinate(0.0199077372, 0.707027609);
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
  
    Isis::Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;
  
    cout << "Test default computation ... " << endl;
    mapGroup.DeleteKeyword("CenterLongitude");
    mapGroup.DeleteKeyword("CenterLatitude");
    mapGroup.DeleteKeyword("ScaleFactor");
    Isis::TransverseMercator p2(lab,true); 
    cout << lab << endl;
    cout << endl;

    cout << "Unit test was obtained from:" << endl << endl;
    cout << "  Map Projections - A Working Manual" << endl;
    cout << "  USGS Professional Paper 1395 by John P. Snyder" << endl;
    cout << "  Pages 268-269" << endl << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl << "Part 2: Ellipsoid..." << endl;

  Isis::Pvl lab2;
  lab2.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGroup2 = lab2.FindGroup("Mapping");
  mapGroup2 += Isis::PvlKeyword("EquatorialRadius",6378206.4);
  mapGroup2 += Isis::PvlKeyword("PolarRadius",6356583.8);
  mapGroup2 += Isis::PvlKeyword("LatitudeType","Planetographic");
  mapGroup2 += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup2 += Isis::PvlKeyword("LongitudeDomain",180);
  mapGroup2 += Isis::PvlKeyword("MinimumLatitude",-70.0);
  mapGroup2 += Isis::PvlKeyword("MaximumLatitude",70.0);
  mapGroup2 += Isis::PvlKeyword("MinimumLongitude",-90.0);
  mapGroup2 += Isis::PvlKeyword("MaximumLongitude",-60.0);
  mapGroup2 += Isis::PvlKeyword("ProjectionName","TransverseMercator");
  mapGroup2 += Isis::PvlKeyword("CenterLongitude",-75.0);
  mapGroup2 += Isis::PvlKeyword("CenterLatitude", 0.0);
  mapGroup2 += Isis::PvlKeyword("ScaleFactor", 0.9996);
  cout << endl;

  try {
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab2);
  //  Isis::TransMercator p(lab);
  
    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (40.5,-73.5)" << endl;
    p.SetGround(40.5,-73.5);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
  
    
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (127106.467, 4484124.43)" << endl;
    p.SetCoordinate(127106.467, 4484124.43);
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

    cout << "Unit test was obtained from:" << endl << endl;
    cout << "  Map Projections - A Working Manual" << endl;
    cout << "  USGS Professional Paper 1395 by John P. Snyder" << endl;
    cout << "  Pages 269-270" << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}

