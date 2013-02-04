#include <iostream>
#include <iomanip>
#include "IException.h"
#include "RingCylindrical.h"
#include "ProjectionFactory.h"
#include "Preference.h"
#include "Projection.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR RingCylindrical" << endl << endl;

  Pvl lab;
  lab.AddGroup(PvlGroup("Mapping"));
  PvlGroup &mapGrp = lab.FindGroup("Mapping");
  // mapGrp += PvlKeyword("EquatorialRadius", 1.0);
  // mapGrp += PvlKeyword("PolarRadius", 1.0);
  // mapGroup += PvlKeyword("TargetName", "Saturn");
  mapGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGrp += PvlKeyword("LongitudeDomain", "180");
  mapGrp += PvlKeyword("MinimumRadius", "0.0");
  mapGrp += PvlKeyword("MaximumRadius", "20000000.0");
  mapGrp += PvlKeyword("MinimumAzimuth", "-20.0");
  mapGrp += PvlKeyword("MaximumAzimuth", "130.0");
  mapGrp += PvlKeyword("ProjectionName", "RingCylindrical");

  cout << "Test missing center azimuth keyword ..." << endl;
  try {
    RingCylindrical p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  try {
    mapGrp += PvlKeyword("CenterAzimuth", "0.0");
    RingCylindrical *p = (RingCylindrical *) ProjectionFactory::Create(lab);

    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(16);
    cout << "Setting ground to (1000.0, 45.0)" << endl;
    p->SetGround(1000.0, 45.0);
    cout << "Radius:               " << p->Radius() << endl;
    cout << "Azimuth:              " << p->Azimuth() << endl;
    cout << "XCoord:                 " << p->XCoord() << endl;
    cout << "YCoord:                 " << p->YCoord() << endl;
    cout << endl;


    // cout << "Test SetCoordinate method ... " << endl;
    // cout << "Setting coordinate to (0.2617993877991494,-0.8726646259971648)" << endl;
    // p->SetCoordinate(0.2617993877991494, -0.8726646259971648);
    // cout << "Latitude:               " << p->Latitude() << endl;
    // cout << "Longitude:              " << p->Longitude() << endl;
    // cout << "XCoord:                 " << p->XCoord() << endl;
    // cout << "YCoord:                 " << p->YCoord() << endl;
    // cout << endl;

    cout << "Test XYRange method ... " << endl;
    double minX, maxX, minY, maxY;
    p->XYRange(minX, maxX, minY, maxY);
    cout << "Minimum X:  " << minX << endl;
    cout << "Maximum X:  " << maxX << endl;
    cout << "Minimum Y:  " << minY << endl;
    cout << "Maximum Y:  " << maxY << endl;
    cout << endl;

    RingCylindrical *s = p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;

    cout << "Testing default option ... " << endl;
    mapGrp.DeleteKeyword("CenterAzimuth");
    RingCylindrical p2(lab, true);
    cout << lab << endl;
    cout << endl;

    cout << "Testing Mapping() methods ... " << endl;

    Pvl tmp1;
    Pvl tmp2;
    Pvl tmp3;
    tmp1.AddGroup(p->Mapping());
    // tmp2.AddGroup(p->MappingRadii());
    tmp3.AddGroup(p->MappingAzimuths());

    cout << "Mapping() = " << endl;
    cout << tmp1 << endl;
    // cout << "MappingRadii() = " << endl;
    // cout << tmp2 << endl;
    cout << "MappingAzimuths() = " << endl;
    cout << tmp3 << endl;
    cout << endl;
  }
  catch(IException &e) {
    e.print();
  }
}



