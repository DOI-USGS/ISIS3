#include "Isis.h"
#include <cstdio>
#include <cmath>
#include <string>
#include <iostream>

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

// Constants
const string caminfo_program  = "caminfo";
  
void GetPVLOutput(void);
void GetCSVOutput(void);

void IsisMain() 
{
  UserInterface &ui = Application::GetUserInterface();

  // Get input filename
  Filename in = ui.GetFilename("FROM");
  
  // Get the format
  string sFormat = ui.GetAsString("FORMAT");
  
  // if true then run spiceinit, xml default is FALSE
  // spiceinit will use system kernels
  if(ui.GetBoolean("SPICE")) {
    string parameters = "FROM=" + in.Expanded();
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
  }

  if(sFormat == "PVL" || sFormat=="pvl") {
    GetPVLOutput();
  }
  else {
    GetCSVOutput();
  }
}

/**
 * Get the output in PVL format
 */
void GetPVLOutput(void) 
{
  UserInterface &ui = Application::GetUserInterface();
  string from    = ui.GetAsString("FROM");
  Filename in    = ui.GetFilename("FROM");
  bool doCamstat = ui.GetBoolean("CAMSTATS");
  bool bAppend   = ui.GetBoolean("APPEND");

  Process p;
  Cube *icube = p.SetInputCube("FROM");

  Pvl pout;

  // Get some common things like #line, #samples, bands.
  PvlObject params("Caminfo");
  PvlObject common("Parameters");
  common += PvlKeyword("Program",     caminfo_program);
  common += PvlKeyword("IsisVersion", Application::Version());
  common += PvlKeyword("RunDate",     iTime::CurrentGMT());
  common += PvlKeyword("IsisId",      SerialNumber::Compose(*icube));
  common += PvlKeyword("From",        in.Basename() + ".cub");
  common += PvlKeyword("Lines",       icube->Lines());
  common += PvlKeyword("Samples",     icube->Samples());
  common += PvlKeyword("Bands",       icube->Bands());
  params.AddObject(common);

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  //  frequent!  Used below if single band image.
  if(doCamstat) {
    Pvl camPvl;    //  This becomes useful if there is only one band, which is
    int linc = ui.GetInteger("LINC");
    int sinc = ui.GetInteger("SINC");
    Filename tempCamPvl;
    tempCamPvl.Temporary(in.Basename() + "_", "pvl");
    string pvlOut = tempCamPvl.Expanded();
    PvlObject pcband("Camstats");
    //set up camstats run and execute
    string parameters = "FROM=" + from +
                        " TO=" + pvlOut +
                        " LINC=" + iString(linc) +
                        " SINC=" + iString(sinc);

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
    double nPixels     = stats.TotalPixels();
    double nullpercent = (stats.NullPixels() / (nPixels)) * 100;
    double hispercent  = (stats.HisPixels() / (nPixels)) * 100;
    double hrspercent  = (stats.HrsPixels() / (nPixels)) * 100;
    double lispercent  = (stats.LisPixels() / (nPixels)) * 100;
    double lrspercent  = (stats.LrsPixels() / (nPixels)) * 100;

    // Statitics keyword output for band
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
  
  if(bAppend) {
    pout.Append(outFile);
  }
  else {
    pout.Write(outFile);
  }
}

/**
 * Get the output in CSV Format. If CSV format is chosen only 
 * CamStats, Stats, Geometry are info are recorded 
 * 
 * @author Sharmila Prasad (2/24/2011)
 */
void GetCSVOutput(void)
{
  UserInterface &ui = Application::GetUserInterface();
  string from       = ui.GetAsString("FROM");
  Filename in       = ui.GetFilename("FROM");
  string sOutFile   = ui.GetAsString("TO");
  
  bool doCamstat  = ui.GetBoolean("CAMSTATS");
  bool bAppend    = ui.GetBoolean("APPEND");
  bool doStats    = ui.GetBoolean("STATISTICS");
  bool doGeometry = ui.GetBoolean("GEOMETRY");
  
  Process p;
  Cube *icube = p.SetInputCube("FROM");

  // Output the result
  fstream outFile;
  bool bFileExists = Filename(sOutFile).Exists();
  if(bFileExists && bAppend) {
    outFile.open(sOutFile.c_str(), std::ios::out | std::ios::app);
  }
  else {
    outFile.open(sOutFile.c_str(), std::ios::out); 
  }

  // Write the header for not Append
  if(!bAppend || !bFileExists) {
    outFile << "Program,IsisVersion,IsisId,From,Lines,Samples,Bands,";
    if(doCamstat) {
      outFile << "CamStats_MinimumLatitude,CamStats_MaximumLatitude,";
      outFile << "CamStats_MinimumLongitude,CamStats_MaximumLongitude,";
      outFile << "CamStats_MinimumResolution,CamStats_MaximumResolution,";
      outFile << "CamStats_MinimumPhase,CamStats_MaximumPhase,";
      outFile << "CamStats_MinimumEmission,CamStats_MaximumEmission,";
      outFile << "CamStats_MinimumIncidence,CamStats_MaximumIncidence,";
      outFile << "CamStats_LocalTimeMinimum,CamStats_LocalTimeMaximum,";
    }
    if(doStats) {
      outFile << "Stats_MeanValue,Stats_StandardDeviation,Stats_MinimumValue,Stats_MaximumValue,";
      outFile << "Stats_PercentHIS,Stats_PercentHRS,";
      outFile << "Stats_PercentLIS,Stats_PercentLRS,Stats_PercentNull,Stats_TotalPixels,";
    }
    if(doGeometry) {
      outFile << "Geom_BandsUsed,Geom_ReferenceBand,Geom_OriginalBand,Geom_Target,";
      outFile << "Geom_StartTime,Geom_EndTime,Geom_CenterLine,Geom_CenterSample,";
      outFile << "Geom_CenterLatitude,Geom_CenterLongitude,Geom_CenterRadius,";
      outFile << "Geom_RightAscension,Geom_Declination,";
      outFile << "Geom_UpperLeftLongitude,Geom_UpperLeftLatitude,";
      outFile << "Geom_LowerLeftLongitude,Geom_LowerLeftLatitude,";
      outFile << "Geom_LowerRightLongitude,Geom_LowerRightLatitude,";
      outFile << "Geom_UpperRightLongitude,Geom_UpperRightLatitude,";
      outFile << "Geom_PhaseAngle,Geom_EmissionAngle,Geom_IncidenceAngle,";
      outFile << "Geom_NorthAzimuth,Geom_OffNadir,Geom_SolarLongitude,Geom_LocalTime,";
      outFile << "Geom_TargetCenterDistance,Geom_SlantDistance,Geom_SampleResolution,Geom_LineResolution,";
      outFile << "Geom_PixelResolution,Geom_MeanGroundResolution,";
      outFile << "Geom_SubSolarAzimuth,Geom_SubSolarGroundAzimuth,";
      outFile << "Geom_SubSolarLatitude,Geom_SubSolarLongitude,";
      outFile << "Geom_SubSpacecraftAzimuth,Geom_SubSpacecraftGroundAzimuth,";
      outFile << "Geom_SubSpacecraftLatitude,Geom_SubSpacecraftLongitude,";
      outFile << "Geom_ParallaxX,Geom_ParallaxY,Geom_ShadowX,Geom_ShadowY,";
      outFile << "Geom_HasLongitudeBoundary,Geom_HasNorthPole,Geom_HasSouthPole";
    }
    outFile << endl;
  }
  
  // Get some common things like #line, #samples, bands.
  outFile << caminfo_program.c_str() << "," << Application::Version() << ",";
  outFile << SerialNumber::Compose(*icube) << "," << in.Basename() + ".cub" << ",";
  outFile << icube->Lines() << "," << icube->Samples() << "," << icube->Bands() << ",";

  // Run camstats on the entire image (all bands)
  // another camstats will be run for each band and output
  // for each band.
  Pvl camPvl;    //  This becomes useful if there is only one band, which is
  //  frequent!  Used below if single band image.
  if(doCamstat) {
    int linc = ui.GetInteger("LINC");
    int sinc = ui.GetInteger("SINC");
    Filename tempCamPvl;
    tempCamPvl.Temporary(in.Basename() + "_", "pvl");
    string pvlOut = tempCamPvl.Expanded();
    PvlObject pcband("Camstats");
    //set up camstats run and execute
    string parameters = "FROM=" + from +
                        " TO=" + pvlOut +
                        " LINC=" + iString(linc) +
                        " SINC=" + iString(sinc);

    ProgramLauncher::RunIsisProgram("camstats", parameters);
    //outFile put to common object of the PVL
    camPvl.Read(pvlOut);
    remove(pvlOut.c_str());

    PvlGroup cg = camPvl.FindGroup("Latitude", Pvl::Traverse);
    outFile << cg["latitudeminimum"][0] << "," << cg["latitudemaximum"][0] << ",";
 
    cg = camPvl.FindGroup("Longitude", Pvl::Traverse);
    outFile << cg["longitudeminimum"][0] << "," << cg["longitudemaximum"][0] << ",";
    
    cg = camPvl.FindGroup("Resolution", Pvl::Traverse);
    outFile << cg["resolutionminimum"][0] << "," << cg["resolutionmaximum"][0] << ",";
    
    cg = camPvl.FindGroup("PhaseAngle", Pvl::Traverse);
    outFile << cg["phaseminimum"][0] << "," << cg["phasemaximum"][0] << ",";

    cg = camPvl.FindGroup("EmissionAngle", Pvl::Traverse);
    outFile << cg["emissionminimum"][0] << "," << cg["emissionmaximum"][0] << ",";

    cg = camPvl.FindGroup("IncidenceAngle", Pvl::Traverse);
    outFile << cg["incidenceminimum"][0] << "," << cg["incidencemaximum"][0] << ",";

    cg = camPvl.FindGroup("LocalSolarTime", Pvl::Traverse);
    outFile << cg["localsolartimeMinimum"][0] << "," << cg["localsolartimeMaximum"][0] << ",";
  }

  //  Compute statistics for entire cube
  if(doStats) {
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
    double nPixels     = stats.TotalPixels();
    double nullpercent = (stats.NullPixels() / (nPixels)) * 100;
    double hispercent  = (stats.HisPixels() / (nPixels)) * 100;
    double hrspercent  = (stats.HrsPixels() / (nPixels)) * 100;
    double lispercent  = (stats.LisPixels() / (nPixels)) * 100;
    double lrspercent  = (stats.LrsPixels() / (nPixels)) * 100;
    //statitics keyword output for band

    outFile << stats.Average() << "," << stats.StandardDeviation() << ",";
    outFile << stats.Minimum() << "," << stats.Maximum() << ",";
    outFile << hispercent << "," << hrspercent << "," << lispercent << ",";
    outFile << lrspercent << "," << nullpercent << "," << stats.TotalPixels() << ",";
  }

  Camera *cam = icube->Camera();

  // for geometry, stats, camstats, or polygon get the info for each band
  BandGeometry bandGeom;
  if(doGeometry) {
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
    bandGeom.collect(*cam, *icube, doGeometry, false);

    // Check if the user requires valid image center geometry
    if(ui.GetBoolean("VCAMERA") && (!bandGeom.hasCenterGeometry())) {
      string msg = "Image center does not project in camera model";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }
    
    PvlObject geomGrp("Geometry");
    bandGeom.generateGeometryKeys(geomGrp);
    
    outFile << geomGrp["BandsUsed"][0] << "," << geomGrp["ReferenceBand"][0] << ",";
    outFile << geomGrp["OriginalBand"][0] << "," << geomGrp["Target"][0] << ",";
    outFile << geomGrp["StartTime"][0] << "," << geomGrp["EndTime"][0] << ",";
    outFile << geomGrp["CenterLine"][0] << "," << geomGrp["CenterSample"][0] << ",";
    outFile << geomGrp["CenterLatitude"][0] << "," << geomGrp["CenterLongitude"][0] << ",";
    outFile << geomGrp["CenterRadius"][0] << "," << geomGrp["RightAscension"][0] << ",";
    outFile << geomGrp["Declination"][0] << ",";

    outFile << geomGrp["UpperLeftLongitude"][0] << "," << geomGrp["UpperLeftLatitude"][0] << ",";
    outFile << geomGrp["LowerLeftLongitude"][0] << "," << geomGrp["LowerLeftLatitude"][0] << ",";
    outFile << geomGrp["LowerRightLongitude"][0] << "," << geomGrp["LowerRightLatitude"][0] << ",";
    outFile << geomGrp["UpperRightLongitude"][0] << "," << geomGrp["UpperRightLatitude"][0] << ",";

    outFile << geomGrp["PhaseAngle"][0] << "," << geomGrp["EmissionAngle"][0] << "," << geomGrp["IncidenceAngle"][0] << ",";

    outFile << geomGrp["NorthAzimuth"][0] << "," << geomGrp["OffNadir"][0] << ",";
    outFile << geomGrp["SolarLongitude"][0] << "," << geomGrp["LocalTime"][0] << ",";
    outFile << geomGrp["TargetCenterDistance"][0] << "," << geomGrp["SlantDistance"][0] << ",";

    outFile << geomGrp["SampleResolution"][0] << "," << geomGrp["LineResolution"][0] << ",";
    outFile << geomGrp["PixelResolution"][0] << "," << geomGrp["MeanGroundResolution"][0] << ",";

    outFile << geomGrp["SubSolarAzimuth"][0] << "," << geomGrp["SubSolarGroundAzimuth"][0] << ",";
    outFile << geomGrp["SubSolarLatitude"][0] << "," << geomGrp["SubSolarLongitude"][0] << ",";

    outFile << geomGrp["SubSpacecraftAzimuth"][0] << "," << geomGrp["SubSpacecraftGroundAzimuth"][0] << ",";
    outFile << geomGrp["SubSpacecraftLatitude"][0] << "," << geomGrp["SubSpacecraftLongitude"][0] << ",";

    outFile << geomGrp["ParallaxX"][0] << "," << geomGrp["ParallaxY"][0] << ",";

    outFile << geomGrp["ShadowX"][0] << "," << geomGrp["ShadowY"][0] << ",";

    //  Determine if image crosses Longitude domain
    outFile << geomGrp["HasLongitudeBoundary"][0] << "," << geomGrp["HasNorthPole"][0] << ",";
    outFile << geomGrp["HasSouthPole"][0];
  }
  
  outFile << endl;
  outFile.close();
}

