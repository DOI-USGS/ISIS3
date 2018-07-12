#include "Isis.h"

#include "Application.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Attempt to open PVL file.
  Pvl pvlFile;
  try {
    pvlFile.read(ui.GetString("FROMPVL"));
  }
  catch (IException &e) {
    QString msg = "Unable to open [" + ui.GetString("FROMPVL") + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  PvlObject tilePvl = pvlFile.object(0);
  QString tileName = tilePvl.name();

  if (!tilePvl.hasKeyword("Columns")) {
    QString msg = "PVL Object [" + tileName + "] is missing [Columns] Keyword.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  int columns = tilePvl.findKeyword("Columns")[0].toInt();

  if (!tilePvl.hasGroup("Fiducial Marks")) {
    QString msg = "PVL Object [" + tileName + "] is missing [Fiducial Marks] Group.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  PvlGroup fiducialGroup = tilePvl.findGroup("Fiducial Marks");

  PvlGroup timingGroup = tilePvl.findGroup("Timing Marks");
  if (!tilePvl.hasGroup("Timing Marks")) {
    QString msg = "PVL Object [" + tileName + "] is missing [Timing Marks] Group.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Build the Fiducial Mark network

  PvlKeyword fiducialSamples = fiducialGroup.findKeyword("Sample");
  PvlKeyword fiducialLines   = fiducialGroup.findKeyword("Line");
  PvlKeyword fiducialNumbers = fiducialGroup.findKeyword("Number");
  PvlKeyword fiducialValid   = fiducialGroup.findKeyword("Valid");

  // Old output files don't have these so we need to check for them.
  PvlKeyword fiducialResidualX;
  PvlKeyword fiducialResidualY;
  PvlKeyword fiducialResidualMagnitude;
  if (fiducialGroup.hasKeyword("Residual_X")) {
    fiducialResidualX         = fiducialGroup.findKeyword("Residual_X");
    fiducialResidualY         = fiducialGroup.findKeyword("Residual_Y");
    fiducialResidualMagnitude = fiducialGroup.findKeyword("Residual_Magnitude");
  }

  PvlObject fiducialNetwork("ControlNetwork");
  fiducialNetwork += PvlKeyword("NetworkId",     tileName + "_FID");
  fiducialNetwork += PvlKeyword("TargetName",   "MOON");
  fiducialNetwork += PvlKeyword("UserName",      Application::UserName());
  fiducialNetwork += PvlKeyword("Created",       Application::DateTime());
  fiducialNetwork += PvlKeyword("LastModified",  Application::DateTime());
  fiducialNetwork += PvlKeyword("Description",  "apolloPanProcess fiducial mark output");

  int fiducialCount = fiducialSamples.size();
  for (int i = 0; i < fiducialCount; i++) {
    PvlGroup measure("ControlMeasure");
    measure += PvlKeyword("SerialNumber",    tileName + ".cub");
    measure += PvlKeyword("MeasureType",    "Candidate");
    measure += PvlKeyword("ChooserName",    "Application apolloPanProcess");
    measure += PvlKeyword("DateTime",        Application::DateTime());
    measure += PvlKeyword("Sample",          fiducialSamples[i]);
    measure += PvlKeyword("Line",            fiducialLines[i]);
    if (fiducialResidualX.size() > 0) {
      measure += PvlKeyword("ErrorLine",      fiducialResidualX[i]);
      measure += PvlKeyword("ErrorSample",    fiducialResidualY[i]);
      measure += PvlKeyword("ErrorMagnitude", fiducialResidualMagnitude[i]);
    }
    else {
      measure += PvlKeyword("ErrorLine",      "0");
      measure += PvlKeyword("ErrorSample",    "0");
      measure += PvlKeyword("ErrorMagnitude", "0");
    }

    PvlObject point("ControlPoint");
    point += PvlKeyword("PointType", "Free");
    if (fiducialValid[i] == "0") {
      point += PvlKeyword("Ignore", "True");
    }
    else {
      point += PvlKeyword("EditLock", "True");
    }
    point += PvlKeyword("PointId", QString::number(i));
    point += measure;

    fiducialNetwork += point;
  }

  // Build the Timing Mark network

  PvlKeyword timingSamples = timingGroup.findKeyword("Sample");
  PvlKeyword timingLines   = timingGroup.findKeyword("Line");
  PvlKeyword timingLengths = timingGroup.findKeyword("Length");
  PvlKeyword timingValid   = timingGroup.findKeyword("Valid");
  PvlObject timingNetwork("ControlNetwork");
  timingNetwork += PvlKeyword("NetworkId",    tileName + "_TIM");
  timingNetwork += PvlKeyword("NetworkType",  "ImageToImage");
  timingNetwork += PvlKeyword("TargetName",   "MOON");
  timingNetwork += PvlKeyword("UserName",     Application::UserName());
  timingNetwork += PvlKeyword("Created",      Application::DateTime());
  timingNetwork += PvlKeyword("LastModified", Application::DateTime());
  timingNetwork += PvlKeyword("Description",  "apolloPanProcess timing mark output");
  int timingCount = timingSamples.size();
  for (int i = 0; i < timingCount; i++) {
    PvlGroup startMeasure("ControlMeasure");
    startMeasure += PvlKeyword("SerialNumber",   tileName + ".cub");
    startMeasure += PvlKeyword("MeasureType",    "Candidate");
    startMeasure += PvlKeyword("Sample",         QString::number(timingSamples[i].toInt() 
                                                                 + timingLengths[i].toInt()));
    startMeasure += PvlKeyword("Line",           timingLines[i]);
    startMeasure += PvlKeyword("ErrorLine",      "0");
    startMeasure += PvlKeyword("ErrorSample",    "0");
    startMeasure += PvlKeyword("ErrorMagnitude", "0");
    startMeasure += PvlKeyword("DateTime",       Application::DateTime());
    startMeasure += PvlKeyword("ChooserName",    "Application apolloPanProcess");
    startMeasure += PvlKeyword("Reference",      "True");

    PvlObject startPoint("ControlPoint");
    startPoint += PvlKeyword("PointType", "Free");
    if (timingValid[i] == "0" && (columns - timingSamples[i].toInt() - timingLengths[i].toInt()) < 20) { 
      startPoint += PvlKeyword("Ignore", "True");
    }
    else {
      startPoint += PvlKeyword("EditLock", "True");
    }
    startPoint += PvlKeyword("PointId",   QString::number(i) + "Start");
    startPoint += startMeasure;

    PvlGroup stopMeasure("ControlMeasure");
    stopMeasure += PvlKeyword("SerialNumber",   tileName + ".cub");
    stopMeasure += PvlKeyword("MeasureType",    "Candidate");
    stopMeasure += PvlKeyword("Sample",         timingSamples[i]);
    stopMeasure += PvlKeyword("Line",           timingLines[i]);
    stopMeasure += PvlKeyword("ErrorLine",      "0");
    stopMeasure += PvlKeyword("ErrorSample",    "0");
    stopMeasure += PvlKeyword("ErrorMagnitude", "0");
    stopMeasure += PvlKeyword("DateTime",       Application::DateTime());
    stopMeasure += PvlKeyword("ChooserName",    "Application apolloPanProcess");
    stopMeasure += PvlKeyword("Reference",      "True");

    PvlObject stopPoint("ControlPoint");
    stopPoint += PvlKeyword("PointType", "Free");
    if (timingValid[i] == "0" && timingSamples[i].toInt() < 20) {
      stopPoint += PvlKeyword("Ignore", "True");
    }
    else {
      stopPoint += PvlKeyword("EditLock", "True");
    }
    stopPoint += PvlKeyword("PointId",   QString::number(i) + "Stop");
    stopPoint += stopMeasure;

    timingNetwork += startPoint;
    timingNetwork += stopPoint;
  }

  // Output files
  Pvl fiducialNetworkFile;
  fiducialNetworkFile += fiducialNetwork;
  try {
    fiducialNetworkFile.write(ui.GetFileName("ONET") + "_FID.net");
  }
  catch (IException &e) {
    QString msg = "Unable to output Fiducial Mark network as ["
                  + ui.GetFileName("ONET") + "_FID.net].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  Pvl timingNetworkFile;
  timingNetworkFile += timingNetwork;
  try {
    timingNetworkFile.write(ui.GetFileName("ONET") + "_TIM.net");
  }
  catch (IException &e) {
    QString msg = "Unable to output Timing Mark network as ["
                  + ui.GetFileName("ONET") + "_TIM.net].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
}