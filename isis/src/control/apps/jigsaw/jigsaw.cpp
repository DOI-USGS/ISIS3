#include "Isis.h"
#include "Process.h"
#include "BundleAdjust.h"
#include "Table.h"
#include "IException.h"
#include "CubeAttribute.h"
#include "iTime.h"
#include <vector>


using namespace std;
using namespace Isis;

void IsisMain() {

  // Get the control network and image list
  UserInterface &ui = Application::GetUserInterface();
  QString cnetFile = ui.GetFileName("CNET");
  QString cubeList = ui.GetFileName("FROMLIST");

  
  BundleAdjust *b = NULL;

  // Get the held list if entered and prep for bundle adjustment, to determine which constructor to use
  if (ui.WasEntered("HELDLIST")) {
    QString heldList = ui.GetFileName("HELDLIST");
    b = new BundleAdjust(cnetFile, cubeList, heldList);
  }
  else {
    b = new BundleAdjust(cnetFile, cubeList);
   }

  //build lists of maximum likelihood estimation model strings and quantiles
  QList<QString> maxLikeModels;
  QList<double> maxQuan;
  if ( ui.GetString("MODEL1").compare("NONE") !=0 ) {
    maxLikeModels.push_back(ui.GetString("MODEL1"));
    maxQuan.push_back(ui.GetDouble("MAX_MODEL1_C_QUANTILE"));
  }
  if ( ui.GetString("MODEL2").compare("NONE") !=0 ) {
    maxLikeModels.push_back(ui.GetString("MODEL2"));
    maxQuan.push_back(ui.GetDouble("MAX_MODEL2_C_QUANTILE"));
  }
  if ( ui.GetString("MODEL3").compare("NONE") !=0 ) {
    maxLikeModels.push_back(ui.GetString("MODEL3"));
    maxQuan.push_back(ui.GetDouble("MAX_MODEL3_C_QUANTILE"));
  }
  b->maximumLikelihoodSetup(maxLikeModels,maxQuan);  //set up maximum likelihood estimater

  // For now don't use SC_SIGMAS.  This is not fully implemented yet.
  // if (ui.WasEntered("SC_SIGMAS"))
  //   b->ReadSCSigmas(ui.GetFileName("SC_SIGMAS"));

  b->SetObservationMode(ui.GetBoolean("OBSERVATIONS"));
  b->SetSolutionMethod(ui.GetString("METHOD"));
  b->SetSolveRadii(ui.GetBoolean("RADIUS"));
  b->SetErrorPropagation(ui.GetBoolean("ERRORPROPAGATION"));
  b->SetOutlierRejection(ui.GetBoolean("OUTLIER_REJECTION"));
  b->SetRejectionMultiplier(ui.GetDouble("REJECTION_MULTIPLIER"));

  b->SetCKDegree(ui.GetInteger("CKDEGREE"));
  b->SetSolveCKDegree(ui.GetInteger("CKSOLVEDEGREE"));
  QString camsolve = ui.GetString("CAMSOLVE");

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
    b->SetSolveCmatrix(BundleAdjust::CKAll);
  }

  b->SetSolveTwist(ui.GetBoolean("TWIST"));

  b->SetSolvePolyOverPointing(ui.GetBoolean("OVEREXISTING"));

  b->SetSPKDegree(ui.GetInteger("SPKDEGREE"));
  b->SetSolveSPKDegree(ui.GetInteger("SPKSOLVEDEGREE"));
  QString spsolve = ui.GetString("SPSOLVE");
  if(spsolve == "NONE") {
    b->SetSolveSpacecraftPosition(BundleAdjust::Nothing);
  }
  else if(spsolve == "POSITION") {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionOnly);
  }
  else if(spsolve == "VELOCITIES") {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionVelocity);
  }
  else if(spsolve == "ACCELERATIONS") {
    b->SetSolveSpacecraftPosition(BundleAdjust::PositionVelocityAcceleration);
  }
  else {
    b->SetSolveSpacecraftPosition(BundleAdjust::SPKAll);
  }

  b->SetSolvePolyOverHermite(ui.GetBoolean("OVERHERMITE"));

  // global parameter uncertainties
  if( ui.WasEntered("POINT_LATITUDE_SIGMA") )
    b->SetGlobalLatitudeAprioriSigma(ui.GetDouble("POINT_LATITUDE_SIGMA"));
  else
    b->SetGlobalLatitudeAprioriSigma(-1.0);

  if( ui.WasEntered("POINT_LONGITUDE_SIGMA") )
    b->SetGlobalLongitudeAprioriSigma(ui.GetDouble("POINT_LONGITUDE_SIGMA"));
  else
    b->SetGlobalLongitudeAprioriSigma(-1.0);

  if( ui.WasEntered("POINT_RADIUS_SIGMA") )
    b->SetGlobalRadiiAprioriSigma(ui.GetDouble("POINT_RADIUS_SIGMA"));
  else
    b->SetGlobalRadiiAprioriSigma(-1.0);

  if( ui.WasEntered("SPACECRAFT_POSITION_SIGMA") )
    b->SetGlobalSpacecraftPositionAprioriSigma(ui.GetDouble("SPACECRAFT_POSITION_SIGMA"));
  else
    b->SetGlobalSpacecraftPositionAprioriSigma(-1.0);

  if( ui.WasEntered("SPACECRAFT_VELOCITY_SIGMA") )
    b->SetGlobalSpacecraftVelocityAprioriSigma(ui.GetDouble("SPACECRAFT_VELOCITY_SIGMA"));
  else
    b->SetGlobalSpacecraftVelocityAprioriSigma(-1.0);

  if( ui.WasEntered("SPACECRAFT_ACCELERATION_SIGMA") )
    b->SetGlobalSpacecraftAccelerationAprioriSigma(ui.GetDouble("SPACECRAFT_ACCELERATION_SIGMA"));
  else
    b->SetGlobalSpacecraftAccelerationAprioriSigma(-1.0);

  if( ui.WasEntered("CAMERA_ANGLES_SIGMA") )
    b->SetGlobalCameraAnglesAprioriSigma(ui.GetDouble("CAMERA_ANGLES_SIGMA"));
  else
    b->SetGlobalCameraAnglesAprioriSigma(-1.0);

  if( ui.WasEntered("CAMERA_ANGULAR_VELOCITY_SIGMA") )
    b->SetGlobalCameraAngularVelocityAprioriSigma(ui.GetDouble("CAMERA_ANGULAR_VELOCITY_SIGMA"));
  else
    b->SetGlobalCameraAngularVelocityAprioriSigma(-1.0);

  if( ui.WasEntered("CAMERA_ANGULAR_ACCELERATION_SIGMA") )
    b->SetGlobalCameraAngularAccelerationAprioriSigma(ui.GetDouble("CAMERA_ANGULAR_ACCELERATION_SIGMA"));
  else
    b->SetGlobalCameraAngularAccelerationAprioriSigma(-1.0);

  // output options
  if (ui.WasEntered("FILE_PREFIX"))  {
      QString outputfileprefix = ui.GetString("FILE_PREFIX");
      b->SetOutputFilePrefix(outputfileprefix);
  }

  b->SetStandardOutput(ui.GetBoolean("BUNDLEOUT_TXT"));
  b->SetCSVOutput(ui.GetBoolean("OUTPUT_CSV"));
  b->SetResidualOutput(ui.GetBoolean("RESIDUALS_CSV"));

  // Check to make sure user entered something to adjust... Or can just points be in solution?
   if (camsolve == "NONE"  &&  spsolve == "NONE") {
     string msg = "Must either solve for camera pointing or spacecraft position";
     throw IException(IException::User, msg, _FILEINFO_);
   }

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
    else if(ui.GetString("METHOD") == "SPARSE") {
      b->SetDecompositionMethod(BundleAdjust::CHOLMOD);
      b->SolveCholesky();
    }
    // the Solve method below is the old, LU Sparse routine, not explicitly used
    // in Jigsaw now, but retained indefinitely as a additional approach to
    // check against
    else
      b->Solve();

    b->ControlNet()->Write(ui.GetFileName("ONET"));
    PvlGroup gp("JigsawResults");

    // Update the cube pointing if requested but ONLY if bundle has converged
    if (ui.GetBoolean("UPDATE") ) {
      if ( !b->IsConverged() )
        gp += PvlKeyword("Status","Bundle did not converge, camera pointing NOT updated");
      else {
        for (int i = 0; i < b->Images(); i++) {
          Process p;
          CubeAttributeInput inAtt;
          Cube *c = p.SetInputCube(b->FileName(i), inAtt, ReadWrite);
          //check for existing polygon, if exists delete it
          if (c->label()->HasObject("Polygon")) {
            c->label()->DeleteObject("Polygon");
          }

          // check for CameraStatistics Table, if exists, delete
          for (int iobj = 0; iobj < c->label()->Objects(); iobj++) {
            PvlObject obj = c->label()->Object(iobj);
            if (obj.Name() != "Table") continue;
            if (obj["Name"][0] != QString("CameraStatistics")) continue;
            c->label()->DeleteObject(iobj);
            break;
          }

          //  Get Kernel group and add or replace LastModifiedInstrumentPointing
          //  keyword.
          if (b->IsHeld(i)) continue;   // Don't update held images at all
          Table cmatrix = b->Cmatrix(i);
          QString jigComment = "Jigged = " + Isis::iTime::CurrentLocalTime();
          cmatrix.Label().AddComment(jigComment);
          Table spvector = b->SpVector(i);
          spvector.Label().AddComment(jigComment);
          c->write(cmatrix);
          c->write(spvector);
          p.WriteHistory(*c);
        }
        gp += PvlKeyword("Status", "Camera pointing updated");
      }
    }
    else {
      gp += PvlKeyword("Status", "Camera pointing NOT updated");
    }
    Application::Log(gp);
  }

  catch(IException &e) {
    b->ControlNet()->Write(ui.GetFileName("ONET"));
    QString msg = "Unable to bundle adjust network [" + cnetFile + "]";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  delete b;

}
