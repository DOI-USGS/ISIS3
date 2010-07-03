

#include <iostream>
#include <iomanip>
#include <cmath>
#include "LunarAzimuthalEqualArea.h"
#include "iException.h"
#include "ProjectionFactory.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "UNIT TEST FOR LunarAzimuthalEqualArea\n\n";

  Isis::Pvl lab;
  lab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup & mapGroup = lab.FindGroup("Mapping");
  mapGroup += Isis::PvlKeyword("EquatorialRadius",6378206.4);
  mapGroup += Isis::PvlKeyword("PolarRadius",6378206.4);
  mapGroup += Isis::PvlKeyword("LatitudeType","Planetographic");
  mapGroup += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
  mapGroup += Isis::PvlKeyword("LongitudeDomain",180);
  mapGroup += Isis::PvlKeyword("MinimumLatitude",-30.0);
  mapGroup += Isis::PvlKeyword("MaximumLatitude",-15.0);
  mapGroup += Isis::PvlKeyword("MinimumLongitude",-30.0);
  mapGroup += Isis::PvlKeyword("MaximumLongitude",-15.0);
  mapGroup += Isis::PvlKeyword("MaximumLibration", 1);
  mapGroup += Isis::PvlKeyword("ProjectionName",
      "LunarAzimuthalEqualArea");

  try {
    Isis::Projection & p = *Isis::ProjectionFactory::Create(lab);
    
    cout << "\n******* Test SetGround and SetCoordinate methods ********"
       << "\n\nfor all lat and lon in [-90..90] step 45"
       << "\n  first call SetGround"
       << "\n  then call SetCoordinate"
       << "\n  trigger fail if SetCoordinate doesn't"
       << "\n  return the original coordinate"
       << "\n  note that at the poles the lon is not relevant!\n\n";
  
    cout << std::setprecision(11);
    for (int i = -90; i <= 90; i += 45)
    {
      for (int j = -90; j <= 90; j+= 45)
      {
        cout << " original Lat: " << setw(4) << i << "   original Lon: "
             << setw(4) << j << "\n"
             << "testing SetCoordinate and SetGround";

        p.SetGround(i * 1.0, j * 1.0);
        p.SetCoordinate(p.XCoord(), p.YCoord());

        if ((fabs(fabs(p.Latitude()) - fabs(i)) > 0.00001) ||
            (fabs(i) != 90 &&  fabs(fabs(p.Longitude()) - fabs(j)) > 0.00001))
          cout << " ......................... [ FAILED ]\n\n";
        else
          cout << " ......................... [   OK   ]\n\n";
      }
    }
    cout << "\nTesting for SetGround and SetCoordiante done!"
         << "\nNow test the Name method, the == operator and finally"
         << "\nprint the output of the XYRange and Mapping methods...\n\n\n";
    
    cout << "Name method returns \"LunarAzimuthalEqualArea\"................ ";
    if (p.Name() == "LunarAzimuthalEqualArea")
      cout << "[   OK   ]\n";
    else
      cout << "[ FAILED ]\n";

    Isis::Projection *s = &p;
    cout << "\nIsis::Projection * s = &p; // p is this projection"
         << "\n(*s == p) returns ";
    if (*s == p)
      cout << "true! ..................................... [   OK   ]\n";
    else
      cout << "false! .................................... [ FAILED ]\n";
    mapGroup["PolarRadius"].SetValue(42);
    Isis::Pvl tmp1;
    Isis::Projection & p2 = *Isis::ProjectionFactory::Create(lab);
    tmp1.AddGroup(p2.Mapping());
    s = &p2;
    cout << "created a second Projection reference p2"
         << "\nusing the same pvl as p except for that PolarRadius = 42"
         << "\nPvl for p2...\n" << tmp1
         << "\ns = &p2"
         << "\n(*s == p) returns ";
    if (*s == p)
      cout << "true! ..................................... [ FAILED ]\n";
    else
      cout << "false! .................................... [   OK   ]\n";

    double minX, maxX, minY, maxY;
    p.XYRange(minX,maxX,minY,maxY);
    cout << "\nXYRange..."
         << "\n  Minimum X:  " << minX
         << "\n  Maximum X:  " << maxX
         << "\n  Minimum Y:  " << minY
         << "\n  Maximum Y:  " << maxY << "\n";
    
    Isis::Pvl tmp2;
    tmp2.AddGroup(p.Mapping());
  
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}

