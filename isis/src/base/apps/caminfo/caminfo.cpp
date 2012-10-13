#include "Isis.h"

#include <cstdio>
#include <string>
#include <iostream>

#include "CameraStatistics.h"
#include "CamTools.h"
#include "FileName.h"
#include "IException.h"
#include "ImagePolygon.h"
#include "IString.h"
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

QPair<IString, IString> MakePair(IString key, IString val);
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<IString, IString> > *general,
                       QList< QPair<IString, IString> > *camstats,
                       QList< QPair<IString, IString> > *statistics,
                       BandGeometry *bandGeom);
void GenerateCSVOutput(Cube *incube,
                       QList< QPair<IString, IString> > *general,
                       QList< QPair<IString, IString> > *camstats,
                       QList< QPair<IString, IString> > *statistics,
                       BandGeometry *bandGeom);

void IsisMain() {
  const string caminfo_program  = "caminfo";
  UserInterface &ui = Application::GetUserInterface();

  QList< QPair<IString, IString> > *general = NULL, *camstats = NULL, *statistics = NULL;
  BandGeometry *bandGeom = NULL;

  // Get input filename
  FileName in = ui.GetFileName("FROM");

  // Get the format
  IString sFormat = ui.GetAsString("FORMAT");

  // if true then run spiceinit, xml default is FALSE
  // spiceinit will use system kernels
  if(ui.GetBoolean("SPICE")) {
    string parameters = "FROM=" + in.expanded();
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
  }

  Process p;
  Cube *incube = p.SetInputCube("FROM");

  // General data gathering
  general = new QList< QPair<IString, IString> >;
  general->append(MakePair("Program",     caminfo_program));
  general->append(MakePair("IsisVersion", Application::Version()));
  general->append(MakePair("RunDate",     iTime::CurrentGMT()));
  general->append(MakePair("IsisId",      SerialNumber::Compose(*incube)));
  general->append(MakePair("From",        in.baseName() + ".cub"));
  general->append(MakePair("Lines",       incube->getLineCount()));
  general->append(MakePair("Samples",     incube->getSampleCount()));
  general->append(MakePair("Bands",       incube->getBandCount()));

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  if(ui.GetBoolean("CAMSTATS")) {
    camstats = new QList< QPair<IString, IString> >;

    string filename = ui.GetAsString("FROM");
    int sinc = ui.GetInteger("SINC");
    int linc = ui.GetInteger("LINC");
    CameraStatistics stats(filename, sinc, linc);
    Pvl camPvl = stats.toPvl();

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
    statistics = new QList< QPair<IString, IString> >;

    LineManager iline(*incube);
    Statistics stats;
    Progress progress;
    progress.SetText("Statistics...");
    progress.SetMaximumSteps(incube->getLineCount()*incube->getBandCount());
    progress.CheckStatus();
    iline.SetLine(1);
    for(; !iline.end() ; iline.next()) {
      incube->read(iline);
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
    Camera *cam = incube->getCamera();

    IString incType = ui.GetString("INCTYPE");
    int polySinc, polyLinc;
    if(doPolygon && incType.UpCase() == "VERTICES") {
      ImagePolygon poly;
      poly.initCube(*incube);
      polySinc = polyLinc = (int)(0.5 + (((poly.validSampleDim() * 2) +
                                 (poly.validLineDim() * 2) - 3.0) /
                                 ui.GetInteger("NUMVERTICES")));
    }
    else if (incType.UpCase() == "LINCSINC"){
      if(ui.WasEntered("POLYSINC")) {
        polySinc = ui.GetInteger("POLYSINC");
      }
      else {
        polySinc = (int)(0.5 + 0.10 * incube->getSampleCount());
        if(polySinc == 0) polySinc = 1;
      }
      if(ui.WasEntered("POLYLINC")) {
        polyLinc = ui.GetInteger("POLYLINC");
      }
      else {
        polyLinc = (int)(0.5 + 0.10 * incube->getLineCount());
        if(polyLinc == 0) polyLinc = 1;
      }
    }
    else {
      string msg = "Invalid INCTYPE option[" + incType + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    bandGeom = new BandGeometry();
    bandGeom->setSampleInc(polySinc);
    bandGeom->setLineInc(polyLinc);
    bandGeom->setMaxIncidence(ui.GetDouble("MAXINCIDENCE"));
    bandGeom->setMaxEmission(ui.GetDouble("MAXEMISSION"));

    bool precision = ui.GetBoolean("INCREASEPRECISION");
    bandGeom->collect(*cam, *incube, doGeometry, doPolygon, precision);

    // Check if the user requires valid image center geometry
    if(ui.GetBoolean("VCAMERA") && (!bandGeom->hasCenterGeometry())) {
      string msg = "Image center does not project in camera model";
      throw IException(IException::Unknown, msg, _FILEINFO_);
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
QPair<IString, IString> MakePair(IString key, IString val) {
  return QPair<IString, IString>(key, val);
}


/**
 * Get the output in PVL format
 */
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<IString, IString> > *general,
                       QList< QPair<IString, IString> > *camstats,
                       QList< QPair<IString, IString> > *statistics,
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
    Pvl label = *(incube->getLabel());
    label.SetName("IsisLabel");
    params.AddObject(label);
  }

  // Add the orginal label blob
  if(ui.GetBoolean("ORIGINALLABEL")) {
    OriginalLabel orig;
    incube->read(orig);
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
  string outFile = ui.GetFileName("TO");
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
                       QList< QPair<IString, IString> > *general,
                       QList< QPair<IString, IString> > *camstats,
                       QList< QPair<IString, IString> > *statistics,
                       BandGeometry *bandGeom) {
  UserInterface &ui = Application::GetUserInterface();

  // Create the vars for holding the info
  IString keys;
  IString values;
  const IString delim = ",";

  // Output the result
  fstream outFile;
  string sOutFile = ui.GetAsString("TO");
  bool appending = ui.GetBoolean("APPEND") && FileName(sOutFile).fileExists();
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
