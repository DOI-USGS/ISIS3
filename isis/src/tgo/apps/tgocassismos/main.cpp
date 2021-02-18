/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
#include "iTime.h"

using namespace std;
using namespace Isis;

#include <vector>

//functions in the code
void compareLabels(Pvl &match, Pvl &comp);


void IsisMain() {

  // Get the list of cubes to mosaic

  UserInterface &ui = Application::GetUserInterface();
  FileList fromList(ui.GetFileName("FROMLIST"));

  vector<Cube *> cubeList;
  try {
    if (fromList.size() < 1) {
      QString msg = "the list file [" + ui.GetFileName("FROMLIST") +
                    "does not contain any data";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // open all the cube and place in vector cubeList

    for (int i = 0; i < fromList.size(); i++) {
      Cube *cube = new Cube();
      cubeList.push_back(cube);
      cube->open(fromList[i].toString());
    }



    // run the compare function here.  This will compare the
    // labels of the first cube to the labels of each following cube.
    Pvl *matchLabel = cubeList[0]->label();
    for (int i = 0; i < (int)cubeList.size(); i++) {
      Pvl *compareLabel = cubeList[i]->label();
      compareLabels(*matchLabel, *compareLabel);
    }

    bool runXY = true;

    //calculate the min and max lon
    double minLat = DBL_MAX;
    double maxLat = -DBL_MAX;
    double minLon = DBL_MAX;
    double maxLon = -DBL_MAX;
    double avgLat;
    double avgLon;
    for (int i = 0; i < (int)cubeList.size(); i++) {
      TProjection *proj = (TProjection *) cubeList[i]->projection();
      if (proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if (proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if (proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if (proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }
    avgLat = (minLat + maxLat) / 2;
    avgLon = (minLon + maxLon) / 2;
    TProjection *proj = (TProjection *) cubeList[0]->projection();
    proj->SetGround(avgLat, avgLon);
    avgLat = proj->UniversalLatitude();
    avgLon = proj->UniversalLongitude();

    // Use camera class to get Inc., emi., phase, and other values
    double emissionAngle;
    double phaseAngle;
    double incidenceAngle;
    double localSolarTime;
    double solarLongitude;
    double sunAzimuth;
    double northAzimuth;
    for (int i = 0; i < (int)cubeList.size(); i++) {
      Camera *cam = cubeList[i]->camera();
      if (cam->SetUniversalGround(avgLat, avgLon)) {
        emissionAngle = cam->EmissionAngle();
        phaseAngle = cam->PhaseAngle();
        incidenceAngle = cam->IncidenceAngle();
        localSolarTime = cam->LocalSolarTime();
        solarLongitude = cam->solarLongitude().degrees();
        sunAzimuth = cam->SunAzimuth();
        northAzimuth = cam->NorthAzimuth();
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
      for (int i = 0; i < (int)cubeList.size(); i++) {
        TProjection *proj = (TProjection *) cubeList[i]->projection();
        proj->SetWorld(0.5, 0.5);
        if (i == 0) {
          startX = proj->XCoord();
          endY = proj->YCoord();
        }
        else {
          if (proj->XCoord() < startX) startX =  proj->XCoord();
          if (proj->YCoord() > endY) endY = proj->YCoord();
        }
        Pvl *p = cubeList[i]->label();
        double nlines = p->findGroup("Dimensions", Pvl::Traverse)["Lines"];
        double nsamps = p->findGroup("Dimensions", Pvl::Traverse)["Samples"];

        proj->SetWorld((nsamps + 0.5), (nlines + 0.5));
        if (i == 0) {
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

      for (int i = 0; i < (int)cubeList.size(); i++) {
        Camera *cam = cubeList[i]->camera();
        if (cam->SetImage(sample, line)) {
          emissionAngle = cam->EmissionAngle();
          phaseAngle = cam->PhaseAngle();
          incidenceAngle = cam->IncidenceAngle();
          localSolarTime = cam->LocalSolarTime();
          solarLongitude = cam->solarLongitude().degrees();
          sunAzimuth = cam->SunAzimuth();
          northAzimuth = cam->NorthAzimuth();
          runXY = false;
          break;
        }
      }
    }
    if (runXY) {
      QString msg = "Camera did not intersect images to gather stats";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // get the min SCLK values ( do this with QString comp.)
    // get the value from the original label blob
    QString startClock;
    QString firstStartTime;
    QString lastStartTime;
    QString instrumentId;
    QString spacecraftName;
    QString observationId;
    QString stopTime;
    QString exposureDuration;

    for (int i = 0; i < (int)cubeList.size(); i++) {
      // himos used original label here.

      Pvl *origLab = cubeList[i]->label();
      PvlGroup instGroup = origLab->findGroup("Instrument", Pvl::Traverse);
      PvlGroup archiveGroup = origLab->findGroup("Archive", Pvl::Traverse);

      if (i == 0) {
        spacecraftName = instGroup["SpacecraftName"][0];
        instrumentId =   instGroup["InstrumentId"][0];
        observationId =  archiveGroup["ObservationId"][0];
        firstStartTime =      instGroup["StartTime"][0];
        startClock =     instGroup["SpacecraftClockStartCount"][0];
        lastStartTime   =     instGroup["StartTime"][0];
        exposureDuration  = instGroup["ExposureDuration"][0];
      }
      else {
        // current cube's StartTime/StopTime values
        iTime currentStartTime = iTime(instGroup["StartTime"][0]);

        if (currentStartTime < iTime(firstStartTime)) {
          firstStartTime = currentStartTime.UTC();
          startClock = instGroup["SpacecraftClockStartCount"][0];
        }
        if (currentStartTime > iTime(lastStartTime)) {
          lastStartTime = currentStartTime.UTC();
        }
      }
    }

   // After selecting the last StartTime, calculate the StopTime
   iTime lastStartTimeValue = iTime(lastStartTime);
   iTime stopTimeValue = lastStartTimeValue + exposureDuration.toDouble();
   stopTime = stopTimeValue.UTC(3);

   // Get the archiveGroup from the first cube in the list
    PvlGroup archiveGroup = cubeList[0]->group("Archive");

    //close all cubes
    for (int i = 0; i < (int)cubeList.size(); i++) {
      cubeList[i]->close();
      delete cubeList[i];
    }
    cubeList.clear();

    // automos step
    QString list = ui.GetFileName("FROMLIST");
    QString toMosaic = ui.GetFileName("TO");
    QString mosaicPriority = ui.GetString("PRIORITY");

    QString parameters = "FROMLIST=" + list + " MOSAIC=" + toMosaic + " PRIORITY=" + mosaicPriority
                          + " TRACK=TRUE";

    if (QString::compare(ui.GetString("GRANGE"), "USER", Qt::CaseInsensitive) == 0) {
      parameters += " GRANGE=USER";
      parameters += " MINLAT=" + ui.GetAsString("MINLAT");
      parameters += " MAXLAT=" + ui.GetAsString("MAXLAT");
      parameters += " MINLON=" + ui.GetAsString("MINLON");
      parameters += " MAXLON=" + ui.GetAsString("MAXLON");
    }

    ProgramLauncher::RunIsisProgram("automos", parameters);

    // write out new information to new group mosaic
    PvlGroup mos("Mosaic");
    mos += PvlKeyword("SpacecraftName", spacecraftName);
    mos += PvlKeyword("InstrumentId", instrumentId);
    mos += PvlKeyword("ObservationId ", observationId);
    mos += PvlKeyword("StartTime ", firstStartTime);
    mos += PvlKeyword("StopTime ", stopTime);
    mos += PvlKeyword("SpacecraftClockStartCount ", startClock);
    mos += PvlKeyword("IncidenceAngle ", toString(incidenceAngle), "degrees");
    mos += PvlKeyword("EmissionAngle ", toString(emissionAngle), "degrees");
    mos += PvlKeyword("PhaseAngle ", toString(phaseAngle), "degrees");
    mos += PvlKeyword("LocalTime ", toString(localSolarTime));
    mos += PvlKeyword("SolarLongitude ", toString(solarLongitude), "degrees");
    mos += PvlKeyword("SubSolarAzimuth ", toString(sunAzimuth), "degrees");
    mos += PvlKeyword("NorthAzimuth ", toString(northAzimuth), "degrees");

    Cube mosCube;
    mosCube.open(ui.GetFileName("TO"), "rw");
    PvlObject &lab = mosCube.label()->findObject("IsisCube");
    lab.addGroup(mos);

    lab.addGroup(archiveGroup);
    mosCube.close();
  }
  catch(IException &e) {
    e.print();
    for (int i = 0; i < (int)cubeList.size(); i++) {
      cubeList[i]->close();
      delete cubeList[i];
    }
    QString msg = "The mosaic [" + ui.GetFileName("TO") + "] was NOT created";
    throw IException(IException::User, msg, _FILEINFO_);
  }
} // end of isis main


/**
 * Function to verify that label values match.
 */
void compareLabels(Pvl &matchLabel, Pvl &compareLabel) {
  // test of the ObservationId
   PvlGroup matchArchive = matchLabel.findGroup("Archive", Pvl::Traverse);
   PvlGroup compareArchive = compareLabel.findGroup("Archive", Pvl::Traverse);

   QString matchObsId = matchArchive["ObservationId"];
   QString compareObsId = compareArchive["ObservationId"];

   if (matchObsId != compareObsId) {
     QString msg = "Images not from the same observation";
     throw IException(IException::User, msg, _FILEINFO_);
   }

  // Test of the BandBin filter name
  PvlGroup matchBandBin = matchLabel.findGroup("BandBin", Pvl::Traverse);
  PvlGroup compareBandBin = compareLabel.findGroup("BandBin", Pvl::Traverse);
  QString matchFilter = matchBandBin["FilterName"];
  QString compareFilter = compareBandBin["FilterName"];

  if (matchFilter != compareFilter) {
    QString msg = "Images not the same filter";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}
