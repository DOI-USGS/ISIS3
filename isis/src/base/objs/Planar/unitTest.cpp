#include <iostream>
#include <iomanip>

#include <QString>

#include "IException.h"
#include "Planar.h"
#include "ProjectionFactory.h"
#include "Preference.h"
#include "RingPlaneProjection.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "UNIT TEST FOR Planar Projection" << endl << endl;

  Pvl lab;
  lab.AddGroup(PvlGroup("Mapping"));
  PvlGroup &mapGroup = lab.FindGroup("Mapping");
  mapGroup += PvlKeyword("ProjectionName", "Planar");
  mapGroup += PvlKeyword("TargetName", "Saturn");
  mapGroup += PvlKeyword("AzimuthDirection", "Clockwise");
  mapGroup += PvlKeyword("AzimuthDomain", "180");
  mapGroup += PvlKeyword("MinimumRadius", "0.0");
  mapGroup += PvlKeyword("MaximumRadius", "2000000.0");
  mapGroup += PvlKeyword("MinimumAzimuth", "-20.0");
  mapGroup += PvlKeyword("MaximumAzimuth", "130.0");

  cout << "Test missing center azimuth keyword ..." << endl;
  try {
    Planar p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterAzimuth", "0.0");

  cout << "Test missing CenterRadius keyword ..." << endl;
  try {
    Planar p(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;

  mapGroup += PvlKeyword("CenterRadius", "200000.0");

  try {
    Isis::Planar *p = (Planar *) ProjectionFactory::Create(lab);

    // Projection 1 test
    cout << "Projection 1 parameters..." << endl;
    cout << "Projection version   = " << p->Version() << endl;
    cout << "  Projection name    =  " <<  p->Name()  << endl;
    cout << "  Target name          =  " << (QString) mapGroup["TargetName"]  << endl;
    cout << "  Azimuth direction  = " << p->AzimuthDirectionString() << endl;
    cout << "  Azimuth domain    = " << p->AzimuthDomainString() << endl;
    cout << "  Minimum radius     = " << p->MinimumRadius() << endl;
    cout << "  Maximum radius     = " << p->MaximumRadius() << endl;
    cout << "  Minimum azimuth   = " << p->MinimumAzimuth() << endl;
    cout << "  Maximum azimuth  = " << p->MaximumAzimuth() << endl;
    cout << "  Center radius         = " << p->CenterRadius() << endl;
    cout << "  Center azimuth      = " << p->CenterAzimuth() << endl;
    cout << endl;

    // Test TrueScaleRadius method
     cout << "Test TrueScaleRadius method..." << endl;
     cout << "TrueScaleRadius = " << p->TrueScaleRadius() << endl;
     cout << endl;

    // SetGround(const double radius, const double az)
    cout << "Test SetGround method ... " << endl;
    cout << std::setprecision(16);
    cout << "Setting ground to (1000.0,45.0)" << endl;
    p->SetGround(1000.0, 45.0);
    cout << "Radius:               " << p->LocalRadius() << endl;
    cout << "Azimuth:              " << p->Azimuth() << endl;
    cout << "XCoord:                 " << p->XCoord() << endl;
    cout << "YCoord:                 " << p->YCoord() << endl;
    cout << endl;

    cout << "Test SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (0.2617993877991494,-0.8726646259971648)" << endl;
    p->SetCoordinate(0.2617993877991494, -0.8726646259971648);
    cout << "Radius:               " << p->LocalRadius() << endl;
    cout << "Azimuth:              " << p->Azimuth() << endl;
    cout << "XCoord:                 " << p->XCoord() << endl;
    cout << "YCoord:                 " << p->YCoord() << endl;
    cout << endl;
  
    // XYRange method not implemented yet. Do we need it?
   cout << "Test XYRange method ... " << endl;
   double minX, maxX, minY, maxY;
    if (p->XYRange(minX, maxX, minY, maxY)) {
      cout << "Minimum X:  " << minX << endl;
      cout << "Maximum X:  " << maxX << endl;
      cout << "Minimum Y:  " << minY << endl;
      cout << "Maximum Y:  " << maxY << endl;
      cout << endl;
    } 

    Planar *s = p;
    cout << "Test Name and comparision methods ... " << endl;
    cout << "Name:       " << s->Name() << endl;
    cout << "operator==  " << (*s == *s) << endl;
    cout << endl;

    cout << "Testing default options in constructor with Projection 2 ... " << endl;
    mapGroup.DeleteKeyword("CenterAzimuth");
    mapGroup.DeleteKeyword("CenterRadius");
    Planar p2(lab, true);
    cout << lab << endl;
    cout << "Default projection parameters == Original projection ?" << (*p == p2) << endl;
    cout << endl;
    cout << endl;

    cout << "Testing more SetGround conditions...AzimuthDirection = CounterClockwise and AzimuthDomain = 360" << endl;
    mapGroup["AzimuthDirection"].SetValue("CounterClockwise");
    mapGroup["AzimuthDomain"].SetValue("360");
    Planar p3(lab, true);

    // Projection 3 test
    cout << "Projection 3 parameters..." << endl;
    cout << "  Projection name    =  " <<  p3.Name()  << endl;
    cout << "  Target name          =  " << (QString) mapGroup["TargetName"]  << endl;
    cout << "  Azimuth direction  = " << p3.AzimuthDirectionString() << endl;
    cout << "  Azimuth domain    = " << p3.AzimuthDomainString() << endl;
    cout << "  Minimum radius     = " << p3.MinimumRadius() << endl;
    cout << "  Maximum radius     = " << p3.MaximumRadius() << endl;
    cout << "  Minimum azimuth   = " << p3.MinimumAzimuth() << endl;
    cout << "  Maximum azimuth  = " << p3.MaximumAzimuth() << endl;
    cout << "  Center radius         = " << p3.CenterRadius() << endl;
    cout << "  Center azimuth      = " << p3.CenterAzimuth() << endl;
    cout << endl;

    cout << "  Setting ground to (1000.0,45.0)" << endl;
    p3.SetGround(1000.0, 45.0);
    cout << "    Radius:             =  " << p3.LocalRadius() << endl;
    cout << "    Azimuth:          =  " << p3.Azimuth() << endl;
    cout << "    XCoord:           =  " << p3.XCoord() << endl;
    cout << "    YCoord:           =  " << p3.YCoord() << endl;
    cout << endl;

    cout << "Testing SetGround error condition..." << endl;
    cout << "  Setting ground to (-1000.0,45.0)" << endl;
    try {
      p3.SetGround(-1000.0, 45.0);
    }
    catch(IException &e) {
      e.print();
    }
    cout << endl;

    cout << "Testing more SetCoordinate method ... " << endl;
    cout << "Setting coordinate to (0.2617993877991494,-0.8726646259971648)" << endl;
    p3.SetCoordinate(0.2617993877991494, -0.8726646259971648);
    cout << "Radius:               " << p3.LocalRadius() << endl;
    cout << "Azimuth:              " << p3.Azimuth() << endl;
    cout << "XCoord:                 " << p3.XCoord() << endl;
    cout << "YCoord:                 " << p3.YCoord() << endl;
    cout << endl;
    cout << "Setting coordinate to (0.2617993877991494,0.8726646259971648)" << endl;
    p3.SetCoordinate(0.2617993877991494, 0.8726646259971648);
    cout << "Radius:               " << p3.LocalRadius() << endl;
    cout << "Azimuth:              " << p3.Azimuth() << endl;
    cout << "XCoord:                 " << p3.XCoord() << endl;
    cout << "YCoord:                 " << p3.YCoord() << endl;
    cout << endl;
    cout << endl;

    cout << "Testing Mapping() methods ... " << endl;

    mapGroup.DeleteKeyword("MinimumRadius");
    mapGroup.DeleteKeyword("MaximumRadius");
    mapGroup.DeleteKeyword("MinimumAzimuth");
    mapGroup.DeleteKeyword("MaximumAzimuth");

    // Projection 4 test
    Planar p4(lab, true);
    cout << "Projection 4 parameters...No range" << endl;
    cout << "  Projection name    =  " <<  p4.Name()  << endl;
    cout << "  Target name          =  " << (QString) mapGroup["TargetName"]  << endl;
    cout << "  Azimuth direction  = " << p4.AzimuthDirectionString() << endl;
    cout << "  Azimuth domain    = " << p4.AzimuthDomainString() << endl;
    cout << "  Center radius         = " << p4.CenterRadius() << endl;
    cout << "  Center azimuth      = " << p4.CenterAzimuth() << endl;
    cout << endl;

    Pvl tmp1;
    Pvl tmp2;
    Pvl tmp3;
    tmp1.AddGroup(p->Mapping());
    tmp2.AddGroup(p->MappingRadii());
    tmp3.AddGroup(p->MappingAzimuths());

    cout << "Mapping() = " << endl;
    cout << tmp1 << endl;
    cout << "MappingRadii() = " << endl;
    cout << tmp2 << endl;
    cout << "MappingAzimuths() = " << endl;
    cout << tmp3 << endl;
    cout << endl;
  }
  catch(IException &e) {
    e.print();
  }
}



