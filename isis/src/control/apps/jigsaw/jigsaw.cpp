#include "Isis.h"

#include <QDir>
#include <QObject>
#include <QList>

#include "BundleAdjust.h"
#include "BundleResults.h"
#include "BundleObservationSolveSettings.h"
#include "BundleSettings.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "iTime.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Process.h"
#include "Table.h"

using namespace std;
using namespace Isis;

BundleSettings bundleSettings(UserInterface &ui);
QList<BundleObservationSolveSettings> observationSolveSettings(UserInterface &ui);

void IsisMain() {

  // Get the control network and image list
  UserInterface &ui = Application::GetUserInterface();

  // Check to make sure user entered something to adjust... Or can just points be in solution?
  // YES - we should be able to just TRIANGULATE the points in the control net
  // right now to do this we have to fake out jigsaw by
  // 1) solving for both position and pointing but giving them high weights or
  // 2) solving for either position OR pointing but giving them high weights (the one not solved for
  //    is effectively "fixed" also)
  if (ui.GetString("CAMSOLVE") == "NONE"  &&  ui.GetString("SPSOLVE") == "NONE") {
    string msg = "Must either solve for camera pointing or spacecraft position";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QString cnetFile = ui.GetFileName("CNET");
  QString cubeList = ui.GetFileName("FROMLIST");
  
  // retrieve settings from jigsaw gui
  BundleSettings settings = bundleSettings(ui);
  BundleAdjust *bundleAdjustment = NULL;

  // Get the held list if entered and prep for bundle adjustment, to determine which constructor to use
  if (ui.WasEntered("HELDLIST")) {
    QString heldList = ui.GetFileName("HELDLIST");
    bundleAdjustment = new BundleAdjust(settings, cnetFile, cubeList, heldList);
  }
  else {
    bundleAdjustment = new BundleAdjust(settings, cnetFile, cubeList);
  }

  // Bundle adjust the network
  try {

    QObject::connect( bundleAdjustment, SIGNAL( statusUpdate(QString) ),
                      bundleAdjustment, SLOT( outputBundleStatus(QString) ) );
    bundleAdjustment->solveCholesky();
    
    // write updated control net if bundle has converged
    if (bundleAdjustment->isConverged()) {
      bundleAdjustment->controlNet()->Write(ui.GetFileName("ONET"));
    }

    PvlGroup gp("JigsawResults");
    
    // Update the cube pointing if requested but ONLY if bundle has converged
    if (ui.GetBoolean("UPDATE") ) {
      if ( !bundleAdjustment->isConverged() ) {
        gp += PvlKeyword("Status","Bundle did not converge, camera pointing NOT updated");
      }
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

//TODO - WHY DOES THIS MAKE VALGRIND ANGRY???  delete bundleAdjustment;
}

BundleSettings bundleSettings(UserInterface &ui) {
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

  QList<BundleObservationSolveSettings> solveSettingsList = observationSolveSettings(ui);
  settings.setObservationSolveOptions(solveSettingsList);
  // convergence criteria
  settings.setConvergenceCriteria(BundleSettings::Sigma0,
                                  ui.GetDouble("SIGMA0"),
                                  ui.GetInteger("MAXITS"));

  // max likelihood estimation
  if (ui.GetString("MODEL1").compare("NONE") != 0) {
    // if model1 is not "NONE", add to the models list with its quantile
    settings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::stringToModel(ui.GetString("MODEL1")),
            ui.GetDouble("MAX_MODEL1_C_QUANTILE"));

    if (ui.GetString("MODEL2").compare("NONE") != 0) {
      // if model2 is not "NONE", add to the models list with its quantile
      settings.addMaximumLikelihoodEstimatorModel(
          MaximumLikelihoodWFunctions::stringToModel(ui.GetString("MODEL2")),
              ui.GetDouble("MAX_MODEL2_C_QUANTILE"));

      if (ui.GetString("MODEL3").compare("NONE") != 0) {
        // if model3 is not "NONE", add to the models list with its quantile
        settings.addMaximumLikelihoodEstimatorModel(
            MaximumLikelihoodWFunctions::stringToModel(ui.GetString("MODEL3")),
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


QList<BundleObservationSolveSettings> observationSolveSettings(UserInterface &ui) {
  //************************************************************************************************
  QList<BundleObservationSolveSettings> observationSolveSettingsList;
  bool usePvl = ui.GetBoolean("USEPVL");
  PvlObject obj;
  if (usePvl) {
    ui.GetFileName("SC_PARAMETERS");
    Pvl scParPvl(FileName(ui.GetFileName("SC_PARAMETERS")).expanded());
    if (!scParPvl.hasObject("SensorParameters")) {
      QString msg = "Input SC_PARAMETERS file missing SensorParameters object";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // loop over parameter groups, read settings for each sensor into a
    // BundleObservationSolveSettings object, and append to observationSolveSettingsList
    obj = scParPvl.findObject("SensorParameters");
    PvlObject::PvlGroupIterator g;
    BundleObservationSolveSettings solveSettings;
    for(g = obj.beginGroup(); g != obj.endGroup(); ++g) {
      solveSettings.setFromPvl(*g);
      observationSolveSettingsList.append(solveSettings);
    }
  }
  else { // we're not using the pvl, so get what will be solve settings for all images from gui
    BundleObservationSolveSettings observationSolveSettings;

    BundleObservationSolveSettings::InstrumentPointingSolveOption pointingSolveOption =
        BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
            ui.GetString("CAMSOLVE"));

    double anglesAprioriSigma, angularVelocityAprioriSigma, angularAccelerationAprioriSigma;
    anglesAprioriSigma = angularVelocityAprioriSigma = angularAccelerationAprioriSigma = -1.0;
    if (ui.WasEntered("CAMERA_ANGLES_SIGMA")) {
      anglesAprioriSigma = ui.GetDouble("CAMERA_ANGLES_SIGMA");
    }
    if (ui.WasEntered("CAMERA_ANGULAR_VELOCITY_SIGMA")) {
      angularVelocityAprioriSigma = ui.GetDouble("CAMERA_ANGULAR_VELOCITY_SIGMA");
    }
    if (ui.WasEntered("CAMERA_ANGULAR_ACCELERATION_SIGMA")) {
      angularAccelerationAprioriSigma = ui.GetDouble("CAMERA_ANGULAR_ACCELERATION_SIGMA");
    }

    observationSolveSettings.setInstrumentPointingSettings(pointingSolveOption,
                                                           ui.GetBoolean("TWIST"),
                                                           ui.GetInteger("CKDEGREE"),
                                                           ui.GetInteger("CKSOLVEDEGREE"),
                                                           ui.GetBoolean("OVEREXISTING"),
                                                           anglesAprioriSigma,
                                                           angularVelocityAprioriSigma,
                                                           angularAccelerationAprioriSigma);

    BundleObservationSolveSettings::InstrumentPositionSolveOption positionSolveOption =
        BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
            ui.GetString("SPSOLVE"));

    double positionAprioriSigma, positionVelocityAprioriSigma, positionAccelerationAprioriSigma;
    positionAprioriSigma = positionVelocityAprioriSigma = positionAccelerationAprioriSigma = -1.0;
    if ( ui.WasEntered("SPACECRAFT_POSITION_SIGMA") ) {
      positionAprioriSigma = ui.GetDouble("SPACECRAFT_POSITION_SIGMA");
    }
    if ( ui.WasEntered("SPACECRAFT_VELOCITY_SIGMA") ) {
      positionVelocityAprioriSigma = ui.GetDouble("SPACECRAFT_VELOCITY_SIGMA");
    }
    if ( ui.WasEntered("SPACECRAFT_ACCELERATION_SIGMA") ) {
      positionAccelerationAprioriSigma = ui.GetDouble("SPACECRAFT_ACCELERATION_SIGMA");
    }

    observationSolveSettings.setInstrumentPositionSettings(positionSolveOption,
                                                           ui.GetInteger("SPKDEGREE"),
                                                           ui.GetInteger("SPKSOLVEDEGREE"),
                                                           ui.GetBoolean("OVERHERMITE"),
                                                           positionAprioriSigma,
                                                           positionVelocityAprioriSigma,
                                                           positionAccelerationAprioriSigma);
    observationSolveSettingsList.append(observationSolveSettings);
  }
  //************************************************************************************************
  return observationSolveSettingsList;
}
