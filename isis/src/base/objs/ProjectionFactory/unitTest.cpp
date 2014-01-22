#include <iostream>
#include <iomanip>
#include "IException.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "Projection.h"
#include "Preference.h"
#include "TProjection.h"

using namespace Isis;
using namespace std;

void ReportError(QString err);
  void doit(Pvl & lab);
  void doit2(Pvl & lab);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  try {
    cout << "Unit test for ProjectionFactory" << endl;

    Pvl lab;
    lab.addGroup(PvlGroup("Mapping"));
    PvlGroup &mapGroup = lab.findGroup("Mapping");
    mapGroup += PvlKeyword("EquatorialRadius", toString(3396190.0));
    mapGroup += PvlKeyword("PolarRadius", toString(3376200.0));

    mapGroup += PvlKeyword("LatitudeType", "Planetographic");
    mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroup += PvlKeyword("LongitudeDomain", toString(360));

    mapGroup += PvlKeyword("ProjectionName", "SimpleCylindrical");
    mapGroup += PvlKeyword("CenterLongitude", toString(220.0));

    cout << "Test for missing pixel resolution ... " << endl;
    doit(lab);
    doit2(lab);

    mapGroup += PvlKeyword("PixelResolution", toString(2000.0));
    cout << "Test for missing upper left X ... " << endl;
    doit(lab);

    mapGroup += PvlKeyword("UpperLeftCornerX", toString(-18000.0));
    cout << "Test for missing upper left Y ... " << endl;
    doit(lab);

    mapGroup += PvlKeyword("UpperLeftCornerY", toString(2062000.0));

    cout << "Testing conversion from image to ground ... " << endl;
    TProjection *proj = (TProjection *) ProjectionFactory::CreateFromCube(lab);
    proj->SetWorld(245.0, 355.0);
    cout << setprecision(14);
    cout << "Latitude:  " << proj->Latitude() << endl;
    cout << "Longitude: " << proj->Longitude() << endl;
    cout << endl;

    cout << "Testing conversion from ground to image ... " << endl;
    proj->SetGround(22.84279897788801, 227.9291842833142);
    cout << "Sample:    " << proj->WorldX() << endl;
    cout << "Line:      " << proj->WorldY() << endl;
    cout << endl;

    cout << "Testing missing ground range on create method ... " << endl;
    doit2(lab);

    mapGroup += PvlKeyword("MinimumLatitude", toString(10.8920539924144));
    mapGroup += PvlKeyword("MaximumLatitude", toString(34.7603960060206));
    mapGroup += PvlKeyword("MinimumLongitude", toString(219.72432466275));
    mapGroup += PvlKeyword("MaximumLongitude", toString(236.186050244411));
    mapGroup.deleteKeyword("UpperLeftCornerX");
    mapGroup.deleteKeyword("UpperLeftCornerY");

    cout << "Testing create method ... " << endl;
    int lines, samples;
    proj = (TProjection *) ProjectionFactory::CreateForCube(lab, samples, lines);
    cout << "Lines:       " << lines << endl;
    cout << "Samples:     " << samples << endl;
    cout << "UpperLeftX:  " << (double) mapGroup["UpperLeftCornerX"] << endl;
    cout << "UpperLeftY:  " << (double) mapGroup["UpperLeftCornerY"] << endl;
    cout << endl;

    cout << "Testing create method with existing cube labels" << endl;
    mapGroup.addKeyword(PvlKeyword("UpperLeftCornerX", toString(-16000.0)), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("UpperLeftCornerY", toString(2060000.0)), Pvl::Replace);

    Pvl lab2;
    PvlObject icube("IsisCube");
    PvlObject core("Core");
    PvlGroup dims("Dimensions");
    dims += PvlKeyword("Lines", toString(400));
    dims += PvlKeyword("Samples", toString(600));
    core.addGroup(dims);
    icube.addObject(core);
    icube.addGroup(mapGroup);
    lab2.addObject(icube);

    proj = (TProjection *) ProjectionFactory::CreateForCube(lab2, samples, lines);
    cout << "Lines:       " << lines << endl;
    cout << "Samples:     " << samples << endl;
    mapGroup = lab2.findGroup("Mapping", Pvl::Traverse);
    cout << "UpperLeftX:  " << (double) mapGroup["UpperLeftCornerX"] << endl;
    cout << "UpperLeftY:  " << (double) mapGroup["UpperLeftCornerY"] << endl;
    cout << endl;

    cout << "Label results" << endl;
    cout << lab2 << endl;
  }
  catch(IException &e) {
    e.print();
  }
  try {
    cout << endl << "Test for ProjectionFactory's Create with unsupported projection" << endl;

    Pvl lab;
    lab.addGroup(PvlGroup("Mapping"));
    PvlGroup &mapGroup = lab.findGroup("Mapping");
    mapGroup += PvlKeyword("EquatorialRadius", toString(3396190.0));
    mapGroup += PvlKeyword("PolarRadius", toString(3376200.0));

    mapGroup += PvlKeyword("LatitudeType", "Planetographic");
    mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroup += PvlKeyword("LongitudeDomain", toString(360));

    mapGroup += PvlKeyword("ProjectionName", "UnsupportedProjection");
    mapGroup += PvlKeyword("CenterLongitude", toString(220.0));
    mapGroup += PvlKeyword("PixelResolution", toString(2000.0));
    mapGroup += PvlKeyword("UpperLeftCornerX", toString(-18000.0));
    mapGroup += PvlKeyword("UpperLeftCornerY", toString(2062000.0));

    TProjection *proj = (TProjection *) ProjectionFactory::Create(lab);
    proj->SetWorld(245.0, 355.0);
  }
  catch(IException &e) {
    ReportError( e.toString() );
  }
}

void doit(Pvl &lab) {
  try {
    ProjectionFactory::CreateFromCube(lab);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;
}

void doit2(Pvl &lab) {
  try {
    int lines, samples;
    ProjectionFactory::CreateForCube(lab, samples, lines);
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl;
}

void ReportError(QString err) {
  cout << err.replace(QRegExp("\\[[^\\]]*\\.plugin\\]"), "[isis/lib/Projection.plugin]") << endl << endl;
}

