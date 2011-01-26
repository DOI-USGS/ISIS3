#include "Isis.h"

#include <map>
#include <sstream>

#include <QString>

#include "Brick.h"
#include "CameraFactory.h"
#include "ControlNet.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "geos/util/GEOSException.h"
#include "ID.h"
#include "iException.h"
#include "iString.h"
#include "ImageOverlap.h"
#include "ImageOverlapSet.h"
#include "PolygonSeeder.h"
#include "PolygonSeederFactory.h"
#include "PolygonTools.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"
#include "UniversalGroundMap.h"
#include "SpecialPixel.h"

#include "ImagePolygon.h"

enum SeedDomain {
  XY,
  SampleLine
};


using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  SerialNumberList serialNumbers(ui.GetFilename("FROMLIST"));

  // Get the AutoSeed PVL internalized
  Pvl seedDef(ui.GetFilename("SEEDDEF"));

  PolygonSeeder *seeder = PolygonSeederFactory::Create(seedDef);
  Pvl invalidInput = seeder->InvalidInput();
  PvlGroup &unusedDefKeywords = invalidInput.FindGroup(
                                  "PolygonSeederAlgorithm", Pvl::Traverse);

  // Get the distance from the edge of an image a measure must be
  double pixelsFromEdge = -1.0;
  if(seedDef.HasKeyword("PixelsFromEdge", Pvl::Traverse)) {
    pixelsFromEdge = seedDef.FindKeyword("PixelsFromEdge", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("PixelsFromEdge"))
      unusedDefKeywords.DeleteKeyword("PixelsFromEdge");
  }

  // Get the Emission range
  double minEmission = 0.0;
  double maxEmission = 180.0;
  if(seedDef.HasKeyword("MinEmission", Pvl::Traverse)) {
    minEmission = seedDef.FindKeyword("MinEmission", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MinEmission"))
      unusedDefKeywords.DeleteKeyword("MinEmission");
  }
  if(seedDef.HasKeyword("MaxEmission", Pvl::Traverse)) {
    maxEmission = seedDef.FindKeyword("MaxEmission", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MaxEmission"))
      unusedDefKeywords.DeleteKeyword("MaxEmission");
  }

  // Get the Incidence range
  double minIncidence = 0.0;
  double maxIncidence = 180.0;
  if(seedDef.HasKeyword("MinIncidence", Pvl::Traverse)) {
    minIncidence = seedDef.FindKeyword("MinIncidence", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MinIncidence"))
      unusedDefKeywords.DeleteKeyword("MinIncidence");
  }
  if(seedDef.HasKeyword("MaxIncidence", Pvl::Traverse)) {
    maxIncidence = seedDef.FindKeyword("MaxIncidence", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MaxIncidence"))
      unusedDefKeywords.DeleteKeyword("MaxIncidence");
  }

  // Get the DN range
  bool hasDNRestriction = false;
  double minDN = -DBL_MAX;
  double maxDN = DBL_MAX;
  if(seedDef.HasKeyword("MinDN", Pvl::Traverse)) {
    minDN = seedDef.FindKeyword("MinDN", Pvl::Traverse);
    hasDNRestriction = true;
    if(unusedDefKeywords.HasKeyword("MinDN"))
      unusedDefKeywords.DeleteKeyword("MinDN");
  }
  if(seedDef.HasKeyword("MaxDN", Pvl::Traverse)) {
    maxDN = seedDef.FindKeyword("MaxDN", Pvl::Traverse);
    hasDNRestriction = true;
    if(unusedDefKeywords.HasKeyword("MaxDN"))
      unusedDefKeywords.DeleteKeyword("MaxDN");
  }

  // Get the resolution
  double minResolution = 0.0;
  double maxResolution = 0.0;
  if(seedDef.HasKeyword("MinResolution", Pvl::Traverse)) {
    minResolution  = seedDef.FindKeyword("MinResolution", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MinResolution"))
      unusedDefKeywords.DeleteKeyword("MinResolution");
  }
  if(seedDef.HasKeyword("MaxResolution", Pvl::Traverse)) {
    maxResolution  = seedDef.FindKeyword("MaxResolution", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("MaxResolution"))
      unusedDefKeywords.DeleteKeyword("MaxResolution");
  }

  // Get seed domain for unit conversion, no keyword == XY
  SeedDomain seedDomain = XY;
  if(seedDef.HasKeyword("SeedDomain", Pvl::Traverse)) {
    iString domain = (std::string) seedDef.FindKeyword("SeedDomain", Pvl::Traverse);
    if(unusedDefKeywords.HasKeyword("SeedDomain"))
      unusedDefKeywords.DeleteKeyword("SeedDomain");

    if(domain.UpCase() == "SAMPLELINE") {
      seedDomain = SampleLine;
    }
    else if(domain.UpCase() != "XY") {
      iString msg = "Invalid value provided for keywork [SeedDomain]";
      msg += " Possible values include [XY, SampleLine]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Grab the labels from the first filename in the SerialNumberList to get
  // some info
  Pvl cubeLab(serialNumbers.Filename(0));

  // Construct a Projection for converting between Lon/Lat and X/Y
  // This is used inside the seeding algorithms.
  // Note: Should this be an option to include this in the program?
  PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
  string target = inst["TargetName"];
  PvlGroup radii = Projection::TargetRadii(target);
  Isis::Pvl maplab;
  maplab.AddGroup(Isis::PvlGroup("Mapping"));
  Isis::PvlGroup &mapGroup = maplab.FindGroup("Mapping");
  mapGroup += Isis::PvlKeyword("EquatorialRadius", (string)radii["EquatorialRadius"]);
  mapGroup += Isis::PvlKeyword("PolarRadius", (string)radii["PolarRadius"]);
  mapGroup += Isis::PvlKeyword("LatitudeType", "Planetocentric");
  mapGroup += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGroup += Isis::PvlKeyword("LongitudeDomain", 360);
  mapGroup += Isis::PvlKeyword("CenterLatitude", 0);
  mapGroup += Isis::PvlKeyword("CenterLongitude", 0);
  mapGroup += Isis::PvlKeyword("ProjectionName", "Sinusoidal");

  //PolygonSeeder *seeder = PolygonSeederFactory::Create(seedDef);

  Projection *proj = NULL;
  UniversalGroundMap *ugmap = NULL;
  if(seedDomain == XY) {
    proj = Isis::ProjectionFactory::Create(maplab);
  }
  else if(seedDomain == SampleLine) {
    Cube cube;
    cube.Open(serialNumbers.Filename(0));
    ugmap = new UniversalGroundMap(*cube.Label());
  }

  // Create the control net to store the points in.
  ControlNet cnet;
  cnet.SetTarget(target);
  cnet.SetNetworkId(ui.GetString("NETWORKID"));
  cnet.SetUserName(Isis::Application::UserName());
  cnet.SetDescription(ui.GetString("DESCRIPTION"));

  // Set up an automatic id generator for the point ids
  ID pointId = ID(ui.GetString("POINTID"));

  // Find all the overlaps between the images in the FROMLIST
  // The overlap polygon coordinates are in Lon/Lat order
  ImageOverlapSet overlaps;
  overlaps.ReadImageOverlaps(ui.GetFilename("OVERLAPLIST"));

  // Create a Universal Ground Map (UGM) for each image in the list
  int stats_noOverlap = 0;
  int stats_tolerance = 0;

  map<std::string, UniversalGroundMap *> gMaps;
  for(int sn = 0; sn < serialNumbers.Size(); ++sn) {
    // Create the UGM for the cube associated with this SN
    Pvl lab = Pvl(serialNumbers.Filename(sn));
    gMaps.insert(std::pair<std::string, UniversalGroundMap *>
                 (serialNumbers.SerialNumber(sn), new UniversalGroundMap(lab)));
  }

  stringstream errors(stringstream::in | stringstream::out);
  int errorNum = 0;

  // Process each overlap area
  //   Seed measurments into it
  //   Store the measurments in the control network

  bool previousControlNet = ui.WasEntered("CNET");

  vector< geos::geom::Point *> points;
  if(previousControlNet) {

    ControlNet precnet(ui.GetFilename("CNET"));

    Progress progress;
    progress.SetText("Calculating Provided Control Net");
    progress.SetMaximumSteps(precnet.GetNumPoints());
    progress.CheckStatus();

    for(int i = 0 ; i < precnet.GetNumPoints(); i ++) {
      ControlPoint *cp = precnet.GetPoint(i);
      ControlMeasure *cm = cp->GetMeasure(0);
      if(cp->HasReference()) {
        cm = cp->GetReferenceMeasure();
      }

      iString c = serialNumbers.Filename(cm->GetCubeSerialNumber());
      Pvl cubepvl(c);
      Camera *cam = CameraFactory::Create(cubepvl);
      cam->SetImage(cm->GetSample(), cm->GetLine());


      points.push_back(Isis::globalFactory.createPoint(geos::geom::Coordinate(
                         cam->UniversalLongitude(), cam->UniversalLatitude())));

      delete cam;
      cam = NULL;

      progress.CheckStatus();
    }

  }

  Progress progress;
  progress.SetText("Seeding Points");
  progress.SetMaximumSteps(overlaps.Size());
  progress.CheckStatus();

  int cpIgnoredCount = 0;
  int cmIgnoredCount = 0;

  for(int ov = 0; ov < overlaps.Size(); ++ov) {
    progress.CheckStatus();

    if(overlaps[ov]->Size() == 1) {
      stats_noOverlap++;
      continue;
    }

    // Checks if this overlap was already seeded
    if(previousControlNet) {

      // Grabs the Multipolygon's Envelope for Lat/Lon comparison
      const geos::geom::MultiPolygon *lonLatPoly = overlaps[ov]->Polygon();

      bool overlapSeeded = false;
      for(unsigned int j = 0; j < lonLatPoly->getNumGeometries()  &&  !overlapSeeded; j ++) {
        const geos::geom::Geometry *lonLatGeom = lonLatPoly->getGeometryN(j);

        // Checks if Control Point is in the MultiPolygon using Lon/Lat
        for(unsigned int i = 0 ; i < points.size()  &&  !overlapSeeded; i ++) {
          if(lonLatGeom->contains(points[i])) overlapSeeded = true;
        }
      }

      if(overlapSeeded) continue;
    }

    // Seed this overlap with points
    const geos::geom::MultiPolygon *polygonOverlaps = overlaps[ov]->Polygon();
    std::vector<geos::geom::Point *> points;

    try {
      geos::geom::MultiPolygon *mp = NULL;
      if(seedDomain == XY) {
        mp = PolygonTools::LatLonToXY(*polygonOverlaps, proj);
      }
      else if(seedDomain == SampleLine) {
        mp = PolygonTools::LatLonToSampleLine(*polygonOverlaps, ugmap);
      }
      points = seeder->Seed(mp);
    }
    catch(iException &e) {

      if(ui.WasEntered("ERRORS")) {

        if(errorNum > 0) {
          errors << endl;
        }
        errorNum ++;

        errors << e.PvlErrors().Group(0).FindKeyword("Message")[0];
        for(int serNum = 0; serNum < overlaps[ov]->Size(); serNum++) {
          if(serNum == 0) {
            errors << ": ";
          }
          else {
            errors << ", ";
          }
          errors << (*overlaps[ov])[serNum];
        }
      }

      e.Clear();
      continue;
    }

    // No points were seeded in this polygon, so collect some stats and move on
    if(points.size() == 0) {
      stats_tolerance++;
      continue;
    }

    vector<geos::geom::Point *> seed;
    if(seedDomain == XY) {
      // Convert the X/Y points back to Lat/Lon points
      for(unsigned int pt = 0; pt < points.size(); pt ++) {
        if(proj->SetCoordinate(points[pt]->getX(), points[pt]->getY())) {
          seed.push_back(Isis::globalFactory.createPoint(
                           geos::geom::Coordinate(proj->UniversalLongitude(),
                                                  proj->UniversalLatitude())));
        }
        else {
          iString msg = "Unable to convert from X/Y to a (lon,lat)";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
    }
    else if(seedDomain == SampleLine) {
      // Convert the Sample/Line points back to Lat/Lon points
      for(unsigned int pt = 0; pt < points.size(); pt ++) {
        if(ugmap->SetImage(points[pt]->getX(), points[pt]->getY())) {
          seed.push_back(Isis::globalFactory.createPoint(
                           geos::geom::Coordinate(ugmap->UniversalLongitude(),
                                                  ugmap->UniversalLatitude())));
        }
        else {
          iString msg = "Unable to convert from Sample/Line to a (lon,lat)";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
    }

    //   Create a control point for each seeded point in this overlap
    for(unsigned int point = 0; point < seed.size(); ++point) {

      ControlPoint *controlpt = new ControlPoint();
      controlpt->SetId(QString::fromStdString(pointId.Next()));
      controlpt->SetType(ControlPoint::Tie);

      // Create a measurment at this point for each image in the overlap area
      for(int sn = 0; sn < overlaps[ov]->Size(); ++sn) {
        bool ignore = false;

        // Get the line/sample of the lat/lon for this cube
        UniversalGroundMap *gmap = gMaps[(*overlaps[ov])[sn]];

        if(!gmap) {
          std::string msg = "Unable to create a Universal Ground for Serial Number [";
          msg += (*overlaps[ov])[sn] + "] The associated image is more than ";
          msg += "likely missing from your FROMLIST.";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }

        if(!gmap->SetUniversalGround(seed[point]->getY(), seed[point]->getX())) {
          // This error is more than likely due to floating point roundoff
          continue;
        }

        // Check the line/sample with the gmap for image edge
        if(pixelsFromEdge > gmap->Sample() || pixelsFromEdge > gmap->Line()
            || gmap->Sample() > gmap->Camera()->Samples() - pixelsFromEdge
            || gmap->Line() > gmap->Camera()->Lines() - pixelsFromEdge) {
          ignore = true;
        }

        // Check the Emission/Incidence Angle with the camera from the gmap
        if(gmap->Camera()->EmissionAngle() < minEmission ||
            gmap->Camera()->EmissionAngle() > maxEmission) {
          ignore = true;
        }
        if(gmap->Camera()->IncidenceAngle() < minIncidence ||
            gmap->Camera()->IncidenceAngle() > maxIncidence) {
          ignore = true;
        }

        // Check the DNs with the cube, Note: this is costly to do
        if(hasDNRestriction) {
          Cube cube;
          string c = serialNumbers.Filename((*overlaps[ov])[sn]);
          cube.Open(c);
          Isis::Brick brick(1, 1, 1, cube.PixelType());
          brick.SetBasePosition((int)gmap->Camera()->Sample(), (int)gmap->Camera()->Line(), (int)gmap->Camera()->Band());
          cube.Read(brick);
          if(Isis::IsSpecial(brick[0]) || brick[0] > maxDN || brick[0] < minDN) {
            ignore = true;
          }
        }

        // Check the Resolution with the camera from the gmap
        if(gmap->Resolution() < minResolution ||
            (maxResolution > 0.0 && gmap->Resolution() > maxResolution)) {
          ignore = true;
        }

        // Put the line/samp into a measurment
        ControlMeasure *measurment = new ControlMeasure();
        measurment->SetCoordinate(gmap->Sample(), gmap->Line(),
                                  ControlMeasure::Candidate);

        if(sn == 0)
          measurment->SetType(ControlMeasure::Reference);
        else
          measurment->SetType(ControlMeasure::Candidate);
        measurment->SetCubeSerialNumber((*(overlaps[ov]))[sn]);
        measurment->SetIgnored(ignore);

        if(ignore) {
          cmIgnoredCount ++;
        }

        controlpt->Add(measurment); //controlpt takes ownership
        measurment = NULL;
      }

      if(controlpt->GetNumValidMeasures() < 2) {
        controlpt->SetIgnored(true);
        cpIgnoredCount ++;
      }

      if(controlpt->GetNumMeasures() > 0) {
        cnet.AddPoint(controlpt); //cnet takes ownership
      }
      delete seed[point];

    } // End of create control points loop

  } // End of seeding loop

  // All done with the UGMs so delete them
  for(unsigned int sn = 0; sn < gMaps.size(); ++sn) {
    UniversalGroundMap *gmap = gMaps[serialNumbers.SerialNumber(sn)];
    delete gmap;
  }
  gMaps.clear();

  for(unsigned int i = 0 ; i < points.size(); i ++) {
    delete points[i];
    points[i] = NULL;
  }

  //Log the ERRORS file
  if(ui.WasEntered("ERRORS")) {
    string errorname = ui.GetFilename("ERRORS");
    std::ofstream errorsfile;
    errorsfile.open(errorname.c_str());
    errorsfile << errors.str();
    errorsfile.close();
  }

  // Make sure the control network is not empty
  if(cnet.GetNumPoints() == 0) {
    string msg = "The ouput Control Network [TO] is empty. This is likely due";
    msg += " to the input cubes failing to overlap.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // Write the control network out
  cnet.Write(ui.GetFilename("TO"));

  // create SeedDef group and add to print.prt
  PvlGroup pluginInfo = seeder->PluginParameters("SeedDefinition");
  pluginInfo.AddKeyword(PvlKeyword("MaxIncidence", maxIncidence));
  pluginInfo.AddKeyword(PvlKeyword("MaxEmission", maxEmission));
  Application::Log(pluginInfo);

  // inform user of any unused (invalid) keywords found in the def file
  if(unusedDefKeywords.Keywords() != 0) {
    PvlGroup unusedKeywords(unusedDefKeywords);
    unusedKeywords.SetName("InvalidKeyordsFoundInDefFile");
    Application::Log(unusedKeywords);
  }

  // calc # of points and measures for results group in print.prt
  int cpCount = cnet.GetNumPoints();
  int msCount = 0;
  for(int i = 0; i < cpCount; i++) {
    msCount += cnet.GetPoint(i)->GetNumMeasures();
  }

  // create Results group and add to print.prt
  PvlKeyword cpCountKeyword("ControlPointCount", iString(cpCount));
  PvlKeyword msCountKeyword("ControlMeasureCount", iString(msCount));
  PvlKeyword cpIgnoredCountKeyword("ControlPointsIgnored", iString(cpIgnoredCount));
  PvlKeyword cmIgnoredCountKeyword("ControlMeasuresIgnored", iString(cmIgnoredCount));

  PvlGroup resultsGrp("Results");
  resultsGrp.AddKeyword(cpCountKeyword);
  resultsGrp.AddKeyword(msCountKeyword);
  resultsGrp.AddKeyword(cpIgnoredCountKeyword);
  resultsGrp.AddKeyword(cmIgnoredCountKeyword);

  Application::Log(resultsGrp);

  if(seedDomain == XY) {
    delete proj;
    proj = NULL;
  }
  else if(seedDomain == SampleLine) {
    delete ugmap;
    ugmap = NULL;
  }

  delete seeder;
  seeder = NULL;

}
