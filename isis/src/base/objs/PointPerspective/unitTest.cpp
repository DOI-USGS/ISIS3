#include <iostream>
#include <iomanip>
#include "PointPerspective.h"
#include "IException.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR PointPerspective" << endl << endl;

  Pvl lab;
  lab.AddGroup(PvlGroup("Mapping"));
  PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += PvlKeyword("EquatorialRadius", toString(6371000.));
  mapGroup += PvlKeyword("PolarRadius", toString(6371000.));
  mapGroup += PvlKeyword("LatitudeType", "Planetographic");
  mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGroup += PvlKeyword("LongitudeDomain", toString(180));
  mapGroup += PvlKeyword("MinimumLatitude", toString(-90.0));
  mapGroup += PvlKeyword("MaximumLatitude", toString(90.0));
  mapGroup += PvlKeyword("MinimumLongitude", toString(-180.0));
  mapGroup += PvlKeyword("MaximumLongitude", toString(180.0));
  mapGroup += PvlKeyword("ProjectionName", "PointPerspective");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    PointPerspective p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLongitude", toString(-77.0));

  cout << "Test missing center latitude keyword..." << endl;
  try {
    PointPerspective p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLatitude", toString(39.0));

  cout << "Test missing distance keyword..." << endl;
  try {
    PointPerspective p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("Distance", toString(500000.0));

  try {
    Projection &p = *ProjectionFactory::Create(lab);
    //  PointPerspective p(lab);

    cout << "Test TrueScaleLatitude method... " << endl;
    cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
    cout << endl;

    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (41,-74)" << endl;
    p.SetGround(41.0, -74.0);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;


    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (251640.079, 226487.551)" << endl;
    p.SetCoordinate(251640.079, 226487.551);
    cout << "Latitude:               " << p.Latitude() << endl;
    cout << "Longitude:              " << p.Longitude() << endl;
    cout << "XCoord:                 " << p.XCoord() << endl;
    cout << "YCoord:                 " << p.YCoord() << endl;
    cout << endl;

    cout << "Test XYRange method ... " << endl;
    double minX, maxX, minY, maxY;
    p.XYRange(minX, maxX, minY, maxY);
    cout << "Minimum X:  " << minX << endl;
    cout << "Maximum X:  " << maxX << endl;
    cout << "Minimum Y:  " << minY << endl;
    cout << "Maximum Y:  " << maxY << endl;
    cout << endl;

    Projection *s = &p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;

    cout << "Test default computation ... " << endl;
    mapGroup.DeleteKeyword("CenterLongitude");
    mapGroup.DeleteKeyword("CenterLatitude");
    PointPerspective p2(lab, true);
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

    cout << "Unit test was obtained from:" << endl << endl;
    cout << "  Map Projections - A Working Manual" << endl;
    cout << "  USGS Professional Paper 1395 by John P. Snyder" << endl;
    cout << "  Pages 320-321" << endl;
  }
  catch(IException &e) {
    e.print();
  }
}
