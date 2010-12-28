#include "Isis.h"

#include "CameraPointInfo.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Displacement.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"
#include "TextFile.h"
#include "UserInterface.h"

#include <QString>

#include <QStringList>
#include <QList>

#include <string>

using namespace std;
using namespace Isis;

void Write(PvlGroup *point, const ControlMeasure &cm);

// Allows for column names to be written on the first pass
bool isFirst;
bool append;

TextFile *txt = NULL;

// For control measure related data and labels
QString measureInfo;
QString measureLabels;

void IsisMain() {
  isFirst = true;
  append = false;
  bool outside = false;
  bool errors = false;
  measureInfo = "";
  measureLabels = "";

  Progress prog;

  // Get user entered information
  UserInterface & ui = Application::GetUserInterface();
  ControlNet cnet(ui.GetFilename("CNET"));
  SerialNumberList serials(ui.GetFilename("FROMLIST"));
  append = ui.GetBoolean("APPEND");

  if(cnet.Size() == 0) {
    string msg = "Your control network must contain at least one point";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  prog.SetMaximumSteps(cnet.Size());

  // If append is true, output will be appended or a new file created
  if (append) {
    // Check to see if its a new file or we open an existing file
    Filename file(ui.GetFilename("FLATFILE"));
    if (!file.Exists()) {
      // It is new, so we aren't appending
      // Set this because it is used elsewhere
      append = false;
    }
    txt = new TextFile(ui.GetFilename("FLATFILE"), "append");
  }
  // Without append, if the files exists it will be overwritten
  else {
    txt = new TextFile(ui.GetFilename("FLATFILE"), "overwrite");
  }

  PvlGroup * grp = NULL;
  CameraPointInfo camPoint;

  outside = ui.GetBoolean("ALLOWOUTSIDE");
  errors = ui.GetBoolean("ALLOWERRORS");

  // Loop through all points in controlnet
  for (int i = 0; i < cnet.Size(); i++) {
    const ControlPoint &cpoint = cnet[i];

    if (isFirst && !append) {
      measureLabels += "ControlPointId,";
      measureLabels += "PointType,";
      measureLabels += "ChooserName,";
      measureLabels += "DateTime,";
      measureLabels += "EditLock,";
      measureLabels += "Ignored,";

      measureLabels += "AprioriSurfacePointSource,";
      measureLabels += "AprioriSurfacePointSourceFile,";

      measureLabels += "AprioriRadiusSource,";
      measureLabels += "AprioriRadiusSourceFile,";

      measureLabels += "AprioriX,";
      measureLabels += "AprioriY,";
      measureLabels += "AprioriZ,";
      measureLabels += "AprioriXSigma,";
      measureLabels += "AprioriYSigma,";
      measureLabels += "AprioriZSigma,";
      measureLabels += "AprioriLatitude,";
      measureLabels += "AprioriLongitude,";
      measureLabels += "AprioriLocalRadius,";
      measureLabels += "AprioriLatitudeSigma,";
      measureLabels += "AprioriLongitudeSigma,";
      measureLabels += "AprioriLocalRadiusSigma,";
      measureLabels += "AprioriLatitudeSigmaDistance,";
      measureLabels += "AprioriLongitudeSigmaDistance,";

      measureLabels += "X,";
      measureLabels += "Y,";
      measureLabels += "Z,";
      measureLabels += "XSigma,";
      measureLabels += "YSigma,";
      measureLabels += "ZSigma,";
      measureLabels += "Latitude,";
      measureLabels += "Longitude,";
      measureLabels += "LocalRadius,";
      measureLabels += "LatitudeSigma,";
      measureLabels += "LongitudeSigma,";
      measureLabels += "LocalRadiusSigma,";
      measureLabels += "LatitudeSigmaDistance,";
      measureLabels += "LongitudeSigmaDistance,";

      measureLabels += "MinimumResidual,";
      measureLabels += "MaximumResidual,";
      measureLabels += "AverageResidual,";
      measureLabels += "MinimumSampleResidual,";
      measureLabels += "MaximumSamlpeResidual,";
      measureLabels += "MinimumLineResidual,";
      measureLabels += "MaximumLineResidual,";
    }
/*
 * ChooserName
 * DateTime
   EditLock
   Ignore
   AprioriXYZSource
   AprioriXYZSourceFile
   AprioriRadiusSrouce
   AprioriRadiusSourceFile
   AprioriX
   AprioriY
   AprioriZ
   AprioriSigmaX
   AprioriSigmaY
   AprioriSigmaZ
   X
   Y
   Z
   ApostSigmaX
   ApostSigmaY
   ApostSigmaZ
   double MinimumSampleResidual() const;
   double MinimumLineResidual() const;
   double MinimumResidual() const;
   double MaximumLineResidual() const;
   double MaximumSampleResidual() const;
   UniversalLatitude
   UniversalLongitude
   Radius
   XYZSource
   RadiusSource
   AverageResidual
   MaximumResidual
 */

    // Always add data
    measureInfo.clear();
    measureInfo += cpoint.GetId().ToQt() + ",";
    measureInfo += cpoint.GetPointTypeString().ToQt() + ",";
    measureInfo += iString(cpoint.GetChooserName()).ToQt() + ",";
    measureInfo += iString(cpoint.GetDateTime()).ToQt() + ",";
    measureInfo += iString(cpoint.IsEditLocked()).ToQt() + ",";
    measureInfo += iString(cpoint.IsIgnored()).ToQt() + ",";

    measureInfo += iString(cpoint.GetSurfacePointSourceString()).ToQt() + ",";
    measureInfo += iString(cpoint.GetAprioriSurfacePointSourceFile()).ToQt() + ",";
    measureInfo += iString(cpoint.GetRadiusSourceString()).ToQt() + ",";
    measureInfo += iString(cpoint.GetAprioriRadiusSourceFile()).ToQt() + ",";

    SurfacePoint Asp = cpoint.GetAprioriSurfacePoint();
    measureInfo += iString(Asp.GetX().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetY().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetZ().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetXSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetYSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetZSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetLatitude().GetPlanetocentric(Angle::Degrees)).ToQt() + ",";
    measureInfo += iString(Asp.GetLongitude().GetPositiveEast(Angle::Degrees)).ToQt() + ",";
    measureInfo += iString(Asp.GetLocalRadius().GetKilometers()).ToQt() + ",";
    measureInfo += iString(Asp.GetLatSigma().GetDegrees()).ToQt() + ",";
    measureInfo += iString(Asp.GetLonSigma().GetDegrees()).ToQt() + ",";
    measureInfo += iString(Asp.GetLocalRadiusSigma().GetKilometers()).ToQt() + ",";
    try { measureInfo += iString(Asp.GetLatSigmaDistance().GetKilometers()).ToQt() + ","; }
    catch (iException e) {
      e.Clear();
      measureInfo += ",";
    }
    try { measureInfo += iString(Asp.GetLonSigmaDistance().GetKilometers()).ToQt() + ","; }
    catch (iException e) {
      e.Clear();
      measureInfo += ",";
    }

    SurfacePoint sp = cpoint.GetSurfacePoint();
    measureInfo += iString(sp.GetX().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetY().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetZ().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetXSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetYSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetZSigma().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetLatitude().GetPlanetocentric(Angle::Degrees)).ToQt() + ",";
    measureInfo += iString(sp.GetLongitude().GetPositiveEast(Angle::Degrees)).ToQt() + ",";
    measureInfo += iString(sp.GetLocalRadius().GetKilometers()).ToQt() + ",";
    measureInfo += iString(sp.GetLatSigma().GetDegrees()).ToQt() + ",";
    measureInfo += iString(sp.GetLonSigma().GetDegrees()).ToQt() + ",";
    measureInfo += iString(sp.GetLocalRadiusSigma().GetKilometers()).ToQt() + ",";
    try { measureInfo += iString(sp.GetLatSigmaDistance().GetKilometers()).ToQt() + ","; }
    catch (iException e) {
      e.Clear();
      measureInfo += ",";
    }
    try { measureInfo += iString(sp.GetLonSigmaDistance().GetKilometers()).ToQt() + ","; }
    catch (iException e) {
      e.Clear();
      measureInfo += ",";
    }

    measureInfo += iString(cpoint.GetMinimumResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetMaximumResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetAverageResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetMinimumSampleResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetMaximumSampleResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetMinimumLineResidual()).ToQt() + ",";
    measureInfo += iString(cpoint.GetMinimumLineResidual()).ToQt() + ",";
    
    // Loop through all measures in controlpoint
    for(int j = 0; j < cpoint.Size(); j++) {

      const ControlMeasure &cmeasure = cpoint[j];

      // Set and then get CameraPointInfo information
      camPoint.SetCube(serials.Filename(cmeasure.GetCubeSerialNumber()));

      grp = camPoint.SetImage(cmeasure.GetSample(), cmeasure.GetLine(), outside, errors);
      // Shouldn't ever happen, but, being safe...
      if(grp == NULL) {
        string msg = "You shouldn't have gotten here. Errors in CameraPointInfo class";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
      Write(grp, cmeasure);
      delete grp;
      grp = NULL;
    }

    // Making progress!
    prog.CheckStatus();
  }

  // All done, clean up
  prog.CheckStatus();
  if(txt != NULL) {
    delete txt;
    txt = NULL;
  }
}

// Write each PvlGroup out to file
void Write(PvlGroup *point, const ControlMeasure &cm) {

  // QStrings are used QString here because of ControlMeasure returning
  // QStrings. There is some monkey motion involving ingesting doubles,
  // this is necessary as iString will accept doubles but QString won't.

  QString output = "";
  QVector < iString > dataNames;

  // Do we have errors?
  int maxCount = 0;
  bool errors = point->HasKeyword("Error");
  if(errors) {
    maxCount = point->Keywords() - 1;
  }
  else {
    maxCount = point->Keywords();
  }

  // If its first and not appending, write the column labels
  if(isFirst && !append) {

    QList< QStringList > printableMeasureData = cm.PrintableClassData();
    QStringList nameValuePair;
    foreach (nameValuePair, printableMeasureData) {
      output += nameValuePair.at(0) + ",";
    }
    
    // point information
    for(int i = 0; i < maxCount; i++) {
      if((*point)[i].Size() == 3) {
        output += QString((*point)[i].Name().c_str()) + "X,";
        output += QString((*point)[i].Name().c_str()) + "Y,";
        output += QString((*point)[i].Name().c_str()) + "Z,";
      }
      else {
        output += QString((*point)[i].Name().c_str()) + ",";
      }
    }

    // control measure information
    //dataNames = cm.GetMeasureDataNames();
    //for(int i = 0; i < dataNames.size(); i++) {
    //  output += iString(dataNames[i] + ",").ToQt();
    //}


    if(errors) output += QString((*point)[maxCount].Name().c_str());
    isFirst = false;
    measureLabels += output;
    txt->PutLine(measureLabels.toStdString());
  }
  output.clear();
  measureLabels.clear();

  QList< QStringList > printableMeasureData = cm.PrintableClassData();
  QStringList nameValuePair;
  foreach (nameValuePair, printableMeasureData) {
    output += nameValuePair.at(1) + ",";
  }

  // Write out date values
  // point information
  for(int i = 0; i < maxCount; i++) {
    if((*point)[i].Size() == 3) {
      output += QString((*point)[i][0]) + ",";
      output += QString((*point)[i][1]) + ",";
      output += QString((*point)[i][2]) + ",";
    }
    else {
      output += QString((*point)[i][0]) + ",";
    }
  }

  //dataNames = cm.GetMeasureDataNames();
  //for(int i = 0; i < dataNames.size(); i++) {
  //  output += iString(cm.GetMeasureData(dataNames[i])).ToQt() + ",";
  //}

  

  if(errors) output += QString((*point)[maxCount][0]);

  // Meaure info comes first
  QString pri = "";
  pri += measureInfo;
  pri += output;

  txt->PutLine(pri.toStdString());

  output.clear();
  pri.clear();
}
