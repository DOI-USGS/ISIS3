#include "Isis.h"

#include "CameraPointInfo.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Displacement.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "Statistics.h"
#include "SurfacePoint.h"
#include "TextFile.h"
#include "UserInterface.h"

#include <QList>
#include <QString>
#include <QStringList>

#include <string>

using namespace std;
using namespace Isis;

IString CheckValue(double value);
IString CheckValue(IString value);
void Write(PvlGroup *point, const ControlMeasure &cm);

// Allows for column names to be written on the first pass
bool isFirst;
bool append;

TextFile *txt = NULL;

// For control measure related data and labels
IString measureInfo;
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
  ControlNet cnet(ui.GetFileName("CNET"));
  SerialNumberList serials(ui.GetFileName("FROMLIST"));
  append = ui.GetBoolean("APPEND");

  if(cnet.GetNumMeasures() == 0) {
    string msg = "Your control network must contain at least one point";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  prog.SetMaximumSteps(cnet.GetNumMeasures());

  // If append is true, output will be appended or a new file created
  if (append) {
    // Check to see if its a new file or we open an existing file
    FileName file(ui.GetFileName("FLATFILE"));
    if (!file.fileExists()) {
      // It is new, so we aren't appending
      // Set this because it is used elsewhere
      append = false;
    }
    txt = new TextFile(ui.GetFileName("FLATFILE"), "append");
  }
  // Without append, if the files exists it will be overwritten
  else {
    txt = new TextFile(ui.GetFileName("FLATFILE"), "overwrite");
  }

  PvlGroup * grp = NULL;
  CameraPointInfo camPoint;

  outside = ui.GetBoolean("ALLOWOUTSIDE");
  errors = ui.GetBoolean("ALLOWERRORS");

  // Loop through all points in controlnet
  for (int i = 0; i < cnet.GetNumPoints(); i++) {
    const ControlPoint * cpoint = cnet[i];

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

      measureLabels += "AdjustedX,";
      measureLabels += "AdjustedY,";
      measureLabels += "AdjustedZ,";
      measureLabels += "AdjustedXSigma,";
      measureLabels += "AdjustedYSigma,";
      measureLabels += "AdjustedZSigma,";
      measureLabels += "AdjustedLatitude,";
      measureLabels += "AdjustedLongitude,";
      measureLabels += "AdjustedLocalRadius,";
      measureLabels += "AdjustedLatitudeSigma,";
      measureLabels += "AdjustedLongitudeSigma,";
      measureLabels += "AdjustedLocalRadiusSigma,";
      measureLabels += "AdjustedLatitudeSigmaDistance,";
      measureLabels += "AdjustedLongitudeSigmaDistance,";

      measureLabels += "MinimumResidual,";
      measureLabels += "MaximumResidual,";
      measureLabels += "AverageResidual,";
      measureLabels += "MinimumSampleResidual,";
      measureLabels += "MaximumSamlpeResidual,";
      measureLabels += "MinimumLineResidual,";
      measureLabels += "MaximumLineResidual,";

      measureLabels += "MaximumSampleShift,";
      measureLabels += "MinimumSampleShift,";
      measureLabels += "AverageSampleShift,";
      measureLabels += "MaximumLineShift,";
      measureLabels += "MinimumLineShift,";
      measureLabels += "AverageLineShift,";
      measureLabels += "MaximumPixelShift,";
      measureLabels += "MinimumPixelShift,";
      measureLabels += "AveragePixelShift,";
      measureLabels += "MinimumPixelZScore,";
      measureLabels += "AveragePixelZScore,";
      measureLabels += "MaximumGoodnessOfFit,";
      measureLabels += "MinimumGoodnessOfFit,";
      measureLabels += "AverageGoodnessOfFit,";
    }

    // Always add data
    measureInfo.clear();
    measureInfo += cpoint->GetId() + ",";
    measureInfo += cpoint->GetPointTypeString() + ",";
    measureInfo += IString(cpoint->GetChooserName()) + ",";
    measureInfo += IString(cpoint->GetDateTime()) + ",";
    measureInfo += IString(cpoint->IsEditLocked()) + ",";
    measureInfo += IString(cpoint->IsIgnored()) + ",";

    measureInfo += IString(cpoint->GetSurfacePointSourceString()) + ",";
    measureInfo += IString(cpoint->GetAprioriSurfacePointSourceFile()) + ",";
    measureInfo += IString(cpoint->GetRadiusSourceString()) + ",";
    measureInfo += IString(cpoint->GetAprioriRadiusSourceFile()) + ",";

    SurfacePoint Asp = cpoint->GetAprioriSurfacePoint();
    measureInfo += IString(CheckValue(Asp.GetX().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetY().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetZ().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetXSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetYSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetZSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetLatitude().planetocentric(Angle::Degrees))) + ",";
    measureInfo += IString(CheckValue(Asp.GetLongitude().positiveEast(Angle::Degrees))) + ",";
    measureInfo += IString(CheckValue(Asp.GetLocalRadius().kilometers())) + ",";
    measureInfo += IString(CheckValue(Asp.GetLatSigma().degrees())) + ",";
    measureInfo += IString(CheckValue(Asp.GetLonSigma().degrees())) + ",";
    measureInfo += IString(CheckValue(Asp.GetLocalRadiusSigma().kilometers())) + ",";
    try { measureInfo += IString(CheckValue(Asp.GetLatSigmaDistance().kilometers())) + ","; }
    catch (IException &) {
      measureInfo += ",";
    }
    try { measureInfo += IString(CheckValue(Asp.GetLonSigmaDistance().kilometers())) + ","; }
    catch (IException &) {
      measureInfo += ",";
    }

    SurfacePoint sp = cpoint->GetAdjustedSurfacePoint();
    measureInfo += IString(CheckValue(sp.GetX().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetY().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetZ().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetXSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetYSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetZSigma().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetLatitude().planetocentric(Angle::Degrees))) + ",";
    measureInfo += IString(CheckValue(sp.GetLongitude().positiveEast(Angle::Degrees))) + ",";
    measureInfo += IString(CheckValue(sp.GetLocalRadius().kilometers())) + ",";
    measureInfo += IString(CheckValue(sp.GetLatSigma().degrees())) + ",";
    measureInfo += IString(CheckValue(sp.GetLonSigma().degrees())) + ",";
    measureInfo += IString(CheckValue(sp.GetLocalRadiusSigma().kilometers())) + ",";
    try { measureInfo += IString(CheckValue(sp.GetLatSigmaDistance().kilometers())) + ","; }
    catch (IException &e) {
      measureInfo += ",";
    }
    try { measureInfo += IString(CheckValue(sp.GetLonSigmaDistance().kilometers())) + ","; }
    catch (IException &e) {
      measureInfo += ",";
    }

    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetResidualMagnitude).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetResidualMagnitude).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetResidualMagnitude).Average())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetSampleResidual).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetSampleResidual).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetLineResidual).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetLineResidual).Maximum())) + ",";


    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetSampleShift).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetSampleShift).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetSampleShift).Average())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetLineShift).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetLineShift).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetLineShift).Average())) + ",";

    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetPixelShift).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetPixelShift).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        &ControlMeasure::GetPixelShift).Average())) + ",";

    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        ControlMeasureLogData::MaximumPixelZScore).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        ControlMeasureLogData::MinimumPixelZScore).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        ControlMeasureLogData::GoodnessOfFit).Maximum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        ControlMeasureLogData::GoodnessOfFit).Minimum())) + ",";
    measureInfo += IString(CheckValue(cpoint->GetStatistic(
        ControlMeasureLogData::GoodnessOfFit).Average())) + ",";

    // Loop through all measures in controlpoint
    for(int j = 0; j < cpoint->GetNumMeasures(); j++) {

      const ControlMeasure * cmeasure = (*cpoint)[j];

      // Set and then get CameraPointInfo information
      camPoint.SetCube(serials.FileName(cmeasure->GetCubeSerialNumber()));

      grp = camPoint.SetImage(cmeasure->GetSample(), cmeasure->GetLine(), outside, errors);
      // Shouldn't ever happen, but, being safe...
      if(grp == NULL) {
        string msg = "You shouldn't have gotten here. Errors in CameraPointInfo class";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      Write(grp, *cmeasure);
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

// This function is meant to check a value, and, if it is a special pixel
// return " " instead of the special pixel value.
IString CheckValue(double value) {
  if (IsSpecial(value)) {
    return IString("");
  }
  else {
    return CheckValue(IString(value));
  }
}

IString CheckValue(IString value) {
  if (value == IString(Isis::Null) ||
      value == IString(Isis::Hrs) ||
      value == IString(Isis::His) ||
      value == IString(Isis::Lrs) ||
      value == IString(Isis::Lis)) {
    return IString("");
  }
  else {
    return value;
  }
}

IString CheckValue(QString value) {
  if (value == QString::number(Isis::Null) ||
      value == QString::number(Isis::Hrs) ||
      value == QString::number(Isis::His) ||
      value == QString::number(Isis::Lrs) ||
      value == QString::number(Isis::Lis)) {
    return IString("");
  }
  else {
    return value.toStdString();
  }
}

// Write each PvlGroup out to file
void Write(PvlGroup *point, const ControlMeasure &cm) {

  // QStrings are used QString here because of ControlMeasure returning
  // QStrings. There is some monkey motion involving ingesting doubles,
  // this is necessary as IString will accept doubles but QString won't.

  QString output = "";
  QVector < IString > dataNames;

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
    //  output += IString(dataNames[i] + ",");
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
    output += CheckValue(nameValuePair.at(1)).ToQt() + ",";
  }

  // Write out date values
  // point information
  for(int i = 0; i < maxCount; i++) {
    if((*point)[i].Size() == 3) {
      output += QString(CheckValue((*point)[i][0])) + ",";
      output += QString(CheckValue((*point)[i][1])) + ",";
      output += QString(CheckValue((*point)[i][2])) + ",";
    }
    else {
      output += QString(CheckValue((*point)[i][0])) + ",";
    }
  }

  //dataNames = cm.GetMeasureDataNames();
  //for(int i = 0; i < dataNames.size(); i++) {
  //  output += IString(cm.GetMeasureData(dataNames[i])) + ",";
  //}



  if(errors) output += QString((*point)[maxCount][0]);

  // Meaure info comes first
  QString pri = "";
  pri += measureInfo.ToQt();
  pri += output;

  txt->PutLine(pri.toStdString());

  output.clear();
  pri.clear();
}
