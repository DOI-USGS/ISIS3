#include <cfloat>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "Constants.h"
#include "IException.h"
#include "Planar.h"
#include "Preference.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "RingPlaneProjection.h"
#include "SpecialPixel.h"
#include "WorldMapper.h"

using namespace std;
using namespace Isis;
class MyProjection : public RingPlaneProjection {
  public:
    // create a child class
    MyProjection(Pvl &lab) : RingPlaneProjection(lab) {
    }
    // override pure virtual methods
    QString Name() const {
      return "None";
    }
    QString Version() const {
      return "1.0";
    }

    // override other virtual methods
    virtual double TrueScaleRadius() const {
      return 45.0;
    }
    bool SetGround(const double rad, const double az) {
      if (rad <= 0.0) {
        m_good = false;
      }
      else {
        m_radius = rad;
        m_azimuth = az;
        double x = az * 10.0;
        double y = rad + 90.0;
        SetComputedXY(x, y);
        m_good = true;
      }
      return m_good;
    }
    virtual bool SetCoordinate(const double x, const double y) {
      SetXY(x, y);
      m_azimuth = GetX() / 10.0;
      m_radius = GetY() - 90.0;
      m_good = true;
      return m_good;
    }
    bool XYRange(double &minX, double &maxX, double &minY, double &maxY) {
      XYRangeCheck(Null, Null);
      minX = DBL_MAX;
      minY = DBL_MAX;
      maxX = -DBL_MAX;
      maxY = -DBL_MAX;
      if(!m_groundRangeGood) return false;
      XYRangeCheck(m_minimumRadius, m_minimumAzimuth);
      XYRangeCheck(m_minimumRadius, m_maximumAzimuth);
      XYRangeCheck(m_maximumRadius, m_minimumAzimuth);
      XYRangeCheck(m_maximumRadius, m_maximumAzimuth);
      minX = m_minimumX;
      minY = m_minimumY;
      maxX = m_maximumX;
      maxY = m_maximumY;
      return true;
    }

};

class EmptyProjection : public RingPlaneProjection {
  public:
    EmptyProjection(Pvl &lab) : RingPlaneProjection(lab) {
    }

    // pure virtuals, must be overriden, no need to test since MyProjection will test these
    QString Name() const {
      return "None";
    }
    QString Version() const {
      return "1.0";
    }
};


class MyMapper : public WorldMapper {
  public:
    MyMapper() : WorldMapper() {};
    double ProjectionX(const double worldX) const {
      return worldX / 2.0;
    };
    double ProjectionY(const double worldY) const {
      return worldY / 3.0;
    };
    double WorldX(const double projectionX) const {
      return projectionX * 2.0;
    };
    double WorldY(const double projectionY) const {
      return projectionY * 3.0;
    };
    virtual double Resolution() const {
      return 0.5;
    }
};



// unitTest main
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  // create a MyProjection object with the given labels
  void Doit(Pvl & lab);

  cout.precision(13);
  cout << "Unit test for RingPlaneProjection ..." << endl << endl;
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << "Test Error Throws from the constructor...\n" << endl;

  cout << "Test for missing Mapping Group" << endl;
  Pvl lab;  // Error throws from Projection
  Doit(lab);
  cout << endl;

  lab.addGroup(PvlGroup("Mapping"));
  PvlGroup &mg = lab.findGroup("Mapping");
  Doit(lab);
  cout << endl;

  cout << "Test for missing azimuth direction in the mapping group" << endl;
  Doit(lab);
  cout << endl;

  cout << "Test for invalid azimuth direction value" << endl;
  mg += PvlKeyword("AzimuthDirection", "Up");
  Doit(lab);
  cout << endl;

  cout << "Test for missing azimuth domain in the mapping group" << endl;
  mg["AzimuthDirection"] = "Clockwise";
  Doit(lab);
  cout << endl;

  cout << "Test for invalid azimuth domain value in the mapping group" << endl;
  mg += PvlKeyword("AzimuthDomain", "75");
  Doit(lab);
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << endl;

  mg["AzimuthDomain"] = "360";
  mg += PvlKeyword("ProjectionName", "MyProjection");

  cout << "Projection Specifications" << endl;
  MyProjection p(lab);
  // test methods that return properties of the projection
  cout << "Is Equatorial Cylindrical: " << p.IsEquatorialCylindrical() << endl;
  cout << "Azimuth Direction:       " << p.AzimuthDirectionString() << endl;
  cout << "Is Clockwise:           " << p.IsClockwise() << endl;
  cout << "Is CounterClockwise:           " << p.IsCounterClockwise() << endl;
  cout << "Azimuth Domain:          " << p.AzimuthDomainString() << endl;
  cout << "Has 360 domain:            " << p.Has360Domain() << endl;
  cout << "Has 180 domain:            " << p.Has180Domain() << endl;
  cout << "Has ground range:          " << p.HasGroundRange() << endl;
  cout << "Rotation:                  " << p.Rotation() << endl;
  cout << "Scale:                  " << p.Scale() << endl;
  cout << endl;
 cout << endl;

  //Test exceptions

  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << "Test More Error Throws...\n" << endl;
  cout << "Testing invalid minimum radius..." << endl;
  mg += PvlKeyword("MinimumRadius", "-45.0");
  mg += PvlKeyword("MaximumRadius", "-80.0");
  mg += PvlKeyword("MinimumAzimuth", "15.0");
  mg += PvlKeyword("MaximumAzimuth", "-190.0");
  Doit(lab);
  cout << endl;

  cout << "Testing invalid maximum radius" << endl;
  mg["MinimumRadius"] = "80.0";
  Doit(lab);
  cout << endl;

  cout << "Testing unordered radius range" << endl;
  mg["MaximumRadius"].setValue("45.0", "units");
  Doit(lab);
  cout << endl;

  cout << "Testing unordered azimuth range" << endl;
  mg["MaximumRadius"].setValue("180.0", "units");
  Doit(lab);
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << endl;

  mg["MaximumAzimuth"] = "190.0";

  cout << "Testing xyRange methods...\n" << endl;
  MyProjection p2(lab);
  cout << "Get ground range from the labels...  " << endl;
  cout << "Has as azimuth range:  " << p2.HasGroundRange() << endl;
  cout << "Minimum radius:        " << p2.MinimumRadius() << endl;
  cout << "Maximum radius:        " << p2.MaximumRadius() << endl;
  cout << "Minimum azimuth:       " << p2.MinimumAzimuth() << endl;
  cout << "Maximum azimuth:       " << p2.MaximumAzimuth() << endl;

  double minX, maxX, minY, maxY;
  if (p2.XYRange(minX, maxX, minY, maxY)) {
    cout << "Find coordinate range ...  " << endl;
    cout << "Minimum X:              " << minX << endl;
    cout << "Maximum X:              " << maxX << endl;
    cout << "Minimum Y:              " << minY << endl;
    cout << "Maximum Y:              " << maxY << endl;
    cout << endl;
  }

  cout << "Testing Ground coordinate routines" << endl;
  cout << "Setting radius to (91,  0):  " << p2.SetGround(91.0, 0.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Setting azimuth to ( 0,  91):  " << p2.SetGround(0., 91.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Setting position to (60,  -5):  " << p2.SetGround(60.0, -5.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Radius:                       " << p2.Radius() << endl;
  cout << "Local Radius:              " << p2.LocalRadius() << endl;
  cout << "Azimuth:                      " << p2.Azimuth() << endl;
  cout << "XCoord:                         " << p2.XCoord() << endl;
  cout << "YCoord:                         " << p2.YCoord() << endl;
  cout << "UniversalRadius:              " << p2.UniversalRadius() << endl;
  cout << "UniversalAzimuth:             " << p2.UniversalAzimuth() << endl;
  cout << endl;


  cout << "Testing Universal Ground coordinate routines" << endl;
  cout << "Setting position to (57.3920057293825,  355):  " << p2.SetUniversalGround(57.3920057293825, 355.0) << endl;
  cout << "Is Good:                                       " << p2.IsGood() << endl;
  cout << "Radius:                                      " << p2.Radius() << endl;
  cout << "Azimuth:                                     " << p2.Azimuth() << endl;
  cout << "XCoord:                                        " << p2.XCoord() << endl;
  cout << "YCoord:                                        " << p2.YCoord() << endl;
  cout << "UniversalRadius:                             " << p2.UniversalRadius() << endl;
  cout << "UniversalAzimuth:                            " << p2.UniversalAzimuth() << endl;
  cout << "Setting bad position..." << p2.SetUniversalGround((double) Null, (double) Null) << endl;
  cout << endl;
  
  /* The following projection is used to test the ToPlanetographic() methods
   *   at the radius boundaries (2000 and 20000 meters). qFuzzyCompare() methods
   *   were added to accommodate for double imprecission. 
   */
  Isis::Pvl radRangeTest;
  radRangeTest.addGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &radTestGroup = radRangeTest.findGroup("Mapping");
  radTestGroup += Isis::PvlKeyword("TargetName", "Saturn");
  radTestGroup += Isis::PvlKeyword("ProjectionName", "Planar");
  radTestGroup += Isis::PvlKeyword("AzimuthDirection", "Clockwise");
  radTestGroup += Isis::PvlKeyword("AzimuthDomain", "180");
  radTestGroup += Isis::PvlKeyword("Scale", "5.0");
  radTestGroup += Isis::PvlKeyword("MinimumRadius", "2000.0");
  radTestGroup += Isis::PvlKeyword("MaximumRadius", "20000.0");
  radTestGroup += Isis::PvlKeyword("MinimumAzimuth", "0.0");
  radTestGroup += Isis::PvlKeyword("MaximumAzimuth", "360.0");
  radTestGroup += Isis::PvlKeyword("CenterRadius", "12000.0");
  radTestGroup += Isis::PvlKeyword("CenterAzimuth", "0.0");
  Isis::Planar *radTestProjection = (Isis::Planar *) Isis::ProjectionFactory::Create(radRangeTest);

  cout << "Planar Projection Specifications" << endl;
  // test methods that return properties of the projection
  cout << "Is Equatorial Cylindrical: " << radTestProjection->IsEquatorialCylindrical() << endl;
  cout << "Azimuth Direction:       " << radTestProjection->AzimuthDirectionString() << endl;
  cout << "Is Clockwise:           " << radTestProjection->IsClockwise() << endl;
  cout << "Is CounterClockwise:           " << radTestProjection->IsCounterClockwise() << endl;
  cout << "Azimuth Domain:          " << radTestProjection->AzimuthDomainString() << endl;
  cout << "Has 360 domain:            " << radTestProjection->Has360Domain() << endl;
  cout << "Has 180 domain:            " << radTestProjection->Has180Domain() << endl;
  cout << "Has ground range:          " << radTestProjection->HasGroundRange() << endl;
  cout << "Rotation:                  " << radTestProjection->Rotation() << endl << endl;
  try {
    cout << "Setting position to (9000.0, 0.0)" << endl;
    radTestProjection->SetUniversalGround(9000.0, 0.0);
    cout << "Is Good:                                       "
         << radTestProjection->IsGood() << endl;
    cout << "Radius:                                      "
         << radTestProjection->Radius() << endl;
    cout << "Azimuth:                                     "
         << radTestProjection->Azimuth() << endl;
    cout << "XCoord:                                        "
         << radTestProjection->XCoord() << endl;
    cout << "YCoord:                                        "
         << radTestProjection->YCoord() << endl;
    cout << "UniversalRadius:                             "
         << radTestProjection->UniversalRadius() << endl;
    cout << "UniversalAzimuth:                            "
         << radTestProjection->UniversalAzimuth() << endl;
    cout << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    radTestProjection->SetUniversalGround(9000.0, 360.0);
    cout << "Setting position to (9000.0, 360.0)" << endl;
    cout << "Is Good:                                       "
         << radTestProjection->IsGood() << endl;
    cout << "Radius:                                      "
         << radTestProjection->Radius() << endl;
    cout << "Azimuth:                                     "
         << radTestProjection->Azimuth() << endl;
    cout << "XCoord:                                        "
         << radTestProjection->XCoord() << endl;
    cout << "YCoord:                                        "
         << radTestProjection->YCoord() << endl;
    cout << "UniversalRadius:                             "
         << radTestProjection->UniversalRadius() << endl;
    cout << "UniversalAzimuth:                            "
         << radTestProjection->UniversalAzimuth() << endl;
    cout << endl;
  }
  catch(IException &error) {
    error.print();
  }

  cout << "Testing == operator Projection conditions..."  << p.Name() << endl;
  cout << "Projection 1 name and resolution = " << p.Name() << " " << p.Resolution() << endl;
  cout << "Projection 2 name = " << p2.Name() << " " << p2.Resolution() << endl;
  cout << "Projection 3 name = " << radTestProjection->Name() << " " << radTestProjection->Resolution() << endl;  

  if (p == p2)
     cout << "Projection 1 = Projection 2" << endl;
  else
    cout << "Projection 1 != Projection 2" << endl;

  if (p == *radTestProjection) 
     cout << "Projection 1 = Projection3" << endl;
  else
    cout << "Projection 1 != Projection3" << endl;
  cout << endl;

  cout << "Testing projection coordinate routines" << endl;
  cout << "Setting x/y position to (-90,15):  " << p2.SetCoordinate(-90.0, 15.0) << endl;
  cout << "Is Good:                             " << p2.IsGood() << endl;
  cout << "Radius:                            " << p2.Radius() << endl;
  cout << "Azimuth:                           " << p2.Azimuth() << endl;
  cout << "XCoord:                              " << p2.XCoord() << endl;
  cout << "YCoord:                              " << p2.YCoord() << endl;
  cout << "UniversalRadius:                   " << p2.UniversalRadius() << endl;
  cout << "UniversalAzimuth:                  " << p2.UniversalAzimuth() << endl;
  cout << "WorldX:                              " << p2.WorldX() << endl;
  cout << "WorldY:                              " << p2.WorldY() << endl;
  cout << endl;

  p2.SetWorldMapper(new MyMapper());

  cout << "Testing world coordinate routines" << endl;
  cout << "Setting world x/y position to (-45,45):  " << p2.SetWorld(-45.0, 45.0) << endl;
  cout << "Is Good:                                   " << p2.IsGood() << endl;
  cout << "Radius:                                  " << p2.Radius() << endl;
  cout << "Azimuth:                                 " << p2.Azimuth() << endl;
  cout << "XCoord:                                    " << p2.XCoord() << endl;
  cout << "YCoord:                                    " << p2.YCoord() << endl;
  cout << "UniversalRadius:                         " << p2.UniversalRadius() << endl;
  cout << "UniversalAzimuth:                        " << p2.UniversalAzimuth() << endl;
  cout << "WorldX:                                    " << p2.WorldX() << endl;
  cout << "WorldY:                                    " << p2.WorldY() << endl;
  // cout << "ToProjectionX (-4500):                     " << p2.ToProjectionX(-4500.0) << endl;
  // cout << "ToProjectionY (45):                        " << p2.ToProjectionY(45.0) << endl;
  // cout << "ToWorldX:                                  " << p2.ToWorldX(p2.ToProjectionX(-4500.0)) << endl;
  // cout << "ToWorldY:                                  " << p2.ToWorldY(p2.ToProjectionY(45.0)) << endl;
  cout << "Resolution:                                " << p2.Resolution() << endl;
  cout << "Scale:                                     " << p2.Scale() << endl;
  cout << "TrueScaleRadius:                         " << p2.TrueScaleRadius() << endl;
  cout << endl;

  cout << "Testing IsSky method" << endl;
  cout << p2.IsSky() << endl;
  mg += PvlKeyword("TargetName", "SKY");
  mg["AzimuthDirection"] = "CounterClockwise";
  Doit(lab);
  MyProjection p3(lab);
  cout << p3.IsSky() << endl;
  cout << endl;

  cout << "Testing == operator condition in class..." << endl;
  cout << "Projection 1 name and resolution = " << p.Name() << " " << p.Resolution() << endl;
  cout << "Projection 1 azimuth direction = " << p.AzimuthDirectionString() << endl;
  cout << "Projection 4 name and resolution = " << p3.Name() << " " << p3.Resolution() << endl;
  cout << "Projection 4 azimuth direction string = " << p3.AzimuthDirectionString() << endl;
  if (p == p3)
     cout << "Projection 1 = Projection 4" << endl;
  else
    cout << "Projection 1 != Projection 4" << endl;

  cout << endl;

  cout << "Testing string routines" << endl;
  cout << p2.AzimuthDirectionString() << endl;
  cout << p2.AzimuthDomainString() << endl;
  cout << endl;

  cout << "Testing Name and comparision routines" << endl;
  cout << "Name:        " << p2.Name() << endl;
  cout << "Version:     " << p2.Version() << endl;
  cout << "operator==:  " << (p == p2) << endl;
  cout << "operator!=:  " << (p != p2) << endl;

  mg["AzimuthDirection"] = "CounterClockwise";
  mg["AzimuthDomain"] = "180";
  EmptyProjection noproj(lab);
  cout << endl;

  cout << "Testing no projection" << endl;
  noproj.SetUniversalGround(45.0, 270.0);
  cout << "Radius:    " << noproj.Radius() << endl;
  cout << "Azimuth:   " << noproj.Azimuth() << endl;
  cout << "Is clockwise?" << noproj.IsClockwise() << endl;
  cout << "Is counterclockwise?" << noproj.IsCounterClockwise() << endl;
  cout << "Azimuth direction = " << noproj.AzimuthDirectionString() << endl;
  cout << "Has 180 domain?" << noproj.Has180Domain() << endl;
  cout << "Has 360 domain?" << noproj.Has360Domain() << endl;
  cout << "Azimuth domain  = " << noproj.AzimuthDomainString() << endl;
  cout << "True scale radius = " << noproj.TrueScaleRadius() << endl;
  double badvalue = Null;
  if (noproj.XYRange(badvalue, badvalue, badvalue,badvalue)) 
    cout << "Bad range" << endl;
  cout << endl;

  cout << "Testing radius methods " << endl;
  cout << noproj.LocalRadius() << endl;
  cout << endl;
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << endl;

  cout << "Testing bad coordinates " << endl;
  cout << "Testing bad radius/azimuth... Is good?  " << noproj.SetGround(Null, Null) << endl;
  cout << "Testing bad x/y... Is good?  " << noproj.SetCoordinate(Null, Null) << endl;
  cout << endl;
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << endl;

  cout << endl;
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << "Test Error Throws for invalid inputs to conversion methods " << endl;
  
  double invalidValue = Null;
  try {
    RingPlaneProjection::To180Domain(invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::To360Domain(invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::ToClockwise(invalidValue, 180);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::ToClockwise(0, invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::ToClockwise(invalidValue, 360);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::ToCounterClockwise(0, invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    RingPlaneProjection::ToCounterClockwise(invalidValue, 360);
  }
  catch(IException &error) {
    error.print();
  }
  cout << endl;
  cout << "///////////////////////////////////////////////////////////" << endl;

  cout << "Testing conversion routines with valid inputs..." << endl;
  try {
    cout << "60 clockwise converted to " << p.ToCounterClockwise(60., 360) << " in 360 domain. " << endl;
    cout << "60 clockwise converted to " << p.ToCounterClockwise(60., 180) << " in 180 domain. " << endl;
  }
  catch(IException &error) {
    error.print();
  }
  try {
    cout << "60 counterclockwise converted to " << p3.ToClockwise(60., 360) << " in 360 domain. " << endl;
    cout << "60 counterclockwise converted to " << p3.ToClockwise(60., 180) << " in 180 domain. " << endl;
  }
  catch(IException &error) {
    error.print();
  }
  try {
    p.ToWorldX(invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  try {
    p.ToWorldY(invalidValue);
  }
  catch(IException &error) {
    error.print();
  }
  cout << "///////////////////////////////////////////////////////////" << endl;
  cout << endl;
  cout << endl;

  // Add remaining keywords for Mapping() test 
  mg += PvlKeyword("PixelResolution", "1.0");
  mg += PvlKeyword("Scale", "1.0");
  mg += PvlKeyword("UpperLeftCornerX", "1.0");
  mg += PvlKeyword("UpperLeftCornerY", "1.0");

  cout << "Rotation Tests" << endl;
  mg += PvlKeyword("Rotation", "90.0");
  mg["AzimuthDirection"] = "Clockwise";
  MyProjection p4(lab);
  cout << "Rotation:     " << p4.Rotation() << endl;
  cout << "Testing Ground coordinate routines" << endl;
  cout << "Setting radius to (-91,  0):  " << p4.SetGround(-91.0, 0.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Setting radius to ( 9001,  0):  " << p4.SetGround(9001.0, 0.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Setting position to (60,  -5):  " << p4.SetGround(60.0, -5.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Radius:                       " << p4.Radius() << endl;
  cout << "Azimuth:                      " << p4.Azimuth() << endl;
  cout << "XCoord:                         " << p4.XCoord() << endl;
  cout << "YCoord:                         " << p4.YCoord() << endl;
  cout << "UniversalRadius:              " << p4.UniversalRadius() << endl;
  cout << "UniversalAzimuth:             " << p4.UniversalAzimuth() << endl;
  cout << endl;

  cout << "Testing projection coordinate routines" << endl;
  cout << "Setting x/y position to (150,50):  " << p4.SetCoordinate(150.0, 50.0) << endl;
  cout << "Is Good:                             " << p4.IsGood() << endl;
  cout << "Radius:                            " << p4.Radius() << endl;
  cout << "Azimuth:                           " << p4.Azimuth() << endl;
  cout << "XCoord:                              " << p4.XCoord() << endl;
  cout << "YCoord:                              " << p4.YCoord() << endl;
  cout << "UniversalRadius:                   " << p4.UniversalRadius() << endl;
  cout << "UniversalAzimuth:                  " << p4.UniversalAzimuth() << endl;
  cout << "WorldX:                              " << p4.WorldX() << endl;
  cout << "WorldY:                              " << p4.WorldY() << endl;
  cout << endl;

  Pvl mapping;
  mapping.addGroup(p4.Mapping());
  cout << "Testing Mapping() methods" << endl;
  cout << "Mapping() = " << endl;
  cout << mapping << endl;
  mapping.deleteGroup("Mapping");
  mapping.addGroup(p4.MappingRadii());
  cout << "MappingRadii() = " << endl;
  cout << mapping << endl;
  mapping.deleteGroup("Mapping");
  mapping.addGroup(p4.MappingAzimuths());
  cout << "MappingAzimuths() = " << endl;
  cout << mapping << endl;
  mapping.deleteGroup("Mapping");
  cout << endl;

  //SetUpperLeftCorner(Displacement x, Displacement y)
}


void Doit(Pvl &lab) {
  try {
    MyProjection p(lab);
  }
  catch(IException &error) {
    error.print();
  }
}
