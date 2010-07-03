#include <iostream>
#include <iomanip>
#include <cmath>
#include "iException.h"
#include "ObliqueCylindrical.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR ObliqueCylindrical" << endl << endl;

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGrp = lab.FindGroup("Mapping");

  mapGrp += Isis::PvlKeyword("EquatorialRadius",2575000.0);
  mapGrp += Isis::PvlKeyword("PolarRadius",2575000.0);

  mapGrp += Isis::PvlKeyword("PoleLatitude", 22.858149);
  mapGrp += Isis::PvlKeyword("PoleLongitude", 297.158602);
  
  mapGrp += Isis::PvlKeyword("LatitudeType","Planetocentric");
  mapGrp += Isis::PvlKeyword("LongitudeDirection","PositiveWest");
  mapGrp += Isis::PvlKeyword("LongitudeDomain",360);
  mapGrp += Isis::PvlKeyword("ProjectionName","ObliqueCylindrical");

  mapGrp += Isis::PvlKeyword("MinimumLatitude",-90);
  mapGrp += Isis::PvlKeyword("MaximumLatitude",0.92523);
  mapGrp += Isis::PvlKeyword("MinimumLongitude",-0.8235);
  mapGrp += Isis::PvlKeyword("MaximumLongitude",180.5);

  cout << "Test missing pole rotation keyword ..." << endl;
  try {
    Isis::ObliqueCylindrical p(lab);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  // Add the missing keyword "PoleRotation"
  mapGrp += Isis::PvlKeyword("PoleRotation", 45.7832);

  // testing operator ==
   cout << "Testing operator == ..." << endl;
  try {
    Isis::ObliqueCylindrical p1(lab);
		Isis::ObliqueCylindrical p2(lab);
		bool flag = (p1 == p2);
		if (flag) {
			cout << "(p1==p2) = True" << endl;
		}
		else {
			cout << "*** Error ****"<< endl;
		}
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  try {    
  
    Isis::Projection &p = *Isis::ProjectionFactory::Create(lab);

    cout << "Test X,Y,Z Axis Vector Calculations ... " << endl;
    cout << "Map Group Data (X[0]):      " << (double)mapGrp["XAxisVector"][0] << endl;
    cout << "Map Group Data (X[1]):      " << (double)mapGrp["XAxisVector"][1] << endl;
    cout << "Map Group Data (X[2]):      " << (double)mapGrp["XAxisVector"][2] << endl;
    cout << "Map Group Data (Y[0]):      " << (double)mapGrp["YAxisVector"][0] << endl;
    cout << "Map Group Data (Y[1]):      " << (double)mapGrp["YAxisVector"][1] << endl;
    cout << "Map Group Data (Y[2]):      " << (double)mapGrp["YAxisVector"][2] << endl;
    cout << "Map Group Data (Z[0]):      " << (double)mapGrp["ZAxisVector"][0] << endl;
    cout << "Map Group Data (Z[1]):      " << (double)mapGrp["ZAxisVector"][1] << endl;
    cout << "Map Group Data (Z[2]):      " << (double)mapGrp["ZAxisVector"][2] << endl;
    cout << endl;

    const double X = -2646.237039, Y = -537.814519;
    cout << setprecision(13);
    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (" << X << "," << Y << ")" << endl;
    p.SetCoordinate(X,Y);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;

    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(10);
    cout << "Setting ground to (" << p.Latitude() << "," << p.Longitude() << ")" << endl;
    p.SetGround(p.Latitude(),p.Longitude());
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;
    
    cout << "Test XYRange method ... " << endl;
    cout << std::setprecision(8);
    double minX = 0.0,maxX = 1.0,minY = 2.0,maxY = 3.0;
    p.XYRange(minX,maxX,minY,maxY);
    cout << "\n\nMinimum X:  " << minX << endl;
    cout << "Maximum X:  " << maxX << endl;
    cout << "Minimum Y:  " << minY << endl;
    cout << "Maximum Y:  " << maxY << endl;
    cout << endl;
  
    Isis::Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;

    mapGrp["PoleRotation"] = 43.8423;
    Isis::ObliqueCylindrical different(lab);
    cout << "Test Name and comparision method with differing data... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (different == *s) << endl;
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



