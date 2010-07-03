#include <iostream>
#include <iomanip>
#include <cmath>
#include <cfloat>
#include "Projection.h"
#include "iException.h"
#include "WorldMapper.h"
#include "Constants.h"
#include "PvlGroup.h"
#include "Preference.h"

using namespace std;
class MyProjection : public Isis::Projection {
  public:
    MyProjection (Isis::Pvl &lab) :
      Isis::Projection (lab) {
    }
    bool XYRange(double &minX, double &maxX, double &minY, double &maxY) {
      minX = DBL_MAX;
      minY = DBL_MAX;
      maxX = -DBL_MAX;
      maxY = -DBL_MAX;
      if (!p_groundRangeGood) return false;
      XYRangeCheck(p_minimumLatitude,p_minimumLongitude);
      XYRangeCheck(p_minimumLatitude,p_maximumLongitude);
      XYRangeCheck(p_maximumLatitude,p_minimumLongitude);
      XYRangeCheck(p_maximumLatitude,p_maximumLongitude);
      minX = p_minimumX;
      minY = p_minimumY;
      maxX = p_maximumX;
      maxY = p_maximumY;
      return true;
    }
    bool SetGround(const double lat, const double lon) {
      if ((lat < -90.0) || (lat > 90.0)) {
        p_good = false;
      }
      else {
        p_latitude = lat;
        p_longitude = lon;
        double x = lon * 10.0;
        double y = lat + 90.0;
        SetComputedXY(x,y);
        p_good = true;
      }
      return p_good;
    }
    virtual bool SetCoordinate(const double x, const double y) {
      SetXY(x,y);
      p_longitude = GetX() / 10.0;
      p_latitude = GetY() - 90.0;
      p_good = true;
      return p_good;
    }

    virtual double TrueScaleLatitude() const {
      return 45.0;
    }

    std::string Name() const { return "None"; }
    std::string Version() const { return "1.0"; }

    void Output() const {
      cout << tCompute(0.0,0.0) << endl;
      cout << tCompute(Isis::HALFPI/2.0,sin(Isis::HALFPI/2.0)) << endl;
      cout << tCompute(Isis::HALFPI,sin(Isis::HALFPI)) << endl;
      cout << mCompute(sin(0.0),cos(0.0)) << endl;
      cout << mCompute(sin(Isis::HALFPI/2.0),cos(Isis::HALFPI/2.0)) << endl;
      cout << e4Compute() << endl;
      cout << phi2Compute(0.0) << endl;
      cout << phi2Compute(10.0) << endl;
      cout << phi2Compute(100.0) << endl;
      cout << phi2Compute(1000.0) << endl;
    }
};

class EmptyProjection : public Isis::Projection {
  public:
    EmptyProjection (Isis::Pvl &lab) :
      Isis::Projection (lab) {
    }

    std::string Name() const { return "None"; }
    std::string Version() const { return "1.0"; }
};


class MyMapper : public Isis::WorldMapper {
  public:
    MyMapper() : Isis::WorldMapper() {};
    double ProjectionX(const double worldX) const { return worldX / 2.0; };
    double ProjectionY(const double worldY) const { return worldY / 3.0; };
    double WorldX(const double projectionX) const { return projectionX * 2.0; };
    double WorldY(const double projectionY) const { return projectionY * 3.0; };
    virtual double Resolution() const { return 0.5; }
};

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  void Doit(Isis::Pvl &lab);

  cout.precision(13);
  cout << "Unit test for Isis::Projection ..." << endl << endl;

  cout << "Test for missing Mapping Group" << endl;
  Isis::Pvl lab;
  Doit(lab);
  cout << endl;

  cout << "Test for missing Equatorial Radius" << endl;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mg = lab.FindGroup("Mapping");
  Doit(lab);
  cout << endl;

  cout << "Test for missing polar radius" << endl;
  mg += Isis::PvlKeyword("EquatorialRadius", -1.0);
  Doit(lab);
  cout << endl;

  cout << "Test for invalid Equatoral Radius" << endl;
  mg += Isis::PvlKeyword("PolarRadius",-0.95);
  Doit(lab);
  cout << endl;

  cout << "Test for invalid polar radius" << endl;
  mg["EquatorialRadius"] = 1.0;
  mg += Isis::PvlKeyword("PolarRadius",-0.95);
  Doit(lab);
  cout << endl;

  cout << "Test for missing latitude type" << endl;
  mg["PolarRadius"] = 0.95;
  Doit(lab);
  cout << endl;

  cout << "Test for invalid latitude type" << endl;
  mg += Isis::PvlKeyword("LatitudeType","Planeto");
  Doit(lab);
  cout << endl;

  cout << "Test for missing longitude direction" << endl;
  mg["LatitudeType"] = "Planetographic";
  Doit(lab);
  cout << endl;

  cout << "Test for invalid longitude direction" << endl;
  mg += Isis::PvlKeyword("LongitudeDirection","Up");
  Doit(lab);
  cout << endl;

  cout << "Test for missing longitude domain" << endl;
  mg["LongitudeDirection"] = "PositiveEast";
  Doit(lab);
  cout << endl;

  cout << "Test for invalid longitude domain" << endl;
  mg += Isis::PvlKeyword("LongitudeDomain",75);
  Doit(lab);
  cout << endl;
  mg["LongitudeDomain"] = 360;

  mg += Isis::PvlKeyword("ProjectionName", "MyProjection");

  cout << "Projection Specifications" << endl;
  MyProjection p(lab);
  cout << "Equatorial Radius:     " << p.EquatorialRadius() << endl;
  cout << "Polar Radius:          " << p.PolarRadius() << endl;
  cout << "Eccentricity:          " << p.Eccentricity() << endl;
  cout << "Is Planetographic:     " << p.IsPlanetographic() << endl;
  cout << "Is Planetocentric:     " << p.IsPlanetocentric() << endl;
  cout << "Is PositiveEast:       " << p.IsPositiveEast() << endl;
  cout << "Is PositiveWest:       " << p.IsPositiveWest() << endl;
  cout << "Has 360 domain:        " << p.Has360Domain() << endl;
  cout << "Has 180 domain:        " << p.Has180Domain() << endl;
  cout << "Has ground range:      " << p.HasGroundRange() << endl;
  cout << "Rotation:              " << p.Rotation() << endl;
  cout << endl;

  cout << "Testing conversion methods" << endl;
  cout << "Bring -50  into 360 Domain:  " << p.To360Domain(-50.0) << endl;
  cout << "Bring  50  into 360 Domain:  " << p.To360Domain(50.0) << endl;
  cout << "Bring   0  into 360 Domain:  " << p.To360Domain(0.0) << endl;
  cout << "Bring 360  into 360 Domain:  " << p.To360Domain(360.0) << endl << endl;
  cout << "Bring 240  into 180 Domain:  " << p.To180Domain(240.0) << endl;
  cout << "Bring 140  into 180 Domain:  " << p.To180Domain(140.0) << endl;
  cout << "Bring -180 into 180 Domain:  " << p.To180Domain(-180.0) << endl;
  cout << "Bring 180  into 180 Domain:  " << p.To180Domain(180.0) << endl;
  cout << endl;

  cout << "Change -90 to planetographic: " << p.ToPlanetographic(-90.0) << endl;
  cout << "Change -45 to planetographic: " << p.ToPlanetographic(-45.0) << endl;
  cout << "Change   0 to planetographic: " << p.ToPlanetographic(0.0) << endl;
  cout << "Change  45 to planetographic: " << p.ToPlanetographic(45.0) << endl;
  cout << "Change  90 to planetographic: " << p.ToPlanetographic(90.0) << endl;

  cout << "Change -90 to planetocentric: " << p.ToPlanetocentric(-90.0) << endl;
  cout << "Change -45 to planetocentric: " << p.ToPlanetocentric(-45.0) << endl;
  cout << "Change   0 to planetocentric: " << p.ToPlanetocentric(0.0) << endl;
  cout << "Change  45 to planetocentric: " << p.ToPlanetocentric(45.0) << endl;
  cout << "Change  90 to planetocentric: " << p.ToPlanetocentric(90.0) << endl;
  cout << endl;

  cout << "ocentric to ographic to ocentric = " << p.ToPlanetocentric(p.ToPlanetographic(45.0)) << endl;
  cout << "ographic to ocentric to ographic = " << p.ToPlanetographic(p.ToPlanetocentric(45.0)) << endl;
  cout << endl;

  cout << "Testing unordered latitude range" << endl;
  mg += Isis::PvlKeyword("MinimumLatitude",45.0);
  mg += Isis::PvlKeyword("MaximumLatitude",-80.0);
  mg += Isis::PvlKeyword("MinimumLongitude",15.0);
  mg += Isis::PvlKeyword("MaximumLongitude",-190.0);
  Doit(lab);
  cout << endl;

  cout << "Testing invalid minimum latitude" << endl;
  mg["MinimumLatitude"] = -95.0;
  Doit(lab);
  cout << endl;

  cout << "Testing invalid maximum latitude" << endl;
  mg["MinimumLatitude"].SetValue(45.0, "units");
  mg["MaximumLatitude"].SetValue(95.0, "units");
  Doit(lab);
  cout << endl;

  cout << "Testing unordered longitude range" << endl;
  mg["MaximumLatitude"].SetValue(80.0, "units");
  Doit(lab);
  cout << endl;
  mg["MaximumLongitude"] = 190.0;

  MyProjection p2(lab);
  cout << "Has as longitude range:  " << p2.HasGroundRange() << endl;
  cout << "Minimum latitude:        " << p2.MinimumLatitude() << endl;
  cout << "Maximum latitude:        " << p2.MaximumLatitude() << endl;
  cout << "Minimum longitude:       " << p2.MinimumLongitude() << endl;
  cout << "Maximum longitude:       " << p2.MaximumLongitude() << endl;

  double minX,maxX,minY,maxY;
  p2.XYRange(minX,maxX,minY,maxY);
  cout << "Minimum X:              " << minX << endl;
  cout << "Maximum X:              " << maxX << endl;
  cout << "Minimum Y:              " << minY << endl;
  cout << "Maximum Y:              " << maxY << endl;
  cout << endl;

  cout << "Testing Ground coordinate routines" << endl;
  cout << "Setting latitude to (-91,  0):  " << p2.SetGround(-91.0,0.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Setting latitude to ( 91,  0):  " << p2.SetGround(91.0,0.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Setting position to (60,  -5):  " << p2.SetGround(60.0,-5.0) << endl;
  cout << "Is Good:                        " << p2.IsGood() << endl;
  cout << "Latitude:                       " << p2.Latitude() << endl;
  cout << "Longitude:                      " << p2.Longitude() << endl;
  cout << "XCoord:                         " << p2.XCoord() << endl;
  cout << "YCoord:                         " << p2.YCoord() << endl;
  cout << "UniversalLatitude:              " << p2.UniversalLatitude() << endl;
  cout << "UniversalLongitude:             " << p2.UniversalLongitude() << endl;
  cout << endl;


  cout << "Testing Universal Ground coordinate routines" << endl;
  cout << "Setting position to (57.3920057293825,  355):  " << p2.SetUniversalGround(57.3920057293825,-5.0) << endl;
  cout << "Is Good:                                       " << p2.IsGood() << endl;
  cout << "Latitude:                                      " << p2.Latitude() << endl;
  cout << "Longitude:                                     " << p2.Longitude() << endl;
  cout << "XCoord:                                        " << p2.XCoord() << endl;
  cout << "YCoord:                                        " << p2.YCoord() << endl;
  cout << "UniversalLatitude:                             " << p2.UniversalLatitude() << endl;
  cout << "UniversalLongitude:                            " << p2.UniversalLongitude() << endl;
  cout << endl;

  cout << "Testing projection coordinate routines" << endl;
  cout << "Setting x/y position to (-2550,15):  " << p2.SetCoordinate(-2250.0,15.0) << endl;
  cout << "Is Good:                             " << p2.IsGood() << endl;
  cout << "Latitude:                            " << p2.Latitude() << endl;
  cout << "Longitude:                           " << p2.Longitude() << endl;
  cout << "XCoord:                              " << p2.XCoord() << endl;
  cout << "YCoord:                              " << p2.YCoord() << endl;
  cout << "UniversalLatitude:                   " << p2.UniversalLatitude() << endl;
  cout << "UniversalLongitude:                  " << p2.UniversalLongitude() << endl;
  cout << "WorldX:                              " << p2.WorldX() << endl;
  cout << "WorldY:                              " << p2.WorldY() << endl;
  cout << endl;

  p2.SetWorldMapper(new MyMapper());

  cout << "Testing world coordinate routines" << endl;
  cout << "Setting world x/y position to (-4500,45):  " << p2.SetWorld(-4500.0,45.0) << endl;
  cout << "Is Good:                                   " << p2.IsGood() << endl;
  cout << "Latitude:                                  " << p2.Latitude() << endl;
  cout << "Longitude:                                 " << p2.Longitude() << endl;
  cout << "XCoord:                                    " << p2.XCoord() << endl;
  cout << "YCoord:                                    " << p2.YCoord() << endl;
  cout << "UniversalLatitude:                         " << p2.UniversalLatitude() << endl;
  cout << "UniversalLongitude:                        " << p2.UniversalLongitude() << endl;
  cout << "WorldX:                                    " << p2.WorldX() << endl;
  cout << "WorldY:                                    " << p2.WorldY() << endl;
  cout << "ToProjectionX (-4500):                     " << p2.ToProjectionX(-4500.0) << endl;
  cout << "ToProjectionY (45):                        " << p2.ToProjectionY(45.0) << endl;
  cout << "ToWorldX:                                  " << p2.ToWorldX(p2.ToProjectionX(-4500.0)) << endl;
  cout << "ToWorldY:                                  " << p2.ToWorldY(p2.ToProjectionY(45.0)) << endl;
  cout << "Resolution:                                " << p2.Resolution() << endl;
  cout << "Scale:                                     " << p2.Scale() << endl;
  cout << "TrueScaleLatitude:                         " << p2.TrueScaleLatitude() << endl;
  cout << endl;

  cout << "Testing IsSky method" << endl;
  cout << p2.IsSky() << endl;
  mg += Isis::PvlKeyword("TargetName","SKY");
  Doit(lab);
  MyProjection p3(lab);
  cout << p3.IsSky() << endl;
  cout << endl;

  cout << "Testing string routines" << endl;
  cout << p2.LatitudeTypeString() << endl;
  cout << p2.LongitudeDirectionString() << endl;
  cout << p2.LongitudeDomainString() << endl;
  cout << endl;

  cout << "Testing Name and comparision routines" << endl;
  cout << "Name:        " << p2.Name() << endl;
  cout << "operator==:  " << (p == p2) << endl;
  cout << "operator!=:  " << (p != p2) << endl;

  mg["LongitudeDirection"] = "PositiveWest";
  mg["LongitudeDomain"] = 180;
  EmptyProjection noproj(lab);
  cout << endl;

  cout << "Testing no projection" << endl;
  noproj.SetUniversalGround(45.0,270.0);
  cout << "Latitude:    " << noproj.Latitude() << endl;
  cout << "Longitude:   " << noproj.Longitude() << endl;
  cout << endl;

  cout << "Testing radius methods " << endl;
  cout << noproj.LocalRadius() << endl;
  cout << noproj.LocalRadius(0.0) << endl;
  cout << noproj.LocalRadius(90.0) << endl;
  cout << noproj.LocalRadius(-90.0) << endl;

  cout << "Testing compute methods " << endl;
  p.Output();
  cout << endl;

  cout << "Testing static conversion methods " << endl;
  cout << "0 degrees in hours: " << p.ToHours(0.0) << endl;
  cout << "0 degrees in HMS format: " << p.ToHMS(0.0) << endl;
  cout << "0 degrees in DMS format: " << p.ToDMS(0.0) << endl;
  cout << "30.5 degrees in hours: " << p.ToHours(30.5) << endl;
  cout << "30.5 degrees in HMS format: " << p.ToHMS(30.5) << endl;
  cout << "30.5 degrees in DMS format: " << p.ToDMS(30.5) << endl;
  cout << "40.3472 degrees in hours: " << p.ToHours(40.3472) << endl;
  cout << "40.3472 degrees in HMS format: " << p.ToHMS(40.3472) << endl;
  cout << "40.3472 degrees in DMS format: " << p.ToDMS(40.3472) << endl;
  cout << "45 degrees in Hours: " << p.ToHours(45.0) << endl;
  cout << "45 degrees in HMS format: " << p.ToHMS(45.0) << endl;
  cout << "45 degrees in DMS format: " << p.ToDMS(45.0) << endl;
  cout << "180 degrees in Hours: " << p.ToHours(180.0) << endl;
  cout << "180 degrees in HMS format: " << p.ToHMS(180.0) << endl;
  cout << "180 degrees in DMS format: " << p.ToDMS(180.0) << endl;
  cout << "360 degrees in Hours: " << p.ToHours(360.0) << endl;
  cout << "360 degrees in HMS format: " << p.ToHMS(360.0) << endl;
  cout << "360 degrees in DMS format: " << p.ToDMS(360.0) << endl;
  cout << "-390 To180Domain:          " << p.To180Domain(-390) << endl;
  cout << "-390 To360Domain:          " << p.To360Domain(-390) << endl;
  cout << "50 to Planetocentric (sphere): " << p.ToPlanetocentric(50, 180000, 180000) << endl;
  cout << "50 to Planetographic (sphere): " << p.ToPlanetographic(50, 180000, 180000) << endl;
  cout << "-30 ToPositiveEast (180 domain): " << p.ToPositiveEast(-30, 180) << endl;
  cout << "30 ToPositiveWest (360 domain):  " << p.ToPositiveEast(30, 360) << endl;
  cout << endl;
  cout << endl;

  cout << "Testing other static methods " << endl;
  try {
    Isis::PvlGroup radii = Isis::Projection::TargetRadii ("Mars");
    cout << "Mars equatorial radius: " << radii["EquatorialRadius"] << endl;
    cout << "Mars polar radius: " << radii["PolarRadius"] << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  cout << "Rotation Tests" << endl;
  mg += Isis::PvlKeyword("Rotation",90.0);
  mg["LongitudeDirection"] = "PositiveEast";
  MyProjection p4(lab);
  cout << "Rotation:     " << p4.Rotation() << endl;
  cout << "Testing Ground coordinate routines" << endl;
  cout << "Setting latitude to (-91,  0):  " << p4.SetGround(-91.0,0.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Setting latitude to ( 91,  0):  " << p4.SetGround(91.0,0.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Setting position to (60,  -5):  " << p4.SetGround(60.0,-5.0) << endl;
  cout << "Is Good:                        " << p4.IsGood() << endl;
  cout << "Latitude:                       " << p4.Latitude() << endl;
  cout << "Longitude:                      " << p4.Longitude() << endl;
  cout << "XCoord:                         " << p4.XCoord() << endl;
  cout << "YCoord:                         " << p4.YCoord() << endl;
  cout << "UniversalLatitude:              " << p4.UniversalLatitude() << endl;
  cout << "UniversalLongitude:             " << p4.UniversalLongitude() << endl;
  cout << endl;

  cout << "Testing projection coordinate routines" << endl;
  cout << "Setting x/y position to (150,50):  " << p4.SetCoordinate(150.0,50.0) << endl;
  cout << "Is Good:                             " << p4.IsGood() << endl;
  cout << "Latitude:                            " << p4.Latitude() << endl;
  cout << "Longitude:                           " << p4.Longitude() << endl;
  cout << "XCoord:                              " << p4.XCoord() << endl;
  cout << "YCoord:                              " << p4.YCoord() << endl;
  cout << "UniversalLatitude:                   " << p4.UniversalLatitude() << endl;
  cout << "UniversalLongitude:                  " << p4.UniversalLongitude() << endl;
  cout << "WorldX:                              " << p4.WorldX() << endl;
  cout << "WorldY:                              " << p4.WorldY() << endl;
  cout << endl;

  Isis::Pvl mapping;
  mapping.AddGroup(p4.Mapping());
  cout << "Testing Mapping() methods" << endl;
  cout << "Mapping() = " << endl;
  cout << mapping << endl;
  mapping.DeleteGroup("Mapping");
  mapping.AddGroup(p4.MappingLatitudes());
  cout << "MappingLatitudes() = " << endl;
  cout << mapping << endl;
  mapping.DeleteGroup("Mapping");
  mapping.AddGroup(p4.MappingLongitudes());
  cout << "MappingLongitudes() = " << endl;
  cout << mapping << endl;
  mapping.DeleteGroup("Mapping");
  cout << endl;
}


void Doit (Isis::Pvl &lab) {
  try {
    MyProjection p(lab);
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
}
