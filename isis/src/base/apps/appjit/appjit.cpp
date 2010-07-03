#include "Isis.h"
#include "Cube.h"
#include "Table.h"
#include "Camera.h"
#include "LineScanCameraRotation.h"
#include "PixelOffset.h"
#include "SpiceRotation.h"
#include "iString.h"
#include "FileList.h"
#include "iException.h"
#include "CameraDetectorMap.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  /*Processing steps
  1.  Open and read the jitter table, convert the pixel offsets to angles,
      and create the polynomials (solve for the coefficients) to use to do
      the high pass filter putting the results into a rotation matrix in the jitter class.
  2.  Apply the jitter correction in the LineScanCameraRotation object of the master cube.
  3.  Loop through FROMLIST correcting the pointing and writing out the
      updated camera pointing from the master cube
      */

  int degree = ui.GetInteger("DEGREE");

  // Get the input file list to make sure it is not empty and the master cube is included
  FileList list;
  list.Read(ui.GetFilename("FROMLIST"));

  if (list.size() < 1) {
    string msg = "The input list file [" + ui.GetFilename("FROMLIST") + "is empty";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  int ifile = 0;
  // Make sure the master file is included in the input file list
  while (ifile < (int) list.size() && Filename(list[ifile]).Expanded() != Filename(ui.GetFilename("MASTER")).Expanded()) {
    ifile++;
  }

  if (ifile >= (int) list.size()) {
    string msg = "The master file, [" + Filename(ui.GetFilename("MASTER")).Expanded() + " is not included in " + 
      "the input list file " + ui.GetFilename("FROMLIST") + "]";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  bool step2 = false;
  PvlGroup gp("AppjitResults");

  //Step 1:  Create the jitter rotation

  try {
    // Open the master cube
    Cube cube;
    cube.Open(ui.GetFilename("MASTER"),"rw");
    
    //check for existing polygon, if exists delete it
    if (cube.Label()->HasObject("Polygon")){
      cube.Label()->DeleteObject("Polygon");
    }

    // Get the camera
    Camera *cam = cube.Camera();
    if (cam->DetectorMap()->LineRate() == 0.0) {
      string msg = "[" + ui.GetFilename("MASTER") + "] is not a line scan camera image";
      throw iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }

    // Create the master rotation to be corrected 
    int frameCode = cam->InstrumentRotation()->Frame();
    cam->SetImage(int(cube.Samples()/2), int(cube.Lines()/2) );
    double tol = cam->PixelResolution();

    if (tol < 0.) {
      // Alternative calculation of .01*ground resolution of a pixel
      tol = cam->PixelPitch()*cam->SpacecraftAltitude()*1000./cam->FocalLength()/100.;
    }
    LineScanCameraRotation crot(frameCode, *(cube.Label()), cam->InstrumentRotation()->GetFullCacheTime(), tol );
    crot.SetPolynomialDegree(ui.GetInteger("DEGREE"));
    crot.SetAxes(1, 2, 3);
    if (ui.WasEntered("PITCHRATE")) crot.ResetPitchRate(ui.GetDouble("PITCHRATE"));
    if (ui.WasEntered("YAW")) crot.ResetYaw(ui.GetDouble("YAW"));
    crot.SetPolynomial();
    double baseTime = crot.GetBaseTime();
    double timeScale = crot.GetTimeScale();
    double fl = cam->FocalLength();
    double pixpitch = cam->PixelPitch();
    std::vector<double> cacheTime = cam->InstrumentRotation()->GetFullCacheTime();

    // Get the jitter in pixels, compute jitter angles, and fit a polynomial to each angle
    PixelOffset jitter(ui.GetFilename("JITTERFILE"), fl, pixpitch, baseTime, timeScale, degree);
    jitter.LoadAngles(cacheTime);
    jitter.SetPolynomial();

    // Set the jitter and apply to the instrument rotation
    crot.SetJitter( &jitter );
    crot.ReloadCache();

    // Pull out the pointing cache as a table and write it
    Table cmatrix = crot.Cache("InstrumentPointing");
    cmatrix.Label().AddComment("Corrected using appjit and" + ui.GetFilename("JITTERFILE"));
    cube.Write(cmatrix);

    // Write out the instrument position table
    Isis::PvlGroup kernels = cube.Label()->FindGroup("Kernels",Isis::Pvl::Traverse);

    // Write out the "Table" label to the tabled kernels in the kernels group
    kernels["InstrumentPointing"] = "Table";
//    kernels["InstrumentPosition"] = "Table";
    cube.PutGroup(kernels);
    cube.Close();
    gp += PvlKeyword("StatusMaster",ui.GetFilename("MASTER") + ":  camera pointing updated");

    // Apply the dejittered pointing to the rest of the files
    step2 = true;
    for (int ifile = 0; ifile < (int) list.size(); ifile++) {
      if (list[ifile] != ui.GetFilename("MASTER")) {
        // Open the cube
        cube.Open(list[ifile],"rw");
        //check for existing polygon, if exists delete it
        if (cube.Label()->HasObject("Polygon")){
          cube.Label()->DeleteObject("Polygon");
        }
        // Get the camera and make sure it is a line scan camera
        Camera *cam = cube.Camera();
        if (cam->DetectorMap()->LineRate() == 0.0) {
          string msg = "[" + ui.GetFilename("FROM") + "] is not a line scan camera";
          throw iException::Message(Isis::iException::User,msg,_FILEINFO_);
        }
        // Pull out the pointing cache as a table and write it
        cube.Write(cmatrix);
        cube.PutGroup(kernels);
        cube.Close();
        gp += PvlKeyword("Status" + iString(ifile), list[ifile] + ":  camera pointing updated");
      }
    }
    Application::Log( gp );
  }
  catch (iException &e) {
    string msg;
    if (!step2) {
      msg = "Unable to fit pointing for [" + ui.GetFilename("MASTER") + "]";
    }
    else {
      msg = "Unable to update pointing for nonMaster file(s)";
    }
    throw iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }
}
