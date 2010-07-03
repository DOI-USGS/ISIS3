#include "Isis.h"
#include "Process.h"
#include "FileList.h"
#include "iException.h"
#include "Cube.h"
#include "Camera.h"
#include "UserInterface.h"
#include "Pvl.h"
#include "Application.h"
#include "iString.h"
#include "OriginalLabel.h"


using namespace std; 
using namespace Isis;

#include <vector>

//functions in the code
void CompareLabels(Pvl &match, Pvl &comp);


void IsisMain() {

  // Get the list of cubes to mosaic

  UserInterface &ui = Application::GetUserInterface();
  FileList flist(ui.GetFilename("FROMLIST"));


  vector<Cube *> clist;
  try {
    if (flist.size() < 1) {
      string msg = "the list file [" +ui.GetFilename("FROMLIST") +
                   "does not contain any data";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // open all the cube and place in vector clist  

    for (int i=0; i<(int)flist.size(); i++) {
      Cube *c = new Cube();
      clist.push_back(c);
      c->Open(flist[i]);
    }



    // run the compair function here.  This will conpair the 
    // labels of the first cube to the labels of each following cube. 
    PvlKeyword sourceProductId("SourceProductId");
    string ProdId;
    for (int i=0; i<(int)clist.size(); i++) {
      Pvl *pmatch = clist[0]->Label();
      Pvl *pcomp = clist[i]->Label();
      CompareLabels(*pmatch, *pcomp);
      PvlGroup g = pcomp->FindGroup("Instrument",Pvl::Traverse);
      if (g.HasKeyword("StitchedProductIds")) {
        PvlKeyword k = g["StitchedProductIds"];
        for (int j=0; j<(int)k.Size(); j++) {
          sourceProductId += g["stitchedProductIds"][j];
        }     
      }
      ProdId = (string)pmatch->FindGroup("Archive",Pvl::Traverse)["ObservationId"];
      iString bandname = (string)pmatch->FindGroup("BandBin",Pvl::Traverse)["Name"];
      bandname = bandname.UpCase();
      ProdId = ProdId + "_" + bandname;
    }
    bool runXY=true;

    //calculate the min and max lon
    double minLat = DBL_MAX;
    double maxLat = -DBL_MAX;
    double minLon = DBL_MAX;
    double maxLon = -DBL_MAX;
    double avgLat;
    double avgLon;
    for (int i=0; i<(int)clist.size(); i++) {
      Projection *proj = clist[i]->Projection();
      if (proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if (proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if (proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if (proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }
    avgLat = (minLat + maxLat) / 2;
    avgLon = (minLon + maxLon) / 2;
    Projection *proj = clist[0]->Projection();
    proj->SetGround(avgLat,avgLon);
    avgLat = proj->UniversalLatitude();
    avgLon = proj->UniversalLongitude();

    // Use camera class to get Inc., emi., phase, and other values
    double Cemiss;
    double Cphase;
    double Cincid;
    double ClocalSolTime;
    double CsolarLong;
    double CsunAzimuth;
    double CnorthAzimuth;
    for (int i=0; i<(int)clist.size(); i++) {
      Camera *cam = clist[i]->Camera();
      if (cam->SetUniversalGround(avgLat,avgLon)) {
        Cemiss = cam->EmissionAngle();
        Cphase = cam->PhaseAngle();
        Cincid = cam->IncidenceAngle();
        ClocalSolTime = cam->LocalSolarTime();
        CsolarLong = cam->SolarLongitude();
        CsunAzimuth = cam->SunAzimuth();
        CnorthAzimuth = cam->NorthAzimuth();
        runXY = false;
        break;
      }
    }

    //The code within the if runXY was added in 10/07 to find an intersect with
    //pole images that would fail when using projection set universal ground.  
    // This is run if no intersect is found when using lat and lon in 
    // projection space.
    if (runXY) {
      double startX = DBL_MAX;
      double endX = DBL_MIN;
      double startY = DBL_MAX;
      double endY =  DBL_MIN;
      for (int i=0; i<(int)clist.size(); i++) {
        Projection *proj = clist[i]->Projection();
        proj->SetWorld(0.5,0.5);
        if (i==0) {
          startX = proj->XCoord();
          endY = proj->YCoord();
        }
        else {
          if (proj->XCoord() < startX) startX =  proj->XCoord();
          if (proj->YCoord() > endY) endY = proj->YCoord();
        }
        Pvl *p = clist[i]->Label();
        double nlines = p->FindGroup("Dimensions",Pvl::Traverse)["Lines"];
        double nsamps = p->FindGroup("Dimensions",Pvl::Traverse)["Samples"];

        proj->SetWorld((nsamps+0.5),(nlines+0.5));
        if (i==0) {
          endX = proj->XCoord();
          startY = proj->YCoord();
        }
        else {
          if (proj->XCoord() > endX) endX =  proj->XCoord();
          if (proj->YCoord() < startY) startY = proj->YCoord();
        }
      }

      double avgX = (startX + endX) / 2;
      double avgY = (startY + endY) / 2;
      double sample = proj->ToWorldX(avgX);
      double line = proj->ToWorldY(avgY);

      for (int i=0; i<(int)clist.size(); i++) {
        Camera *cam = clist[i]->Camera();
        if (cam->SetImage(sample,line)) {
          Cemiss = cam->EmissionAngle();
          Cphase = cam->PhaseAngle();
          Cincid = cam->IncidenceAngle();
          ClocalSolTime = cam->LocalSolarTime();
          CsolarLong = cam->SolarLongitude();
          CsunAzimuth = cam->SunAzimuth();
          CnorthAzimuth = cam->NorthAzimuth();
          runXY = false;
          break;
        }
      }
    }
    if (runXY) {
      string msg = "Camera did not intersect images to gather stats";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // get the min and max SCLK values ( do this with string comp.)
    // get the value from the original label blob
    string startClock;
    string stopClock;
    string startTime;
    string stopTime;
    for (int i=0; i<(int)clist.size(); i++) {
      OriginalLabel origLab;
      clist[i]->Read(origLab);
      PvlGroup timegrp = origLab.ReturnLabels().FindGroup("TIME_PARAMETERS",Pvl::Traverse);
      if (i==0) {
        startClock = (string)timegrp["SpacecraftClockStartCount"];
        stopClock = (string)timegrp["SpacecraftClockStopCount"];
        startTime = (string)timegrp["StartTime"];
        stopTime = (string)timegrp["StopTime"];
      }
      else {
        string testStartTime = (string)timegrp["StartTime"];
        string testStopTime = (string)timegrp["StopTime"];
        if (testStartTime < startTime) {
          startTime = testStartTime;
          startClock = (string)timegrp["SpacecraftClockStartCount"];
        }
        if (testStopTime > stopTime) {
          stopTime = testStopTime;
          stopClock = (string)timegrp["spacecraftClockStopCount"];
        }
      }
    }

    //  Concatenate all TDI's and summing and specialProcessingFlat into one keyword 
    PvlKeyword cpmmTdiFlag("cpmmTdiFlag");
    PvlKeyword cpmmSummingFlag("cpmmSummingFlag");
    PvlKeyword specialProcessingFlag("SpecialProcessingFlag");
    for (int i=0; i<14; i++) {
      cpmmTdiFlag +=(string)"";
      cpmmSummingFlag +=(string)"";
      specialProcessingFlag +=(string)"";
    }

    for (int i=0; i<(int)clist.size(); i++) {
      Pvl *clab = clist[i]->Label();
      PvlGroup cInst = clab->FindGroup("Instrument",Pvl::Traverse);
      OriginalLabel cOrgLab;
      clist[i]->Read(cOrgLab);
      PvlGroup cGrp = cOrgLab.ReturnLabels().FindGroup("INSTRUMENT_SETTING_PARAMETERS",Pvl::Traverse);
      cpmmTdiFlag[(int)cInst["CpmmNumber"]] = (string) cGrp["MRO:TDI"];
      cpmmSummingFlag[(int)cInst["CpmmNumber"]] = (string) cGrp["MRO:BINNING"];

      if (cInst.HasKeyword("Special_Processing_Flag")) {
        specialProcessingFlag[cInst["CpmmNumber"]] = (string) cInst["Special_Processing_Flag"];
      }
      else {
        // there may not be the keyword Special_Processing_Flag if no
        //keyword then set the output to NOMINAL
        specialProcessingFlag[cInst["CpmmNumber"]] = "NOMINAL";
      }
    }


    // Get the blob of original labels from first image in list
    OriginalLabel org;
    clist[0]->Read(org);

    //close all cubes
    for (int i=0; i<(int)clist.size(); i++) {
      clist[i]->Close();
      delete clist[i];
    }
    clist.clear();

    // automos step
    string list = ui.GetFilename("FROMLIST");
    string toMosaic = ui.GetFilename("TO");
    string MosaicPriority = ui.GetString("PRIORITY");

    string parameters = "FROMLIST=" + list + " MOSAIC=" + toMosaic + " PRIORITY=" + MosaicPriority;
    Isis::iApp ->Exec("automos",parameters);

    // write out new information to new group mosaic 

    PvlGroup mos("Mosaic");
    mos += PvlKeyword("ProductId ", ProdId);
    mos += PvlKeyword(sourceProductId); 
    mos += PvlKeyword("StartTime ", startTime);
    mos += PvlKeyword("SpacecraftClockStartCount ", startClock);
    mos += PvlKeyword("StopTime ", stopTime);
    mos += PvlKeyword("SpacecraftClockStopCount ", stopClock);
    mos += PvlKeyword("IncidenceAngle ", Cincid, "DEG");
    mos += PvlKeyword("EmissionAngle ", Cemiss, "DEG");
    mos += PvlKeyword("PhaseAngle ", Cphase, "DEG");
    mos += PvlKeyword("LocalTime ", ClocalSolTime, "LOCALDAY/24");
    mos += PvlKeyword("SolarLongitude ", CsolarLong, "DEG");
    mos += PvlKeyword("SubSolarAzimuth ", CsunAzimuth, "DEG");
    mos += PvlKeyword("NorthAzimuth ", CnorthAzimuth, "DEG");
    mos += cpmmTdiFlag;
    mos += cpmmSummingFlag;
    mos += specialProcessingFlag;

    Cube mosCube;
    mosCube.Open(ui.GetFilename("TO"), "rw");
    PvlObject &lab=mosCube.Label()->FindObject("IsisCube");
    lab.AddGroup(mos);
    //add orginal label blob to the output cube
    mosCube.Write(org);
    mosCube.Close();

  }
  catch (iException &e) {
    for (int i=0; i<(int)clist.size(); i++) {
      clist[i]->Close();
      delete clist[i];
    }
    string msg = "The mosaic [" + ui.GetFilename("TO") + "] was NOT created";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
} // end of isis main

//Function to compare label - CompareLabels
void CompareLabels(Pvl &pmatch, Pvl &pcomp) {
  // test of the ObservationId
  PvlGroup matchgrp = pmatch.FindGroup("Archive",Pvl::Traverse);
  PvlGroup compgrp = pcomp.FindGroup("Archive",Pvl::Traverse);
  string obsMatch = matchgrp["ObservationId"];
  string obsComp = compgrp["ObservationId"];

  if (obsMatch != obsComp) {
    string msg = "Images not from the same observation";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Test of the BandBin filter name  
  PvlGroup bmatchgrp = pmatch.FindGroup("BandBin",Pvl::Traverse);
  PvlGroup bcompgrp = pcomp.FindGroup("BandBin",Pvl::Traverse);
  string bandMatch = bmatchgrp["Name"];
  string bandComp = bcompgrp["Name"];

  if (bandMatch != bandComp) {
    string msg = "Images not the same filter";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
}
