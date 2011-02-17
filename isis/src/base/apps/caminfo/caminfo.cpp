#include "Isis.h"
#include <cstdio>
#include <cmath>
#include <string>

#include "CamTools.h"
#include "Constants.h"
#include "CubeAttribute.h"
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
#include "SpecialPixel.h"
#include "Statistics.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  const string caminfo_program = "caminfo";
  const string caminfo_version = "2.2";
  const string caminfo_revision = "$Revision: 1.20 $";
  string caminfo_runtime = iTime::CurrentGMT();

  Process p;
  // Grab the file to import
  UserInterface &ui = Application::GetUserInterface();
  string from = ui.GetAsString("FROM");
  Filename in = ui.GetFilename("FROM");
  bool doCamstat = ui.GetBoolean("CAMSTATS");

  Pvl pout;
  // if true then run spiceinit, xml default is FALSE
  //spiceinit will use system kernels
  if(ui.GetBoolean("SPICE")) {
    string parameters = "FROM=" + in.Expanded();
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
  }

  Cube *icube = p.SetInputCube("FROM");

  // get some common things like #line, #samples, bands.
  PvlObject params("Caminfo");
  PvlObject common("Parameters");
  common += PvlKeyword("Program", caminfo_program);
  common += PvlKeyword("Version", caminfo_version);
  common += PvlKeyword("IsisVersion", Application::Version());
  common += PvlKeyword("RunDate", caminfo_runtime);
  common += PvlKeyword("IsisId", SerialNumber::Compose(*icube));
  common += PvlKeyword("From", icube->Filename());
  common += PvlKeyword("Lines", icube->Lines());
  common += PvlKeyword("Samples", icube->Samples());
  common += PvlKeyword("Bands", icube->Bands());
  params.AddObject(common);

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  Pvl camPvl;    //  This becomes useful if there is only one band, which is
  //  frequent!  Used below if single band image.
  if(doCamstat) {
    int statsLinc = ui.GetInteger("STATSLINC");
    int statsSinc = ui.GetInteger("STATSSINC");
    Filename tempCamPvl;
    tempCamPvl.Temporary(in.Basename() + "_", "pvl");
    string pvlOut = tempCamPvl.Expanded();
    PvlObject pcband("Camstats");
    //set up camstats run and execute
    string parameters = "FROM=" + from +
                        " TO=" + pvlOut +
                        " LINC=" + iString(statsLinc) +
                        " SINC=" + iString(statsSinc);

    ProgramLauncher::RunIsisProgram("camstats", parameters);
    //out put to common object of the PVL
    camPvl.Read(pvlOut);
    remove(pvlOut.c_str());

    PvlGroup cg = camPvl.FindGroup("Latitude", Pvl::Traverse);
    pcband += ValidateKey("MinimumLatitude", cg["latitudeminimum"]);
    pcband += ValidateKey("MaximumLatitude", cg["latitudemaximum"]);
    cg = camPvl.FindGroup("Longitude", Pvl::Traverse);
    pcband += ValidateKey("MinimumLongitude", cg["longitudeminimum"]);
    pcband += ValidateKey("MaximumLongitude", cg["longitudemaximum"]);
    cg = camPvl.FindGroup("Resolution", Pvl::Traverse);
    pcband += ValidateKey("MinimumResolution", cg["resolutionminimum"]);
    pcband += ValidateKey("MaximumResolution", cg["resolutionmaximum"]);
    cg = camPvl.FindGroup("PhaseAngle", Pvl::Traverse);
    pcband += ValidateKey("MinimumPhase", cg["phaseminimum"]);
    pcband += ValidateKey("MaximumPhase", cg["phasemaximum"]);
    cg = camPvl.FindGroup("EmissionAngle", Pvl::Traverse);
    pcband += ValidateKey("MinimumEmission", cg["emissionminimum"]);
    pcband += ValidateKey("MaximumEmission", cg["emissionmaximum"]);
    cg = camPvl.FindGroup("IncidenceAngle", Pvl::Traverse);
    pcband += ValidateKey("MinimumIncidence", cg["incidenceminimum"]);
    pcband += ValidateKey("MaximumIncidence", cg["incidencemaximum"]);
    cg = camPvl.FindGroup("LocalSolarTime", Pvl::Traverse);
    pcband += ValidateKey("LocalTimeMinimum", cg["localsolartimeMinimum"]);
    pcband += ValidateKey("LocalTimeMaximum", cg["localsolartimeMaximum"]);
    params.AddObject(pcband);
  }


  //  Add the input ISIS label if requested
  if(ui.GetBoolean("ISISLABEL")) {
    Pvl label = *(icube->Label());
    label.SetName("IsisLabel");
    params.AddObject(label);
  }

  // write out the orginal label blob
  if(ui.GetBoolean("ORIGINALLABEL")) {
    OriginalLabel orig;
    icube->Read(orig);
    Pvl p = orig.ReturnLabels();
    p.SetName("OriginalLabel");
    params.AddObject(p);
  }


  //  Compute statistics for entire cube
  if(ui.GetBoolean("STATISTICS")) {
    LineManager iline(*icube);
    Statistics stats;
    Progress progress;
    progress.SetText("Statistics...");
    progress.SetMaximumSteps(icube->Lines()*icube->Bands());
    progress.CheckStatus();
    iline.SetLine(1);
    for(; !iline.end() ; iline.next()) {
      icube->Read(iline);
      stats.AddData(iline.DoubleBuffer(), iline.size());
      progress.CheckStatus();
    }

    //  Compute stats of entire cube
    double nPixels = stats.TotalPixels();
    double nullpercent = (stats.NullPixels() / (nPixels)) * 100;
    double hispercent = (stats.HisPixels() / (nPixels)) * 100;
    double hrspercent = (stats.HrsPixels() / (nPixels)) * 100;
    double lispercent = (stats.LisPixels() / (nPixels)) * 100;
    double lrspercent = (stats.LrsPixels() / (nPixels)) * 100;
    //statitics keyword output for band

    PvlObject sgroup("Statistics");
    sgroup += ValidateKey("MeanValue", stats.Average());
    sgroup += ValidateKey("StandardDeviation", stats.StandardDeviation());
    sgroup += ValidateKey("MinimumValue", stats.Minimum());
    sgroup += ValidateKey("MaximumValue", stats.Maximum());
    sgroup += PvlKeyword("PercentHIS", hispercent);
    sgroup += PvlKeyword("PercentHRS", hrspercent);
    sgroup += PvlKeyword("PercentLIS", lispercent);
    sgroup += PvlKeyword("PercentLRS", lrspercent);
    sgroup += PvlKeyword("PercentNull", nullpercent);
    sgroup += PvlKeyword("TotalPixels", stats.TotalPixels());

    params.AddObject(sgroup);
  }

  Camera *cam = icube->Camera();

  // for geometry, stats, camstats, or polygon get the info for each band
  BandGeometry bandGeom;
  bool doGeometry = ui.GetBoolean("GEOMETRY");
  bool doPolygon = ui.GetBoolean("POLYGON");
  if(doGeometry || doPolygon) {
    int polySinc, polyLinc;
    if(ui.WasEntered("POLYSINC")) {
      polySinc = ui.GetInteger("POLYSINC");
    }
    else {
      polySinc = (int)round(0.10 * icube->Samples());
      if (polySinc == 0)
        polySinc = 1;
    }
    if(ui.WasEntered("POLYLINC")) {
      polyLinc = ui.GetInteger("POLYLINC");
    }
    else {
      polyLinc = (int)round(0.10 * icube->Lines());
      if (polyLinc == 0)
        polyLinc = 1;
    }

    bandGeom.setSampleInc(polySinc);
    bandGeom.setLineInc(polyLinc);
    bandGeom.setMaxIncidence(ui.GetDouble("MAXINCIDENCE"));
    bandGeom.setMaxEmission(ui.GetDouble("MAXEMISSION"));
    bandGeom.collect(*cam, *icube, doGeometry, doPolygon);

    // Check if the user requires valid image center geometry
    if(ui.GetBoolean("VCAMERA") && (!bandGeom.hasCenterGeometry())) {
      string msg = "Image center does not project in camera model";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    // Write geometry data if requested
    if(doGeometry) {
      PvlObject ggroup("Geometry");
      bandGeom.generateGeometryKeys(ggroup);
      params.AddObject(ggroup);
    }

    // Write polygon group if requested
    if(doPolygon) {
      PvlObject ggroup("Polygon");
      bandGeom.generatePolygonKeys(ggroup);
      params.AddObject(ggroup);
    }
  }

  //  Output the result
  string outFile = ui.GetFilename("TO");
  pout.AddObject(params);
  pout.Write(outFile);
}


