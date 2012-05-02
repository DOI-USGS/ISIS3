#include "Isis.h"

#include <map>
#include <iomanip>
#include <sstream>

#include "Application.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "FileList.h"
#include "IException.h"
#include "iString.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "TextFile.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  std::map <int, string> fscMap;
  std::map <string, int> snMap;

  UserInterface &ui = Application::GetUserInterface();

  FileList list2(ui.GetFileName("LIST2"));

  SerialNumberList snl(ui.GetFileName("LIST3"));
  for(unsigned int f = 0; f < list2.size(); f++) {
    iString currFile(list2[f]);
    Pvl lab(currFile);
    PvlObject qube(lab.FindObject("QUBE"));
    string fsc;
    if(qube.HasKeyword("IMAGE_NUMBER")) {
      fsc = qube.FindKeyword("IMAGE_NUMBER")[0];
    }
    else if(qube.HasKeyword("IMAGE_ID")) {
      fsc = qube.FindKeyword("IMAGE_ID")[0];
    }
    else {
      string msg = "Unable to find keyword [\"IMAGE_NUMBER\" or \"IMAGE_ID\"] in file [";
      msg += fsc + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    iString sn(snl.SerialNumber(f));
    fscMap.insert(std::pair<int, string>(f, fsc));
    snMap.insert(std::pair<string, int>(sn, f));
  }

  ControlNet cnet(ui.GetFileName("CNET"));

  int mpTotal = 0;

  for(int i = 0; i < cnet.GetNumPoints(); i++) {
    mpTotal += cnet.GetPoint(i)->GetNumMeasures();
  }

  TextFile mpFile(ui.GetFileName("MATCH"), "Overwrite", "");

  ostringstream str;
  iString textLine;

  textLine = "Matchpoint total =    ";
  textLine += iString(mpTotal);
  mpFile.PutLine(textLine);
  str.clear();
  str.str("");
  str.width(40);
  str.setf(ios::left);
  str << "Point ID";
  textLine = str.str();

  str.clear();
  str.str("");
  str.width(7);
  str.setf(ios::left);
  str << "FSC";
  textLine += str.str();

  str.clear();
  str.str("");
  str.width(8);
  str.setf(ios::left);
  str << "LINE";
  textLine += str.str();

  str.clear();
  str.str("");
  str.width(5);
  str.setf(ios::left);
  str << "SAMP";
  textLine += str.str();


  str.clear();
  str.str("");
  str.width(14);
  str.setf(ios::left);
  str << "CLASS";
  textLine += str.str();

  str.clear();
  str.str("");
  str.width(8);
  str.setf(ios::left);
  str << "DIAMETER";
  textLine += str.str();

  mpFile.PutLine(textLine);

  // Loop for each point in the control network
  for(int i = 0; i < cnet.GetNumPoints(); i++) {
    ControlPoint *currPoint = cnet.GetPoint(i);

    // Loop for each measure in the control point
    for(int m = 0; m < currPoint->GetNumMeasures(); m++) {
      ostringstream formatter;
      ControlMeasure *currMeas = currPoint->GetMeasure(m);

      //Set Point ID
      formatter.clear();
      formatter.str("");
      formatter.width(30);
      formatter.setf(ios::left);
      formatter << currPoint->GetId() << " ";
      textLine = formatter.str();

      //Set FSC
      formatter.clear();
      formatter.str("");
      formatter.width(12);
      formatter.setf(ios::right);
      string sn = currMeas->GetCubeSerialNumber();
      string fsc = fscMap[snMap[sn]];
      formatter << fsc << " ";
      textLine += formatter.str();

      //Set Line
      formatter.clear();
      formatter.str("");
      formatter.width(7);
      formatter.setf(ios::right);
      formatter.setf(ios::fixed);
      formatter.precision(2);
      formatter << currMeas->GetLine() << " ";
      textLine += formatter.str();

      //Set Sample
      formatter.clear();
      formatter.str("");
      formatter.width(7);
      formatter.setf(ios::right);
      formatter.setf(ios::fixed);
      formatter.precision(2);
      formatter << currMeas->GetSample() << "   ";
      textLine += formatter.str();

      //Set Class
      string ptClass;
      ControlMeasure::MeasureType mType = currMeas->GetType();

      if(currMeas->IsIgnored() || currPoint->IsIgnored()) {
        ptClass = "U   "; //Unmeasured
      }
      else if(currPoint->GetRefMeasure() == currMeas) {
        ptClass = "T   "; //Truth
      }
      else if(mType == ControlMeasure::RegisteredSubPixel) {
        ptClass = "S   "; //SubPixel
      }
      else if(mType == ControlMeasure::RegisteredPixel
              || mType == ControlMeasure::Manual) {
        ptClass = "M   "; //Measured
      }
      else { // if(mType == ControlMeasure::Candidate) {
        ptClass = "A   "; //Approximate
      }
      textLine += ptClass;

      //Set Diameter
      formatter.clear();
      formatter.str("");
      formatter.width(16);
      formatter.setf(ios::right);
      iString diam;
      if(currMeas->GetDiameter() == Isis::Null) {
        diam = 0.0;
      }
      else {
        diam = currMeas->GetDiameter();
      }
      formatter << diam;
      textLine += formatter.str();

      mpFile.PutLine(textLine);
    }
  }
}
