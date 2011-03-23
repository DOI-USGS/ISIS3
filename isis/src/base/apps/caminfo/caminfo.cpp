#include "Isis.h"

#include <cstdio>
#include <string>
#include <iostream>

#include "CamTools.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "iTime.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "Process.h"
#include "ProgramLauncher.h"
#include "Progress.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "Statistics.h"
#include "UserInterface.h"

#include <QList>
#include <QPair>

using namespace std;
using namespace Isis;

QPair<iString, iString> MakePair(iString key, iString val);
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<iString, iString> > *general,
                       QList< QPair<iString, iString> > *camstats,
                       QList< QPair<iString, iString> > *statistics,
                       BandGeometry *bandGeom);
void GenerateCSVOutput(Cube *incube,
                       QList< QPair<iString, iString> > *general,
                       QList< QPair<iString, iString> > *camstats,
                       QList< QPair<iString, iString> > *statistics,
                       BandGeometry *bandGeom);

void IsisMain() {
  const string caminfo_program  = "caminfo";
  UserInterface &ui = Application::GetUserInterface();

  QList< QPair<iString, iString> > *general = NULL, *camstats = NULL, *statistics = NULL;
  BandGeometry *bandGeom = NULL;

  // Get input filename
  Filename in = ui.GetFilename("FROM");

  // Get the format
  iString sFormat = ui.GetAsString("FORMAT");

  // if true then run spiceinit, xml default is FALSE
  // spiceinit will use system kernels
  if(ui.GetBoolean("SPICE")) {
    string parameters = "FROM=" + in.Expanded();
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
  }

  Process p;
  Cube *incube = p.SetInputCube("FROM");

  // General data gathering
  general = new QList< QPair<iString, iString> >;
  general->append(MakePair("Program",     caminfo_program));
  general->append(MakePair("IsisVersion", Application::Version()));
  general->append(MakePair("RunDate",     iTime::CurrentGMT()));
  general->append(MakePair("IsisId",      SerialNumber::Compose(*incube)));
  general->append(MakePair("From",        in.Basename() + ".cub"));
  general->append(MakePair("Lines",       incube->Lines()));
  general->append(MakePair("Samples",     incube->Samples()));
  general->append(MakePair("Bands",       incube->Bands()));

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  if(ui.GetBoolean("CAMSTATS")) {
    camstats = new QList< QPair<iString, iString> >;

    Pvl camPvl; // This becomes useful if there is only one band, which is
    Filename tempCamPvl;
    tempCamPvl.Temporary(in.Basename() + "_", "pvl");
    string pvlOut = tempCamPvl.Expanded();
    // Set up camstats run and execute
    string parameters = "FROM=" + ui.GetAsString("FROM") +
                        " TO=" + pvlOut +
                        " LINC=" + iString(ui.GetInteger("LINC")) +
                        " SINC=" + iString(ui.GetInteger("SINC"));

    ProgramLauncher::RunIsisProgram("camstats", parameters);
    // Output to common object of the PVL
    camPvl.Read(pvlOut);
    remove(pvlOut.c_str());

    PvlGroup cg = camPvl.FindGroup("Latitude", Pvl::Traverse);
    camstats->append(MakePair("MinimumLatitude", cg["latitudeminimum"][0]));
    camstats->append(MakePair("MaximumLatitude", cg["latitudemaximum"][0]));

    cg = camPvl.FindGroup("Longitude", Pvl::Traverse);
    camstats->append(MakePair("MinimumLongitude", cg["longitudeminimum"][0]));
    camstats->append(MakePair("MaximumLongitude", cg["longitudemaximum"][0]));

    cg = camPvl.FindGroup("Resolution", Pvl::Traverse);
    camstats->append(MakePair("MinimumResolution", cg["resolutionminimum"][0]));
    camstats->append(MakePair("MaximumResolution", cg["resolutionmaximum"][0]));

    cg = camPvl.FindGroup("PhaseAngle", Pvl::Traverse);
    camstats->append(MakePair("MinimumPhase", cg["phaseminimum"][0]));
    camstats->append(MakePair("MaximumPhase", cg["phasemaximum"][0]));

    cg = camPvl.FindGroup("EmissionAngle", Pvl::Traverse);
    camstats->append(MakePair("MinimumEmission", cg["emissionminimum"][0]));
    camstats->append(MakePair("MaximumEmission", cg["emissionmaximum"][0]));

    cg = camPvl.FindGroup("IncidenceAngle", Pvl::Traverse);
    camstats->append(MakePair("MinimumIncidence", cg["incidenceminimum"][0]));
    camstats->append(MakePair("MaximumIncidence", cg["incidencemaximum"][0]));

    cg = camPvl.FindGroup("LocalSolarTime", Pvl::Traverse);
    camstats->append(MakePair("LocalTimeMinimum", cg["localsolartimeMinimum"][0]));
    camstats->append(MakePair("LocalTimeMaximum", cg["localsolartimeMaximum"][0]));
  }

  // Compute statistics for entire cube
  if(ui.GetBoolean("STATISTICS")) {
    statistics = new QList< QPair<iString, iString> >;

    LineManager iline(*incube);
    Statistics stats;
    Progress progress;
    progress.SetText("Statistics...");
    progress.SetMaximumSteps(incube->Lines()*incube->Bands());
    progress.CheckStatus();
    iline.SetLine(1);
    for(; !iline.end() ; iline.next()) {
      incube->Read(iline);
      stats.AddData(iline.DoubleBuffer(), iline.size());
      progress.CheckStatus();
    }

    //  Compute stats of entire cube
    double nPixels     = stats.TotalPixels();
    double nullpercent = (stats.NullPixels() / (nPixels)) * 100;
    double hispercent  = (stats.HisPixels() / (nPixels)) * 100;
    double hrspercent  = (stats.HrsPixels() / (nPixels)) * 100;
    double lispercent  = (stats.LisPixels() / (nPixels)) * 100;
    double lrspercent  = (stats.LrsPixels() / (nPixels)) * 100;

    // Statitics output for band
    statistics->append(MakePair("MeanValue", stats.Average()));
    statistics->append(MakePair("StandardDeviation", stats.StandardDeviation()));
    statistics->append(MakePair("MinimumValue", stats.Minimum()));
    statistics->append(MakePair("MaximumValue", stats.Maximum()));
    statistics->append(MakePair("PercentHIS", hispercent));
    statistics->append(MakePair("PercentHRS", hrspercent));
    statistics->append(MakePair("PercentLIS", lispercent));
    statistics->append(MakePair("PercentLRS", lrspercent));
    statistics->append(MakePair("PercentNull", nullpercent));
    statistics->append(MakePair("TotalPixels", stats.TotalPixels()));
  }

  bool doGeometry = ui.GetBoolean("GEOMETRY");
  bool doPolygon = ui.GetBoolean("POLYGON");
  if(doGeometry || doPolygon) {
    Camera *cam = incube->Camera();

    iString incType = ui.GetString("INCTYPE");
    int polySinc, polyLinc;
    if(doPolygon && incType.UpCase() == "VERTICES") {
      polySinc = polyLinc = (int)(0.5 + (((incube->Samples() * 2) +
                                 (incube->Lines() * 2) - 3.0) /
                                 ui.GetInteger("NUMVERTICES")));
    }
    else if (incType.UpCase() == "LINCSINC"){
      if(ui.WasEntered("POLYSINC")) {
        polySinc = ui.GetInteger("POLYSINC");
      }
      else {
        polySinc = (int)(0.5 + 0.10 * incube->Samples());
        if(polySinc == 0) polySinc = 1;
      }
      if(ui.WasEntered("POLYLINC")) {
        polyLinc = ui.GetInteger("POLYLINC");
      }
      else {
        polyLinc = (int)(0.5 + 0.10 * incube->Lines());
        if(polyLinc == 0) polyLinc = 1;
      }
    }
    else {
      string msg = "Invalid INCTYPE option[" + incType + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    bandGeom = new BandGeometry();
    bandGeom->setSampleInc(polySinc);
    bandGeom->setLineInc(polyLinc);
    bandGeom->setMaxIncidence(ui.GetDouble("MAXINCIDENCE"));
    bandGeom->setMaxEmission(ui.GetDouble("MAXEMISSION"));
    bandGeom->collect(*cam, *incube, doGeometry, doPolygon);

    // Check if the user requires valid image center geometry
    if(ui.GetBoolean("VCAMERA") && (!bandGeom->hasCenterGeometry())) {
      string msg = "Image center does not project in camera model";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }
  }

  if(sFormat.UpCase() == "PVL")
    GeneratePVLOutput(incube, general, camstats, statistics, bandGeom);
  else
    GenerateCSVOutput(incube, general, camstats, statistics, bandGeom);

  // Clean the data
  delete general;
  general = NULL;
  if(camstats) {
    delete camstats;
    camstats = NULL;
  }
  if(statistics) {
    delete statistics;
    statistics = NULL;
  }
  if(bandGeom) {
    delete bandGeom;
    bandGeom = NULL;
  }

}


/**
 * Convience method for gracefully staying in 80 characters
 */
QPair<iString, iString> MakePair(iString key, iString val) {
  return QPair<iString, iString>(key, val);
}


/**
 * Get the output in PVL format
 */
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<iString, iString> > *general,
                       QList< QPair<iString, iString> > *camstats,
                       QList< QPair<iString, iString> > *statistics,
                       BandGeometry *bandGeom) {
  UserInterface &ui = Application::GetUserInterface();

  // Add some common/general things
  PvlObject params("Caminfo");
  PvlObject common("Parameters");
  for(int i = 0; i < general->size(); i++)
    common += PvlKeyword((*general)[i].first, (*general)[i].second);
  params.AddObject(common);

  // Add the camstats
  if(camstats) {
    PvlObject pcband("Camstats");
    for(int i = 0; i < camstats->size(); i++)
      pcband += ValidateKey((*camstats)[i].first, (*camstats)[i].second);
    params.AddObject(pcband);
  }

  // Add the input ISIS label if requested
  if(ui.GetBoolean("ISISLABEL")) {
    Pvl label = *(incube->Label());
    label.SetName("IsisLabel");
    params.AddObject(label);
  }

  // Add the orginal label blob
  if(ui.GetBoolean("ORIGINALLABEL")) {
    OriginalLabel orig;
    incube->Read(orig);
    Pvl p = orig.ReturnLabels();
    p.SetName("OriginalLabel");
    params.AddObject(p);
  }

  // Add the stats
  if(statistics) {
    PvlObject sgroup("Statistics");
    for(int i = 0; i < statistics->size(); i++)
      sgroup += ValidateKey((*statistics)[i].first, (*statistics)[i].second);
    params.AddObject(sgroup);
  }

  // Add the geometry info
  if(bandGeom) {
    if(ui.GetBoolean("GEOMETRY")) {
      PvlObject ggroup("Geometry");
      bandGeom->generateGeometryKeys(ggroup);
      params.AddObject(ggroup);
    }

    if(ui.GetBoolean("POLYGON")) {
      PvlObject ggroup("Polygon");
      bandGeom->generatePolygonKeys(ggroup);
      params.AddObject(ggroup);
    }
  }

  // Output the result
  Pvl pout;
  string outFile = ui.GetFilename("TO");
  pout.AddObject(params);

  if(ui.GetBoolean("APPEND"))
    pout.Append(outFile);
  else
    pout.Write(outFile);
}


/**
 * Get the output in CSV Format. If CSV format is chosen only
 * CamStats, Stats, Geometry are info are recorded.
 */
void GenerateCSVOutput(Cube *incube,
                       QList< QPair<iString, iString> > *general,
                       QList< QPair<iString, iString> > *camstats,
                       QList< QPair<iString, iString> > *statistics,
                       BandGeometry *bandGeom) {
  UserInterface &ui = Application::GetUserInterface();

  // Create the vars for holding the info
  iString keys;
  iString values;
  const iString delim = ",";

  // Output the result
  fstream outFile;
  string sOutFile = ui.GetAsString("TO");
  bool appending = ui.GetBoolean("APPEND") && Filename(sOutFile).Exists();
  if(appending)
    outFile.open(sOutFile.c_str(), std::ios::out | std::ios::app);
  else
    outFile.open(sOutFile.c_str(), std::ios::out);

  // Add some common/general things
  for(int i = 0; i < general->size(); i++)
    if((*general)[i].first != "RunDate") {
      if(not appending) keys += (*general)[i].first + delim;
      values += (*general)[i].second + delim;
    }

  // Add the camstats
  if(ui.GetBoolean("CAMSTATS")) {
    for(int i = 0; i < camstats->size(); i++) {
      if(not appending) keys += "CamStats_" + (*camstats)[i].first + delim;
      values += (*camstats)[i].second + delim;
    }
  }

  // Add the stats
  if(ui.GetBoolean("STATISTICS")) {
    for(int i = 0; i < statistics->size(); i++) {
      if(not appending) keys += "Stats_" + (*statistics)[i].first + delim;
      values += (*statistics)[i].second + delim;
    }
  }

  // Add the geometry info
  if(ui.GetBoolean("GEOMETRY")) {
    PvlObject geomGrp("Geometry");
    bandGeom->generateGeometryKeys(geomGrp);
    for(int i = 0; i < geomGrp.Keywords(); i++) {
      if(not appending) keys += "Geom_" + geomGrp[i].Name() + delim;
      values += geomGrp[i][0] + delim;
    }
  }

  keys.TrimTail(delim); // Get rid of the extra delim char (",")
  values.TrimTail(delim); // Get rid of the extra delim char (",")
  outFile << keys << endl << values << endl;
  outFile.close();
}
