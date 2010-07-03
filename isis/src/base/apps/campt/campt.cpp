#include "Isis.h"

#include <string>
#include <iomanip>

#include "Brick.h"
#include "Camera.h"
#include "CameraPointInfo.h"
#include "iException.h"
#include "iTime.h"
#include "Progress.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  bool outsideAllowed = ui.GetBoolean("ALLOWOUTSIDE");

  // Set up CameraPointInfo and set file
  CameraPointInfo campt;
  campt.SetCube(ui.GetFilename("FROM") + "+" + ui.GetInputAttribute("FROM").BandsStr());

  Progress prog;
  prog.SetMaximumSteps(1);

  // Depending on what type is selected, set values accordingly
  PvlGroup *point = NULL;
  if(ui.GetString("TYPE") == "IMAGE") {
    double sample = 0.0;
    double line = 0.0;
    if(ui.WasEntered("SAMPLE") && ui.WasEntered("LINE")) {
      sample = ui.GetDouble("SAMPLE");
      line = ui.GetDouble("LINE");
      point = campt.SetImage(sample, line, outsideAllowed);
    }
    else {
      if(ui.WasEntered("SAMPLE")) {
        sample = ui.GetDouble("SAMPLE");
        point = campt.SetSample(sample, outsideAllowed);
      }
      else if(ui.WasEntered("LINE")) {
        line = ui.GetDouble("LINE");
        point = campt.SetLine(line, outsideAllowed);
      }
      else {
        point = campt.SetCenter(outsideAllowed);
      }
    }
  }
  else {
    double lat = ui.GetDouble("LATITUDE");
    double lon = ui.GetDouble("LONGITUDE");
    point = campt.SetGround(lat, lon, outsideAllowed);
  }

  prog.CheckStatus();

  // Log it
  Application::Log((*point));

  if(ui.WasEntered("TO")) {
    // Get user params from ui
    string outFile = Filename(ui.GetFilename("TO")).Expanded();
    bool exists = Filename(outFile).Exists();
    bool append = ui.GetBoolean("APPEND");

    // Write the pvl group out to the file
    if(ui.GetString("FORMAT") == "PVL") {
      Pvl temp;
      temp.SetTerminator("");
      temp.AddGroup((*point));
      if(append) {
        temp.Append(outFile);
      }
      else {
        temp.Write(outFile);
      }
    }

    // Create a flatfile from PVL data
    // The flatfile is comma delimited and can be imported into Excel
    else {
      ofstream os;
      bool writeHeader = false;
      if(append) {
        os.open(outFile.c_str(), ios::app);
        if(!exists) {
          writeHeader = true;
        }
      }
      else {
        os.open(outFile.c_str(), ios::out);
        writeHeader = true;
      }

      if(writeHeader) {
        for(int i = 0; i < (*point).Keywords(); i++) {
          if((*point)[i].Size() == 3) {
            os << (*point)[i].Name() << "X,"
               << (*point)[i].Name() << "Y,"
               << (*point)[i].Name() << "Z";
          }
          else {
            os << (*point)[i].Name();
          }

          if(i < point->Keywords() - 1) {
            os << ",";
          }
        }
        os << endl;
      }

      for(int i = 0; i < (*point).Keywords(); i++) {
        if((*point)[i].Size() == 3) {
          os << (string)(*point)[i][0] << ","
             << (string)(*point)[i][1] << ","
             << (string)(*point)[i][2];
        }
        else {
          os << (string)(*point)[i];
        }

        if(i < (*point).Keywords() - 1) {
          os << ",";
        }
      }
      os << endl;
    }
  }
  else {
    if(ui.GetString("FORMAT") == "FLAT") {
      string msg = "Flat file must have a name.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }
  delete point;
  point = NULL;
  prog.CheckStatus();
}
