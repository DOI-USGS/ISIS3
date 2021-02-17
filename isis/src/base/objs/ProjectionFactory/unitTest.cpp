/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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


  try {

     cout << endl << "Test for ProjectionFactory's Create method when the maximum latitude = 0.0"
          << endl;

    Pvl southMap;
    int southLines,southSamples;
    southMap.addGroup(PvlGroup("Mapping"));
    PvlGroup   &mapGroupSouth = southMap.findGroup("Mapping");
    mapGroupSouth += PvlKeyword("ProjectionName", "Equirectangular");
    mapGroupSouth += PvlKeyword("CenterLongitude", toString(0.0));
    mapGroupSouth += PvlKeyword("CenterLatitude", toString(0.0));
    mapGroupSouth += PvlKeyword("EquatorialRadius", toString(13400.0));
    mapGroupSouth += PvlKeyword("PolarRadius", toString(9200.0));
    mapGroupSouth += PvlKeyword("LatitudeType", "Planetocentric");
    mapGroupSouth += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroupSouth += PvlKeyword("LongitudeDomain", toString(360));
    mapGroupSouth += PvlKeyword("MinimumLatitude", toString(-60.0));
    mapGroupSouth += PvlKeyword("MaximumLatitude", toString(0.0));
    mapGroupSouth += PvlKeyword("MinimumLongitude", toString(0.0));
    mapGroupSouth += PvlKeyword("MaximumLongitude", toString(360.0));
    mapGroupSouth += PvlKeyword("PixelResolution", toString(10.0));


    Pvl northMap;
    int northLines,northSamples;
    northMap.addGroup(PvlGroup("Mapping"));
    PvlGroup &mapGroupNorth = northMap.findGroup("Mapping");
    mapGroupNorth += PvlKeyword("ProjectionName", "Equirectangular");
    mapGroupNorth += PvlKeyword("CenterLongitude", toString(0.0));
    mapGroupNorth += PvlKeyword("CenterLatitude", toString(0.0));
    mapGroupNorth += PvlKeyword("EquatorialRadius", toString(13400.0));
    mapGroupNorth += PvlKeyword("PolarRadius", toString(9200.0));
    mapGroupNorth += PvlKeyword("LatitudeType", "Planetocentric");
    mapGroupNorth += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroupNorth += PvlKeyword("LongitudeDomain", toString(360));
    mapGroupNorth += PvlKeyword("MinimumLatitude", toString(0.0));
    mapGroupNorth += PvlKeyword("MaximumLatitude", toString(60.0));
    mapGroupNorth += PvlKeyword("MinimumLongitude", toString(0.0));
    mapGroupNorth += PvlKeyword("MaximumLongitude", toString(360.0));
    mapGroupNorth += PvlKeyword("PixelResolution", toString(10.0));


    TProjection *projNorth = (TProjection *) ProjectionFactory::CreateForCube(southMap,
                                                                 southSamples, southLines);
    TProjection *projSouth  = (TProjection *) ProjectionFactory::CreateForCube(northMap,
                                                                 northSamples, northLines);

    projNorth->SetWorld(245.0, 355.0);
    projSouth->SetWorld(245.0, 355.0);



    cout << "South Map Samples = "  << southSamples << endl;
    cout << "North Map Samples = "  << northSamples << endl;
    cout << "South Map Lines = "    << southLines << endl;
    cout << "North Map Lines = "    << northLines << endl;




  }

  catch (IException &e) {

    ReportError (e.toString() );

  }


  try {

     cout << endl << "Test for ProjectionFactory's Create method when the maximum longitude = 0.0"
          << endl;

    Pvl eastMap;
    int eastLines,eastSamples;
    eastMap.addGroup(PvlGroup("Mapping"));
    PvlGroup   &mapGroupEast = eastMap.findGroup("Mapping");
    mapGroupEast += PvlKeyword("ProjectionName", "Equirectangular");
    mapGroupEast += PvlKeyword("CenterLongitude", toString(0.0));
    mapGroupEast += PvlKeyword("CenterLatitude", toString(0.0));
    mapGroupEast += PvlKeyword("EquatorialRadius", toString(13400.0));
    mapGroupEast += PvlKeyword("PolarRadius", toString(9200.0));
    mapGroupEast += PvlKeyword("LatitudeType", "Planetocentric");
    mapGroupEast += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroupEast += PvlKeyword("LongitudeDomain", toString(360));
    mapGroupEast += PvlKeyword("MinimumLatitude", toString(0.0));
    mapGroupEast += PvlKeyword("MaximumLatitude", toString(60.0));
    mapGroupEast += PvlKeyword("MinimumLongitude", toString(0.0));
    mapGroupEast += PvlKeyword("MaximumLongitude", toString(90.0));
    mapGroupEast += PvlKeyword("PixelResolution", toString(10.0));


    Pvl westMap;
    int westLines,westSamples;
    westMap.addGroup(PvlGroup("Mapping"));
    PvlGroup &mapGroupWest = westMap.findGroup("Mapping");
    mapGroupWest += PvlKeyword("ProjectionName", "Equirectangular");
    mapGroupWest += PvlKeyword("CenterLongitude", toString(0.0));
    mapGroupWest += PvlKeyword("CenterLatitude", toString(0.0));
    mapGroupWest += PvlKeyword("EquatorialRadius", toString(13400.0));
    mapGroupWest += PvlKeyword("PolarRadius", toString(9200.0));
    mapGroupWest += PvlKeyword("LatitudeType", "Planetocentric");
    mapGroupWest += PvlKeyword("LongitudeDirection", "PositiveEast");
    mapGroupWest += PvlKeyword("LongitudeDomain", toString(360));
    mapGroupWest += PvlKeyword("MinimumLatitude", toString(0.0));
    mapGroupWest += PvlKeyword("MaximumLatitude", toString(60.0));
    mapGroupWest += PvlKeyword("MinimumLongitude", toString(-90.0));
    mapGroupWest += PvlKeyword("MaximumLongitude", toString(0.0));
    mapGroupWest += PvlKeyword("PixelResolution", toString(10.0));


    TProjection *projEast = (TProjection *) ProjectionFactory::CreateForCube(eastMap,
                                                                 eastSamples, eastLines);
    TProjection *projWest  = (TProjection *) ProjectionFactory::CreateForCube(westMap,
                                                                 westSamples, westLines);

    projEast->SetWorld(245.0, 355.0);
    projWest->SetWorld(245.0, 355.0);



    cout << "West Map Samples = "  << westSamples << endl;
    cout << "East Map Samples = "  << eastSamples << endl;
    cout << "West Map Lines = "    << westLines << endl;
    cout << "East Map Lines = "    << eastLines << endl;




  }

  catch (IException &e) {

    ReportError (e.toString() );

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

