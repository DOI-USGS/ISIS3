#include <iostream>
#include <iomanip>
#include "IException.h"
#include "RingCylindrical.h"
#include "ProjectionFactory.h"
#include "Preference.h"
#include "Projection.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR RingCylindrical" << endl << endl;

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGrp = lab.FindGroup("Mapping");
  // mapGrp += Isis::PvlKeyword("EquatorialRadius", 1.0);
  // mapGrp += Isis::PvlKeyword("PolarRadius", 1.0);
  // mapGroup += PvlKeyword("TargetName", "Saturn");
  mapGrp += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGrp += Isis::PvlKeyword("LongitudeDomain", 180);
  mapGrp += Isis::PvlKeyword("MinimumRadius", 0.0);
  mapGrp += Isis::PvlKeyword("MaximumRadius", 20000000.0);
  mapGrp += Isis::PvlKeyword("MinimumAzimuth", -20.0);
  mapGrp += Isis::PvlKeyword("MaximumAzimuth", 130.0);
  mapGrp += Isis::PvlKeyword("ProjectionName", "RingCylindrical");

  cout << "Test missing center azimuth keyword ..." << endl;
  try {
    Isis::RingCylindrical p(lab);
  }
  catch(Isis::IException &e) {
    e.print();
  }
  cout << endl;

  try {
    mapGrp += Isis::PvlKeyword("CenterAzimuth", 0.0);
    Isis::RingCylindrical *p = (Isis::RingCylindrical *) Isis::ProjectionFactory::Create(lab);

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

    Isis::RingCylindrical *s = p;
    cout << "Test Name and comparision method ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;

    cout << "Testing default option ... " << endl;
    mapGrp.DeleteKeyword("CenterAzimuth");
    Isis::RingCylindrical p2(lab, true);
    cout << lab << endl;
    cout << endl;

    cout << "Testing Mapping() methods ... " << endl;

    Isis::Pvl tmp1;
    Isis::Pvl tmp2;
    Isis::Pvl tmp3;
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
  catch(Isis::IException &e) {
    e.print();
  }
}



