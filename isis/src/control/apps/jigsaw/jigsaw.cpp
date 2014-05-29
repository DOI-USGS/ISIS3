#include "Isis.h"

#include <QDir>

#include <vector>

#include "BundleAdjust.h"
#include "BundleSettings.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "iTime.h"
#include "Process.h"
#include "Table.h"

using namespace std;
using namespace Isis;

BundleSettings bundleSettings();

void IsisMain() {

  // Get the control network and image list
  UserInterface &ui = Application::GetUserInterface();
  QString cnetFile = ui.GetFileName("CNET");
  QString cubeList = ui.GetFileName("FROMLIST");
  
  BundleSettings settings = bundleSettings();
  BundleAdjust *bundleAdjustment = NULL;

  // Get the held list if entered and prep for bundle adjustment, to determine which constructor to use
  if (ui.WasEntered("HELDLIST")) {
    QString heldList = ui.GetFileName("HELDLIST");
    bundleAdjustment = new BundleAdjust(settings, cnetFile, cubeList, heldList);
  }
  else {
    bundleAdjustment = new BundleAdjust(settings, cnetFile, cubeList);
  }

  //build lists of maximum likelihood estimation model strings and quantiles
  // change params needed QList<QString> maxLikeModels;
  // change params needed QList<double> maxQuan;
  // change params needed bundleAdjustment->maximumLikelihoodSetup(maxLikeModels,maxQuan);  //set up maximum likelihood estimater

  // For now don't use SC_SIGMAS.  This is not fully implemented yet.
  // if (ui.WasEntered("SC_SIGMAS"))
  //   bundleAdjustment->readSCSigmas(ui.GetFileName("SC_SIGMAS"));

//  if (ui.WasEntered("BINARYFILEPATH")) {
//    QString binaryfilepath = ui.GetString("BINARYFILEPATH");
//    QDir dir(binaryfilepath);

//    // verify path exists
//    if (!dir.exists()) {
//      QString msg = QString("BINARYFILEPATH [%1] does not exist").arg(binaryfilepath);
//      throw IException(IException::User, msg, _FILEINFO_);
//    }
//    else
//      bundleAdjustment->setErrorPropagationBinaryFilePath(binaryfilepath);
//  }


  // Check to make sure user entered something to adjust... Or can just points be in solution?
  if (ui.GetString("CAMSOLVE") == "NONE"  &&  ui.GetString("SPSOLVE") == "NONE") {
    string msg = "Must either solve for camera pointing or spacecraft position";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Bundle adjust the network
  try {

    if (ui.GetString("METHOD") == "OLDSPARSE") {
      // the Solve method below is the old, LU Sparse routine, not explicitly used
      // in Jigsaw now, but retained indefinitely as a additional approach to
      // check against
      bundleAdjustment->solve();
    }
    else {
      bundleAdjustment->solveCholesky();
    }
    
    bundleAdjustment->controlNet()->Write(ui.GetFileName("ONET"));
    PvlGroup gp("JigsawResults");
    
    // Update the cube pointing if requested but ONLY if bundle has converged
    if (ui.GetBoolean("UPDATE") ) {
      if ( !bundleAdjustment->isConverged() )
        gp += PvlKeyword("Status","Bundle did not converge, camera pointing NOT updated");
      else {
        for (int i = 0; i < bundleAdjustment->images(); i++) {
          Process p;
          CubeAttributeInput inAtt;
          Cube *c = p.SetInputCube(bundleAdjustment->fileName(i), inAtt, ReadWrite);
          //check for existing polygon, if exists delete it
          if (c->label()->hasObject("Polygon")) {
            c->label()->deleteObject("Polygon");
          }

          // check for CameraStatistics Table, if exists, delete
          for (int iobj = 0; iobj < c->label()->objects(); iobj++) {
            PvlObject obj = c->label()->object(iobj);
            if (obj.name() != "Table") continue;
            if (obj["Name"][0] != QString("CameraStatistics")) continue;
            c->label()->deleteObject(iobj);
            break;
          }

          //  Get Kernel group and add or replace LastModifiedInstrumentPointing
          //  keyword.
          if (bundleAdjustment->isHeld(i)) continue;   // Don't update held images at all
          Table cmatrix = bundleAdjustment->cMatrix(i);
          QString jigComment = "Jigged = " + Isis::iTime::CurrentLocalTime();
          cmatrix.Label().addComment(jigComment);
          Table spvector = bundleAdjustment->spVector(i);
          spvector.Label().addComment(jigComment);
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
    bundleAdjustment->controlNet()->Write(ui.GetFileName("ONET"));
    QString msg = "Unable to bundle adjust network [" + cnetFile + "]";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  delete bundleAdjustment;
}

BundleSettings bundleSettings() {

  UserInterface  &ui      = Application::GetUserInterface();
  BundleSettings settings;
  
  settings.setValidateNetwork(true);
  // solve options
  double latitudeSigma  = -1.0;
  double longitudeSigma = -1.0;
  double radiusSigma    = -1.0;
  if (ui.WasEntered("POINT_LATITUDE_SIGMA")) {
    latitudeSigma = ui.GetDouble("POINT_LATITUDE_SIGMA");
  }
  if (ui.WasEntered("POINT_LONGITUDE_SIGMA")) {
    longitudeSigma = ui.GetDouble("POINT_LONGITUDE_SIGMA");
  }
  if (ui.WasEntered("POINT_RADIUS_SIGMA")) {
    radiusSigma = ui.GetDouble("POINT_RADIUS_SIGMA");
  }
  settings.setSolveOptions(BundleSettings::stringToSolveMethod(ui.GetString("METHOD")), 
                           ui.GetBoolean("OBSERVATIONS"), ui.GetBoolean("UPDATE"), 
                           ui.GetBoolean("ERRORPROPAGATION"), ui.GetBoolean("RADIUS"),
                           latitudeSigma, longitudeSigma, radiusSigma);
  settings.setOutlierRejection(ui.GetBoolean("OUTLIER_REJECTION"),
                               ui.GetDouble("REJECTION_MULTIPLIER"));

  // position options
  double positionSigma     = -1.0;
  double velocitySigma     = -1.0;
  double accelerationSigma = -1.0;
  if (ui.WasEntered("SPACECRAFT_POSITION_SIGMA")) {
    positionSigma = ui.GetDouble("SPACECRAFT_POSITION_SIGMA");
  }
  if (ui.WasEntered("SPACECRAFT_VELOCITY_SIGMA")) {
    velocitySigma = ui.GetDouble("SPACECRAFT_VELOCITY_SIGMA");
  }
  if (ui.WasEntered("SPACECRAFT_ACCELERATION_SIGMA")) {
    accelerationSigma = ui.GetDouble("SPACECRAFT_ACCELERATION_SIGMA");
  }
  settings.setObserverPositionSolveOptions(
      BundleSettings::stringToObserverPositionSolveOption(ui.GetString("SPSOLVE")),
      ui.GetBoolean("OVERHERMITE"),
      ui.GetInteger("SPKDEGREE"),
      ui.GetInteger("SPKSOLVEDEGREE"),
      positionSigma, velocitySigma, accelerationSigma);

  // orientation settings
  double anglesSigma              = -1.0;
  double angularVelocitySigma     = -1.0;
  double angularAccelerationSigma = -1.0;
  if (ui.WasEntered("CAMERA_ANGLES_SIGMA")) {
    anglesSigma = ui.GetDouble("CAMERA_ANGLES_SIGMA");
  }
  if (ui.WasEntered("CAMERA_ANGULAR_VELOCITY_SIGMA")) {
    angularVelocitySigma = ui.GetDouble("CAMERA_ANGULAR_VELOCITY_SIGMA");
  }
  if (ui.WasEntered("CAMERA_ANGULAR_ACCELERATION_SIGMA")) {
    angularAccelerationSigma = ui.GetDouble("CAMERA_ANGULAR_ACCELERATION_SIGMA");
  }
  settings.setObserverOrientationSolveOptions(
      BundleSettings::stringToObserverOrientationSolveOption(ui.GetString("CAMSOLVE")),
      ui.GetBoolean("TWIST"),
      ui.GetBoolean("OVEREXISTING"),
      ui.GetInteger("CKDEGREE"),
      ui.GetInteger("CKSOLVEDEGREE"),
      anglesSigma, angularVelocitySigma, angularAccelerationSigma);

  // convergence criteria
  settings.setConvergenceCriteria(BundleSettings::Sigma0, 
                                  ui.GetDouble("SIGMA0"), 
                                  ui.GetInteger("MAXITS"));

  // max likelihood estimation
  if (ui.GetString("MODEL1").compare("NONE") != 0) {
    // if model1 is not "NONE", add to the models list with its quantile
    settings.addMaximumLikelihoodEstimatorModel(
        BundleSettings::stringToMaximumLikelihoodModel(ui.GetString("MODEL1")),
        ui.GetDouble("MAX_MODEL1_C_QUANTILE"));

    if (ui.GetString("MODEL2").compare("NONE") != 0) {
      // if model2 is not "NONE", add to the models list with its quantile
      settings.addMaximumLikelihoodEstimatorModel(
          BundleSettings::stringToMaximumLikelihoodModel(ui.GetString("MODEL2")),
          ui.GetDouble("MAX_MODEL2_C_QUANTILE"));

      if (ui.GetString("MODEL3").compare("NONE") != 0) {
        // if model3 is not "NONE", add to the models list with its quantile
        settings.addMaximumLikelihoodEstimatorModel(
            BundleSettings::stringToMaximumLikelihoodModel(ui.GetString("MODEL3")),
            ui.GetDouble("MAX_MODEL3_C_QUANTILE"));
      }
    }
  }

  // output options
  QString outputfileprefix = "";
  if (ui.WasEntered("FILE_PREFIX"))  {
    outputfileprefix = ui.GetString("FILE_PREFIX");
  }
  settings.setOutputFiles(outputfileprefix, ui.GetBoolean("BUNDLEOUT_TXT"),
                          ui.GetBoolean("OUTPUT_CSV"), ui.GetBoolean("RESIDUALS_CSV"));

  return settings;
}

