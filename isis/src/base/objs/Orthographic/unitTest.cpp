#include <iostream>
#include <iomanip>
#include "Orthographic.h"
#include "iException.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR Orthographic" << endl << endl;

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
  mapGroup += Isis::PvlKeyword("MinimumLongitude",-180.0);
  mapGroup += Isis::PvlKeyword("MaximumLongitude",180.0);
  mapGroup += Isis::PvlKeyword("ProjectionName","Orthographic");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Isis::Orthographic p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLongitude",-100.0);

  cout << "Test missing center latitude keyword..." << endl;
  try {
    Isis::Orthographic p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  mapGroup += Isis::PvlKeyword("CenterLatitude", 40.0);

  try {
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);
  //  Isis::Orthographic p(lab);
  
    cout << "Test TrueScaleLatitude method... " << endl;
    cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
    cout << endl;
  
    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (30,-110)" << endl;
    p.SetGround(30.0,-110.0);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
  
    
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (-0.150383733, -0.165191103)" << endl;
    p.SetCoordinate(-0.150383733, -0.165191103);
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
    Isis::Orthographic p2(lab,true); 
    cout << lab << endl;
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
    cout << "  Pages 311-312" << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}
