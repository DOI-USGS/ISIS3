#include <iostream>
#include <iomanip>
#include "iException.h"
#include "SimpleCylindrical.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR SimpleCylindrical" << endl << endl;

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGrp = lab.FindGroup("Mapping");
  mapGrp += Isis::PvlKeyword("EquatorialRadius",1.0);
  mapGrp += Isis::PvlKeyword("PolarRadius",1.0);
  mapGrp += Isis::PvlKeyword("LatitudeType","Planetocentric");
  mapGrp += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGrp += Isis::PvlKeyword("LongitudeDomain",180);
  mapGrp += Isis::PvlKeyword("MinimumLatitude",-90.0);
  mapGrp += Isis::PvlKeyword("MaximumLatitude",90.0);
  mapGrp += Isis::PvlKeyword("MinimumLongitude",-180.0);
  mapGrp += Isis::PvlKeyword("MaximumLongitude",180.0);
  mapGrp += Isis::PvlKeyword("ProjectionName","SimpleCylindrical");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Isis::SimpleCylindrical p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  try {
    mapGrp += Isis::PvlKeyword("CenterLongitude",-90.0);
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);
  
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
  
    Isis::Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;
  
    cout << "Testing default option ... " << endl;
    mapGrp.DeleteKeyword("CenterLongitude");
    Isis::SimpleCylindrical p2(lab,true);
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
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}



