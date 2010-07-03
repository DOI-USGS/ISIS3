#include "Isis.h"
#include "Camera.h"
#include "iException.h"
#include "Brick.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Get input cube and get camera model for it  
  string channel = ui.GetFilename("FROM");
  Cube cube;
  cube.Open(channel);
  Camera *cam = cube.Camera();

  // Get the type of conversion that we are doing
  string type = ui.GetString("TYPE");
  double samp,line;

  // Do conversion from samp/line to ra/dec
  if (type == "IMAGE") {
    // Get users sample & line values and do a setImage for the camera
    samp = ui.GetDouble("SAMPLE");
    line = ui.GetDouble("LINE");
    cam->SetImage(samp,line);
  }
  // Do conversion from ra/dec to samp/line
  else {
    double ra = ui.GetDouble("RA");
    double dec = ui.GetDouble("DEC");
    if (!cam->SetRightAscensionDeclination(ra,dec)) {
      string msg = "Invalid Ra/Dec coordinate";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    samp = cam->Sample();
    line = cam->Line();
  }

  // Create Brick on samp, line to get the dn value of the pixel
  Brick b(3,3,1,cube.PixelType());
  int intSamp = (int)(samp+0.5);
  int intLine = (int)(line+0.5);
  b.SetBasePosition(intSamp,intLine,1);
  cube.Read(b);

  // Create group with sky position
  PvlGroup sp("SkyPoint"); 
  {
    sp += PvlKeyword("Filename",Filename(channel).Expanded());
    sp += PvlKeyword("Sample",cam->Sample());
    sp += PvlKeyword("Line",cam->Line());
    sp += PvlKeyword("RightAscension",cam->RightAscension());
    sp += PvlKeyword("Declination",cam->Declination());
    sp += PvlKeyword("EphemerisTime",cam->EphemerisTime());
    sp += PvlKeyword("PixelValue",PixelToString(b[0]));
  }
  //Write the group to the screen
  Application::Log(sp);
  
  // Write an output label file if necessary
  if (ui.WasEntered("TO")) {
    // Get user params from ui
    string outFile = Filename(ui.GetFilename("TO")).Expanded();
    bool exists = Filename(outFile).Exists();
    bool append = ui.GetBoolean("APPEND");

    // Write the pvl group out to the file
    if (ui.GetString("FORMAT") == "PVL") {
      Pvl temp;
      temp.SetTerminator("");
      temp.AddGroup(sp);
      if (append) {
        temp.Append(ui.GetAsString("TO"));
      }
      else {
        temp.Write(ui.GetAsString("TO"));
      }
    }
    // Create a flatfile of the same data
    // The flatfile is comma delimited and can be imported into Excel
    else {
      ofstream os;
      bool writeHeader = false;
      if (append) {
        os.open(outFile.c_str(),ios::app);
        if (!exists) {
          writeHeader = true;
        }
      }
      else {
        os.open(outFile.c_str(),ios::out);
        writeHeader = true;
      }
         
      if(writeHeader) {
        for(int i = 0; i < sp.Keywords(); i++) {
          os << sp[i].Name();

          if(i < sp.Keywords()-1) {
            os << ",";
          }
        }
        os << endl;
      }
      
      for(int i = 0; i < sp.Keywords(); i++) {
        os << (string)sp[i];

        if(i < sp.Keywords()-1) {
          os << ",";
        }
      }
      os << endl;
    }
  }
  else if(ui.GetString("FORMAT") == "FLAT") {
    string msg = "Flat file must have a name.";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }
}
