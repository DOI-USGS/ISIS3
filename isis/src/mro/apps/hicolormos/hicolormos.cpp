#include "Isis.h"

#include "Application.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "FileList.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Longitude.h"
#include "OriginalLabel.h"
#include "Process.h"
#include "ProgramLauncher.h"
#include "TProjection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TextFile.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

//functions in the code
void IsisMain() {

  // Get the values for the from 1 cube
  UserInterface &ui = Application::GetUserInterface();
  QString from1 = ui.GetFileName("FROM1");

  // Make a temporary list file for automos
  FileName tempFile = FileName::createTempFile("$TEMPORARY/hicolormos.temp.lis");
  TextFile tf;
  tf.Open(tempFile.expanded(), "output");
  tf.PutLine(from1 + "\n");

  Pvl from1lab(from1);
  PvlGroup from1Mosaic = from1lab.findGroup("Mosaic", Pvl::Traverse);

  // Make the procuct ID (from1 archive group)
  QString ProdId = from1lab.findGroup("Archive", Pvl::Traverse)["ObservationId"];
  ProdId += "_COLOR";

  // Prep for second image if we have one
  Pvl from2lab;
  Cube from2cube;
  PvlGroup from2Mosaic("Mosaic");
  if(ui.WasEntered("FROM2")) {
    QString from2 = ui.GetFileName("FROM2");
    //Add from2 file to the temporary automos input list
    tf.PutLine(from2 + "\n");
    from2lab.read(from2);

    // Test the observation ID between from1 and from2
    if((QString)from1lab.findGroup("Archive", Pvl::Traverse)["ObservationId"] !=
        (QString)from2lab.findGroup("Archive", Pvl::Traverse)["ObservationId"]) {
      QString msg = "Images not from the same observation";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    from2Mosaic = from2lab.findGroup("Mosaic", Pvl::Traverse);
    from2cube.open(ui.GetFileName("FROM2"), "r");
  }
  tf.Close();  // Close list remember to delete

  // Make the source product ID (from1 mosaic group)
  PvlKeyword sourceProductId = from1Mosaic["SourceProductId"];
  if(ui.WasEntered("FROM2")) {
    // Add source product Id for from2
    PvlKeyword from2SPI =  from2Mosaic["SourceProductId"];
    for(int i = 0; i < (int)from2SPI.size(); i++) {
      sourceProductId += from2SPI[i];
    }
  }

  // Work with latitude and longitude use projection factory (from1)
  TProjection *proj = (TProjection *) ProjectionFactory::CreateFromCube(from1lab);
  double minLat = proj->MinimumLatitude();
  double maxLat = proj->MaximumLatitude();
  double minLon = proj->MinimumLongitude();
  double maxLon = proj->MaximumLongitude();
  if(ui.WasEntered("FROM2")) {
    TProjection *proj = (TProjection *) ProjectionFactory::CreateFromCube(from2lab);
    if(proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
    if(proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
    if(proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
    if(proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
  }

  double avgLat = (minLat + maxLat) / 2;
  double avgLon = (minLon + maxLon) / 2;
  proj->SetGround(avgLat, avgLon);
  avgLat = proj->UniversalLatitude();
  avgLon = proj->UniversalLongitude();

  //Added 10/07 this bool will used to determain if need to use X and Y avg.
  // to find an intersection. Added because of polor image problems.
  bool runXY = true;
  // Use camera class to get from1 emsiion, phase, and incidence.
  double Cemiss;
  double Cphase;
  double Cincid;
  double ClocalSolTime;
  double CsolarLong;
  double CnorthAzimuth;
  double CsunAzimuth;

  //The code that sets the universal grond with projection avglat and avglon
  // has been left in to be backward compatiable.  See code below that sets
  // image,  this was in 10/07 because pole images would not find an
  // intersect in projection lat. lon. space.
  Cube from1cube(from1, "r");
  Camera *cam = CameraFactory::Create(from1cube);
  if(cam->SetUniversalGround(avgLat, avgLon)) {
    Cemiss = cam->EmissionAngle();
    Cphase = cam->PhaseAngle();
    Cincid = cam->IncidenceAngle();
    ClocalSolTime = cam->LocalSolarTime();
    CsolarLong = cam->solarLongitude().degrees();
    CnorthAzimuth = cam->NorthAzimuth();
    CsunAzimuth = cam->SunAzimuth();
    runXY = false;
  }
  else if(ui.WasEntered("FROM2")) {
    Camera *cam = CameraFactory::Create(from2cube);
    if(cam->SetUniversalGround(avgLat, avgLon)) {
      Cemiss = cam->EmissionAngle();
      Cphase = cam->PhaseAngle();
      Cincid = cam->IncidenceAngle();
      ClocalSolTime = cam->LocalSolarTime();
      CsolarLong = cam->solarLongitude().degrees();
      CnorthAzimuth = cam->NorthAzimuth();
      CsunAzimuth = cam->SunAzimuth();
      runXY = false;
    }
  }

  //The code within the if runXY was added in 10/07 to find an intersect with
  //pole images that would fail when using projection set universal ground.
  // This is run if no intersect is found when using lat and lon in
  // projection space.
  if(runXY) {
    TProjection *proj = (TProjection *) ProjectionFactory::CreateFromCube(from1lab);
    proj->SetWorld(0.5, 0.5);
    double startX = proj->XCoord();
    double endY = proj->YCoord();
    double nlines = from1lab.findGroup("Dimensions", Pvl::Traverse)["Lines"];
    double nsamps = from1lab.findGroup("Dimensions", Pvl::Traverse)["Samples"];
    proj->SetWorld((nsamps + 0.5), (nlines + 0.5));
    double endX = proj->XCoord();
    double startY = proj->YCoord();

    if(ui.WasEntered("FROM2")) {
      TProjection *proj = (TProjection *) ProjectionFactory::CreateFromCube(from2lab);
      proj->SetWorld(0.5, 0.5);
      if(proj->XCoord() < startX) startX = proj->XCoord();
      if(proj->YCoord() > endY) endY = proj->YCoord();
      nlines = from2lab.findGroup("Dimensions", Pvl::Traverse)["Lines"];
      nsamps = from2lab.findGroup("Dimensions", Pvl::Traverse)["Samples"];
      proj->SetWorld((nsamps + 0.5), (nlines + 0.5));
      if(proj->XCoord() > endX) endX = proj->XCoord();
      if(proj->YCoord() < startY) startY = proj->YCoord();
    }

    double avgX = (startX + endX) / 2;
    double avgY = (startY + endY) / 2;
    double sample = proj->ToWorldX(avgX);
    double line = proj->ToWorldY(avgY);
    Camera *cam = CameraFactory::Create(from1cube);
    if(cam->SetImage(sample, line)) {
      Cemiss = cam->EmissionAngle();
      Cphase = cam->PhaseAngle();
      Cincid = cam->IncidenceAngle();
      ClocalSolTime = cam->LocalSolarTime();
      CsolarLong = cam->solarLongitude().degrees();
      CnorthAzimuth = cam->NorthAzimuth();
      CsunAzimuth = cam->SunAzimuth();
      runXY = false;
    }
    else if(ui.WasEntered("FROM2")) {
      Camera *cam = CameraFactory::Create(from2cube);
      if(cam->SetImage(sample, line)) {
        Cemiss = cam->EmissionAngle();
        Cphase = cam->PhaseAngle();
        Cincid = cam->IncidenceAngle();
        ClocalSolTime = cam->LocalSolarTime();
        CsolarLong = cam->solarLongitude().degrees();
        CnorthAzimuth = cam->NorthAzimuth();
        CsunAzimuth = cam->SunAzimuth();
        runXY = false;
      }
    }
  }

  if(runXY) {
    QString tmp(tempFile.expanded());
    remove(tmp.toLatin1().data());
    QString msg = "Camera did not intersect images to gather stats";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  //work on the times (from mosaic group)
  QString startTime = from1Mosaic["StartTime"];
  QString stopTime = from1Mosaic["StopTime"];
  QString startClk = from1Mosaic["SpacecraftClockStartCount"];
  QString stopClk = from1Mosaic["SpacecraftClockStopCount"];

  if(ui.WasEntered("FROM2")) {
    if((QString)from2Mosaic["StartTime"] < startTime) startTime = (QString)from2Mosaic["StartTime"];
    if((QString)from2Mosaic["StopTime"] > stopTime) stopTime = (QString)from2Mosaic["StopTime"];
    if((QString)from2Mosaic["SpacecraftClockStartCount"] < startClk) startClk = (QString)from2Mosaic["SpacecraftClockStartCount"];
    if((QString)from2Mosaic["SpacecraftClockStopCount"] < stopClk) stopClk = (QString)from2Mosaic["SpacecraftClockStopCount"];
  }

  // Get TDI and summing array
  PvlKeyword cpmmTdiFlag = from1Mosaic["cpmmTdiFlag"];
  PvlKeyword cpmmSummingFlag = from1Mosaic["cpmmSummingFlag"];
  PvlKeyword specialProcessingFlag = from1Mosaic["SpecialProcessingFlag"];
  if(ui.WasEntered("FROM2")) {
    for(int i = 0; i < 14; i++) {
      if(! from2Mosaic["cpmmTdiFlag"].isNull(i)) {
        cpmmTdiFlag[i] = from2Mosaic["cpmmTdiFlag"][i];
      }
      if(!from2Mosaic["cpmmSummingFlag"].isNull(i)) {
        cpmmSummingFlag[i] = from2Mosaic["cpmmSummingFlag"][i];
      }
      if(!from2Mosaic["SpecialProcessingFlag"].isNull()) {
        specialProcessingFlag[i] = from2Mosaic["SpecialProcessingFlag"][i];
      }
    }
  }


  // automos step
  QString MosaicPriority = ui.GetString("PRIORITY");

  QString parameters = "FROMLIST=" + tempFile.expanded() +
                      " MOSAIC=" + ui.GetFileName("TO") +
                      " PRIORITY=" + MosaicPriority;
  ProgramLauncher::RunIsisProgram("automos", parameters);

  PvlGroup mos("Mosaic");
  mos += PvlKeyword("ProductId ", ProdId);
  mos += sourceProductId;
  mos += PvlKeyword("StartTime ", startTime);
  mos += PvlKeyword("SpacecraftClockStartCount ", startClk);
  mos += PvlKeyword("StopTime ", stopTime);
  mos += PvlKeyword("SpacecraftClockStopCount ", stopClk);
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

  //get the orginal label
  OriginalLabel from1OrgLab;
  from1OrgLab.Blob::Read(from1);

  Cube c;
  c.open(ui.GetFileName("TO"), "rw");
  c.label()->findObject("IsisCube", Pvl::Traverse).addGroup(mos);
  c.write(from1OrgLab);
  c.close();

  // Clean up the temporary automos list file
  QString tmp(tempFile.expanded());
  remove(tmp.toLatin1().data());
} // end of isis main


