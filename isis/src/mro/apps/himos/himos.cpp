#include "Isis.h"

#include "Application.h"
#include "Camera.h"
#include "Cube.h"
#include "FileList.h"
#include "IException.h"
#include "Pvl.h"
#include "IString.h"
#include "Longitude.h"
#include "OriginalLabel.h"
#include "Process.h"
#include "ProgramLauncher.h"
#include "Projection.h"
#include "UserInterface.h"


using namespace std;
using namespace Isis;

#include <vector>

//functions in the code
void CompareLabels(Pvl &match, Pvl &comp);


void IsisMain() {

  // Get the list of cubes to mosaic

  UserInterface &ui = Application::GetUserInterface();
  FileList flist(ui.GetFileName("FROMLIST"));


  vector<Cube *> clist;
  try {
    if(flist.size() < 1) {
      QString msg = "the list file [" + ui.GetFileName("FROMLIST") +
                    "does not contain any data";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // open all the cube and place in vector clist

    for(int i = 0; i < flist.size(); i++) {
      Cube *c = new Cube();
      clist.push_back(c);
      c->open(flist[i].toString());
    }



    // run the compair function here.  This will conpair the
    // labels of the first cube to the labels of each following cube.
    PvlKeyword sourceProductId("SourceProductId");
    QString ProdId;
    for(int i = 0; i < (int)clist.size(); i++) {
      Pvl *pmatch = clist[0]->label();
      Pvl *pcomp = clist[i]->label();
      CompareLabels(*pmatch, *pcomp);
      PvlGroup g = pcomp->FindGroup("Instrument", Pvl::Traverse);
      if(g.HasKeyword("StitchedProductIds")) {
        PvlKeyword k = g["StitchedProductIds"];
        for(int j = 0; j < (int)k.Size(); j++) {
          sourceProductId += g["stitchedProductIds"][j];
        }
      }
      ProdId = (QString)pmatch->FindGroup("Archive", Pvl::Traverse)["ObservationId"];
      QString bandname = (QString)pmatch->FindGroup("BandBin", Pvl::Traverse)["Name"];
      bandname = bandname.toUpper();
      ProdId = ProdId + "_" + bandname;
    }
    bool runXY = true;

    //calculate the min and max lon
    double minLat = DBL_MAX;
    double maxLat = -DBL_MAX;
    double minLon = DBL_MAX;
    double maxLon = -DBL_MAX;
    double avgLat;
    double avgLon;
    for(int i = 0; i < (int)clist.size(); i++) {
      Projection *proj = clist[i]->projection();
      if(proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if(proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if(proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if(proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }
    avgLat = (minLat + maxLat) / 2;
    avgLon = (minLon + maxLon) / 2;
    Projection *proj = clist[0]->projection();
    proj->SetGround(avgLat, avgLon);
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
    for(int i = 0; i < (int)clist.size(); i++) {
      Camera *cam = clist[i]->camera();
      if(cam->SetUniversalGround(avgLat, avgLon)) {
        Cemiss = cam->EmissionAngle();
        Cphase = cam->PhaseAngle();
        Cincid = cam->IncidenceAngle();
        ClocalSolTime = cam->LocalSolarTime();
        CsolarLong = cam->solarLongitude().degrees();
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
    if(runXY) {
      double startX = DBL_MAX;
      double endX = DBL_MIN;
      double startY = DBL_MAX;
      double endY =  DBL_MIN;
      for(int i = 0; i < (int)clist.size(); i++) {
        Projection *proj = clist[i]->projection();
        proj->SetWorld(0.5, 0.5);
        if(i == 0) {
          startX = proj->XCoord();
          endY = proj->YCoord();
        }
        else {
          if(proj->XCoord() < startX) startX =  proj->XCoord();
          if(proj->YCoord() > endY) endY = proj->YCoord();
        }
        Pvl *p = clist[i]->label();
        double nlines = p->FindGroup("Dimensions", Pvl::Traverse)["Lines"];
        double nsamps = p->FindGroup("Dimensions", Pvl::Traverse)["Samples"];

        proj->SetWorld((nsamps + 0.5), (nlines + 0.5));
        if(i == 0) {
          endX = proj->XCoord();
          startY = proj->YCoord();
        }
        else {
          if(proj->XCoord() > endX) endX =  proj->XCoord();
          if(proj->YCoord() < startY) startY = proj->YCoord();
        }
      }

      double avgX = (startX + endX) / 2;
      double avgY = (startY + endY) / 2;
      double sample = proj->ToWorldX(avgX);
      double line = proj->ToWorldY(avgY);

      for(int i = 0; i < (int)clist.size(); i++) {
        Camera *cam = clist[i]->camera();
        if(cam->SetImage(sample, line)) {
          Cemiss = cam->EmissionAngle();
          Cphase = cam->PhaseAngle();
          Cincid = cam->IncidenceAngle();
          ClocalSolTime = cam->LocalSolarTime();
          CsolarLong = cam->solarLongitude().degrees();
          CsunAzimuth = cam->SunAzimuth();
          CnorthAzimuth = cam->NorthAzimuth();
          runXY = false;
          break;
        }
      }
    }
    if(runXY) {
      QString msg = "Camera did not intersect images to gather stats";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // get the min and max SCLK values ( do this with QString comp.)
    // get the value from the original label blob
    QString startClock;
    QString stopClock;
    QString startTime;
    QString stopTime;
    for(int i = 0; i < (int)clist.size(); i++) {
      OriginalLabel origLab;
      clist[i]->read(origLab);
      PvlGroup timegrp = origLab.ReturnLabels().FindGroup("TIME_PARAMETERS", Pvl::Traverse);
      if(i == 0) {
        startClock = (QString)timegrp["SpacecraftClockStartCount"];
        stopClock = (QString)timegrp["SpacecraftClockStopCount"];
        startTime = (QString)timegrp["StartTime"];
        stopTime = (QString)timegrp["StopTime"];
      }
      else {
        QString testStartTime = (QString)timegrp["StartTime"];
        QString testStopTime = (QString)timegrp["StopTime"];
        if(testStartTime < startTime) {
          startTime = testStartTime;
          startClock = (QString)timegrp["SpacecraftClockStartCount"];
        }
        if(testStopTime > stopTime) {
          stopTime = testStopTime;
          stopClock = (QString)timegrp["spacecraftClockStopCount"];
        }
      }
    }

    //  Concatenate all TDI's and summing and specialProcessingFlat into one keyword
    PvlKeyword cpmmTdiFlag("cpmmTdiFlag");
    PvlKeyword cpmmSummingFlag("cpmmSummingFlag");
    PvlKeyword specialProcessingFlag("SpecialProcessingFlag");
    for(int i = 0; i < 14; i++) {
      cpmmTdiFlag += (QString)"";
      cpmmSummingFlag += (QString)"";
      specialProcessingFlag += (QString)"";
    }

    for(int i = 0; i < (int)clist.size(); i++) {
      Pvl *clab = clist[i]->label();
      PvlGroup cInst = clab->FindGroup("Instrument", Pvl::Traverse);
      OriginalLabel cOrgLab;
      clist[i]->read(cOrgLab);
      PvlGroup cGrp = cOrgLab.ReturnLabels().FindGroup("INSTRUMENT_SETTING_PARAMETERS", Pvl::Traverse);
      cpmmTdiFlag[(int)cInst["CpmmNumber"]] = (QString) cGrp["MRO:TDI"];
      cpmmSummingFlag[(int)cInst["CpmmNumber"]] = (QString) cGrp["MRO:BINNING"];

      if(cInst.HasKeyword("Special_Processing_Flag")) {
        specialProcessingFlag[cInst["CpmmNumber"]] = (QString) cInst["Special_Processing_Flag"];
      }
      else {
        // there may not be the keyword Special_Processing_Flag if no
        //keyword then set the output to NOMINAL
        specialProcessingFlag[cInst["CpmmNumber"]] = "NOMINAL";
      }
    }


    // Get the blob of original labels from first image in list
    OriginalLabel org;
    clist[0]->read(org);

    //close all cubes
    for(int i = 0; i < (int)clist.size(); i++) {
      clist[i]->close();
      delete clist[i];
    }
    clist.clear();

    // automos step
    QString list = ui.GetFileName("FROMLIST");
    QString toMosaic = ui.GetFileName("TO");
    QString MosaicPriority = ui.GetString("PRIORITY");

    QString parameters = "FROMLIST=" + list + " MOSAIC=" + toMosaic + " PRIORITY=" + MosaicPriority;
    ProgramLauncher::RunIsisProgram("automos", parameters);

    // write out new information to new group mosaic

    PvlGroup mos("Mosaic");
    mos += PvlKeyword("ProductId ", ProdId);
    mos += PvlKeyword(sourceProductId);
    mos += PvlKeyword("StartTime ", startTime);
    mos += PvlKeyword("SpacecraftClockStartCount ", startClock);
    mos += PvlKeyword("StopTime ", stopTime);
    mos += PvlKeyword("SpacecraftClockStopCount ", stopClock);
    mos += PvlKeyword("IncidenceAngle ", toString(Cincid), "DEG");
    mos += PvlKeyword("EmissionAngle ", toString(Cemiss), "DEG");
    mos += PvlKeyword("PhaseAngle ", toString(Cphase), "DEG");
    mos += PvlKeyword("LocalTime ", toString(ClocalSolTime), "LOCALDAY/24");
    mos += PvlKeyword("SolarLongitude ", toString(CsolarLong), "DEG");
    mos += PvlKeyword("SubSolarAzimuth ", toString(CsunAzimuth), "DEG");
    mos += PvlKeyword("NorthAzimuth ", toString(CnorthAzimuth), "DEG");
    mos += cpmmTdiFlag;
    mos += cpmmSummingFlag;
    mos += specialProcessingFlag;

    Cube mosCube;
    mosCube.open(ui.GetFileName("TO"), "rw");
    PvlObject &lab = mosCube.label()->FindObject("IsisCube");
    lab.AddGroup(mos);
    //add orginal label blob to the output cube
    mosCube.write(org);
    mosCube.close();

  }
  catch(IException &e) {
    for(int i = 0; i < (int)clist.size(); i++) {
      clist[i]->close();
      delete clist[i];
    }
    QString msg = "The mosaic [" + ui.GetFileName("TO") + "] was NOT created";
    throw IException(IException::User, msg, _FILEINFO_);
  }
} // end of isis main

//Function to compare label - CompareLabels
void CompareLabels(Pvl &pmatch, Pvl &pcomp) {
  // test of the ObservationId
  PvlGroup matchgrp = pmatch.FindGroup("Archive", Pvl::Traverse);
  PvlGroup compgrp = pcomp.FindGroup("Archive", Pvl::Traverse);
  QString obsMatch = matchgrp["ObservationId"];
  QString obsComp = compgrp["ObservationId"];

  if(obsMatch != obsComp) {
    QString msg = "Images not from the same observation";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Test of the BandBin filter name
  PvlGroup bmatchgrp = pmatch.FindGroup("BandBin", Pvl::Traverse);
  PvlGroup bcompgrp = pcomp.FindGroup("BandBin", Pvl::Traverse);
  QString bandMatch = bmatchgrp["Name"];
  QString bandComp = bcompgrp["Name"];

  if(bandMatch != bandComp) {
    QString msg = "Images not the same filter";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}
