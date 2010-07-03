#include <iostream>
#include <iomanip>
#include "iException.h"
#include "PolarStereographic.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit Test For PolarStereographic" << endl << endl;
  cout << std::setprecision(14);

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mg = lab.FindGroup("Mapping");
  mg += Isis::PvlKeyword("EquatorialRadius",6378388.0);
  mg += Isis::PvlKeyword("PolarRadius",6356911.9);
  mg += Isis::PvlKeyword("LatitudeType","Planetographic");
  mg += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mg += Isis::PvlKeyword("LongitudeDomain",180);
  mg += Isis::PvlKeyword("MinimumLatitude",-90.0);
  mg += Isis::PvlKeyword("MaximumLatitude",0.0);
  mg += Isis::PvlKeyword("MinimumLongitude",-180.0);
  mg += Isis::PvlKeyword("MaximumLongitude",180.0);
  mg += Isis::PvlKeyword("ProjectionName","PolarStereographic");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Isis::PolarStereographic p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
  mg += Isis::PvlKeyword("CenterLongitude",-100.0);

  cout << "Test missing center latitude keyword ..." << endl;
  try {
    Isis::PolarStereographic p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
  cout << "Test invalid center latitude keyword ..." << endl;
  mg += Isis::PvlKeyword("CenterLatitude",0.0);
  try {
    Isis::PolarStereographic p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
  mg.AddKeyword(Isis::PvlKeyword("CenterLatitude",-71.0),Isis::PvlGroup::Replace);

  try {
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);
  //  Isis::PolarStereographic p(lab);
  
    cout << "Test SetGround method ... " << endl;
    cout << "Setting ground to (-75,150)" << endl;
    p.SetGround(-75.0,150.0);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
  
    
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (-1540033.620970689,-560526.3978025292)" << endl;
    p.SetCoordinate(-1540033.620970689,-560526.3978025292);
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
  
    cout << "Test TrueScaleLatitude method... " << endl;
    cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
    cout << endl;
  
    Isis::Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;
  
    cout << "Test default computation ... " << endl;
    mg.DeleteKeyword("CenterLongitude");
    mg.DeleteKeyword("CenterLatitude");
    Isis::PolarStereographic p2(lab,true); 
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
    cout << "  Pages 315-319" << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}
