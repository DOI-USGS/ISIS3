#include <iomanip>
#include <iostream>

#include "IException.h"
#include "Orthographic.h"
#include "Preference.h"
#include "ProjectionFactory.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR Orthographic" << endl << endl;

  Pvl lab;
  lab.addGroup(PvlGroup("Mapping"));
  PvlGroup &mapGroup = lab.findGroup("Mapping");
  mapGroup += PvlKeyword("EquatorialRadius", "1.0");
  mapGroup += PvlKeyword("PolarRadius", "1.0");
  mapGroup += PvlKeyword("LatitudeType", "Planetographic");
  mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGroup += PvlKeyword("LongitudeDomain", "180");
  mapGroup += PvlKeyword("MinimumLatitude", "-70.0");
  mapGroup += PvlKeyword("MaximumLatitude", "70.0");
  mapGroup += PvlKeyword("MinimumLongitude", "-180.0");
  mapGroup += PvlKeyword("MaximumLongitude", "180.0");
  mapGroup += PvlKeyword("ProjectionName", "Orthographic");

  cout << "Test missing center longitude keyword ..." << endl;
  try {
    Orthographic p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLongitude", "-100.0");

  cout << "Test missing center latitude keyword..." << endl;
  try {
    Orthographic p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterLatitude", "40.0");

  try {
    Projection &p = *ProjectionFactory::Create(lab);

    cout << "Test TrueScaleLatitude method... " << endl;
    cout << "TrueScaleLatitude = " << p.TrueScaleLatitude() << endl;
    cout << endl;

    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(9);
    cout << "Setting ground to (30,-110)" << endl;
    p.SetGround(30.0, -110.0);
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
    mapGroup.deleteKeyword("CenterLongitude");
    mapGroup.deleteKeyword("CenterLatitude");
    Orthographic p2(lab, true);
    cout << lab << endl;
    cout << endl;

    cout << "Testing Mapping() methods ... " << endl;

    Pvl tmp1;
    Pvl tmp2;
    Pvl tmp3;
    tmp1.addGroup(p.Mapping());
    tmp2.addGroup(p.MappingLatitudes());
    tmp3.addGroup(p.MappingLongitudes());

    cout << "Mapping() = " << endl;
    cout << tmp1 << endl;
    cout << "MappingLatitudes() = " << endl;
    cout << tmp2 << endl;
    cout << "MappingLongitudes() = " << endl;
    cout << tmp3 << endl;
    cout << endl;

    mapGroup.findKeyword("MinimumLatitude").setValue("-60.0");
    mapGroup.findKeyword("MaximumLatitude").setValue("60.0");
    mapGroup.findKeyword("MinimumLongitude").setValue("-80.0");
    mapGroup.findKeyword("MaximumLongitude").setValue("80.0");

    mapGroup.findKeyword("CenterLatitude").setValue("40.0");
    mapGroup.findKeyword("CenterLongitude").setValue("0.0");

    cout << "Test XYRange method (not boundary conditions)" << endl;
    Projection &m = *ProjectionFactory::Create(lab);
    minX = 0;
    maxX = 0;
    minY = 0;
    maxY = 0;
    m.XYRange(minX, maxX, minY, maxY);
    cout << "Minimum X:  " << minX << endl;
    cout << "Maximum X:  " << maxX << endl;
    cout << "Minimum Y:  " << minY << endl;
    cout << "Maximum Y:  " << maxY << endl;
    cout << endl;

    mapGroup.findKeyword("MinimumLatitude").setValue("-90.0");
    mapGroup.findKeyword("MaximumLatitude").setValue("-88.0");
    mapGroup.findKeyword("MinimumLongitude").setValue("0.0");
    mapGroup.findKeyword("MaximumLongitude").setValue("360.0");

    mapGroup.findKeyword("CenterLatitude").setValue("-90.0");
    mapGroup.findKeyword("CenterLongitude").setValue("0.0");

    cout << "Test polar projection (clat = -90)" << endl;
    Projection &pole = *ProjectionFactory::Create(lab);
    minX = 0;
    maxX = 0;
    minY = 0;
    maxY = 0;
    pole.XYRange(minX, maxX, minY, maxY);
    cout << "Successful" << endl;
    cout << "X Range:  [" << minX << ", " << maxX << "]" << endl;
    cout << "Y Range:  [" << minY << ", " << maxY << "]" << endl;
    cout << endl;

    cout << "Unit test was obtained from:" << endl << endl;
    cout << "  Map Projections - A Working Manual" << endl;
    cout << "  USGS Professional Paper 1395 by John P. Snyder" << endl;
    cout << "  Pages 311-312" << endl;

  }
  catch(IException &e) {
    e.print();
  }
}
