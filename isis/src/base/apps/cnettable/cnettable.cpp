#include "Isis.h"

#include "CameraPointInfo.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "TextFile.h"
#include "UserInterface.h"

#include <QString>
#include <QVector>

#include <string>

using namespace std;
using namespace Isis;

void Write(PvlGroup * point, ControlMeasure & cm);

// Allows for column names to be written on the first pass
bool isFirst;
bool append;

TextFile * txt = NULL;

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
  UserInterface &ui = Application::GetUserInterface();
  ControlNet cnet(ui.GetFilename("CNET"));
  SerialNumberList serials(ui.GetFilename("FROMLIST"));
  append = ui.GetBoolean("APPEND");

  if (cnet.Size() == 0) {
    string msg = "Your control network must contain at least one point";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  prog.SetMaximumSteps(cnet.Size());

  // If append is true, output will be appended or a new file created
  if (append) {
    // Check to see if its a new file or we open an existing file
    Filename file(ui.GetFilename("TO"));
    if (!file.Exists()) {
      // It is new, so we aren't appending
      append = false;
    }
    txt = new TextFile(ui.GetFilename("TO"), "append");
  }
  // Without append, if the files exists it will be overwritten  
  else {
    txt = new TextFile(ui.GetFilename("TO"), "overwrite");
  }

  PvlGroup * grp = NULL;
  CameraPointInfo camPoint;

  outside = ui.GetBoolean("ALLOWOUTSIDE");
  errors = ui.GetBoolean("ALLOWERRORS");

  // Loop through all points in controlnet
  for (int i = 0; i < cnet.Size(); i++) {
    ControlPoint & cpoint = cnet[i];
    
    if (isFirst && !append) {
      measureLabels += "ControlPointId,"; 
      measureLabels += "PointType,";
      measureLabels += "Ignored,";
      measureLabels += "Held,";
      measureLabels += "Invalid,";
      measureLabels += "UniversalLatitude,";
      measureLabels += "UniversalLongitude,";
      measureLabels += "Radius,";
    }

    // Always add data
    measureInfo.clear();
    measureInfo += QString(cpoint.Id().c_str()) + ",";
    measureInfo += QString(cpoint.PointTypeToString().c_str()) + ",";
    measureInfo += iString(cpoint.Ignore()).ToQt() + ",";
    measureInfo += iString(cpoint.Held()).ToQt() + ",";
    measureInfo += iString(cpoint.Invalid()).ToQt() + ",";
    measureInfo += iString(cpoint.UniversalLatitude()).ToQt() + ",";
    measureInfo += iString(cpoint.UniversalLongitude()).ToQt() + ",";
    measureInfo += iString(cpoint.Radius()).ToQt() + ",";
    
    // Loop through all measures in controlpoint
    for (int j = 0; j < cpoint.Size(); j++) {

      ControlMeasure & cmeasure = cpoint[j];

      // Set and then get CameraPointInfo information
      camPoint.SetCube(serials.Filename(cmeasure.CubeSerialNumber()));

      grp = camPoint.SetImage(cmeasure.Sample(), cmeasure.Line(), outside, errors);
      // Shouldn't ever happen, but, being safe...
      if (grp == NULL) {
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
  if (txt != NULL) {
    delete txt;
    txt = NULL;
  }
}

// Write each PvlGroup out to file
void Write(PvlGroup * point, ControlMeasure & cm) {

  // QStrings are used QString here because of ControlMeasure returning
  // QStrings. There is some monkey motion involving ingesting doubles,
  // this is necessary as iString will accept doubles but QString won't.

  QString output = "";
  QVector < QString > dataNames;
   
  // Do we have errors?
  int maxCount = 0;
  bool errors = point->HasKeyword("Error");
  if (errors) {
    maxCount = point->Keywords() - 1;
  }
  else {
    maxCount = point->Keywords();
  }

  // If its first and not appending, write the column labels
  if (isFirst && !append) {
    // point information
    for (int i = 0; i < maxCount; i++) {
      if ((*point)[i].Size() == 3) {              
        output += QString((*point)[i].Name().c_str()) + "X,";
        output += QString((*point)[i].Name().c_str()) + "Y,";
        output += QString((*point)[i].Name().c_str()) + "Z,";
      }
      else {
        output += QString((*point)[i].Name().c_str()) + ",";
      }
    }

    // control measure information
    dataNames = cm.GetMeasureDataNames();
    for (int i = 0; i < dataNames.size(); i++) {
      output += dataNames[i] + ",";
    }
    if (errors) output += QString((*point)[maxCount].Name().c_str());
    isFirst = false;
    measureLabels += output; 
    txt->PutLine(measureLabels.toStdString());
  }
  output.clear();
  measureLabels.clear();

  // Write out date values
  // point information
  for (int i = 0; i < maxCount; i++) {
    if ((*point)[i].Size() == 3) {
      output += QString((*point)[i][0]) + ",";
      output += QString((*point)[i][1]) + ",";
      output += QString((*point)[i][2]) + ",";
    }
    else {
      output += QString((*point)[i][0]) + ",";
    }
  }

  dataNames = cm.GetMeasureDataNames(); 
  for (int i = 0; i < dataNames.size(); i++) {
      output += iString(cm.GetMeasureData(dataNames[i])).ToQt() + ",";
  }

  if (errors) output += QString((*point)[maxCount][0]);

  // Meseaure info comes first
  QString pri = "";
  pri += measureInfo;
  pri += output; 

  txt->PutLine(pri.toStdString());

  output.clear();
  pri.clear();
}
