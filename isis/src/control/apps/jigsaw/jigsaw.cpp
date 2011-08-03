#include "Isis.h"
#include "Process.h"
#include "BundleAdjust.h"
#include "Table.h"
#include "iException.h"
#include "CubeAttribute.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  // Get the control network and image list
  UserInterface &ui = Application::GetUserInterface();
  std::string cnetFile = ui.GetFilename("CNET");
  std::string cubeList = ui.GetFilename("FROMLIST");

  // Get the held list if entered and prep for bundle adjustment
  BundleAdjust *b = NULL;

  if (ui.WasEntered("HELDLIST")) {
    std::string heldList = ui.GetFilename("HELDLIST");
    b = new BundleAdjust(cnetFile, cubeList, heldList);
  }
  else {
    b = new BundleAdjust(cnetFile, cubeList);
   }

  if (ui.WasEntered("SC_SIGMAS"))
    b->ReadSCSigmas(ui.GetFilename("SC_SIGMAS"));

  b->SetObservationMode(ui.GetBoolean("OBSERVATIONS"));
  b->SetSolutionMethod(ui.GetString("METHOD"));
  b->SetSolveRadii(ui.GetBoolean("RADIUS"));
  b->SetErrorPropagation(ui.GetBoolean("ERRORPROPAGATION"));
  b->SetOutlierRejection(ui.GetBoolean("OUTLIER_REJECTION"));

  b->SetCkDegree(ui.GetInteger("CKDEGREE"));
  b->SetSolveCamDegree(ui.GetInteger("SOLVEDEGREE"));
  std::string camsolve = ui.GetString("CAMSOLVE");

  if (camsolve == "NONE") {
    b->SetSolveCmatrix(BundleAdjust::None);
  }
  else if (camsolve == "ANGLES") {
    b->SetSolveCmatrix(BundleAdjust::AnglesOnly);
  }
  else if (camsolve == "VELOCITIES") {
    b->SetSolveCmatrix(BundleAdjust::AnglesVelocity);
  }
  else if (camsolve == "ACCELERATIONS") {
    b->SetSolveCmatrix(BundleAdjust::AnglesVelocityAcceleration);
  }
  else {
    b->SetSolveCmatrix(BundleAdjust::All);
  }

  b->SetSolveTwist(ui.GetBoolean("TWIST"));

  std::string spsolve = ui.GetString("SPSOLVE");
  if(spsolve == "NONE") {
    b->SetSolveSpacecraftPosition(BundleAdjust::Nothing);
  }
  else if(spsolve == "POSITION") {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionOnly);
  }
  else if(spsolve == "VELOCITIES") {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionVelocity);
  }
  else {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionVelocityAcceleration);
  }

  // global parameter uncertainties
  b->SetGlobalLatitudeAprioriSigma(ui.GetDouble("POINT_LATITUDE_SIGMA"));
  b->SetGlobalLongitudeAprioriSigma(ui.GetDouble("POINT_LONGITUDE_SIGMA"));
  b->SetGlobalRadiiAprioriSigma(ui.GetDouble("POINT_RADIUS_SIGMA"));

  b->SetGlobalSpacecraftPositionAprioriSigma(ui.GetDouble("SPACECRAFT_POSITION_SIGMA"));
  b->SetGlobalSpacecraftVelocityAprioriSigma(ui.GetDouble("SPACECRAFT_VELOCITY_SIGMA"));
  b->SetGlobalSpacecraftAccelerationAprioriSigma(ui.GetDouble("SPACECRAFT_ACCELERATION_SIGMA"));

  b->SetGlobalCameraAnglesAprioriSigma(ui.GetDouble("CAMERA_ANGLES_SIGMA"));
  b->SetGlobalCameraAngularVelocityAprioriSigma(ui.GetDouble("CAMERA_ANGULAR_VELOCITY_SIGMA"));
  b->SetGlobalCameraAngularAccelerationAprioriSigma(ui.GetDouble("CAMERA_ANGULAR_ACCELERATION_SIGMA"));

  // output options
  if (ui.WasEntered("FILE_PREFIX"))  {
      std::string outputfileprefix = ui.GetString("FILE_PREFIX");
      b->SetOutputFilePrefix(outputfileprefix);
  }

  b->SetStandardOutput(ui.GetBoolean("BUNDLEOUT_TXT"));
  b->SetCSVOutput(ui.GetBoolean("OUTPUT_CSV"));  
  b->SetResidualOutput(ui.GetBoolean("RESIDUALS_CSV"));

  // Check to make sure user entered something to adjust... Or can just points be in solution?
//   if (camsolve == "NONE"  &&  spsolve == "NONE") {
//     string msg = "Must either solve for camera pointing or spacecraft position";
//     throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
//   }

  // set convergence threshold
  b->SetConvergenceThreshold(ui.GetDouble("SIGMA0"));

  // set maximum number of iterations
  b->SetMaxIterations(ui.GetInteger("MAXITS"));

  // Bundle adjust the network
  try {

    if(ui.GetString("METHOD") == "SPECIALK") {
      b->SetDecompositionMethod(BundleAdjust::SPECIALK);
      b->SolveCholesky();
    }
    else if(ui.GetString("METHOD") == "CHOLMOD") {
      b->SetDecompositionMethod(BundleAdjust::CHOLMOD);
      b->SolveCholesky();
    }
    // the Solve method below is the old, LU Sparse routine, not explicitly used
    // in Jigsaw now, but retained indefinitely as a additional approach to
    // check against
    else
      b->Solve();

    b->ControlNet()->Write(ui.GetFilename("ONET"));
    PvlGroup gp("JigsawResults");

    // Update the cube pointing if requested
    if (ui.GetBoolean("UPDATE")) {

      for (int i = 0; i < b->Images(); i++) {
        Process p;
        CubeAttributeInput inAtt;
        Cube *c = p.SetInputCube(b->Filename(i), inAtt, ReadWrite);
        //check for existing polygon, if exists delete it
        if (c->getLabel()->HasObject("Polygon")) {
          c->getLabel()->DeleteObject("Polygon");
        }

        //check for CameraStatistics Table, if exists, delete
        for (int iobj = 0; iobj < c->getLabel()->Objects(); iobj++) {
          PvlObject obj = c->getLabel()->Object(iobj);
          if (obj.Name() != "Table") continue;
          if (obj["Name"][0] != iString("CameraStatistics")) continue;
          c->getLabel()->DeleteObject(iobj);
          break;
        }

        //  Get Kernel group and add or replace LastModifiedInstrumentPointing
        //  keyword.
        if (b->IsHeld(i)) continue;   // Don't update held images at all
        Table cmatrix = b->Cmatrix(i);
        std::string jigComment = "Jigged = " + Isis::iTime::CurrentLocalTime();
        cmatrix.Label().AddComment(jigComment);
        Table spvector = b->SpVector(i);
        spvector.Label().AddComment(jigComment);
        c->write(cmatrix);
        c->write(spvector);
        p.WriteHistory(*c);
      }
      gp += PvlKeyword("Status", "Camera pointing updated");
    }
    else {
      gp += PvlKeyword("Status", "Camera pointing NOT updated");
    }
    Application::Log(gp);
  }

  catch(iException &e) {

    b->ControlNet()->Write(ui.GetFilename("ONET"));
    string msg = "Unable to bundle adjust network [" + cnetFile + "]";
    throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  delete b;

}
