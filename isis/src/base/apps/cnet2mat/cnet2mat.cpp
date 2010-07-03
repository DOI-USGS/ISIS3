#include "Isis.h"

#include <map>
#include <iomanip>
#include <sstream>

#include "Pvl.h"
#include "TextFile.h"
#include "FileList.h"
#include "SerialNumberList.h"
#include "ControlNet.h"
#include "iException.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  std::map <int,string> fscMap;
  std::map <string,int> snMap;

  UserInterface &ui = Application::GetUserInterface();

  FileList list2(ui.GetFilename("LIST2"));

  SerialNumberList snl(ui.GetFilename("LIST3"));
  for (unsigned int f=0; f<list2.size(); f++) {
    iString currFile (list2[f]);
    Pvl lab (currFile);
    PvlObject qube (lab.FindObject("QUBE"));
    string fsc;
    if (qube.HasKeyword("IMAGE_NUMBER")) {
      fsc = qube.FindKeyword("IMAGE_NUMBER")[0];
    }
    else if (qube.HasKeyword("IMAGE_ID")) {
      fsc = qube.FindKeyword("IMAGE_ID")[0];
    }
    else {
      string msg = "Unable to find keyword [\"IMAGE_NUMBER\" or \"IMAGE_ID\"] in file [";
      msg += fsc + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    iString sn (snl.SerialNumber(f));
    fscMap.insert(std::pair<int,string>(f,fsc));
    snMap.insert(std::pair<string,int>(sn,f));
  }

  ControlNet cnet(ui.GetFilename("CONTROL"));

  int mpTotal = 0;

  for (int i=0; i<cnet.Size(); i++) {
    mpTotal += cnet[i].Size();
  }

  TextFile mpFile(ui.GetFilename("MATCH"),"Overwrite","");

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
  for (int i=0; i<cnet.Size(); i++){

    // Loop for each measure in the control point
    for (int m=0; m<cnet[i].Size(); m++) {
      ostringstream formatter;
      ControlMeasure currMeas = cnet[i][m];

      //Set Point ID
      formatter.clear();
      formatter.str("");
      formatter.width(30);
      formatter.setf(ios::left);
      formatter << cnet[i].Id() << " ";
      textLine = formatter.str();

      //Set FSC
      formatter.clear();
      formatter.str("");
      formatter.width(12);
      formatter.setf(ios::right);
      string sn = currMeas.CubeSerialNumber();
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
      formatter << currMeas.Line() << " ";
      textLine += formatter.str();

      //Set Sample
      formatter.clear();
      formatter.str("");
      formatter.width(7);
      formatter.setf(ios::right);
      formatter.setf(ios::fixed);
      formatter.precision(2);
      formatter << currMeas.Sample() << "   ";
      textLine += formatter.str();

      //Set Class
      string ptClass;
      ControlMeasure::MeasureType mType = currMeas.Type();
      if (mType == ControlMeasure::Unmeasured) {
        ptClass = "U   ";
      }
      else if (mType == ControlMeasure::ValidatedManual) {
        ptClass = "M   ";
      }
      else if (mType == ControlMeasure::ValidatedAutomatic) {
        ptClass = "S   ";
      }
      else {
        ptClass = "M   "; //! U was causing qmatch "havoc"
      }
      if (currMeas.IsReference()) {
        ptClass = "T   ";
      }
      if (currMeas.Ignore() || cnet[i].Ignore()) {
        ptClass = "U   ";
      }
      textLine += ptClass;

      //Set Diameter
      formatter.clear();
      formatter.str("");
      formatter.width(16);
      formatter.setf(ios::right);
      iString diam;
      if (currMeas.Diameter() == Isis::Null) {
        diam = 0.0;
      }
      else {
        diam = currMeas.Diameter();
      }
      formatter << diam;
      textLine += formatter.str();

      mpFile.PutLine(textLine);
    }
  }
}
