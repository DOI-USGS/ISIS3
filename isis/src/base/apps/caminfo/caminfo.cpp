#include "Isis.h"

#include <cstdio>
#include <QString>
#include <iostream>

#include "CameraStatistics.h"
#include "CamTools.h"
#include "FileName.h"
#include "History.h"
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

QPair<QString, QString> MakePair(QString key, QString val);
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<QString, QString> > *general,
                       QList< QPair<QString, QString> > *camstats,
                       QList< QPair<QString, QString> > *statistics,
                       BandGeometry *bandGeom);
void GenerateCSVOutput(Cube *incube,
                       QList< QPair<QString, QString> > *general,
                       QList< QPair<QString, QString> > *camstats,
                       QList< QPair<QString, QString> > *statistics,
                       BandGeometry *bandGeom);

void IsisMain() {
  const QString caminfo_program  = "caminfo";
  UserInterface &ui = Application::GetUserInterface();

  QList< QPair<QString, QString> > *general = NULL, *camstats = NULL, *statistics = NULL;
  BandGeometry *bandGeom = NULL;

  // Get input filename
  FileName in = ui.GetFileName("FROM");

  // Get the format
  QString sFormat = ui.GetAsString("FORMAT");

  // if true then run spiceinit, xml default is FALSE
  // spiceinit will use system kernels
  if(ui.GetBoolean("SPICE")) {
    QString parameters = "FROM=" + in.expanded();
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
  }

  Process p;
  Cube *incube = p.SetInputCube("FROM");

  // General data gathering
  general = new QList< QPair<QString, QString> >;
  general->append(MakePair("Program",     caminfo_program));
  general->append(MakePair("IsisVersion", Application::Version()));
  general->append(MakePair("RunDate",     iTime::CurrentGMT()));
  general->append(MakePair("IsisId",      SerialNumber::Compose(*incube)));
  general->append(MakePair("From",        in.baseName() + ".cub"));
  general->append(MakePair("Lines",       toString(incube->lineCount())));
  general->append(MakePair("Samples",     toString(incube->sampleCount())));
  general->append(MakePair("Bands",       toString(incube->bandCount())));

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  if(ui.GetBoolean("CAMSTATS")) {
    camstats = new QList< QPair<QString, QString> >;

    QString filename = ui.GetAsString("FROM");
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
    statistics = new QList< QPair<QString, QString> >;

    LineManager iline(*incube);
    Statistics stats;
    Progress progress;
    progress.SetText("Statistics...");
    progress.SetMaximumSteps(incube->lineCount()*incube->bandCount());
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
    statistics->append(MakePair("MeanValue", toString(stats.Average())));
    statistics->append(MakePair("StandardDeviation", toString(stats.StandardDeviation())));
    statistics->append(MakePair("MinimumValue", toString(stats.Minimum())));
    statistics->append(MakePair("MaximumValue", toString(stats.Maximum())));
    statistics->append(MakePair("PercentHIS", toString(hispercent)));
    statistics->append(MakePair("PercentHRS", toString(hrspercent)));
    statistics->append(MakePair("PercentLIS", toString(lispercent)));
    statistics->append(MakePair("PercentLRS", toString(lrspercent)));
    statistics->append(MakePair("PercentNull", toString(nullpercent)));
    statistics->append(MakePair("TotalPixels", toString(stats.TotalPixels())));
  }

  bool getFootBlob = ui.GetBoolean("USELABEL");
  bool doGeometry = ui.GetBoolean("GEOMETRY");
  bool doPolygon = ui.GetBoolean("POLYGON");
  if(doGeometry || doPolygon || getFootBlob) {
    Camera *cam = incube->camera();

    QString incType = ui.GetString("INCTYPE");
    int polySinc, polyLinc;
    if(doPolygon && incType.toUpper() == "VERTICES") {
      ImagePolygon poly;
      poly.initCube(*incube);
      polySinc = polyLinc = (int)(0.5 + (((poly.validSampleDim() * 2) +
                                 (poly.validLineDim() * 2) - 3.0) /
                                 ui.GetInteger("NUMVERTICES")));
    }
    else if (incType.toUpper() == "LINCSINC"){
      if(ui.WasEntered("POLYSINC")) {
        polySinc = ui.GetInteger("POLYSINC");
      }
      else {
        polySinc = (int)(0.5 + 0.10 * incube->sampleCount());
        if(polySinc == 0) polySinc = 1;
      }
      if(ui.WasEntered("POLYLINC")) {
        polyLinc = ui.GetInteger("POLYLINC");
      }
      else {
        polyLinc = (int)(0.5 + 0.10 * incube->lineCount());
        if(polyLinc == 0) polyLinc = 1;
      }
    }
    else {
      QString msg = "Invalid INCTYPE option[" + incType + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    bandGeom = new BandGeometry();
    bandGeom->setSampleInc(polySinc);
    bandGeom->setLineInc(polyLinc);
    bandGeom->setMaxIncidence(ui.GetDouble("MAXINCIDENCE"));
    bandGeom->setMaxEmission(ui.GetDouble("MAXEMISSION"));
    bool precision = ui.GetBoolean("INCREASEPRECISION");

    if (getFootBlob) {
      // Need to read history to obtain parameters that were used to
      // create the footprint
      History hist("IsisCube", in.expanded());
      Pvl pvl = hist.ReturnHist();
      PvlObject::PvlObjectIterator objIter;
      bool found = false;
      PvlGroup fpgrp;
      for (objIter=pvl.EndObject()-1; objIter>=pvl.BeginObject(); objIter--) {
        if (objIter->Name().toUpper() == "FOOTPRINTINIT") {
          found = true;
          fpgrp = objIter->FindGroup("UserParameters");
          break;
        }
      }
      if (!found) {
        QString msg = "Footprint blob was not found in input image history";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      QString prec = (QString)fpgrp.FindKeyword("INCREASEPRECISION");
      prec = prec.toUpper();
      if (prec == "TRUE") {
        precision = true;
      }
      else {
        precision = false;
      }
      QString inctype = (QString)fpgrp.FindKeyword("INCTYPE");
      inctype = inctype.toUpper();
      if (inctype == "LINCSINC") {
        int linc = fpgrp.FindKeyword("LINC");
        int sinc = fpgrp.FindKeyword("SINC");
        bandGeom->setSampleInc(sinc);
        bandGeom->setLineInc(linc);
      }
      else {
        int vertices = fpgrp.FindKeyword("NUMVERTICES");
        int lincsinc = (int)(0.5 + (((incube->sampleCount() * 2) +
                       (incube->lineCount() * 2) - 3.0) /
                       vertices));
        bandGeom->setSampleInc(lincsinc);
        bandGeom->setLineInc(lincsinc);
      }
      if (fpgrp.HasKeyword("MAXINCIDENCE")) {
        double maxinc = fpgrp.FindKeyword("MAXINCIDENCE");
        bandGeom->setMaxIncidence(maxinc);
      }
      if (fpgrp.HasKeyword("MAXEMISSION")) {
        double maxema = fpgrp.FindKeyword("MAXEMISSION");
        bandGeom->setMaxEmission(maxema);
      }
    }
    
    bandGeom->collect(*cam, *incube, doGeometry, doPolygon, getFootBlob, precision);

    // Check if the user requires valid image center geometry
    if(ui.GetBoolean("VCAMERA") && (!bandGeom->hasCenterGeometry())) {
      QString msg = "Image center does not project in camera model";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

  if(sFormat.toUpper() == "PVL")
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
QPair<QString, QString> MakePair(QString key, QString val) {
  return QPair<QString, QString>(key, val);
}


/**
 * Get the output in PVL format
 */
void GeneratePVLOutput(Cube *incube,
                       QList< QPair<QString, QString> > *general,
                       QList< QPair<QString, QString> > *camstats,
                       QList< QPair<QString, QString> > *statistics,
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
      pcband += ValidateKey((*camstats)[i].first, toDouble((*camstats)[i].second));
    params.AddObject(pcband);
  }

  // Add the input ISIS label if requested
  if(ui.GetBoolean("ISISLABEL")) {
    Pvl label = *(incube->label());
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
      sgroup += ValidateKey((*statistics)[i].first, toDouble((*statistics)[i].second));
    params.AddObject(sgroup);
  }

  // Add the geometry info
  if(bandGeom) {
    if(ui.GetBoolean("GEOMETRY")) {
      PvlObject ggroup("Geometry");
      bandGeom->generateGeometryKeys(ggroup);
      params.AddObject(ggroup);
    }

    if(ui.GetBoolean("POLYGON") || ui.GetBoolean("USELABEL")) {
      PvlObject ggroup("Polygon");
      bandGeom->generatePolygonKeys(ggroup);
      params.AddObject(ggroup);
    }
  }

  // Output the result
  Pvl pout;
  QString outFile = ui.GetFileName("TO");
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
                       QList< QPair<QString, QString> > *general,
                       QList< QPair<QString, QString> > *camstats,
                       QList< QPair<QString, QString> > *statistics,
                       BandGeometry *bandGeom) {
  UserInterface &ui = Application::GetUserInterface();

  // Create the vars for holding the info
  QString keys;
  QString values;
  const QString delim = ",";

  // Output the result
  fstream outFile;
  QString sOutFile = ui.GetAsString("TO");
  bool appending = ui.GetBoolean("APPEND") && FileName(sOutFile).fileExists();
  if(appending)
    outFile.open(sOutFile.toAscii().data(), std::ios::out | std::ios::app);
  else
    outFile.open(sOutFile.toAscii().data(), std::ios::out);

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

  keys.remove(QRegExp(delim + "$")); // Get rid of the extra delim char (",")
  values.remove(QRegExp(delim + "$")); // Get rid of the extra delim char (",")
  outFile << keys << endl << values << endl;
  outFile.close();
}
