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
#include "TProjection.h"
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



    // run the compare function here.  This will conpare the
    // labels of the first cube to the labels of each following cube.
//     PvlKeyword sourceProductId("SourceProductId");
    QString ProdId;
    Pvl *pmatch = clist[0]->label();
    for(int i = 0; i < (int)clist.size(); i++) {
      Pvl *pcomp = clist[i]->label();
      CompareLabels(*pmatch, *pcomp);
      ProdId = (QString)pmatch->findGroup("Archive", Pvl::Traverse)["UniqueIdentifier"];
      QString bandname = (QString)pmatch->findGroup("BandBin", Pvl::Traverse)["FilterName"];
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
      TProjection *proj = (TProjection *) clist[i]->projection();
      if(proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if(proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if(proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if(proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }
    avgLat = (minLat + maxLat) / 2;
    avgLon = (minLon + maxLon) / 2;
    TProjection *proj = (TProjection *) clist[0]->projection();
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
        TProjection *proj = (TProjection *) clist[i]->projection();
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
        double nlines = p->findGroup("Dimensions", Pvl::Traverse)["Lines"];
        double nsamps = p->findGroup("Dimensions", Pvl::Traverse)["Samples"];

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

    // get the min SCLK values ( do this with QString comp.)
    // get the value from the original label blob
    QString startClock;
    QString startTime;
    for(int i = 0; i < (int)clist.size(); i++) {
      // himos used original label here. 

      Pvl *origLab = clist[i]->label();
      PvlGroup timegrp = origLab->findGroup("Instrument", Pvl::Traverse);

      if(i == 0) {
        startClock = (QString)timegrp["SpacecraftClockStartCount"];
        startTime = (QString)timegrp["StartTime"];
      }
      else {
        QString testStartTime = (QString)timegrp["StartTime"];
        if(testStartTime < startTime) {
          startTime = testStartTime;
          startClock = (QString)timegrp["SpacecraftClockStartCount"];
        }
      }
    }

    // Get the blob of original labels from first image in list
   // Pvl *org = clist[0]->label();

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
//     mos += PvlKeyword(sourceProductId);
    mos += PvlKeyword("StartTime ", startTime);
    mos += PvlKeyword("SpacecraftClockStartCount ", startClock);
    mos += PvlKeyword("IncidenceAngle ", toString(Cincid), "DEG");
    mos += PvlKeyword("EmissionAngle ", toString(Cemiss), "DEG");
    mos += PvlKeyword("PhaseAngle ", toString(Cphase), "DEG");
    mos += PvlKeyword("LocalTime ", toString(ClocalSolTime), "LOCALDAY/24");
    mos += PvlKeyword("SolarLongitude ", toString(CsolarLong), "DEG");
    mos += PvlKeyword("SubSolarAzimuth ", toString(CsunAzimuth), "DEG");
    mos += PvlKeyword("NorthAzimuth ", toString(CnorthAzimuth), "DEG");

    Cube mosCube;
    mosCube.open(ui.GetFileName("TO"), "rw");
    PvlObject &lab = mosCube.label()->findObject("IsisCube");
    lab.addGroup(mos);
    //add orginal label blob to the output cube
//    mosCube.write(org);
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
//   PvlGroup matchgrp = pmatch.findGroup("Archive", Pvl::Traverse);
//   PvlGroup compgrp = pcomp.findGroup("Archive", Pvl::Traverse);
//   QString obsMatch = matchgrp["ObservationId"];
//   QString obsComp = compgrp["ObservationId"];
// 
//   if(obsMatch != obsComp) {
//     QString msg = "Images not from the same observation";
//     throw IException(IException::User, msg, _FILEINFO_);
//   }

  // Test of the BandBin filter name
  PvlGroup bmatchgrp = pmatch.findGroup("BandBin", Pvl::Traverse);
  PvlGroup bcompgrp = pcomp.findGroup("BandBin", Pvl::Traverse);
  QString bandMatch = bmatchgrp["FilterName"];
  QString bandComp = bcompgrp["FilterName"];

  if(bandMatch != bandComp) {
    QString msg = "Images not the same filter";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}
