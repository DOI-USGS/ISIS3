#include "BundleAdjust.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QCoreApplication>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "Application.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "BundleSolutionInfo.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "CameraGroundMap.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "ControlPoint.h"
#include "Control.h"
#include "CorrelationMatrix.h"
#include "Distance.h"
#include "ImageList.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MaximumLikelihoodWFunctions.h"
#include "SpecialPixel.h"
#include "StatCumProbDistDynCalc.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace boost::numeric::ublas;
using namespace Isis;

namespace Isis {

  static void cholmod_error_handler(int nStatus, 
                                    const char* file, 
                                    int nLineNo,
                                    const char* message) {
    QString errlog;

    errlog = "SPARSE: ";
    errlog += message;

    PvlGroup gp(errlog);

    gp += PvlKeyword("File",file);
    gp += PvlKeyword("Line_Number", toString(nLineNo));
    gp += PvlKeyword("Status", toString(nStatus));

//    Application::Log(gp);

    errlog += ". (See print.prt for details)";

//    throw IException(IException::Unknown, errlog, _FILEINFO_);
  }


  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             const QString &cnetFile,
                             const QString &cubeList,
                             bool bPrintSummary) {
    m_abort = false;
    Progress progress;
    // initialize constructor dependent settings...
    // m_bPrintSummary, m_bCleanUp, m_strCnetFileName, m_pCnet, m_pSnList, m_pHeldSnList,
    // m_bundleSettings
    m_bPrintSummary = bPrintSummary;
    m_bCleanUp = true;
    m_strCnetFileName = cnetFile;
    m_pCnet = new Isis::ControlNet(cnetFile, &progress);
    m_pSnList = new Isis::SerialNumberList(cubeList);
    m_pHeldSnList = NULL;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init(&progress);
  }


  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             const QString &cnetFile,
                             const QString &cubeList,
                             const QString &heldList,
                             bool bPrintSummary) {
    m_abort = false;
    Progress progress;
    // initialize constructor dependent settings...
    // m_bPrintSummary, m_bCleanUp, m_strCnetFileName, m_pCnet, m_pSnList, m_pHeldSnList,
    // m_bundleSettings
    m_bPrintSummary = bPrintSummary;
    m_bCleanUp = true;
    m_strCnetFileName = cnetFile;
    m_pCnet = new Isis::ControlNet(cnetFile, &progress);
    m_pSnList = new Isis::SerialNumberList(cubeList);
    m_pHeldSnList = new Isis::SerialNumberList(heldList);
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init(&progress);
  }


  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             QString &cnet,
                             SerialNumberList &snlist,
                             bool bPrintSummary) {
    // initialize m_dConvergenceThreshold ???
    // ??? deleted keyword ??? m_dConvergenceThreshold = 0.0;    // This is needed for deltack???
    // ???                                   //JWB - this gets overwritten in Init... move to constructor ???????????????????????????????????????????????????????????????????????
    // ???
    // initialize constructor dependent settings...
    // m_bPrintSummary, m_bCleanUp, m_strCnetFileName, m_pCnet, m_pSnList, m_pHeldSnList,
    // m_bundleSettings
    m_abort = false;
    Progress progress;
    m_bPrintSummary = bPrintSummary;
    m_bCleanUp = false;
    m_strCnetFileName = cnet;
    m_pCnet = new Isis::ControlNet(cnet, &progress);
    m_pSnList = &snlist;
    m_pHeldSnList = NULL;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             Control &cnet,
                             SerialNumberList &snlist,
                             bool bPrintSummary) {
    // initialize m_dConvergenceThreshold ???
    // ??? deleted keyword ??? m_dConvergenceThreshold = 0.0;    // This is needed for deltack???
    // ???                                   //JWB - this gets overwritten in Init... move to constructor ???????????????????????????????????????????????????????????????????????
    // ???
    // initialize constructor dependent settings...
    // m_bPrintSummary, m_bCleanUp, m_strCnetFileName, m_pCnet, m_pSnList, m_pHeldSnList,
    // m_bundleSettings
    m_abort = false;
    Progress progress;
    m_bPrintSummary = bPrintSummary;
    m_bCleanUp = false;
    m_strCnetFileName = cnet.fileName();
    m_pCnet = new Isis::ControlNet(cnet.fileName(), &progress);
    m_pSnList = &snlist;
    m_pHeldSnList = NULL;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             ControlNet &cnet,
                             SerialNumberList &snlist,
                             bool bPrintSummary) {
    // initialize m_dConvergenceThreshold ???
    // ??? deleted keyword ??? m_dConvergenceThreshold = 0.0;    // This is needed for deltack???
    // ???                                   //JWB - this gets overwritten in Init... move to constructor ???????????????????????????????????????????????????????????????????????
    // ???
    // initialize constructor dependent settings...
    // m_bPrintSummary, m_bCleanUp, m_strCnetFileName, m_pCnet, m_pSnList, m_pHeldSnList,
    // m_bundleSettings
    m_abort = false;
    m_bPrintSummary = bPrintSummary;
    m_bCleanUp = false;
    m_strCnetFileName = "";
    m_pCnet = &cnet;
    m_pSnList = &snlist;
    m_pHeldSnList = NULL;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  /**
   * Thread safe constructor.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             Control &control,
                             QList<ImageList *> imgLists,
                             bool bPrintSummary) {
    m_bundleSettings = bundleSettings;

    m_abort = false;
    Progress progress;
    m_pCnet = new Isis::ControlNet(control.fileName(), &progress);

    // this is too slow and we need to get rid of the serial number list anyway
    // should be unnecessary as Image class has serial number
    // could hang on to image list until creating BundleObservations?
    m_pSnList = new SerialNumberList;
    foreach (ImageList *imgList, imgLists) {
      foreach (Image *image, *imgList) {
        m_pSnList->add(image->fileName());
//      m_pSnList->add(image->serialNumber(), image->fileName());
      }
    }

    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    m_bPrintSummary = bPrintSummary;

    m_pHeldSnList = NULL;
    m_bCleanUp = false;
    m_strCnetFileName = control.fileName();
    
    init();
  }


  BundleAdjust::~BundleAdjust() {
    // If we have ownership
    if (m_bCleanUp) {
      delete m_pCnet;
      delete m_pSnList;

      if (m_bundleResults.numberHeldImages() > 0) {
        delete m_pHeldSnList;
      }

    }

    if ( m_bundleSettings->solveMethod() == BundleSettings::Sparse ) {
      freeCHOLMODLibraryVariables();
    }

  }


  /**
   * Initialize solution parameters
   *
   * @internal
   *   @history 2011-08-14 Debbie A. Cook - Opt out of network validation
   *                      for deltack network in order to allow
   *                      a single measure on a point
   */
  void BundleAdjust::init(Progress *progress) {
//printf("BOOST_UBLAS_CHECK_ENABLE = %d\n", BOOST_UBLAS_CHECK_ENABLE);
//printf("BOOST_UBLAS_TYPE_CHECK = %d\n", BOOST_UBLAS_TYPE_CHECK);
    // initialize
    //
    // JWB
    // - some of these not originally initialized.. better values???
    m_nIteration = 0;
    m_dError = DBL_MAX;
    m_dRTM = 0.0;
    m_dMTR = 0.0;
    m_nRank = 0;
    m_iterationSummary = "";

    // Get the cameras set up for all images
    // NOTE - THIS IS NOT THE SAME AS "setImage" as called in BundleAdjust::computePartials_DC
    // this call only does initializations; sets measure's camera pointer, etc
    // RENAME????????????
    m_pCnet->SetImages(*m_pSnList, progress);

    // clear JigsawRejected flags
    m_pCnet->ClearJigsawRejected();

    // initialize held variables
    int nImages = m_pSnList->size();

    // get count of held image and ensure they're in the control net
    if (m_pHeldSnList != NULL) {
      checkHeldList();

      for (int i = 0; i < nImages; i++) {
        if (m_pHeldSnList->hasSerialNumber(m_pSnList->serialNumber(i))) {
          m_bundleResults.incrementHeldImages();
        }
      }
    }

    // matrix stuff
    m_Normals.clear();
    m_nj.clear();
    m_Qs_SPECIALK.clear();
    m_imageSolution.clear();

    // we don't want to call initializeCHOLMODLibraryVariables() here since mRank=0
    // m_cm, m_SparseNormals are not initialized
    m_L = NULL;
    m_N = NULL;
    m_pTriplet = NULL;

    // should we initialize objects m_Statsx, m_Statsy, m_Statsrx, m_Statsry, m_Statsrxy

    // (must be a smarter way)
    // get target body radii and body specific conversion factors between radians and meters.
    // need validity checks and different conversion factors for lat and long
    // initialize m_bodyRadii
    m_bodyRadii[0] = m_bodyRadii[1] = m_bodyRadii[2] = Distance();
    Camera *pCamera = m_pCnet->Camera(0);
    if (pCamera) {
      pCamera->radii(m_bodyRadii);  // meters

//      printf("radii: %lf %lf %lf\n",m_bodyRadii[0],m_bodyRadii[1],m_bodyRadii[2]);

      if (m_bodyRadii[0] >= Distance(0, Distance::Meters)) {
        m_dMTR = 0.001 / m_bodyRadii[0].kilometers(); // at equator
        m_dRTM = 1.0 / m_dMTR;
      }
//      printf("MTR: %lf\nRTM: %lf\n",m_dMTR,m_dRTM);
     }

      // TESTING
      // code below should go into a separate method?
      // set up BundleObservations and assign solve settings for each from BundleSettings class
      for (int i = 0; i < nImages; i++) {

        Camera* camera = m_pCnet->Camera(i);
        QString observationNumber = m_pSnList->observationNumber(i);
        QString instrumentId = m_pSnList->spacecraftInstrumentId(i);
        QString serialNumber = m_pSnList->serialNumber(i);
        QString fileName = m_pSnList->fileName(i);

        // create a new BundleImage and add to new (or existing if observation mode is on)
        // BundleObservation
        BundleImage* image = new BundleImage(camera, serialNumber, fileName);

        if (!image) {
          QString msg = "In BundleAdjust::init(): image " + fileName + "is null" + "\n";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        BundleObservation *observation =
            m_bundleObservations.addnew(image, observationNumber, instrumentId, m_bundleSettings);

        if (!observation) {
          QString msg = "In BundleAdjust::init(): observation " + observationNumber + "is null" + "\n";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
      }

      // initialize exterior orientation (spice) for all BundleImages in all BundleObservations
      // TODO!!!! - should these initializations just be done when we add the new observation above?
      m_bundleObservations.initializeExteriorOrientation();

      if (m_bundleSettings->solveTargetBody())
        m_bundleObservations.initializeBodyRotation();

      // set up vector of BundleControlPoints
      int nControlPoints = m_pCnet->GetNumPoints();
      for (int i = 0; i < nControlPoints; i++) {
        ControlPoint* point = m_pCnet->GetPoint(i);
        if (point->IsIgnored())
          continue;

        BundleControlPointQsp bundleControlPoint(new BundleControlPoint(point));
        m_bundleControlPoints.append(bundleControlPoint);

        bundleControlPoint->setWeights(m_bundleSettings, m_dMTR);

        // set parent observation for each BundleMeasure

        int nMeasures = bundleControlPoint->size();
        for (int j=0; j < nMeasures; j++) {
          BundleMeasure *measure = bundleControlPoint->at(j);
          QString cubeSerialNumber = measure->cubeSerialNumber();

          BundleObservation *observation =
              m_bundleObservations.observationByCubeSerialNumber(cubeSerialNumber);

          measure->setParentObservation(observation);
        }
      }

    // ===========================================================================================//
    // ==== Use the bundle settings to initialize more member variables and set up solutions =====//
    // ===========================================================================================//

    // TODO:  Need to have some validation code to make sure everything is
    // on the up-and-up with the control network.  Add checks for multiple
    // networks, images without any points, and points on images removed from
    // the control net (when we start adding software to remove points with high
    // residuals) and ?.  For "deltack" a single measure on a point is allowed
    // so skip the test.
    if (m_bundleSettings->validateNetwork()) {
      validateNetwork();
    }
    m_bundleResults.maximumLikelihoodSetUp(m_bundleSettings->maximumLikelihoodEstimatorModels());

    // ===========================================================================================//
    // =============== End Bundle Settings =======================================================//
    // ===========================================================================================//
  }


  /**
   * control network validation - on the very real chance that the net
   * has not been checked before running the bundle
   *
   * checks implemented for ...
   *  (1) images with 0 or 1 measures
   * @internal
   *   @history  2011-08-4 Debbie A. Cook - Changed error message to
   *                        indicate it fails with one measure as
   *                        well as no measures.
   */
  bool BundleAdjust::validateNetwork() {
    emit statusUpdate("Validating network...");

    int nimagesWithInsufficientMeasures = 0;
    QString msg = "Images with one or less measures:\n";

    int nObservations = m_bundleObservations.size();
    for (int i = 0; i < nObservations; i++) {
      int nImages = m_bundleObservations.at(i)->size();
      for (int j = 0; j < nImages; j++) {
        BundleImage *bundleImage = m_bundleObservations.at(i)->at(j);
        int nMeasures = m_pCnet->GetNumberOfValidMeasuresInImage(bundleImage->serialNumber());

        if (nMeasures > 1)
          continue;

        nimagesWithInsufficientMeasures++;
        msg += bundleImage->fileName() + ": " + toString(nMeasures) + "\n";
      }
    }

    if ( nimagesWithInsufficientMeasures > 0 ) {
      throw IException(IException::User, msg, _FILEINFO_);
    }

    emit statusUpdate("Validation complete!...");

    return true;
  }

  /**
   * Initializations for Cholmod sparse matrix package
   */
  bool BundleAdjust::initializeCHOLMODLibraryVariables() {

      if ( m_nRank <= 0 ) {
          return false;
      }

      m_pTriplet = NULL;

      cholmod_start(&m_cm);

      // set user-defined cholmod error handler
      m_cm.error_handler = cholmod_error_handler;

      // testing not using metis
      m_cm.nmethods = 1;
      m_cm.method[0].ordering = CHOLMOD_AMD;

      // set size of sparse block normal equations matrix
      if (m_bundleSettings->solveTargetBody())
        m_SparseNormals.setNumberOfColumns(m_bundleObservations.size()+1);
      else
        m_SparseNormals.setNumberOfColumns(m_bundleObservations.size());

      return true;
  }


  /**
   * Initializations for Cholmod sparse matrix package
   */
  bool BundleAdjust::freeCHOLMODLibraryVariables() {

    cholmod_free_triplet(&m_pTriplet, &m_cm);
    cholmod_free_sparse(&m_N, &m_cm);
    cholmod_free_factor(&m_L, &m_cm);

    cholmod_finish(&m_cm);

    return true;
  }


  /**
   * This method checks all cube files in the held list to make sure they are in the
   * input list.
   */
  void BundleAdjust::checkHeldList() {
    for (int ih = 0; ih < m_pHeldSnList->size(); ih++) {
      if (!(m_pSnList->hasSerialNumber(m_pHeldSnList->serialNumber(ih)))) {
        QString msg = "Held image [" + m_pHeldSnList->serialNumber(ih)
                          + "not in FROMLIST";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Initializes matrices and parameters for bundle adjustment.
   */
  void BundleAdjust::initialize() { // ??? rename this method, maybe call it in the init() method if Sparse

    // size of reduced normals matrix

    // TODO
    // this should be determined from BundleSettings
    // m_nRank will be the sum of observation, target, and self-cal parameters
    // TODO
    m_nRank = m_bundleObservations.numberParameters();

    if (m_bundleSettings->solveTargetBody())
      m_nRank += m_bundleSettings->numberTargetBodyParameters();
//    m_nRank = m_bundleSettings->sizeReducedNormalEquationsMatrix();

    int n3DPoints = m_bundleControlPoints.size();

    if ( m_bundleSettings->solveMethod() == BundleSettings::SpecialK ) {
      m_Normals.resize(m_nRank);           // set size of reduced normals matrix
      m_Normals.clear();                   // zero all elements
      m_Qs_SPECIALK.resize(n3DPoints);
    }

    m_bundleResults.setNumberUnknownParameters(m_nRank + 3 * n3DPoints);

    m_imageSolution.resize(m_nRank);


    // initialize NICS, Qs, and point correction vectors to zero
    for (int i = 0; i < n3DPoints; i++) {

      // TODO_CHOLMOD: is this needed with new cholmod implementation?
      if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
        m_Qs_SPECIALK[i].clear();
      }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initializations for cholmod
    if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
      initializeCHOLMODLibraryVariables();
    }
  }

  /**
   * Least squares bundle adjustment solution using Cholesky decomposition.
   */
  // TODO: make solveCholesky return a BundleSolutionInfo object and delete this placeholder ???
  BundleSolutionInfo BundleAdjust::solveCholeskyBR() {
    solveCholesky();
    return bundleSolveInformation();
  }


  /**
   * Flag to abort when bundle is threaded. Flag is set outside the bundle thread, typically by the
   * gui thread.
   */
  void BundleAdjust::abortBundle() {
    m_abort = true;
  }


  /**
   * Least squares bundle adjustment solution using Cholesky decomposition.
   */
  bool BundleAdjust::solveCholesky() {
    try {

      // throw error if a frame camera is included AND if m_bundleSettings->solveInstrumentPositionOverHermiteSpline()
      // is set to true (can only use for line scan or radar)
  //    if (m_bundleSettings->solveInstrumentPositionOverHermiteSpline() == true) {
  //      int nImages = images();
  //      for (int i = 0; i < nImages; i++) {
  //        if (m_pCnet->Camera(i)->GetCameraType() == 0) {
  //          QString msg = "At least one sensor is a frame camera. Spacecraft Option OVERHERMITE is not valid for frame cameras\n";
  //          throw IException(IException::User, msg, _FILEINFO_);
  //        }
  //      }
  //    }

      initialize();

      // Compute the apriori lat/lons for each nonheld point
      m_pCnet->ComputeApriori(); // original location

      // ken testing - if solving for target mean radius, set point radius to current mean radius
      // if solving for triaxial radii, set point radius to local radius
      if (m_bundleTargetBody && m_bundleTargetBody->solveMeanRadius()) {
        int nControlPoints = m_bundleControlPoints.size();
        for (int i = 0; i < nControlPoints; i++) {
          ControlPoint *point = m_bundleControlPoints.at(i)->rawControlPoint();
          SurfacePoint surfacepoint = point->GetAdjustedSurfacePoint();

          surfacepoint.ResetLocalRadius(m_bundleTargetBody->meanRadius());

          point->SetAdjustedSurfacePoint(surfacepoint);
        }
      }

      if (m_bundleTargetBody && m_bundleTargetBody->solveTriaxialRadii()) {
        int nControlPoints = m_bundleControlPoints.size();
        for (int i = 0; i < nControlPoints; i++) {
          ControlPoint *point = m_bundleControlPoints.at(i)->rawControlPoint();
          SurfacePoint surfacepoint = point->GetAdjustedSurfacePoint();

          Distance localRadius = m_bundleTargetBody->localRadius(surfacepoint.GetLatitude(),
                                                                 surfacepoint.GetLongitude());
          surfacepoint.ResetLocalRadius(localRadius);

          point->SetAdjustedSurfacePoint(surfacepoint);
        }
      }

      m_nIteration = 1;
      double dvtpv = 0.0;
      double dSigma0_previous = 0.0;

      // start the clock
      clock_t t1 = clock();

      for (;;) {

        emit iterationUpdate(m_nIteration, m_bundleResults.sigma0());

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        emit statusUpdate( QString("\n starting iteration %1 \n").arg(m_nIteration) );

        clock_t iterationclock1 = clock();

        // zero normals (after iteration 0)
        if (m_nIteration != 1) {
          if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
            m_Normals.clear();
          }
          else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
            m_SparseNormals.zeroBlocks();
          }
        }

        // form normal equations
  //      clock_t formNormalsclock1 = clock();
  //      printf("starting FormNormals\n");

        if (!formNormalEquations()) {
          m_bundleResults.setConverged(false);
          break;
        }

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

  //      clock_t formNormalsclock2 = clock();
  //      double dFormNormalsTime = ((formNormalsclock2-formNormalsclock1)/(double)CLOCKS_PER_SEC);
  //      printf("FormNormals Elapsed Time: %20.10lf\n",dFormNormalsTime);

        // solve system
  //      clock_t Solveclock1 = clock();
  //      printf("Starting Solve System\n");

        if (!solveSystem()) {
          printf("solve failed!\n");
          m_bundleResults.setConverged(false);
          break;
        }

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

  //      clock_t Solveclock2 = clock();
  //      double dSolveTime = ((Solveclock2-Solveclock1)/(double)CLOCKS_PER_SEC);
  //      printf("Solve Elapsed Time: %20.10lf\n",dSolveTime);

        // apply parameter corrections
  //      clock_t Correctionsclock1 = clock();
        applyParameterCorrections();
  //      clock_t Correctionsclock2 = clock();
  //      double dCorrectionsTime = ((Correctionsclock2-Correctionsclock1)/(double)CLOCKS_PER_SEC);
  //      printf("Corrections Elapsed Time: %20.10lf\n",dCorrectionsTime);

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        // compute residuals
  //      clock_t Residualsclock1 = clock();

        dvtpv = computeResiduals();
  //      clock_t Residualsclock2 = clock();
  //      double dResidualsTime = ((Residualsclock2-Residualsclock1)/(double)CLOCKS_PER_SEC);
  //      printf("Residuals Elapsed Time: %20.10lf\n",dResidualsTime);

        // flag outliers
        if ( m_bundleSettings->outlierRejection() ) {
          computeRejectionLimit();
          flagOutliers();
        }

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        // variance of unit weight (also reference variance, variance factor, etc.)
        m_bundleResults.computeSigma0(dvtpv, m_bundleSettings->convergenceCriteria());

        emit statusUpdate(QString("Iteration: %1").arg(m_nIteration));
        emit statusUpdate(QString("Sigma0: %1").arg(m_bundleResults.sigma0()));
        emit statusUpdate(QString("Observations: %1").arg(m_bundleResults.numberObservations()));
        emit statusUpdate(QString("Constrained Parameters:%1").arg(
                          m_bundleResults.numberConstrainedPointParameters()));
        emit statusUpdate(QString("Unknowns: %1").arg(m_bundleResults.numberUnknownParameters()));
        emit statusUpdate( QString("Degrees of Freedom: %1").arg(m_bundleResults.degreesOfFreedom()));
        emit iterationUpdate(m_nIteration, m_bundleResults.sigma0());

        // check for convergence
        if (m_bundleSettings->convergenceCriteria() == BundleSettings::Sigma0) {
          if (fabs(dSigma0_previous - m_bundleResults.sigma0())
                <= m_bundleSettings->convergenceCriteriaThreshold()) { // convergence detected
            // if maximum likelihood tiers are being processed, check to see if there's another tier
            if (m_bundleResults.maximumLikelihoodModelIndex()
                   < m_bundleResults.numberMaximumLikelihoodModels() - 1
                && m_bundleResults.maximumLikelihoodModelIndex()
                     < 2) { // is this second condition redundant???
                                                                      // should bundlestats require num models <= 3, so num models - 1 <= 2
              // to go, then continue with the next maximum likelihood model.
              if (m_bundleResults.numberMaximumLikelihoodModels()
                      > m_bundleResults.maximumLikelihoodModelIndex() + 1) {
                // we will increment the index if there is another model after this one
                m_bundleResults.incrementMaximumLikelihoodModelIndex();
              }
            }
            else {  // otherwise iterations are complete
              m_bundleResults.setConverged(true);
              emit statusUpdate("\n Bundle has converged");
              break;
            }
          }
        }
        else { // bundleSettings.convergenceCriteria() == BundleSettings::ParameterCorrections
          int nconverged = 0;
          int numimgparam = m_imageSolution.size();
          for (int ij = 0; ij < numimgparam; ij++) {
            if (fabs(m_imageSolution(ij)) > m_bundleSettings->convergenceCriteriaThreshold()) {
              break;
            }
            else
              nconverged++;
          }

          if ( nconverged == numimgparam ) {
            m_bundleResults.setConverged(true);
            emit statusUpdate("Bundle has converged");
            break;
          }
        }

        m_bundleResults.printMaximumLikelihoodTierInformation();
        clock_t iterationclock2 = clock();
        double dIterationTime = ((iterationclock2 - iterationclock1) / (double)CLOCKS_PER_SEC);
        emit statusUpdate( QString("End of Iteration %1").arg(m_nIteration) );
        emit statusUpdate( QString("Elapsed Time: %1").arg(dIterationTime) );

        // check for maximum iterations
        if (m_nIteration >= m_bundleSettings->convergenceCriteriaMaximumIterations()) {
          break;
        }

        // restart the dynamic calculation of the cumulative probility distribution of residuals
        // (in unweighted pixels) --so it will be up to date for the next iteration
        if (!m_bundleResults.converged()) {
          m_bundleResults.initializeResidualsProbabilityDistribution(101);
        }// TODO: is this necessary ??? probably all ready initialized to 101 nodes in bundle settings constructor...

        // if we're using CHOLMOD and still going, release cholmod_factor (if we don't, memory leaks will occur),
        // otherwise we need it for error propagation
        if ( m_bundleSettings->solveMethod() == BundleSettings::Sparse ) {
          if (!m_bundleResults.converged() || !m_bundleSettings->errorPropagation())
            cholmod_free_factor(&m_L, &m_cm);
        }


        iterationSummary();

        m_nIteration++;

        dSigma0_previous = m_bundleResults.sigma0();
      }

      if (m_bundleResults.converged() && m_bundleSettings->errorPropagation()) {
        clock_t terror1 = clock();
        emit statusUpdate("Starting Error Propagation");
        errorPropagation();
        emit statusUpdate("Error Propagation Complete");
        clock_t terror2 = clock();
        m_bundleResults.setElapsedTimeErrorProp((terror2 - terror1) / (double)CLOCKS_PER_SEC);
      }

      clock_t t2 = clock();
      m_bundleResults.setElapsedTime((t2 - t1) / (double)CLOCKS_PER_SEC);

      wrapUp();

      emit statusUpdate("\n Generating report files");
      output();

      emit statusUpdate("\n Bundle Complete");
      BundleSolutionInfo *results = new BundleSolutionInfo(bundleSolveInformation());
      emit resultsReady(results);

      iterationSummary();
    }
    catch (IException &e) {
      QString exceptionStr = e.what();
      emit bundleException(exceptionStr);
      m_bundleResults.setConverged(false);
      emit statusUpdate("\n aborting...");
      emit finished();
      return false;
    }

    return true;
  }

  BundleSolutionInfo BundleAdjust::bundleSolveInformation() {
    BundleSolutionInfo results(m_bundleSettings, FileName(m_strCnetFileName), m_bundleResults);
    results.setRunTime("");
    return results;
  }

  /**
   * Forming the least-squares normal equations matrix.
   */
  bool BundleAdjust::formNormalEquations() {
    if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
      return formNormalEquations_CHOLMOD();
    }
    else {
      return formNormalEquations_SPECIALK();
    }

    return false;
  }


  /**
   * Solve normal equations system.
   */
  bool BundleAdjust::solveSystem() {
    if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
      return solveSystem_CHOLMOD();
    }
    else {
      return solveSystem_SPECIALK();
    }

    return false;
  }


  /**
   * Forming the least-squares normal equations matrix via cholmod.
   */
  bool BundleAdjust::formNormalEquations_CHOLMOD() {
    bool bStatus = false;

    m_bundleResults.setNumberObservations(0);// ???
    m_bundleResults.resetNumberConstrainedPointParameters();//???

    static boost::numeric::ublas::matrix<double> coeff_target;
    static boost::numeric::ublas::matrix<double> coeff_image;
    static boost::numeric::ublas::matrix<double> coeff_point3D(2, 3);
    static boost::numeric::ublas::vector<double> coeff_RHS(2);
    static boost::numeric::ublas::symmetric_matrix<double, upper> N22(3);                // 3x3 upper triangular
    SparseBlockColumnMatrix N12;
    static boost::numeric::ublas::vector<double> n2(3);                                  // 3x1 vector
    boost::numeric::ublas::compressed_vector<double> n1(m_nRank);                        // image parameters x 1

    m_nj.resize(m_nRank);

    // if solving for target body parameters, set size of coeff_target (note this size will not
    // change through the adjustment
    if (m_bundleSettings->solveTargetBody()) {
      int numTargetBodyParameters = m_bundleSettings->numberTargetBodyParameters();
      // TODO make sure numTargetBodyParameters is greater than 0
      coeff_target.resize(2,numTargetBodyParameters);
    }

    // clear N12, n1, and nj
    N12.clear();
    n1.clear();
    m_nj.clear();

    // clear static matrices
    coeff_point3D.clear();
    coeff_RHS.clear();
    N22.clear();
    n2.clear();

    // loop over 3D points
    int nGood3DPoints = 0;
    int nRejected3DPoints = 0;
    int nPointIndex = 0;
    int n3DPoints = m_bundleControlPoints.size();

//  char buf[1056];
//  sprintf(buf,"\n\t                      Points:%10d\n", n3DPoints);
//  m_fp_log << buf;
//  printf("%s", buf);

    printf("\n\n");

    for (int i = 0; i < n3DPoints; i++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if (point->isRejected()) {
        nRejected3DPoints++;
//            sprintf(buf, "\tRejected %s - 3D Point %d of %d RejLimit = %lf\n", point.Id().toLatin1().data(),nPointIndex,n3DPoints,m_bundleResults.rejectionLimit());
//      m_fp_log << buf;

        nPointIndex++;
        continue;
      }

      // send notification to UI indicating index of point currently being processed
      // m_nProcessedPoint = i+1;
      // UI.Notify(BundleEvent.NEW_POINT_PROCESSED);

      if ( i != 0 ) {
        N22.clear();
//      N12.clear();
        N12.wipe();
        n2.clear();
      }

      // loop over measures for this point
      int nMeasures = point->size();
      for (int j = 0; j < nMeasures; j++) {
        BundleMeasure *measure = point->at(j);

        // flagged as "JigsawFail" implies this measure has been rejected
        // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
        if (measure->isRejected())
          continue;

        // printf("   Processing Measure %d of %d\n", j,nMeasures);

        bStatus = computePartials_DC(coeff_target, coeff_image, coeff_point3D, coeff_RHS, *measure,
                                     *point);


//        std::cout << "observation index" << measure->observationIndex() << std::
// ;
//std::cout << coeff_target(0,0) << "," << coeff_target(0,1) << ",";
//        std::cout << std::setprecision(12) << coeff_target(0,0) << ",";
//        std::cout << coeff_image(0,0) << "," << coeff_image(0,1) << ",";
//        std::cout << coeff_point3D(0,0) << "," << coeff_point3D(0,1) << "," << coeff_point3D(0,2) << ",";
//        std::cout << coeff_RHS[0] << std::endl;
//std::cout << coeff_target(1,0) << "," << coeff_target(1,1) << ",";
//        std::cout << coeff_target(1,0) << ",";
//        std::cout << coeff_image(1,0) << "," << coeff_image(1,1) << ",";
//        std::cout << coeff_point3D(1,0) << "," << coeff_point3D(1,1) << "," << coeff_point3D(1,2) << ",";
//        std::cout << coeff_RHS[1] << std::endl;

        if (!bStatus)      // this measure should be flagged as rejected
          continue;

        // update number of observations
        int numObs = m_bundleResults.numberObservations();
        m_bundleResults.setNumberObservations(numObs + 2);
        formNormals1_CHOLMOD(N22, N12, n1, n2, coeff_target, coeff_image, coeff_point3D, coeff_RHS,
                             measure->observationIndex());
      } // end loop over this points measures

      formNormals2_CHOLMOD(N22, N12, n2, m_nj, point);

//m_SparseNormals.printClean(std::cout);

      nPointIndex++;

      nGood3DPoints++;

  } // end loop over 3D points

//m_SparseNormals.printClean(std::cout);

  // finally, form the reduced normal equations
//std::cout << "nj" << std::endl << m_nj << std::endl;

  formNormals3_CHOLMOD(n1, m_nj);

//m_SparseNormals.printClean(std::cout);

  // update number of unknown parameters
  m_bundleResults.setNumberUnknownParameters(m_nRank + 3 * nGood3DPoints);

  return bStatus;
}


  /**
   * Forming first set of auxiliary matrices for normal equations matrix via cholmod.
   */
  bool BundleAdjust::formNormals1_CHOLMOD(symmetric_matrix<double, upper>&N22,
      SparseBlockColumnMatrix &N12, compressed_vector<double> &n1,
      vector<double> &n2, matrix<double> &coeff_target, matrix<double> &coeff_image,
      matrix<double> &coeff_point3D, vector<double> &coeff_RHS, int observationIndex) {

    static symmetric_matrix<double, upper> N11;
    static matrix<double> NTargetImage;

    int blockIndex = observationIndex;

    // if we are solving for target body parameters
    int nTargetPartials = coeff_target.size2();
    if (m_bundleSettings->solveTargetBody()) {
      blockIndex++;

      static vector<double> n1_target(nTargetPartials);
      n1_target.resize(nTargetPartials);
      n1_target.clear();

      // form N11 (normals for target body)
      N11.resize(nTargetPartials);
      N11.clear();

//std::cout << "target" << std::endl << coeff_target << std::endl;
//std::cout << "point" << std::endl << coeff_point3D << std::endl;
//std::cout << "rhs" << std::endl << coeff_RHS << std::endl;

      N11 = prod(trans(coeff_target), coeff_target);

      // insert submatrix at column, row
      m_SparseNormals.insertMatrixBlock(0, 0, nTargetPartials, nTargetPartials);

//std::cout << "SparseNormals: " << 0 << std::endl << N11 << std::endl;

      (*(*m_SparseNormals[0])[0]) += N11;

//std::cout << (*(*m_SparseNormals[0])[0]) << std::endl;

      // form portion of N11 between target and image
      NTargetImage.resize(nTargetPartials, coeff_image.size2());
      NTargetImage.clear();
      NTargetImage = prod(trans(coeff_target),coeff_image);

      m_SparseNormals.insertMatrixBlock(observationIndex+1, 0, nTargetPartials, coeff_image.size2());
      (*(*m_SparseNormals[observationIndex+1])[0]) += NTargetImage;

      // form N12_Target
      static matrix<double> N12_Target(nTargetPartials, 3);
      N12_Target.clear();

      N12_Target = prod(trans(coeff_target), coeff_point3D);

//printf("N12_Target before insert\n");
//std::cout << N12_Target << std::endl;

      // insert N12_Target into N12
      N12.insertMatrixBlock(0, nTargetPartials, 3);
      *N12[0] += N12_Target;

      // form n1
      n1_target = prod(trans(coeff_target), coeff_RHS);

      // insert n1_target into n1
      for (int i = 0; i < nTargetPartials; i++) {
        n1(i) += n1_target(i);
      }
    }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ below is ok (2015-06-03)
// TODO - if solving for target (and/or self-cal) have to use not observationIndex below but
// observationIndex plus 1 or 2

    int nImagePartials = coeff_image.size2();

    static boost::numeric::ublas::vector<double> n1_image(nImagePartials);
    n1_image.resize(nImagePartials);
    n1_image.clear();

    // form N11 (normals for photo)
//    static boost::numeric::ublas::symmetric_matrix<double, upper> N11(nImagePartials);
    N11.resize(nImagePartials);
    N11.clear();

//std::cout << "image" << std::endl << coeff_image << std::endl;
//std::cout << "point" << std::endl << coeff_point3D << std::endl;
//std::cout << "rhs" << std::endl << coeff_RHS << std::endl;

    N11 = prod(trans(coeff_image), coeff_image);

//std::cout << "N11" << std::endl << N11 << std::endl;

//  int t = nImagePartials * observationIndex;
    int t = 0;
    //testing
    for (int a = 0; a < observationIndex; a++) {
      BundleObservation *observation = m_bundleObservations.at(a);
      t += observation->numberParameters();
    }
    // account for target parameters
    t += nTargetPartials;

    // insert submatrix at column, row
    m_SparseNormals.insertMatrixBlock(blockIndex, blockIndex, nImagePartials,
                                      nImagePartials);

//std::cout << "SparseNormals: " << blockIndex << std::endl << N11 << std::endl;

    (*(*m_SparseNormals[blockIndex])[blockIndex]) += N11;

//std::cout << (*(*m_SparseNormals[blockIndex])[blockIndex]) << std::endl;

    // form N12_Image
    static matrix<double> N12_Image(nImagePartials, 3);
    N12_Image.resize(nImagePartials, 3);
    N12_Image.clear();

    N12_Image = prod(trans(coeff_image), coeff_point3D);

//printf("N12_Image before insert\n");
//N12.print(std::cout);

//std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
    N12.insertMatrixBlock(blockIndex, nImagePartials, 3);
    *N12[blockIndex] += N12_Image;

//printf("N12\n");
//N12.print(std::cout);

    // form n1
    n1_image = prod(trans(coeff_image), coeff_RHS);

//std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    // TODO - MUST ACCOUNT FOR TARGET BODY PARAMETERS
    // WHEN INSERTING INTO n1 HERE!!!!!
    for (int i = 0; i < nImagePartials; i++) {
      n1(i + t) += n1_image(i);
    }
//std::cout << "n1" << std::endl << n1 << std::endl;

    // form N22
    N22 += prod(trans(coeff_point3D), coeff_point3D);

//std::cout << "N22" << std::endl << N22 << std::endl;

    // form n2
    n2 += prod(trans(coeff_point3D), coeff_RHS);

//std::cout << "n2" << std::endl << n2 << std::endl;

    return true;
  }


  /**
   * Forming second set of auxiliary matrices for normal equations matrix via cholmod.
   */
  bool BundleAdjust::formNormals2_CHOLMOD(symmetric_matrix<double, upper>&N22,
                                          SparseBlockColumnMatrix &N12,
                                          vector<double> &n2,
                                          vector<double> &nj,
                                          BundleControlPointQsp &bundleControlPoint) {

//N12.printClean(std::cout);

    boost::numeric::ublas::bounded_vector<double, 3> &NIC = bundleControlPoint->nicVector();
    SparseBlockRowMatrix &Q = bundleControlPoint->cholmod_QMatrix();

    NIC.clear();
    Q.zeroBlocks();

    // weighting of 3D point parameters
//    const ControlPoint *point = m_pCnet->GetPoint(i);
    boost::numeric::ublas::bounded_vector<double, 3> &weights
        = bundleControlPoint->weights();
    boost::numeric::ublas::bounded_vector<double, 3> &corrections
        = bundleControlPoint->corrections();

//    std::cout << "Point" << point->GetId() << "weights" << std::endl << weights << std::endl;

//    std::cout << "corrections" << std::endl << corrections << std::endl;

    if (weights(0) > 0.0) {
      N22(0,0) += weights(0);
      n2(0) += (-weights(0) * corrections(0));
      m_bundleResults.incrementNumberConstrainedPointParameters(1);
    }

    if (weights(1) > 0.0) {
      N22(1,1) += weights(1);
      n2(1) += (-weights(1) * corrections(1));
      m_bundleResults.incrementNumberConstrainedPointParameters(1);
    }

    if (weights(2) > 0.0) {
      N22(2,2) += weights(2);
      n2(2) += (-weights(2) * corrections(2));
      m_bundleResults.incrementNumberConstrainedPointParameters(1);
    }

    //std::cout << "N22 before inverse" << std::endl << N22 << std::endl;
    // invert N22
    Invert_3x3(N22);
   // std::cout << "N22 after inverse" << std::endl << N22 << std::endl;

    // save upper triangular covariance matrix for error propagation
    // TODO:  The following method does not exist yet (08-13-2010)
//    SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();
    SurfacePoint SurfacePoint = bundleControlPoint->getAdjustedSurfacePoint();
    SurfacePoint.SetSphericalMatrix(N22);
    bundleControlPoint->setAdjustedSurfacePoint(SurfacePoint);

    // form Q (this is N22{-1} * N12{T})
//    clock_t FormQ1 = clock();
//    Q = prod(N22, trans(N12));
    product_ATransB(N22, N12, Q);
//    clock_t FormQ2 = clock();
//    double dFormQTime = ((FormQ2-FormQ1)/(double)CLOCKS_PER_SEC);
//    printf("FormQ Elapsed Time: %20.10lf\n",dFormQTime);

//    std::cout << "Q:" << std::endl;
//    Q.print(std::cout);

    // form product of N22(inverse) and n2; store in NIC
//    clock_t FormNIC1 = clock();
    NIC = prod(N22, n2);
//    clock_t FormNIC2 = clock();
//    double dFormNICTime = ((FormNIC2-FormNIC1)/(double)CLOCKS_PER_SEC);
//    printf("FormNIC Elapsed Time: %20.10lf\n",dFormNICTime);

//    std::cout << "NIC:" << std::endl << NIC << std::endl;

    // accumulate -R directly into reduced normal equations
//  clock_t AccumIntoNormals1 = clock();
//  m_Normals -= prod(N12,Q);
//  printf("starting AmultAdd_CNZRows\n");
//    AmultAdd_CNZRows(-1.0, N12, Q, m_Normals);
    AmultAdd_CNZRows_CHOLMOD(-1.0, N12, Q);
//  clock_t AccumIntoNormals2 = clock();
//  double dAccumIntoNormalsTime = ((AccumIntoNormals2-AccumIntoNormals1)/(double)CLOCKS_PER_SEC);
//  printf("Accum Into Normals Elapsed Time: %20.10lf\n",dAccumIntoNormalsTime);

    // accumulate -nj
//    clock_t Accum_nj_1 = clock();
//    m_nj -= prod(trans(Q),n2);
    transA_NZ_multAdd_CHOLMOD(-1.0, Q, n2, m_nj);
//    clock_t Accum_nj_2 = clock();
//    double dAccumnjTime = ((Accum_nj_2-Accum_nj_1)/(double)CLOCKS_PER_SEC);
//    printf("Accum nj Elapsed Time: %20.10lf\n",dAccumnjTime);

//std::cout << "nj" << std::endl << m_nj << std::endl;

    return true;
  }


  /**
   * Apply weighting for spacecraft position, velocity, acceleration and camera angles, angular
   * velocities, angular accelerations if so stipulated (legalese).
   */
  bool BundleAdjust::formNormals3_CHOLMOD(compressed_vector<double> &n1,
                                  vector<double> &nj) {

    m_bundleResults.resetNumberConstrainedImageParameters();

    int n = 0;

    for (int i = 0; i < m_SparseNormals.size(); i++) {
      boost::numeric::ublas::matrix<double> *diagonalBlock = m_SparseNormals.getBlock(i, i);
      if ( !diagonalBlock )
        continue;

      if (m_bundleSettings->solveTargetBody() && i == 0) {
        m_bundleResults.resetNumberConstrainedTargetParameters();

        // get parameter weights for target body
        vector<double> weights = m_bundleTargetBody->parameterWeights();
        vector<double> corrections = m_bundleTargetBody->parameterCorrections();

        int blockSize = diagonalBlock->size1();
        for (int j = 0; j < blockSize; j++) {
          if (weights[j] > 0.0) {
            (*diagonalBlock)(j,j) += weights[j];
            m_nj[n] -= weights[j] * corrections(j);
            m_bundleResults.incrementNumberConstrainedTargetParameters(1);
          }
          n++;
        }
      }
      else {
        BundleObservation *observation;

        // get parameter weights for this observation
        if (m_bundleSettings->solveTargetBody())
          observation = m_bundleObservations.at(i-1);
        else
          observation = m_bundleObservations.at(i);

        boost::numeric::ublas::vector< double > weights = observation->parameterWeights();
        boost::numeric::ublas::vector< double > corrections = observation->parameterCorrections();

        int blockSize = diagonalBlock->size1();
        for (int j = 0; j < blockSize; j++) {
          if (weights(j) > 0.0) {
            (*diagonalBlock)(j,j) += weights(j);
            m_nj[n] -= weights(j) * corrections(j);
            m_bundleResults.incrementNumberConstrainedImageParameters(1);
          }
          n++;
        }
      }
    }

    // add n1 to nj
    m_nj += n1;

//    std::cout << "m_nj" << std::endl;
//    for (int i = 0; i < m_nj.size(); i++)
//      printf("%lf\n",m_nj(i));
//    std::cout << endl;

    return true;
  }


  /**
   * Forming the least-squares normal equations matrix via specialK.
   */
  bool BundleAdjust::formNormalEquations_SPECIALK() {

//    if (m_pProgressBar != NULL)
//    {
//      m_pProgressBar->SetText("Forming Normal Equations...");
//      m_pProgressBar->SetMaximumSteps(m_pCnet->Size());
//      m_pProgressBar->CheckStatus();
//    }

    bool bStatus = false;
/*
    m_bundleResults.setNumberObservations(0); // ??? necessary???
    m_bundleResults.resetNumberConstrainedPointParameters();

    static boost::numeric::ublas::matrix<double> coeff_image;
    static boost::numeric::ublas::matrix<double> coeff_point3D(2, 3);
    static boost::numeric::ublas::vector<double> coeff_RHS(2);
    static boost::numeric::ublas::symmetric_matrix<double, upper> N22(3);     // 3x3 upper triangular
    static boost::numeric::ublas::matrix< double> N12(m_nRank, 3);            // image parameters x 3 (should this be compressed? We only make one, so probably not)
    static boost::numeric::ublas::vector<double> n2(3);                       // 3x1 vector
    boost::numeric::ublas::compressed_vector<double> n1(m_nRank);             // image parameters x 1

    m_nj.resize(m_nRank);

    coeff_image.resize(2, m_nNumImagePartials);
    N12.resize(m_nRank, 3);

    // clear N12, n1, and nj
    N12.clear();
    n1.clear();
    m_nj.clear();

    // clear static matrices
    coeff_point3D.clear();
    coeff_RHS.clear();
    N22.clear();
    n2.clear();

    // loop over 3D points
    int nGood3DPoints = 0;
    int nRejected3DPoints = 0;
    int nPointIndex = 0;
    int nImageIndex;
    int n3DPoints = m_bundleControlPoints.size();

//    char buf[1056];
//    sprintf(buf,"\n\t                      Points:%10d\n", n3DPoints);
//    m_fp_log << buf;
//    printf("%s", buf);

    for (int i = 0; i < n3DPoints; i++) {
      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if ( point->isRejected() ) {
        nRejected3DPoints++;
//      sprintf(buf, "\tRejected %s - 3D Point %d of %d RejLimit = %lf\n", point.Id().toLatin1().data(),nPointIndex,n3DPoints,m_bundleResults.rejectionLimit());
//      m_fp_log << buf;

        nPointIndex++;
        continue;
      }

      // send notification to UI indicating index of point currently being processed
      // m_nProcessedPoint = i+1;
      // UI.Notify(BundleEvent.NEW_POINT_PROCESSED);

      if ( i != 0 ) {
          N22.clear();
          N12.clear();
          n2.clear();
      }

      // loop over measures for this point
      int nMeasures = point->size();
      for (int j = 0; j < nMeasures; j++) {
        BundleMeasure *measure = point->at(j);

        // flagged as "JigsawFail" implies this measure has been rejected
        // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
        if ( measure->isRejected() ) {
          continue;
        }

        // printf("   Processing Measure %d of %d\n", j,nMeasures);

        bStatus = computePartials_DC(coeff_image, coeff_point3D, coeff_RHS, *measure, *point);

//        std::cout << coeff_image << std::endl;
//        std::cout << coeff_point3D << std::endl;
//        std::cout << coeff_RHS << std::endl;

        if ( !bStatus ) {
          continue;     // this measure should be flagged as rejected
        }

        // update number of observations
        int numObs = m_bundleResults.numberObservations();
        m_bundleResults.setNumberObservations(numObs + 2);


        formNormals1_SPECIALK(N22, N12, n1, n2, coeff_image, coeff_point3D,
                             coeff_RHS, measure->observationIndex());
      } // end loop over this points measures

      formNormals2_SPECIALK(N22, N12, n2, m_nj, nPointIndex, i);
      nPointIndex++;

//    if (m_pProgressBar != NULL)
//      m_pProgressBar->CheckStatus();

      nGood3DPoints++;

    } // end loop over 3D points

    // finally, form the reduced normal equations
    formNormals3_SPECIALK(n1, m_nj);

//  std::cout << m_Normals << std::endl;
//  m_SparseNormals.print();

    // update number of unknown parameters
    m_bundleResults.setNumberUnknownParameters(m_nRank + 3 * nGood3DPoints);
*/
    return bStatus;
  }


  /**
   * Forming first set of auxiliary matrices for normal equations matrix via specialK.
   */
  bool BundleAdjust::formNormals1_SPECIALK(symmetric_matrix<double, upper>&N22,
                                           matrix<double> &N12,
                                           compressed_vector<double> &n1,
                                           vector<double> &n2,
                                           matrix<double> &coeff_image,
                                           matrix<double> &coeff_point3D,
                                           vector<double> &coeff_RHS,
                                           int nImageIndex) {
/*
    int i, j;

    static vector<double> n1_image(m_nNumImagePartials);
    n1_image.clear();

    // form N11 (normals for photo)
    static symmetric_matrix<double, upper> N11(m_nNumImagePartials);
    N11.clear();

//    std::cout << "image" << std::endl << coeff_image << std::endl;
//    std::cout << "point" << std::endl << coeff_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_RHS << std::endl;

    N11 = prod(trans(coeff_image), coeff_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    // insert into reduced normal equations
    int t = m_nNumImagePartials * nImageIndex;
    for (i = 0; i < m_nNumImagePartials; i++) {
      for (j = i; j < m_nNumImagePartials; j++) {
        m_Normals(i + t, j + t) += N11(i, j);
      }
    }

    // form N12_Image
    static matrix<double> N12_Image(m_nNumImagePartials, 3);
    N12_Image.clear();

    N12_Image = prod(trans(coeff_image), coeff_point3D);


//    printf("N12 before insert\n");
//    std::cout << N12 << std::endl;

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
    for (i = 0; i < m_nNumImagePartials; i++) {
      for (j = 0; j < 3; j++) {
        N12(i + t, j) += N12_Image(i, j);
      }
    }

//    printf("N12\n");
//    std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(coeff_image), coeff_RHS);

//    std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < m_nNumImagePartials; i++) {
      n1(i + t) += n1_image(i);
    }

    // form N22
    N22 += prod(trans(coeff_point3D), coeff_point3D);

//    std::cout << "N22" << std::endl << N22 << std::endl;

    // form n2
    n2 += prod(trans(coeff_point3D), coeff_RHS);

//    std::cout << "n2" << std::endl << n2 << std::endl;
*/
    return true;
  }


  /**
   * Forming second set of auxiliary matrices for normal equations matrix via specialK.
   */
  bool BundleAdjust::formNormals2_SPECIALK(symmetric_matrix<double, upper>&N22,
                                           matrix<double> &N12,
                                           vector<double> &n2,
                                           vector<double> &nj,
                                           int nPointIndex,
                                           int i) {
/*
      bounded_vector<double, 3> &NIC = m_NICs[nPointIndex];
      compressed_matrix<double> &Q = m_Qs_SPECIALK[nPointIndex];

      NIC.clear();
      Q.clear();

      // weighting of 3D point parameters
  //    const ControlPoint *point = m_pCnet->GetPoint(i);
      ControlPoint *point = m_pCnet->GetPoint(i); //TODO: what about this const business, regarding SetAdjustedSurfacePoint below???

      bounded_vector<double, 3> &weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3> &corrections = m_Point_Corrections[nPointIndex];

  //    std::cout << "Point" << point->GetId() << "weights" << std::endl << weights << std::endl;

  //    std::cout << "corrections" << std::endl << corrections << std::endl;

      if (weights[0] > 0.0) {
        N22(0, 0) += weights[0];
        n2(0) += (-weights[0] * corrections(0));
        m_bundleResults.incrementNumberConstrainedPointParameters(1);
      }

      if (weights[1] > 0.0) {
        N22(1, 1) += weights[1];
        n2(1) += (-weights[1] * corrections(1));
        m_bundleResults.incrementNumberConstrainedPointParameters(1);
      }

      if (weights[2] > 0.0) {
        N22(2, 2) += weights[2];
        n2(2) += (-weights[2] * corrections(2));
        m_bundleResults.incrementNumberConstrainedPointParameters(1);
      }

  //    std::cout << "N22 before inverse" << std::endl << N22 << std::endl;
      // invert N22
      Invert_3x3(N22);
  //    std::cout << "N22 after inverse" << std::endl << N22 << std::endl;

      // save upper triangular covariance matrix for error propagation
      // TODO:  The following method does not exist yet (08-13-2010)
      SurfacePoint SurfacePoint = point->getAdjustedSurfacePoint();
      SurfacePoint.SetSphericalMatrix(N22);
      point->setAdjustedSurfacePoint(SurfacePoint);
  //    point->getAdjustedSurfacePoint().setSphericalMatrix(N22);

  // TODO  Test to make sure spherical matrix is truly set.  Try to read it back

      // Next 3 lines obsolete because only the matrix is stored and sigmas are calculated from it.
  //    point->SetSigmaLatitude(N22(0,0));
  //    point->SetSigmaLongitude(N22(1,1));
  //    point->SetSigmaRadius(N22(2,2));

      // form Q (this is N22{-1} * N12{T})
  //    clock_t FormQ1 = clock();
      Q = prod(N22, trans(N12));
  //    clock_t FormQ2 = clock();
  //    double dFormQTime = ((FormQ2-FormQ1)/(double)CLOCKS_PER_SEC);
  //    printf("FormQ Elapsed Time: %20.10lf\n",dFormQTime);

  //    std::cout << "Q: " << Q << std::endl;

      // form product of N22(inverse) and n2; store in NIC
  //    clock_t FormNIC1 = clock();
      NIC = prod(N22, n2);
  //    clock_t FormNIC2 = clock();
  //    double dFormNICTime = ((FormNIC2-FormNIC1)/(double)CLOCKS_PER_SEC);
  //    printf("FormNIC Elapsed Time: %20.10lf\n",dFormNICTime);

      // accumulate -R directly into reduced normal equations
  //  clock_t AccumIntoNormals1 = clock();
  //  m_Normals -= prod(N12,Q);
  //  printf("starting AmultAdd_CNZRows\n");
      AmultAdd_CNZRows_SPECIALK(-1.0, N12, Q, m_Normals);
  //  clock_t AccumIntoNormals2 = clock();
  //  double dAccumIntoNormalsTime = ((AccumIntoNormals2-AccumIntoNormals1)/(double)CLOCKS_PER_SEC);
  //  printf("Accum Into Normals Elapsed Time: %20.10lf\n",dAccumIntoNormalsTime);

      // accumulate -nj
  //    clock_t Accum_nj_1 = clock();
  //    m_nj -= prod(trans(Q),n2);
      transA_NZ_multAdd_SPECIALK(-1.0, Q, n2, m_nj);
  //    clock_t Accum_nj_2 = clock();
  //    double dAccumnjTime = ((Accum_nj_2-Accum_nj_1)/(double)CLOCKS_PER_SEC);
  //    printf("Accum nj Elapsed Time: %20.10lf\n",dAccumnjTime);
*/
      return true;
    }


  /**
   * apply weighting for spacecraft position, velocity, acceleration and camera angles, angular
   * velocities, angular accelerations if so stipulated (legalese).
   */
  bool BundleAdjust::formNormals3_SPECIALK(compressed_vector<double> &n1,
                                           vector< double > &nj) {
/*
  //  std::cout << m_dImageParameterWeights << std::endl;

    m_bundleResults.resetNumberConstrainedImageParameters();

    int n = 0;
    do {
      for (int j = 0; j < m_nNumImagePartials; j++) {
        if (m_dImageParameterWeights[j] > 0.0) {
          m_Normals(n, n) += m_dImageParameterWeights[j];
          m_nj[n] -= m_dImageParameterWeights[j] * m_imageCorrections[n];
          m_bundleResults.incrementNumberConstrainedImageParameters(1);
        }

        n++;
      }

    }
    while (n < m_nRank);

    // add n1 to nj
    m_nj += n1;
*/
    return true;
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   */
  void BundleAdjust::product_AV(double alpha, bounded_vector<double,3> &v2,
                                SparseBlockRowMatrix &Q,
                                vector<double> &v1) {

    QMapIterator< int, boost::numeric::ublas::matrix<double> * > iQ(Q);

    // subrange start, end
    int srStart, srEnd;

    while ( iQ.hasNext() ) {
      iQ.next();

      int ncol = iQ.key();

      srStart = m_SparseNormals.getLeadingColumnsForBlock(ncol);
      srEnd = srStart + iQ.value()->size2();

      v2 += alpha * prod(*(iQ.value()),subrange(v1,srStart,srEnd));
    }
  }


  /**
   * C = A x B(transpose) where
   *     A is a boost matrix
   *     B is a SparseBlockColumn matrix
   *     C is a SparseBlockRowMatrix
   *     each block of B and C are boost matrices
   */
  bool BundleAdjust::product_ATransB(symmetric_matrix <double,upper> &N22,
                                     SparseBlockColumnMatrix &N12,
                                     SparseBlockRowMatrix &Q) {

    QMapIterator< int, boost::numeric::ublas::matrix<double> * > iN12(N12);

    while ( iN12.hasNext() ) {
      iN12.next();

      int ncol = iN12.key();

      // insert submatrix in Q at block "ncol"
      Q.insertMatrixBlock(ncol, 3, iN12.value()->size1());

      *(Q[ncol]) = prod(N22,trans(*(iN12.value())));
    }

    return true;
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   *
   * NOTE: A = N12, B = Q
   */
  void BundleAdjust::AmultAdd_CNZRows_CHOLMOD(double alpha,
                                              SparseBlockColumnMatrix &N12,
                                              SparseBlockRowMatrix &Q) {

    if (alpha == 0.0) {
      return;
    }

    // iterators for N12 and Q
    QMapIterator<int, boost::numeric::ublas::matrix<double>*> iN12(N12);
    QMapIterator<int, boost::numeric::ublas::matrix<double>*> iQ(Q);

    // now multiply blocks and subtract from m_SparseNormals
    while ( iN12.hasNext() ) {
      iN12.next();

      int nrow = iN12.key();
      boost::numeric::ublas::matrix<double> *in12 = iN12.value();

      while ( iQ.hasNext() ) {
        iQ.next();

        int ncol = iQ.key();

        if ( nrow > ncol )
          continue;

        boost::numeric::ublas::matrix<double> *iq = iQ.value();

        // insert submatrix at column, row
        m_SparseNormals.insertMatrixBlock(ncol, nrow,
            in12->size1(), iq->size2());

        (*(*m_SparseNormals[ncol])[nrow]) -= prod(*in12,*iq);
      }
      iQ.toFront();
    }
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   *
   */
  void BundleAdjust::AmultAdd_CNZRows_SPECIALK(double alpha, 
                                               matrix<double> &A,
                                               compressed_matrix<double> &B,
                                               symmetric_matrix<double, upper, column_major> &C) {
/*
    if (alpha == 0.0) {
      return;
    }

    register int i, j, k, ii, jj;
    double d;

    int nColsA = A.size2();

    // create array of non-zero indices of matrix B
    std::vector<int> nz(B.nnz() / B.size1());

    // iterators for B
    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = B.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();
    for (i = 0; i < nzlength; ++i) {
      ii = nz[i];
      for (j = i; j < nzlength; ++j) {
        jj = nz[j];
        d = 0.0;

        for (k = 0; k < nColsA; ++k) {
          d += A(ii, k) * B(k, jj);
        }

        C(ii, jj) += alpha * d;
      }
    }
*/
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   */
  void BundleAdjust::transA_NZ_multAdd_CHOLMOD(double alpha,
                                               SparseBlockRowMatrix &Q,
                                               vector<double> &n2,
                                               vector<double> &m_nj) {

    if (alpha == 0.0)
      return;

    int nTargetParameters = m_bundleSettings->numberTargetBodyParameters();

    QMapIterator<int, boost::numeric::ublas::matrix<double>*> iQ(Q);

    while ( iQ.hasNext() ) {
      iQ.next();

      int nrow = iQ.key();
      boost::numeric::ublas::matrix<double> *m = iQ.value();

      boost::numeric::ublas::vector<double> v = prod(trans(*m),n2);

      //testing - should ask m_bundleObservations for this???
//      int n = Q.getLeadingColumnsForBlock(nrow);
      int t=0;
      for (int a = 0; a < nrow; a++) {
        if (nTargetParameters > 0 && a == 0)
          t += nTargetParameters;
        else {
          if (nTargetParameters > 0 ) {
            BundleObservation *observation = m_bundleObservations.at(a-1);
            t += observation->numberParameters();
          }
          else {
            BundleObservation *observation = m_bundleObservations.at(a);
           t += observation->numberParameters();
          }
        }
      }

      for (unsigned i = 0; i < v.size(); i++) {
        m_nj(t+i) += alpha*v(i);
      }
    }
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   */
  void BundleAdjust::transA_NZ_multAdd_SPECIALK(double alpha, compressed_matrix<double> &A,
                                                vector<double> &B,
                                                vector<double> &C) {
/*
    if (alpha == 0.0)
      return;

    register int i, j, ii;
    double d;

    int nRowsA = A.size1();

    // create array of non-zero indices of matrix A
    std::vector<int> nz(A.nnz() / A.size1());

    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = A.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();
    for (i = 0; i < nzlength; ++i) {
      ii = nz[i];
      d = 0.0;

      for (j = 0; j < nRowsA; ++j) {
        d += A(j, ii) * B(j);
      }

      C(ii) += alpha * d;
    }
*/
  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   */
  void BundleAdjust::AmulttransBNZ(matrix<double> &A,
                                   compressed_matrix<double> &B,
                                   matrix<double> &C,
                                   double alpha) {

    if ( alpha == 0.0 ) {
      return;
    }

    register int i,j,k,kk;
    double d;

    int nRowsB = B.size1();

    // create array of non-zero indices of matrix B
    std::vector<int> nz(B.nnz() / nRowsB);

    typedef boost::numeric::ublas::compressed_matrix<double>::iterator1 it_row;
    typedef boost::numeric::ublas::compressed_matrix<double>::iterator2 it_col;

    it_row itr = B.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();

    int nRowsA = A.size1();
    int nColsC = C.size2();

    for (i = 0; i < nRowsA; ++i) {
      for (j = 0; j < nColsC; ++j) {
        d = 0;

        for (k = 0; k < nzlength; ++k) {
          kk = nz[k];
          d += A(i, kk) * B(j, kk);
        }

        C(i,j) += alpha * d;
      }
    }

  }


  /**
   * Dedicated matrix multiplication method.
   *
   * TODO: Define.
   */
  void BundleAdjust::ANZmultAdd(compressed_matrix<double> &A,
                                symmetric_matrix<double, upper, column_major> &B,
                                matrix<double> &C,
                                double alpha) {

    if ( alpha == 0.0 )
      return;

    register int i,j,k,kk;
    double d;

    int nRowsA = A.size1();

    // create array of non-zero indices of matrix A
    std::vector<int> nz(A.nnz() /nRowsA);

    typedef boost::numeric::ublas::compressed_matrix<double>::iterator1 it_row;
    typedef boost::numeric::ublas::compressed_matrix<double>::iterator2 it_col;

    it_row itr = A.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();

    int nColsC = C.size2();
    for (i = 0; i < nRowsA; ++i) {
      for (j = 0; j < nColsC; ++j) {
        d = 0;

        for (k = 0; k < nzlength; ++k) {
          kk = nz[k];
          d += A(i, kk) * B(kk, j);
        }

        C(i, j) += alpha * d;
      }
    }

  }


  /**
   * Solution with CHOLMOD.
   *
   */
  bool BundleAdjust::solveSystem_CHOLMOD() {

    // load cholmod triplet
    if ( !loadCholmodTriplet() ) {
      QString msg = "CHOLMOD: Failed to load Triplet matrix";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // convert triplet to sparse matrix
//      FILE * pFile1;
//      pFile1 = fopen ("//work//users//kedmundson//Normals.txt" , "w");
    m_N = cholmod_triplet_to_sparse(m_pTriplet,  m_pTriplet->nnz, &m_cm);
//      cholmod_write_sparse(pFile1, m_N, 0, 0, &m_cm);
//      fclose(pFile1);

    // analyze matrix
    //clock_t t1 = clock();
    m_L = cholmod_analyze(m_N, &m_cm); // should we analyze just 1st iteration?
    //clock_t t2 = clock();
    //double delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Analyze Elapsed Time: %20.10lf\n",delapsedtime);

    // create cholmod cholesky factor (LDLT?)
    //t1 = clock();
    cholmod_factorize(m_N, m_L, &m_cm);
    //t2 = clock();
    //delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Factorize Elapsed Time: %20.10lf\n",delapsedtime);

    // check for "matrix not positive definite" error
    if (m_cm.status == CHOLMOD_NOT_POSDEF) {
      QString msg = QString("matrix NOT positive-definite: failure at column %1").arg(m_L->minor);
//    throw IException(IException::User, msg, _FILEINFO_);
      error(msg);
      emit(finished());
      return false;
    }

//      FILE * pFile2;
//      pFile2 = fopen ("//work1//kedmundson//factor.txt" , "w");
//      cholmod_sparse* factor   = cholmod_factor_to_sparse(L, &c);
//      cholmod_write_sparse(pFile2, factor, 0, 0, &c);
//      fclose(pFile2);

    // cholmod solution and right-hand side vectors
    cholmod_dense *x, *b;

    // initialize right-hand side vector
    b = cholmod_zeros(m_N->nrow, 1, m_N->xtype, &m_cm);

//std::cout << "nj" << std::endl << m_nj << std::endl;

    // copy right-hand side vector into b
    double *px = (double*)b->x;
    for (int i = 0; i < m_nRank; i++) {
      px[i] = m_nj[i];
    }

//      FILE * pFile3;
//      pFile3 = fopen ("//work1//kedmundson//rhs.txt" , "w");
//      cholmod_write_dense(pFile3, b, 0, &c);
//      fclose(pFile3);

    // cholmod solve
    //t1 = clock();
    x = cholmod_solve(CHOLMOD_A, m_L, b, &m_cm);
    //t2 = clock();
    //delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Solution Elapsed Time: %20.10lf\n",delapsedtime);

//      FILE * pFile4;
//      pFile4 = fopen ("//work//users//kedmundson//solution.txt" , "w");
//      cholmod_write_dense(pFile4, x, 0, &m_cm);
//      fclose(pFile4);

    // copy solution vector x out into m_imageSolution
    double *sx = (double*)x->x;
    for (int i = 0; i < m_nRank; i++) {
      m_imageSolution[i] = sx[i];
    }

//std::cout << "solution vector" << std::endl << m_imageSolution << std::endl;

    // free cholmod structures
    cholmod_free_sparse(&m_N, &m_cm); // necessary?
    cholmod_free_dense(&b, &m_cm);
    cholmod_free_dense(&x, &m_cm);

    return true;
  }


  /**
   * Load sparse normal equations matrix into CHOLMOD triplet.
   *
   */
  bool BundleAdjust::loadCholmodTriplet() {
    double d;

//    std::cout << "Sparse Normals" << std::endl;
//    qDebug() << m_SparseNormals;
//    m_SparseNormals.printClean(std::cout);

    if ( m_nIteration == 1 ) {
      int nelements = m_SparseNormals.numberOfElements();
      //printf("Matrix rank: %d # of Triplet elements: %d", m_nRank,nelements);
      m_pTriplet = cholmod_allocate_triplet(m_nRank, m_nRank, nelements,
          -1, CHOLMOD_REAL, &m_cm);

      if ( !m_pTriplet ) {
        printf("Triplet allocation failure");
        return false;
      }

      m_pTriplet->nnz = 0;
    }

    int* Ti = (int*) m_pTriplet->i;
    int* Tj = (int*) m_pTriplet->j;
    double* v = (double*)m_pTriplet->x;

    int nentries = 0;

    int nblockcolumns = m_SparseNormals.size();
    for (int ncol = 0; ncol < nblockcolumns; ncol++) {

      SparseBlockColumnMatrix* sbc = m_SparseNormals[ncol];

      if ( !sbc ) {
        printf("SparseBlockColumnMatrix retrieval failure at column %d", ncol);
        return false;
      }

      int nLeadingColumns = m_SparseNormals.getLeadingColumnsForBlock(ncol);

      QMapIterator< int, boost::numeric::ublas::matrix<double> * > it(*sbc);

      while ( it.hasNext() ) {
        it.next();

        int nrow = it.key();

        int nLeadingRows = m_SparseNormals.getLeadingRowsForBlock(nrow);

        boost::numeric::ublas::matrix<double> *m = it.value();
        if ( !m ) {
          printf("matrix block retrieval failure at column %d, row %d", ncol,nrow);
          printf("Total # of block columns: %d", nblockcolumns);
          printf("Total # of blocks: %d", m_SparseNormals.numberOfBlocks());
          return false;
        }

        if ( ncol == nrow )  {   // diagonal block (upper-triangular)
          for (unsigned ii = 0; ii < m->size1(); ii++) {
            for (unsigned jj = ii; jj < m->size2(); jj++) {
              d = m->at_element(ii,jj);
              int ncolindex = jj+nLeadingColumns;
              int nrowindex = ii+nLeadingRows;

              if ( m_nIteration == 1 ) {
                Ti[nentries] = ncolindex;
                Tj[nentries] = nrowindex;
                m_pTriplet->nnz++;
              }

//              printf("UT - writing to row: %d column: %d\n", ncolindex, nrowindex);
              v[nentries] = d;

              nentries++;
            }
          }
//          printf("\n");
        }
        else {                // off-diagonal block (square)
          for (unsigned ii = 0; ii < m->size1(); ii++) {
            for (unsigned jj = 0; jj < m->size2(); jj++) {
              d = m->at_element(ii,jj);
              int ncolindex = jj+nLeadingColumns;
              int nrowindex = ii+nLeadingRows;

              if ( m_nIteration ==1 ) {
                Ti[nentries] = nrowindex;
                Tj[nentries] = ncolindex;
                m_pTriplet->nnz++;
              }

              v[nentries] = d;

//              printf("     writing to row: %d column: %d\n", ncolindex, nrowindex);

              nentries++;
            }
          }
//          printf("\n");
        }
      }
    }

    return true;
  }


  /**
   * Solution with specialK (dense).
   *
   */
  bool BundleAdjust::solveSystem_SPECIALK() {
/*
    // decomposition (this is UTDU - need to test row vs column major
    // storage and access, and use of matrix iterators)
//    clock_t CholeskyUtDUclock1 = clock();
//    printf("Starting Cholesky\n");
//    cholesky_decompose(m_Normals);
    if ( !CholeskyUT_NOSQR() )
      return false;
//    clock_t CholeskyUtDUclock2 = clock();
//    double dCholeskyTime = ((CholeskyUtDUclock2-CholeskyUtDUclock1)/(double)CLOCKS_PER_SEC);
//    printf("Cholesky Elapsed Time: %20.10lf\n",dCholeskyTime);

//    cholesky_solve(m_Normals, m_nj);

    // solve via back-substitution
//    clock_t BackSubclock1 = clock();
//    printf("Starting Back Substitution\n");
//    if (!CholeskyUT_NOSQR_BackSub(m_Normals, m_imageSolution, m_nj))
//      return false;
//    clock_t BackSubclock2 = clock();
//    double dBackSubTime = ((BackSubclock1-BackSubclock2)/(double)CLOCKS_PER_SEC);
//    printf("Back Substitution Elapsed Time: %20.10lf\n",dBackSubTime);

//    std::cout << m_imageSolution << std::endl;
*/
    return true;
  }


  /**
   * Upper triangular, square-root free cholesky.
   *
   */
  bool BundleAdjust::CholeskyUT_NOSQR() {
    int i, j, k;
    double sum, divisor;
    double den;
    double d1, d2;

    int nRows = m_Normals.size1();
//    comment here
//        for (i = 0; i < nRows; i++) {
//          for (j = i; j < nRows; j++) {
//            printf("%lf ",m_Normals(i,j));
//          }
//          printf("\n");
//        }
//    comment here
    for (i = 0; i < nRows; i++) {
//    printf("Cholesky Row %d of %d\n",i+1,nRows);
      sum = 0.0;

      for (j = 0; j < i; j++) {
//      sum += m_Normals(j,i-j) * m_Normals(j,i-j) * m_Normals(j,0);   // old way

        d1 = m_Normals(j, i);
        if (d1 == 0.0)
          continue;
        sum += d1 * d1 * m_Normals(j, j);  // new way
      }

//    m_Normals(i,0) -= sum;                                           // old
      m_Normals(i, i) -= sum;                                          // new

      // check for divide by 0
//    den = m_Normals(i,0);                                            // old
      den = m_Normals(i, i);                                           // new
      if (fabs(den) < 1e-100) {
        return false;
      }

      divisor = 1.0 / den;

      for (j = (i + 1); j < nRows; j++) {
        sum = 0.0;

        for (k = 0; k < i; k++) {
//        sum += m_Normals(k,j-k) * m_Normals(k,i-k) * m_Normals(k,0); // old

          d1 = m_Normals(k, j);
          if (d1 == 0.0)
            continue;

          d2 = m_Normals(k, i);
          if (d2 == 0.0)
            continue;

          sum += d1 * d2 * m_Normals(k, k); // new
        }

//      m_Normals(i,j-i) = (m_Normals(i,j-i) - sum) * divisor;         // old
        m_Normals(i, j)   = (m_Normals(i, j) - sum) * divisor;         // new
      }

      // decompose right-hand side
      sum = 0.0;
      for (k = 0; k < i; k++) {
        d1 = m_nj(k);
        if (d1 == 0.0)
          continue;

        d2 = m_Normals(k, i);
        if (d2 == 0.0)
          continue;

        sum += d1 * d2 * m_Normals(k, k);
      }

      m_nj(i) = (m_nj(i) - sum) * divisor;
    }

    return true;
  }


  /**
   * Backsubstitution for above square-root free cholesky method.
   *
   */
  bool BundleAdjust::CholeskyUT_NOSQR_BackSub(symmetric_matrix<double, upper, column_major> &m,
                                              vector<double> &s,
                                              vector<double> &rhs) {
    int i, j;
    double sum;
    double d1, d2;

    int nRows = m.size1();

    s(nRows - 1) = rhs(nRows - 1);

    for (i = nRows - 2; i >= 0; i--) {
      sum = 0.0;

      for (j = i + 1; j < nRows; j++) {
        d1 = m(i, j);
        if (d1 == 0.0) {
          continue;
        }

        d2 = s(j);
        if (d2 == 0.0) {
          continue;
        }

        sum += d1 * d2;
      }

      s(i) = rhs(i) - sum;
    }

//    std::cout << s << std::endl;

    return true;
  }


  /**
   * Compute inverse of normal equations matrix for above square-root free cholesky method.
   *
   */
  bool BundleAdjust::CholeskyUT_NOSQR_Inverse() {
    int i, j, k;
    double div, sum;
    double colk, tmpkj, tmpkk;

    // create temporary copy, inverse will be stored in m_Normals
    boost::numeric::ublas::symmetric_matrix< double, upper, column_major > tmp = m_Normals;
//    symmetric_matrix<double,lower> tmp = m_Normals;

    // solution vector
    boost::numeric::ublas::vector<double> s(m_nRank);

    // initialize column vector
    boost::numeric::ublas::vector<double> column(m_nRank);
    column.clear();
    column(0) = 1.0;

    for (i = 0; i < m_nRank; i++) {
      // set current column of identity
      column.clear();
      column(i) = 1.0;

      // factorize current column of identity matrix
      for (j = 0; j < m_nRank; j++) {
        div = 1.0 / tmp(j, j);
        sum = 0.0;

        for (k = 0; k < j; k++) {
          colk = column(k);
          tmpkj = tmp(k, j);
          tmpkk = tmp(k, k);

          if (colk == 0.0 || tmpkj == 0.0 || tmpkk == 0.0)
            continue;

          sum += colk * tmpkj * tmpkk;
        }

        column(j) = (column(j) - sum) * div;
      }

      // back-substitution
      if (!CholeskyUT_NOSQR_BackSub(tmp, s, column))
        return false;

      // store solution in corresponding column of inverse (replacing column in
      // m_Normals)
      for (j = 0; j <= i; j++) {
        m_Normals(j, i) = s(j);
      }
    }

    return true;
  }


  /**
   * Compute inverse of normal equations matrix for CHOLMOD.
   *
   */
  bool BundleAdjust::cholmod_Inverse() {
    int i, j;

    // allocate matrix inverse
    m_Normals.resize(m_nRank);

    cholmod_dense *x;        // solution vector
    cholmod_dense *b;        // right-hand side (column vectors of identity)

    b = cholmod_zeros ( m_nRank, 1, CHOLMOD_REAL, &m_cm ) ;
    double* pb = (double*)b->x;

    double* px = NULL;

    for (i = 0; i < m_nRank; i++) {
      if ( i > 0 ) {
        pb[i-1] = 0.0;
      }
      pb[i] = 1.0;

      x = cholmod_solve ( CHOLMOD_A, m_L, b, &m_cm ) ;
      px = (double*)x->x;

      // store solution in corresponding column of inverse (replacing column in
      // m_Normals)
      for (j = 0; j <= i; j++) {
        m_Normals(j, i) = px[j];
      }

      cholmod_free_dense(&x,&m_cm);
    }

    //
    //std::cout << m_Normals;
    //

    cholmod_free_dense(&b,&m_cm);

    return true;
  }


  /**
   * Dedicated quick inverse of 3x3 matrix
   *
   * TODO: belongs in matrix class or wrapper.
   */
  bool BundleAdjust::Invert_3x3(symmetric_matrix<double, upper> &m) {
    double det;
    double den;

    boost::numeric::ublas::symmetric_matrix< double, upper > c = m;

    den = m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
          - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
          + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

    // check for divide by zero
    if (fabs(den) < 1.0e-100) {
      return false;
    }

    det = 1.0 / den;

    m(0, 0) = (c(1, 1) * c(2, 2) - c(1, 2) * c(2, 1)) * det;
    m(0, 1) = (c(0, 2) * c(2, 1) - c(0, 1) * c(2, 2)) * det;
    m(0, 2) = (c(0, 1) * c(1, 2) - c(0, 2) * c(1, 1)) * det;
    m(1, 1) = (c(0, 0) * c(2, 2) - c(0, 2) * c(2, 0)) * det;
    m(1, 2) = (c(0, 2) * c(1, 0) - c(0, 0) * c(1, 2)) * det;
    m(2, 2) = (c(0, 0) * c(1, 1) - c(0, 1) * c(1, 0)) * det;

    return true;
  }


  /**
   * compute partials for measure
   */
  bool BundleAdjust::computePartials_DC(matrix<double> &coeff_target, 
                                        matrix<double> &coeff_image,
                                        matrix<double> &coeff_point3D,
                                        vector<double> &coeff_RHS,
                                        BundleMeasure &measure,
                                        BundleControlPoint &point) {

    // additional vectors
    std::vector<double> d_lookB_WRT_LAT;
    std::vector<double> d_lookB_WRT_LON;
    std::vector<double> d_lookB_WRT_RAD;

    Camera *pCamera = NULL;

    double dMeasuredx, dComputedx, dMeasuredy, dComputedy;
    double deltax, deltay;
    double dObservationSigma;
    double dObservationWeight;

    pCamera = measure.camera();

    // TODO - who do I get these from?
    // from this measures BundleObservation
    const BundleObservationSolveSettings *observationSolveSettings =
        measure.observationSolveSettings();
    BundleObservation *observation = measure.parentBundleObservation();

    int nImagePartials = observation->numberParameters();
    coeff_image.resize(2,nImagePartials);

    // clear partial derivative matrices and vectors
    if (m_bundleSettings->solveTargetBody())
      coeff_target.clear();

    coeff_image.clear();
    coeff_point3D.clear();
    coeff_RHS.clear();

    // no need to call SetImage for framing camera ( CameraType  = 0 )
    if (pCamera->GetCameraType() != 0) {
      // Set the Spice to the measured point
      // TODO - can we explain this better?
      pCamera->SetImage(measure.sample(), measure.line());
    }

    // REMOVE
    SurfacePoint surfacePoint = point.getAdjustedSurfacePoint();
    // REMOVE

    // Compute the look vector in instrument coordinates based on time of observation and apriori
    // lat/lon/radius
    if (!(pCamera->GroundMap()->GetXY(point.getAdjustedSurfacePoint(), &dComputedx, &dComputedy))) {
      QString msg = "Unable to map apriori surface point for measure ";
      msg += measure.cubeSerialNumber() + " on point " + point.getId() + " into focal plane";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // partials for fixed point w/r lat, long, radius in Body-Fixed
    d_lookB_WRT_LAT = pCamera->GroundMap()->PointPartial(point.getAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = pCamera->GroundMap()->PointPartial(point.getAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Longitude);
    d_lookB_WRT_RAD = pCamera->GroundMap()->PointPartial(point.getAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Radius);

//  std::cout << "d_lookB_WRT_LAT" << d_lookB_WRT_LAT << std::endl;
//  std::cout << "d_lookB_WRT_LON" << d_lookB_WRT_LON << std::endl;
//  std::cout << "d_lookB_WRT_RAD" << d_lookB_WRT_RAD << std::endl;

    int nIndex = 0;
    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePoleRA()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_RightAscension, 0,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePoleRAVelocity()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_RightAscension, 1,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePoleDec()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Declination, 0,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePoleDecVelocity()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Declination, 1,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePM()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Twist, 0,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleSettings->solvePMVelocity()) {
      pCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Twist, 1,
                                              &coeff_target(0, nIndex),
                                              &coeff_target(1, nIndex));
      nIndex++;
    }
      
    if (m_bundleSettings->solveTargetBody() && m_bundleTargetBody->solveMeanRadius()) {
      std::vector<double> d_lookB_WRT_MEANRADIUS =
          pCamera->GroundMap()->MeanRadiusPartial(surfacePoint, m_bundleTargetBody->meanRadius());

      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_MEANRADIUS, &coeff_target(0, nIndex),
                                         &coeff_target(1, nIndex));
      nIndex++;
    }

    if (m_bundleSettings->solveTargetBody() && m_bundleTargetBody->solveTriaxialRadii()) {
//      std::vector<Distance> triaxialRadii = m_bundleTargetBody->radii();

      std::vector<double> d_lookB_WRT_RADIUSA =
          pCamera->GroundMap()->EllipsoidPartial(surfacePoint, CameraGroundMap::WRT_MajorAxis);

      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RADIUSA, &coeff_target(0, nIndex),
                                         &coeff_target(1, nIndex));
      nIndex++;

      std::vector<double> d_lookB_WRT_RADIUSB =
          pCamera->GroundMap()->EllipsoidPartial(surfacePoint, CameraGroundMap::WRT_MinorAxis);

      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RADIUSB, &coeff_target(0, nIndex),
                                         &coeff_target(1, nIndex));
      nIndex++;

      std::vector<double> d_lookB_WRT_RADIUSC =
          pCamera->GroundMap()->EllipsoidPartial(surfacePoint, CameraGroundMap::WRT_PolarAxis);

      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RADIUSC, &coeff_target(0, nIndex),
                                         &coeff_target(1, nIndex));
      nIndex++;
    }

//  std::cout << coeff_target << std::endl;

    nIndex = 0;

    if (observationSolveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      //      SpicePosition* pInstPos = pCamera->instrumentPosition();

      int nCamPositionCoefficients =
          observationSolveSettings->numberCameraPositionCoefficientsSolved();

      // Add the partial for the x coordinate of the position (differentiating
      // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
      for (int icoef = 0; icoef < nCamPositionCoefficients; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

//      std::cout << coeff_image << std::endl;

      // Add the partial for the y coordinate of the position
      for (int icoef = 0; icoef < nCamPositionCoefficients; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for the z coordinate of the position
      for (int icoef = 0; icoef < nCamPositionCoefficients; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

    }

    if (observationSolveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      int nCamAngleCoefficients =
          observationSolveSettings->numberCameraAngleCoefficientsSolved();

      // Add the partials for ra
      for (int icoef = 0; icoef < nCamAngleCoefficients; icoef++) {
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension,
                                                 icoef, &coeff_image(0, nIndex),
                                                 &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partials for dec
      for (int icoef = 0; icoef < nCamAngleCoefficients; icoef++) {
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination,
                                                 icoef, &coeff_image(0, nIndex),
                                                 &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for twist if necessary
      if (observationSolveSettings->solveTwist()) {
        for (int icoef = 0; icoef < nCamAngleCoefficients; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist,
                                                   icoef, &coeff_image(0, nIndex),
                                                   &coeff_image(1, nIndex));
          nIndex++;
        }
      }
    }

    // partials for 3D point
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &coeff_point3D(0, 0),
                                       &coeff_point3D(1, 0));
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &coeff_point3D(0, 1),
                                       &coeff_point3D(1, 1));

    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &coeff_point3D(0, 2),
                                       &coeff_point3D(1, 2));

//    std::cout << coeff_point3D << std::endl;
    // right-hand side (measured - computed)
    dMeasuredx = measure.focalPlaneMeasuredX();
    dMeasuredy = measure.focalPlaneMeasuredY();

    deltax = dMeasuredx - dComputedx;
    deltay = dMeasuredy - dComputedy;

    coeff_RHS(0) = deltax;
    coeff_RHS(1) = deltay;

    // residual prob distribution is calculated even if there is no maximum likelihood estimation
    double obsValue = deltax / pCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    obsValue = deltay / pCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    dObservationSigma = 1.4 * pCamera->PixelPitch();
    dObservationWeight = 1.0 / dObservationSigma;

    if (m_bundleResults.numberMaximumLikelihoodModels()
          > m_bundleResults.maximumLikelihoodModelIndex()) {
      // if maximum likelihood estimation is being used
      double residualR2ZScore
                 = sqrt(deltax * deltax + deltay * deltay) / dObservationSigma / sqrt(2.0);
      m_bundleResults.addProbabilityDistributionObservation(residualR2ZScore);  //dynamically build the cumulative probability distribution of the R^2 residual Z Scores
                                                                                   //double tempScaler = m_wFunc[m_bundleResults.maximumLikelihoodModelIndex()]->sqrtWeightScaler(residualR2ZScore);
                                                                                   //if ( tempScaler == 0.0) printf("ZeroScaler\n");
                                                                                   //if ( tempScaler < 0.0)  printf("NegativeScaler\n");

      int currentModelIndex = m_bundleResults.maximumLikelihoodModelIndex();
      dObservationWeight *= m_bundleResults.maximumLikelihoodModelWFunc(currentModelIndex)
                            .sqrtWeightScaler(residualR2ZScore);
    }

//    std::cout << "Measuredx " << dMeasuredx << " Measuredy = " << dMeasuredy << std::endl;
//    std::cout << "dComputedx " << dComputedx << " dComputedy = " << dComputedy << std::endl;
//    std::cout << coeff_image << std::endl;
//    std::cout << coeff_point3D << std::endl;
//    std::cout << dMeasuredx << " " << dComputedx << std::endl << dMeasuredy << " " << dComputedy << std::endl;
//    std::cout << coeff_RHS << std::endl;

    // multiply coefficients by observation weight
    coeff_image *= dObservationWeight;
    coeff_point3D *= dObservationWeight;
    coeff_RHS *= dObservationWeight;

    if (m_bundleSettings->solveTargetBody())
      coeff_target *= dObservationWeight;

    m_Statsx.AddData(deltax);
    m_Statsy.AddData(deltay);

    return true;
  }


  /**
   * apply parameter corrections
   */
  void BundleAdjust::applyParameterCorrections() {
    if ( m_bundleSettings->solveMethod() == BundleSettings::Sparse ) {
      applyParameterCorrections_CHOLMOD();
    }
    else { //??? does a check belong here ??? what if it's not special k
      applyParameterCorrections_SPECIALK();
    }
  }


  /**
   * apply parameter corrections for CHOLMOD solution.
   */
  void BundleAdjust::applyParameterCorrections_CHOLMOD() {

    int t = 0;

//std::cout << m_imageSolution << std::endl;

    // TODO - update target body parameters if in solution
    // note these come before BundleObservation parameters in normal equations matrix
    if (m_bundleSettings->solveTargetBody()) {
      int ntargetBodyParameters = m_bundleTargetBody->numberParameters();

      m_bundleTargetBody->applyParameterCorrections(subrange(m_imageSolution,0,
                                                            ntargetBodyParameters));

      t += ntargetBodyParameters;
    }

    // Update spice for each BundleObservation
    int nobservations = m_bundleObservations.size();
    for (int i = 0; i < nobservations; i++) {
      BundleObservation *observation = m_bundleObservations.at(i);

      int nParameters = observation->numberParameters();

      observation->applyParameterCorrections(subrange(m_imageSolution,t,t+nParameters));

      if (m_bundleSettings->solveTargetBody())
        observation->updateBodyRotation();

      t += nParameters;
    }

    // TODO: CHECK - do we need point index in case of rejected points????

    // TODO: Below code should move into BundleControlPoint->updateParameterCorrections
    //       except, what about the product_AV method?

    // Update lat/lon for each control point
    double dLatCorr, dLongCorr, dRadCorr;
    int nPointIndex = 0;
    int nControlPoints = m_bundleControlPoints.size();
    for (int i = 0; i < nControlPoints; i++) {
      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if (point->isRejected()) {
          nPointIndex++;
          continue;
      }

      // get NIC, Q, and correction vector for this point
      boost::numeric::ublas::bounded_vector< double, 3 > &NIC = point->nicVector();
      SparseBlockRowMatrix &Q = point->cholmod_QMatrix();
      boost::numeric::ublas::bounded_vector< double, 3 > &corrections = point->corrections();

//      printf("Q\n");
//      std::cout << Q << std::endl;

//      printf("NIC\n");
//      std::cout << NIC << std::endl;

//      std::cout << m_imageSolution << std::endl;

      // subtract product of Q and nj from NIC
//      NIC -= prod(Q, m_imageSolution);
      product_AV(-1.0, NIC, Q, m_imageSolution);

      // get point parameter corrections
      dLatCorr = NIC(0);
      dLongCorr = NIC(1);
      dRadCorr = NIC(2);

//      printf("Point %s Corrections\n Latitude: %20.10lf\nLongitude: %20.10lf\n   Radius: %20.10lf\n",point->GetId().toLatin1().data(),dLatCorr, dLongCorr, dRadCorr);
//      std::cout <<"Point " <<  point->GetId().toLatin1().data() << " Corrections\n" << "Latitude: " << dLatCorr << std::endl << "Longitude: " << dLongCorr << std::endl << "Radius: " << dRadCorr << std::endl;

      SurfacePoint surfacepoint = point->getAdjustedSurfacePoint();

      double dLat = surfacepoint.GetLatitude().degrees();
      double dLon = surfacepoint.GetLongitude().degrees();
      double dRad = surfacepoint.GetLocalRadius().meters();

      dLat += RAD2DEG * dLatCorr;
      dLon += RAD2DEG * dLongCorr;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (dLat < -90.0) {
        dLat = -180.0 - dLat;
        dLon = dLon + 180.0;
      }
      if (dLat > 90.0) {
        dLat = 180.0 - dLat;
        dLon = dLon + 180.0;
      }
      while (dLon > 360.0) dLon = dLon - 360.0;
      while (dLon < 0.0) dLon = dLon + 360.0;

      dRad += 1000.*dRadCorr;

//      std::cout << corrections << std::endl;

      // sum and save corrections
      corrections(0) += dLatCorr;
      corrections(1) += dLongCorr;
      corrections(2) += dRadCorr;

//      std::cout << corrections << std::endl;

      // ken testing - if solving for target body mean radius, set radius to current
      // mean radius value
      if (m_bundleTargetBody && (m_bundleTargetBody->solveMeanRadius()
          || m_bundleTargetBody->solveTriaxialRadii())) {
        if (m_bundleTargetBody->solveMeanRadius()) {
          surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                               Longitude(dLon, Angle::Degrees),
                                               m_bundleTargetBody->meanRadius());
        }
        else if (m_bundleTargetBody->solveTriaxialRadii()) {
            Distance localRadius = m_bundleTargetBody->localRadius(Latitude(dLat, Angle::Degrees),
                                                                   Longitude(dLon, Angle::Degrees));
            surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                                 Longitude(dLon, Angle::Degrees),
                                                 localRadius);
        }
      }
      else {
        surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                             Longitude(dLon, Angle::Degrees),
                                             Distance(dRad, Distance::Meters));
      }

      point->setAdjustedSurfacePoint(surfacepoint);

      nPointIndex++;

    } // end loop over point corrections
  }


  /**
   * Apply parameter corrections for specialK solution.
   */
  void BundleAdjust::applyParameterCorrections_SPECIALK() {
/*
//    std::cout << "image corrections: " << m_imageCorrections << std::endl;
//    std::cout << "   image solution: " << m_imageSolution << std::endl;

      int index;
      int currentindex = -1;
      bool bsameindex = false;

      // Update selected spice for each image
      int nImages = images();
      for (int i = 0; i < nImages; i++) {

          if ( m_bundleResults.numberHeldImages() > 0 ) {
              if ((m_pHeldSnList->hasSerialNumber(m_pSnList->serialNumber(i)))) {
                  continue;
              }
          }

          Camera *pCamera = m_pCnet->Camera(i);
          index = imageIndex(i);
          if ( index == currentindex ) {
              bsameindex = true;
          }
          else {
              bsameindex = false;
          }

          currentindex = index;

          if (m_bundleSettings->instrumentPositionSolveOption() != BundleSettings::NoPositionFactors) {
            SpicePosition         *pInstPos = pCamera->instrumentPosition();
            std::vector<double> coefX(m_nNumberCamPosCoefSolved),
                coefY(m_nNumberCamPosCoefSolved),
                coefZ(m_nNumberCamPosCoefSolved);
            pInstPos->GetPolynomial(coefX, coefY, coefZ);

//        printf("X0:%20.10lf X1:%20.10lf X2:%20.10lf\n",abcX[0],abcX[1],abcX[2]);

            // Update the X coordinate coefficient(s) and sum parameter correction
            for (int icoef = 0; icoef < m_nNumberCamPosCoefSolved; icoef++) {
              coefX[icoef] += m_imageSolution(index);
              if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
              index++;
            }

            // Update the Y coordinate coefficient(s)
            for (int icoef = 0; icoef < m_nNumberCamPosCoefSolved; icoef++) {
              coefY[icoef] += m_imageSolution(index);
              if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
              index++;
            }

            // Update the Z coordinate coefficient(s)
            for (int icoef = 0; icoef < m_nNumberCamPosCoefSolved; icoef++) {
              coefZ[icoef] += m_imageSolution(index);
              if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
              index++;
            }

            pInstPos->SetPolynomial(coefX, coefY, coefZ, m_nPositionType);
          }

      if (m_bundleSettings->instrumentPointingSolveOption() != BundleSettings::NoPointingFactors) {
        SpiceRotation         *pInstRot = pCamera->instrumentRotation();
        std::vector<double> coefRA(m_nNumberCamAngleCoefSolved),
            coefDEC(m_nNumberCamAngleCoefSolved),
            coefTWI(m_nNumberCamAngleCoefSolved);
        pInstRot->GetPolynomial(coefRA, coefDEC, coefTWI);

        // Update right ascension coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCamAngleCoefSolved; icoef++) {
          coefRA[icoef] += m_imageSolution(index);
          if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
          index++;
        }

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCamAngleCoefSolved; icoef++) {
          coefDEC[icoef] += m_imageSolution(index);
          if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
          index++;
        }

        if (m_bundleSettings->solveTwist()) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCamAngleCoefSolved; icoef++) {
            coefTWI[icoef] += m_imageSolution(index);
            if (!bsameindex) m_imageCorrections(index) += m_imageSolution(index);
            index++;
          }
        }

        pInstRot->SetPolynomial(coefRA, coefDEC, coefTWI, m_nPointingType);
      }
    }

    // Update lat/lon for each control point
    double dLatCorr, dLongCorr, dRadCorr;
    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      if ( point->IsRejected() ) {
          nPointIndex++;
          continue;
      }

      // get NIC, Q, and correction vector for this point
      bounded_vector<double, 3> &NIC = m_NICs[nPointIndex];
      compressed_matrix<double> &Q = m_Qs_SPECIALK[nPointIndex];
      bounded_vector<double, 3> &corrections = m_Point_Corrections[nPointIndex];

//      printf("Q\n");
//      std::cout << Q << std::endl;

//      printf("NIC\n");
//      std::cout << NIC << std::endl;

//      std::cout << m_imageSolution << std::endl;

      // subtract product of Q and nj from NIC
      NIC -= prod(Q, m_imageSolution);

      // get point parameter corrections
      dLatCorr = NIC(0);
      dLongCorr = NIC(1);
      dRadCorr = NIC(2);

//      printf("Point %s Corrections\n Latitude: %20.10lf\nLongitude: %20.10lf\n   Radius: %20.10lf\n",point->GetId().toLatin1().data(),dLatCorr, dLongCorr, dRadCorr);
//      std::cout <<"Point " <<  point->GetId().toLatin1().data() << " Corrections\n" << "Latitude: " << dLatCorr << std::endl << "Longitude: " << dLongCorr << std::endl << "Radius: " << dRadCorr << std::endl;

      double dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
      double dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
      double dRad = point->GetAdjustedSurfacePoint().GetLocalRadius().meters();

      dLat += RAD2DEG * dLatCorr;
      dLon += RAD2DEG * dLongCorr;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (dLat < -90.0) {
        dLat = -180.0 - dLat;
        dLon = dLon + 180.0;
      }
      if (dLat > 90.0) {
        dLat = 180.0 - dLat;
        dLon = dLon + 180.0;
      }
      while (dLon > 360.0) dLon = dLon - 360.0;
      while (dLon < 0.0) dLon = dLon + 360.0;

      dRad += 1000.*dRadCorr;

//      std::cout << corrections << std::endl;

      // sum and save corrections
      corrections(0) += dLatCorr;
      corrections(1) += dLongCorr;
      corrections(2) += dRadCorr;

//      std::cout << corrections << std::endl;

      SurfacePoint surfacepoint = point->GetAdjustedSurfacePoint();

      surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                              Longitude(dLon, Angle::Degrees),
                                              Distance(dRad, Distance::Meters));

      point->SetAdjustedSurfacePoint(surfacepoint);

      nPointIndex++;

      // testing
      // Compute fixed point in body-fixed coordinates
//      double pB[3];
//      latrec_c( dRad * 0.001,
//               (dLon * DEG2RAD),
//               (dLat * DEG2RAD),
//               pB);
//      printf("%s %lf %lf %lf\n",point->Id().toLatin1().data(),pB[0],pB[1],pB[2]);
    } // end loop over point corrections
*/
  }


  /**
   * This method computes the focal plane residuals for the measures.
   *
   * @history 2012-01-18 Debbie A. Cook - Fixed the computation of vx
   *                            and vy to make sure they are focal
   *                            plane x and y residuals instead of
   *                            image sample and line residuals.
   *
   */
  double BundleAdjust::computeResiduals() {
    double vtpv = 0.0;
    double vtpv_control = 0.0;
    double vtpv_image = 0.0;
    double dWeight;
    double v, vx, vy;

    // clear residual stats vectors
    m_Statsrx.Reset();
    m_Statsry.Reset();
    m_Statsrxy.Reset();

//    m_bundleResults.setRmsXYResiduals(sqrt(m_Statsrx.SumSquare()/(m_bundleResults.numberObservations()/2),
//                                         sqrt(m_Statsry.SumSquare()/(m_bundleResults.numberObservations()/2),
//                                         sqrt(m_Statsrxy.SumSquare()/m_bundleResults.numberObservations()));

    // vtpv for image coordinates
    int nObjectPoints = m_bundleControlPoints.size();

    for (int i = 0; i < nObjectPoints; i++) {

      BundleControlPointQsp bundleControlPoint = m_bundleControlPoints.at(i);
      ControlPoint* point = bundleControlPoint->rawControlPoint();

      point->ComputeResiduals();

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored()) {
          continue;
        }

        dWeight = 1.4 * (measure->Camera())->PixelPitch();
        dWeight = 1.0 / dWeight;
        dWeight *= dWeight;

        vx = measure->GetFocalPlaneMeasuredX() - measure->GetFocalPlaneComputedX();
        vy = measure->GetFocalPlaneMeasuredY() - measure->GetFocalPlaneComputedY();

        // if rejected, don't include in statistics
        if (measure->IsRejected()) {
          continue;
        }

        m_Statsrx.AddData(vx);
        m_Statsry.AddData(vy);
        m_Statsrxy.AddData(vx);
        m_Statsrxy.AddData(vy);

//        printf("Point: %s rx: %20.10lf  ry: %20.10lf\n",point->GetId().toLatin1().data(),vx,vy);

        vtpv += vx * vx * dWeight + vy * vy * dWeight;
      }
    }

//     std::cout << "vtpv image = " << vtpv << std::endl;
//     std::cout << "dWeight = " << dWeight << std::endl;

    // add vtpv from constrained 3D points
    int nPointIndex = 0;
    for (int i = 0; i < nObjectPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_bundleControlPoints.at(i);

      // get weight and correction vector for this point
      boost::numeric::ublas::bounded_vector<double, 3> weights = bundleControlPoint->weights();
      boost::numeric::ublas::bounded_vector<double, 3> corrections = bundleControlPoint->corrections();

      //printf("Point: %s PointIndex: %d Loop(i): %d\n",point->GetId().toLatin1().data(),nPointIndex,i);
      //std::cout << weights << std::endl;
      //std::cout << corrections << std::endl;

      if ( weights(0) > 0.0 ) {
          vtpv_control += corrections(0) * corrections(0) * weights(0);
      }
      if ( weights(1) > 0.0 ) {
          vtpv_control += corrections(1) * corrections(1) * weights(1);
      }
      if ( weights(2) > 0.0 ) {
          vtpv_control += corrections(2) *corrections(2) * weights(2);
      }

      nPointIndex++;
    }

//    std::cout << "vtpv control = " << vtpv_control << std::endl;

    // add vtpv from constrained image parameters
    for (int i = 0; i < m_bundleObservations.size(); i++) {
      BundleObservation *observation = m_bundleObservations.at(i);

      // get weight and correction vector for this observation
      const boost::numeric::ublas::vector<double> &weights = observation->parameterWeights();
      const boost::numeric::ublas::vector<double> &corrections = observation->parameterCorrections();

      for (int j = 0; j < (int)corrections.size(); j++) {
        if (weights[j] > 0.0) {
          v = corrections[j];
          vtpv_image += v * v * weights[j];
        }
      }
    }

//    std::cout << "vtpv_image = " << vtpv_image << std::endl;

    // TODO - add vtpv from constrained target body parameters
    double vtpv_targetBody = 0.0;
    if ( m_bundleTargetBody) {
      vtpv_targetBody = m_bundleTargetBody->vtpv();
    }

//    std::cout << "vtpv_targetBody" = " << vtpv_targetBody << std::endl;

   vtpv = vtpv + vtpv_control + vtpv_image + vtpv_targetBody;

    // Compute rms for all image coordinate residuals
    // separately for x, y, then x and y together
    m_bundleResults.setRmsXYResiduals(m_Statsrx.Rms(), m_Statsry.Rms(), m_Statsrxy.Rms());

    return vtpv;
  }


  /**
   * Bundle wrap up.
   *
   */
  bool BundleAdjust::wrapUp() {
    // compute residuals in pixels

    // vtpv for image coordinates
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored()) {
        continue;
      }

      point->ComputeResiduals();
    }
    computeBundleStatistics();
//     m_bundleResults.computeBundleStatistics(m_pSnList, m_pCnet, m_bundleSettings->errorPropagation(), m_bundleSettings->solveRadius());


    return true;
  }


  /**
   * Compute rejection limit.
   *
   */
  bool BundleAdjust::computeRejectionLimit() { // ??? i believe this should be in bundle stats...
      double vx, vy;

      int nResiduals = m_bundleResults.numberObservations() / 2;

//      std::cout << "total observations: " << m_bundleResults.numberObservations() << std::endl;
//      std::cout << "rejected observations: " << m_bundleResults.numberRejectedObservations() << std::endl;
//      std::cout << "good observations: " << m_bundleResults.numberObservations()-m_bundleResults.numberRejectedObservations() << std::endl;

      std::vector<double> resvectors;

      resvectors.resize(nResiduals);

      // load magnitude (squared) of residual vector
      int nObservation = 0;
      int nObjectPoints = m_pCnet->GetNumPoints();
      for (int i = 0; i < nObjectPoints; i++) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() ) {
            continue;
          }

          if ( point->IsRejected() ) {
            continue;
          }

          int nMeasures = point->GetNumMeasures();
          for (int j = 0; j < nMeasures; j++) {

              const ControlMeasure *measure = point->GetMeasure(j);
              if ( measure->IsIgnored() ) {
                  continue;
              }

              if ( measure->IsRejected() ) {
                  continue;
              }

              vx = measure->GetSampleResidual();
              vy = measure->GetLineResidual();

              resvectors[nObservation] = sqrt(vx*vx + vy*vy);

              nObservation++;
          }
      }

//      std::cout << "x residuals\n" << x_residuals << std::endl;
//      std::cout << "y_residuals\n" << y_residuals << std::endl;

      // sort vectors
      std::sort(resvectors.begin(), resvectors.end());

//      std::cout << "residuals range: \n" << resvectors[0] << resvectors[nResiduals- 1] << std::endl;

//      std::cout << "x residuals sorted\n" << x_residuals << std::endl;
//      std::cout << "y_residuals sorted\n" << y_residuals << std::endl;

      double median;
      double mediandev;
      double mad;

      int nmidpoint = nResiduals / 2;

      if ( nResiduals % 2 == 0 ) {
        median = (resvectors[nmidpoint- 1] + resvectors[nmidpoint]) / 2;
      }
      else {
        median = resvectors[nmidpoint];
      }

//      std::cout << "median: " << median << std::endl;

      // compute M.A.D.
      for (int i = 0; i < nResiduals; i++) {
          resvectors[i] = fabs(resvectors[i] - median);
      }

      std::sort(resvectors.begin(), resvectors.end());

//      std::cout << "residuals range: \n" << resvectors[0] << resvectors[nResiduals- 1] << std::endl;

      if ( nResiduals % 2 == 0 ) {
        mediandev = (resvectors[nmidpoint- 1] + resvectors[nmidpoint]) / 2;
      }
      else {
        mediandev = resvectors[nmidpoint];
      }

      std::cout << "median deviation: " << mediandev << std::endl;

      mad = 1.4826 * mediandev;

      std::cout << "mad: " << mad << std::endl;

//      double dLow = median - m_bundleSettings->rejectionMultiplier() * mad;
//      double dHigh = median + m_bundleSettings->rejectionMultiplier() * mad;

//      std::cout << "reject range: \n" << dLow << " " << dHigh << std::endl;
//      std::cout << "Rejection multipler: \n" << m_bundleSettings->rejectionMultiplier() << std::endl;

      m_bundleResults.setRejectionLimit(median + m_bundleSettings->outlierRejectionMultiplier() * mad);

//      std::cout << "Rejection Limit: " << m_bundleResults.rejectionLimit() << std::endl;

      return true;
  }


  /**
   * Flag outlier measurements.
   *
   */
  bool BundleAdjust::flagOutliers() {
    double vx, vy;
    int nRejected;
    int ntotalrejected = 0;

    int nIndexMaxResidual;
    double dMaxResidual;
    double dSumSquares;
    double dUsedRejectionLimit = m_bundleResults.rejectionLimit();

//    if ( m_bundleResults.rejectionLimit() < 0.05 )
//        dUsedRejectionLimit = 0.14;
    //        dUsedRejectionLimit = 0.05;

    int nComingBack = 0;

    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if ( point->IsIgnored() ) {
        continue;
      }

      point->ZeroNumberOfRejectedMeasures();

      nRejected = 0;
      nIndexMaxResidual = -1;
      dMaxResidual = -1.0;

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {

          ControlMeasure *measure = point->GetMeasure(j);
          if ( measure->IsIgnored() ) {
              continue;
          }

          vx = measure->GetSampleResidual();
          vy = measure->GetLineResidual();

          dSumSquares = sqrt(vx*vx + vy*vy);

          // measure is good
          if ( dSumSquares <= dUsedRejectionLimit ) {

            // was it previously rejected?
            if ( measure->IsRejected() ) {
                  printf("Coming back in: %s\r",point->GetId().toLatin1().data());
                  nComingBack++;
                  m_pCnet->DecrementNumberOfRejectedMeasuresInImage(measure->GetCubeSerialNumber());
              }

              measure->SetRejected(false);
              continue;
          }

          // if it's still rejected, skip it
          if ( measure->IsRejected() ) {
              nRejected++;
              ntotalrejected++;
              continue;
          }

          if ( dSumSquares > dMaxResidual ) {
              dMaxResidual = dSumSquares;
              nIndexMaxResidual = j;
          }
      }

      // no observations above the current rejection limit for this 3D point
      if ( dMaxResidual == -1.0 || dMaxResidual <= dUsedRejectionLimit ) {
          point->SetNumberOfRejectedMeasures(nRejected);
          continue;
      }

      // this is another kluge - if we only have two observations
      // we won't reject (for now)
      if ((nMeasures - (nRejected + 1)) < 2) {
          point->SetNumberOfRejectedMeasures(nRejected);
          continue;
      }

      // otherwise, we have at least one observation
      // for this point whose residual is above the
      // current rejection limit - we'll flag the
      // worst of these as rejected
      ControlMeasure *rejected = point->GetMeasure(nIndexMaxResidual);
      rejected->SetRejected(true);
      nRejected++;
      point->SetNumberOfRejectedMeasures(nRejected);
      m_pCnet->IncrementNumberOfRejectedMeasuresInImage(rejected->GetCubeSerialNumber());
      ntotalrejected++;

      // do we still have sufficient remaining observations for this 3D point?
      if ( ( nMeasures-nRejected ) < 2 ) {
          point->SetRejected(true);
          printf("Rejecting Entire Point: %s\r",point->GetId().toLatin1().data());
      }
      else
          point->SetRejected(false);

//      int ndummy = point->GetNumberOfRejectedMeasures();
//      printf("Rejected for point %s = %d\n", point->GetId().toLatin1().data(), ndummy);
//      printf("%s: %20.10lf  %20.10lf*\n",point->GetId().toLatin1().data(), rejected->GetSampleResidual(), rejected->GetLineResidual());
  }

    int numberRejectedObservations = 2*ntotalrejected;

    printf("\n\t       Rejected Observations:%10d (Rejection Limit:%12.5lf\n",
           numberRejectedObservations, dUsedRejectionLimit);
    m_bundleResults.setNumberRejectedObservations(numberRejectedObservations);

    std::cout << "Measures that came back: " << nComingBack << std::endl;

    return true;
}


  /**
   * error propagation.
   */
  bool BundleAdjust::errorPropagation() {
    if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
      return errorPropagation_CHOLMOD();
    }
    else {
      return errorPropagation_SPECIALK();
    }

    return false;
  }


  /**
   * error propagation for specialK solution.
   */
  bool BundleAdjust::errorPropagation_SPECIALK() {

    // create inverse of normal equations matrix
    if ( !CholeskyUT_NOSQR_Inverse() )
        return false;

    boost::numeric::ublas::matrix<double> T(3, 3);
    boost::numeric::ublas::matrix<double> QS(3, m_nRank);
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double t;

    double dSigma02 = m_bundleResults.sigma0() * m_bundleResults.sigma0();

    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
        if ( point->IsIgnored() )
            continue;

        if ( point->IsRejected() )
            continue;

        printf("\rProcessing point %d of %d",i+1,nObjectPoints);

        T.clear();
        QS.clear();

        // get corresponding Q matrix
        boost::numeric::ublas::compressed_matrix<double> &Q = m_Qs_SPECIALK[nPointIndex];

        // form QS
        QS = prod(Q, m_Normals);

        // form T
        T = prod(QS, trans(Q));

        // Ask Ken what is happening here...Setting just the sigmas is not very accurate
        // Shouldn't we be updating and setting the matrix???  TODO
        SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();

        dSigmaLat = SurfacePoint.GetLatSigma().radians();
        dSigmaLong = SurfacePoint.GetLonSigma().radians();
        dSigmaRadius = SurfacePoint.GetLocalRadiusSigma().meters();

//      std::cout << dSigmaLat << " " << dSigmaLong << " " << dSigmaRadius << std::endl;

//      dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
//      dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
//      dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();

        t = dSigmaLat*dSigmaLat + T(0, 0);
        Distance tLatSig(sqrt(dSigma02 * t) * m_dRTM, Distance::Meters);

        t = dSigmaLong*dSigmaLong + T(1, 1);
        t = sqrt(dSigma02 * t) * m_dRTM;
        Distance tLonSig(
            t * cos(point->GetAdjustedSurfacePoint().GetLatitude().radians()),
            Distance::Meters);

        t = dSigmaRadius*dSigmaRadius + T(2, 2);
        t = sqrt(dSigma02 * t) * 1000.0;

        SurfacePoint.SetSphericalSigmasDistance(tLatSig, tLonSig,
            Distance(t, Distance::Meters));

        point->SetAdjustedSurfacePoint(SurfacePoint);

       nPointIndex++;
    }

    return true;
  }


  /**
   * error propagation for CHOLMOD solution.
   */
  bool BundleAdjust::errorPropagation_CHOLMOD() {

    // free unneeded memory
    cholmod_free_triplet(&m_pTriplet, &m_cm);
    cholmod_free_sparse(&m_N, &m_cm);

    boost::numeric::ublas::matrix<double> T(3, 3);
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double t;

    double dSigma02 = m_bundleResults.sigma0() * m_bundleResults.sigma0();

    int nObjectPoints = m_bundleControlPoints.size();

    std::string strTime = Isis::iTime::CurrentLocalTime().toLatin1().data();
    printf("     Time: %s\n\n", strTime.c_str());

    // create and initialize array of 3x3 matrices for all object points
    std::vector< symmetric_matrix<double> > point_covs(nObjectPoints,symmetric_matrix<double>(3));
    for (int d = 0; d < nObjectPoints; d++) {
      point_covs[d].clear();
    }

    cholmod_dense *x;        // solution vector
    cholmod_dense *b;        // right-hand side (column vectors of identity)

    b = cholmod_zeros ( m_nRank, 1, CHOLMOD_REAL, &m_cm );
    double* pb = (double*)b->x;

    double* px = NULL;

    SparseBlockColumnMatrix sbcMatrix;

    QString str;

    // Create unique file name
    FileName matrixFile(m_bundleSettings->outputFilePrefix() + "inverseMatrix.dat");
    //???FileName matrixFile = FileName::createTempFile(m_bundleSettings.outputFilePrefix() 
    //???                                               + "inverseMatrix.dat");
    // Create file handle
    QFile matrixOutput(matrixFile.expanded());
    // Open file to write to
    matrixOutput.open(QIODevice::WriteOnly);
    QDataStream outStream(&matrixOutput);

    int i, j, k;
    int nCurrentColumn = 0;
    int ncolsCurrentBlockColumn = 0;
    int nBlockColumns = m_SparseNormals.size();

    for (i = 0; i < nBlockColumns; i++) {

      // columns in this column block
      SparseBlockColumnMatrix* normalsColumn = m_SparseNormals.at(i);
      if (i == 0) {
        ncolsCurrentBlockColumn = normalsColumn->numberOfColumns();
        int nRows = m_SparseNormals.at(i)->numberOfRows();
        sbcMatrix.insertMatrixBlock(i, nRows, ncolsCurrentBlockColumn);
        sbcMatrix.zeroBlocks();
      }
      else {
        if (normalsColumn->numberOfColumns() == ncolsCurrentBlockColumn) {
          int nRows = m_SparseNormals.at(i)->numberOfRows();
          sbcMatrix.insertMatrixBlock(i, nRows, ncolsCurrentBlockColumn);
          sbcMatrix.zeroBlocks();
        }
        else {
          ncolsCurrentBlockColumn = normalsColumn->numberOfColumns();

          // reset sbcMatrix
          sbcMatrix.wipe();

          // insert blocks
          for (j = 0; j < (i+1); j++) {
            SparseBlockColumnMatrix* normalsRow = m_SparseNormals.at(j);
            int nRows = normalsRow->numberOfRows();

            sbcMatrix.insertMatrixBlock(j, nRows, ncolsCurrentBlockColumn);
          }
        }
      }

      int localCol = 0;

      // solve for inverse for nCols
      for (j = 0; j < ncolsCurrentBlockColumn; j++) {
        if ( nCurrentColumn > 0 )
          pb[nCurrentColumn- 1] = 0.0;
        pb[nCurrentColumn] = 1.0;

        x = cholmod_solve ( CHOLMOD_A, m_L, b, &m_cm );
        px = (double*)x->x;
        int rp = 0;

        // store solution in corresponding column of inverse
        for (k = 0; k < sbcMatrix.size(); k++) {
          boost::numeric::ublas::matrix<double> *matrix = sbcMatrix.value(k);

          int sz1 = matrix->size1();

          for (int ii = 0; ii < sz1; ii++) {
            (*matrix)(ii,localCol) = px[ii + rp];
          }
          rp += matrix->size1();
        }

        nCurrentColumn++;
        localCol++;

        cholmod_free_dense(&x,&m_cm);
      }

      // save adjusted target body sigmas if solving for target
      if (m_bundleSettings->solveTargetBody() && i == 0) {
        vector< double > &adjustedSigmas = m_bundleTargetBody->adjustedSigmas();
        matrix<double>* targetCovMatrix = sbcMatrix.value(i);

        for ( int z = 0; z < ncolsCurrentBlockColumn; z++)
          adjustedSigmas[z] = sqrt((*targetCovMatrix)(z,z))*m_bundleResults.sigma0();
      }
      // save adjusted image sigmas
      else {
        BundleObservation *observation;
        if (m_bundleSettings->solveTargetBody())
          observation = m_bundleObservations.at(i-1);
        else
          observation = m_bundleObservations.at(i);
        vector< double > &adjustedSigmas = observation->adjustedSigmas();
        matrix<double>* imageCovMatrix = sbcMatrix.value(i);
        for ( int z = 0; z < ncolsCurrentBlockColumn; z++) {
          adjustedSigmas[z] = sqrt((*imageCovMatrix)(z,z))*m_bundleResults.sigma0();
        }
      }


      // Output inverse matrix to the open file.
      outStream << sbcMatrix;

      // now loop over all object points to sum contributions into 3x3 point covariance matrix
      int nPointIndex = 0;
      for (j = 0; j < nObjectPoints; j++) {

        BundleControlPointQsp point = m_bundleControlPoints.at(nPointIndex);
        if ( point->isRejected() )
          continue;

        // only update point every 100 points
        if (j%100 == 0) {
            printf("\rError Propagation: Inverse Block %8d of %8d; Point %8d of %8d", i+1,
                   nBlockColumns,  j+1, nObjectPoints);
//          str = QString("Inverse Block %1 of %2; Point %3 of %4").arg(i+1).arg(nBlockColumns)
//              .arg(j+1).arg(nObjectPoints);

            emit iterationUpdate(i+1, j+1);
        }

        // get corresponding Q matrix
        SparseBlockRowMatrix Q = point->cholmod_QMatrix();

        T.clear();

        // get corresponding point covariance matrix
        boost::numeric::ublas::symmetric_matrix<double> &cv = point_covs[nPointIndex];

        // get qT - index i is the key into Q for qT
        boost::numeric::ublas::matrix<double> *qT = Q.value(i);
        if (!qT) {
          nPointIndex++;
          continue;
        }

        // iterate over Q
        // q is current map value
        QMapIterator< int, boost::numeric::ublas::matrix<double> * > it(Q);
        while ( it.hasNext() ) {
          it.next();

          int nKey = it.key();

          if (it.key() > i) {
            break;
          }

          boost::numeric::ublas::matrix<double> *q = it.value();

          if ( !q ) {// should never be NULL
            continue;
          }

          boost::numeric::ublas::matrix<double> *nI = sbcMatrix.value(it.key());

          if ( !nI ) {// should never be NULL
            continue;
          }

          T = prod(*nI, trans(*qT));
          T = prod(*q,T);

          if (nKey != i) {
            T += trans(T);
          }

          cv += T;
        }
        nPointIndex++;
      }
    }
    // Close the file.
    matrixOutput.close();
    // Save the location of the "covariance" matrix
    m_bundleResults.setCorrMatCovFileName(matrixFile);

    // can free sparse normals now
    m_SparseNormals.wipe();

    // free b (right-hand side vector
    cholmod_free_dense(&b,&m_cm);

    printf("\n\n");
    strTime = Isis::iTime::CurrentLocalTime().toLatin1().data();
    printf("\rFilling point covariance matrices: Time %s", strTime.c_str());
    printf("\n\n");

    // now loop over points again and set final covariance stuff
    int nPointIndex = 0;
    for (j = 0; j < nObjectPoints; j++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(nPointIndex);

      if ( point->isRejected() )
        continue;

      if (j%100 == 0) {
        printf("\rError Propagation: Filling point covariance matrices %8d of %8d\r",j+1,
               nObjectPoints);
      }

      // get corresponding point covariance matrix
      boost::numeric::ublas::symmetric_matrix<double> &cv = point_covs[nPointIndex];

      // Ask Ken what is happening here...Setting just the sigmas is not very accurate
      // Shouldn't we be updating and setting the matrix???  TODO
      SurfacePoint SurfacePoint = point->getAdjustedSurfacePoint();

      dSigmaLat = SurfacePoint.GetLatSigma().radians();
      dSigmaLong = SurfacePoint.GetLonSigma().radians();
      dSigmaRadius = SurfacePoint.GetLocalRadiusSigma().meters();

      t = dSigmaLat*dSigmaLat + cv(0, 0);
      Distance tLatSig(sqrt(dSigma02 * t) * m_dRTM, Distance::Meters);

      t = dSigmaLong*dSigmaLong + cv(1, 1);
      t = sqrt(dSigma02 * t) * m_dRTM;
      Distance tLonSig(
          t * cos(point->getAdjustedSurfacePoint().GetLatitude().radians()),
          Distance::Meters);

      t = dSigmaRadius*dSigmaRadius + cv(2, 2);
      t = sqrt(dSigma02 * t) * 1000.0;

      SurfacePoint.SetSphericalSigmasDistance(tLatSig, tLonSig,
                                              Distance(t, Distance::Meters));

      point->setAdjustedSurfacePoint(SurfacePoint);

      nPointIndex++;
    }

    return true;
  }


  /**
   * Return index to basis function for image with index i.
   *
   */
/*
  int BundleAdjust::imageIndex(int i) const {
    if (!m_bundleSettings->solveObservationMode())
      return m_nImageIndexMap[i] * m_nNumImagePartials;
    else
      return m_pObsNumList->observationNumberMapIndex(i) * m_nNumImagePartials;
  }
*/

  /**
   * Return the ith filename in the cube list file given to constructor.
   *
   */
  // TODO: probably don't need this, can get from BundleObservation
  QString BundleAdjust::fileName(int i) {
    return m_pSnList->fileName(i);
  }


  /**
   * Return whether the ith file in the cube list is held.
   *
   */
  bool BundleAdjust::isHeld(int i) {
    if ( m_bundleResults.numberHeldImages() > 0 )
         if ((m_pHeldSnList->hasSerialNumber(m_pSnList->serialNumber(i))))
           return true;
    return false;
  }


  /**
   * Return a table cmatrix for the ith cube in the cube list given to the constructor.
   *
   */
  Table BundleAdjust::cMatrix(int i) {
    return m_pCnet->Camera(i)->instrumentRotation()->Cache("InstrumentPointing");
  }

  //! Return a table spacecraft vector for the ith cube in the cube list given to the
  //! constructor
  Table BundleAdjust::spVector(int i) {
    return m_pCnet->Camera(i)->instrumentPosition()->Cache("InstrumentPosition");
  }


  /**
   * Return the number of observations in list given to the constructor.
   *
   */
//  int BundleAdjust::observations() const {
//    if (!m_bundleSettings->solveObservationMode()) {
//      return m_pSnList->size();
//      //    return m_pSnList->size() - m_bundleResults.numberHeldImages();
//    }
//    else {
//      return m_pObsNumList->ObservationSize();
//    }
//  }


//std::vector<double> BundleAdjust::PointPartial(Isis::ControlPoint &point, PartialDerivative wrt)
//{
//  double lat = point.GetAdjustedSurfacePoint().GetLatitude();  // Radians
//  double lon = point.GetAdjustedSurfacePoint().GetLongitude();
//  double sinLon = sin(lon);
//  double cosLon = cos(lon);
//  double sinLat = sin(lat);
//  double cosLat = cos(lat);
//  double radius = point.GetAdjustedSurfacePoint().GetLocalRadius() * 0.001;  /km
//
//  std::vector<double> v(3);
//
//  if (wrt == WRT_Latitude)
//  {
//    v[0] = -radius * sinLat * cosLon;
//    v[1] = -radius * sinLon * sinLat;
//    v[2] =  radius * cosLat;
//  }
//  else if (wrt == WRT_Longitude)
//  {
//    v[0] = -radius * cosLat * sinLon;
//    v[1] =  radius * cosLat * cosLon;
//    v[2] =  0.0;
//  }
//  else
//  {
//    v[0] = cosLon * cosLat;
//    v[1] = sinLon * cosLat;
//    v[2] = sinLat;
//  }
//
//  return v;
//}
//


  /**
   * Creates an iteration summary and an iteration group for the solution summary
   */
  void BundleAdjust::iterationSummary() {
    QString itlog;

    if ( m_bundleResults.converged() ) {
        itlog = "Iteration" + toString(m_nIteration) + ": Final";
    }
    else {
        itlog = "Iteration" + toString(m_nIteration);
    }

    PvlGroup gp(itlog);

    gp += PvlKeyword( "Sigma0",
                      toString( m_bundleResults.sigma0() ) );
    gp += PvlKeyword( "Observations",
                      toString( m_bundleResults.numberObservations() ) );
    gp += PvlKeyword( "Constrained_Point_Parameters",
                      toString( m_bundleResults.numberConstrainedPointParameters() ) );
    gp += PvlKeyword( "Constrained_Image_Parameters",
                      toString( m_bundleResults.numberConstrainedImageParameters() ) );
    gp += PvlKeyword( "Constrained_Target_Parameters",
                      toString( m_bundleResults.numberConstrainedTargetParameters() ) );
    gp += PvlKeyword( "Unknown_Parameters",
                      toString( m_bundleResults.numberUnknownParameters() ) );
    gp += PvlKeyword( "Degrees_of_Freedom",
                      toString( m_bundleResults.degreesOfFreedom() ) );
    gp += PvlKeyword( "Rejected_Measures",
                      toString( m_bundleResults.numberRejectedObservations()/2) );


    if ( m_bundleResults.numberMaximumLikelihoodModels() >
         m_bundleResults.maximumLikelihoodModelIndex() ) {
      // if maximum likelihood estimation is being used

      gp += PvlKeyword( "Maximum_Likelihood_Tier: ",
                        toString( m_bundleResults.maximumLikelihoodModelIndex() ) );
      gp += PvlKeyword( "Median_of_R^2_residuals: ",
                        toString( m_bundleResults.maximumLikelihoodMedianR2Residuals() ) );
    }

    if ( m_bundleResults.converged() ) {
      gp += PvlKeyword("Converged", "TRUE");
      gp += PvlKeyword( "TotalElapsedTime", toString( m_bundleResults.elapsedTime() ) );

      if (m_bundleSettings->errorPropagation()) {
        gp += PvlKeyword( "ErrorPropagationElapsedTime",
                         toString( m_bundleResults.elapsedTimeErrorProp() ) );
      }
    }

    std::ostringstream ostr;
    ostr << gp << std::endl;
    m_iterationSummary += QString::fromStdString( ostr.str() );
    if (m_bPrintSummary) Application::Log(gp);
  }


  /**
   * Output bundle results to file.
   */
  bool BundleAdjust::output() {
    if (m_bundleSettings->createBundleOutputFile()) {
        outputText();
    }

    if (m_bundleSettings->createCSVFiles()) {
      outputPointsCSV();
//    outputImagesCSV();
    }

    if (m_bundleSettings->createResidualsFile()) {
      outputResiduals();
    }

    return true;
  }


  /**
   * Output header for bundle results file.
   *
   */
  bool BundleAdjust::outputHeader(std::ofstream &fp_out) {

    if (!fp_out) {
      return false;
    }

    char buf[1056];
    int nImages = images();
    int nValidPoints = m_pCnet->GetNumValidPoints();
    int nInnerConstraints = 0;
    int nDistanceConstraints = 0;
    int nDegreesOfFreedom = m_bundleResults.numberObservations()
                            + m_bundleResults.numberConstrainedPointParameters()
                            + m_bundleResults.numberConstrainedImageParameters()
                            + m_bundleResults.numberConstrainedTargetParameters()
                            - m_bundleResults.numberUnknownParameters(); // ??? same as bstat dof ???
    //??? m_bundleResults.computeDegreesOfFreedom();
    //??? int nDegreesOfFreedom = m_bundleResults.degreesOfFreedom();

    int nConvergenceCriteria = 1;

    sprintf(buf, "JIGSAW: BUNDLE ADJUSTMENT\n=========================\n");
    fp_out << buf;
    sprintf(buf, "\n                       Run Time: %s",
                                           Isis::iTime::CurrentLocalTime().toLatin1().data());
    fp_out << buf;
    sprintf(buf, "\n               Network Filename: %s", m_strCnetFileName.toLatin1().data());
    fp_out << buf;
    sprintf(buf, "\n                     Network Id: %s", m_pCnet->GetNetworkId().toLatin1().data());
    fp_out << buf;
    sprintf(buf, "\n            Network Description: %s", m_pCnet->Description().toLatin1().data());
    fp_out << buf;
    sprintf(buf, "\n                         Target: %s", m_pCnet->GetTarget().toLatin1().data());
    fp_out << buf;
    sprintf(buf, "\n\n                   Linear Units: kilometers");
    fp_out << buf;
    sprintf(buf, "\n                  Angular Units: decimal degrees");
    fp_out << buf;
    sprintf(buf, "\n\nINPUT: SOLVE OPTIONS\n====================\n");
    fp_out << buf;

    m_bundleSettings->solveObservationMode() ?
      sprintf(buf, "\n                   OBSERVATIONS: ON"):
      sprintf(buf, "\n                   OBSERVATIONS: OFF");
    fp_out << buf;

    m_bundleSettings->solveRadius() ?
      sprintf(buf, "\n                         RADIUS: ON"):
      sprintf(buf, "\n                         RADIUS: OFF");
    fp_out << buf;

    m_bundleSettings->solveTargetBody() ?
      sprintf(buf, "\n                    TARGET BODY: ON"):
      sprintf(buf, "\n                    TARGET BODY: OFF");
    fp_out << buf;

    m_bundleSettings->updateCubeLabel() ?
      sprintf(buf, "\n                         UPDATE: YES"):
      sprintf(buf, "\n                         UPDATE: NO");
    fp_out << buf;

    sprintf(buf, "\n                  SOLUTION TYPE: %s",
            BundleSettings::solveMethodToString(
                m_bundleSettings->solveMethod()).toUpper().toLatin1().data());
    fp_out << buf;

    m_bundleSettings->errorPropagation() ?
      sprintf(buf, "\n              ERROR PROPAGATION: ON"):
      sprintf(buf, "\n              ERROR PROPAGATION: OFF");
    fp_out << buf;

    if (m_bundleSettings->outlierRejection()) {
      sprintf(buf, "\n              OUTLIER REJECTION: ON");
      fp_out << buf;
      sprintf(buf, "\n           REJECTION MULTIPLIER: %lf",
                                 m_bundleSettings->outlierRejectionMultiplier());// ??? is this correct ???
      fp_out << buf;

    }
    else {
      sprintf(buf, "\n              OUTLIER REJECTION: OFF");
      fp_out << buf;
      sprintf(buf, "\n           REJECTION MULTIPLIER: N/A");// ??? is this correct ???
      fp_out << buf;
    }

    sprintf(buf, "\n\nMAXIMUM LIKELIHOOD ESTIMATION\n============================\n");
    fp_out << buf;

    for (int tier = 0; tier < 3; tier++) {
      if (tier < m_bundleResults.numberMaximumLikelihoodModels()) { // replace number of models variable with settings models.size()???
        sprintf(buf, "\n                         Tier %d Enabled: TRUE", tier);
        fp_out << buf;
        sprintf(buf, "\n               Maximum Likelihood Model: %s",
                MaximumLikelihoodWFunctions::modelToString(
                    m_bundleResults.maximumLikelihoodModelWFunc(tier).model()).toLatin1().data());
        fp_out << buf;
        sprintf(buf, "\n    Quantile used for tweaking constant: %lf",
                            m_bundleResults.maximumLikelihoodModelQuantile(tier));
        fp_out << buf;
        sprintf(buf, "\n   Quantile weighted R^2 Residual value: %lf",
                           m_bundleResults.maximumLikelihoodModelWFunc(tier).tweakingConstant());
        fp_out << buf;
        sprintf(buf, "\n       Approx. weighted Residual cutoff: %s",
                               m_bundleResults.maximumLikelihoodModelWFunc(tier)
                                   .weightedResidualCutoff().toLatin1().data());
        fp_out << buf;
        if (tier != 2) fp_out << "\n";
      }
      else {
        sprintf(buf, "\n                         Tier %d Enabled: FALSE", tier);
        fp_out << buf;
      }
    }

    sprintf(buf, "\n\nINPUT: CONVERGENCE CRITERIA\n===========================\n");
    fp_out << buf;
    sprintf(buf, "\n                         SIGMA0: %e",
                                             m_bundleSettings->convergenceCriteriaThreshold());
    fp_out << buf;
    sprintf(buf, "\n             MAXIMUM ITERATIONS: %d",
                                 m_bundleSettings->convergenceCriteriaMaximumIterations());
    fp_out << buf;
    sprintf(buf, "\n\nINPUT: CAMERA POINTING OPTIONS\n==============================\n");
    fp_out << buf;
/*
    //??? this line could replace the switch/case below...   sprintf(buf, "\n                          CAMSOLVE: %s", BundleSettings::instrumentPointingSolveOptionToString(m_bundleSettings->instrumentPointingSolveOption()).toUpper();
    switch (m_bundleSettings->instrumentPointingSolveOption()) {
      case BundleSettings::AnglesOnly:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES");
        break;
      case BundleSettings::AnglesVelocity:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES, VELOCITIES");
        break;
      case BundleSettings::AnglesVelocityAcceleration:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES, VELOCITIES, ACCELERATIONS");
        break;
      case BundleSettings::AllPointingCoefficients:
        sprintf(buf,"\n                          CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)",m_bundleSettings->ckSolveDegree());
        break;
      case BundleSettings::NoPointingFactors:
        sprintf(buf,"\n                          CAMSOLVE: NONE");
        break;
    default:
      break;
    }
    fp_out << buf;
    m_bundleSettings->solveTwist() ? sprintf(buf, "\n                             TWIST: ON"):
        sprintf(buf, "\n                             TWIST: OFF");
    fp_out << buf;

    m_bundleSettings->fitInstrumentPointingPolynomialOverExisting() ? sprintf(buf, "\n POLYNOMIAL OVER EXISTING POINTING: ON"):
        sprintf(buf, "\nPOLYNOMIAL OVER EXISTING POINTING : OFF");
    fp_out << buf;

    sprintf(buf, "\n\nINPUT: SPACECRAFT OPTIONS\n=========================\n");
    fp_out << buf;


//??? this line could replace the switch/case below...    sprintf(buf, "\n                          SPSOLVE: %s", BundleSettings::instrumentPositionSolveOptionToString(m_bundleSettings->instrumentPositionSolveOption()).toUpper();
    switch (m_bundleSettings->instrumentPositionSolveOption()) {
      case BundleSettings::NoPositionFactors:
        sprintf(buf,"\n                        SPSOLVE: NONE");
        break;
      case BundleSettings::PositionOnly:
        sprintf(buf,"\n                        SPSOLVE: POSITION");
        break;
      case BundleSettings::PositionVelocity:
        sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES");
        break;
      case BundleSettings::PositionVelocityAcceleration:
        sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES, ACCELERATIONS");
        break;
      case BundleSettings::AllPositionCoefficients:
        sprintf(buf,"\n                       CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)",m_bundleSettings->spkSolveDegree());
        break;
      default:
        break;
    }
    fp_out << buf;

    m_bundleSettings->solveInstrumentPositionOverHermiteSpline() ? sprintf(buf, "\n POLYNOMIAL OVER HERMITE SPLINE: ON"):
        sprintf(buf, "\nPOLYNOMIAL OVER HERMITE SPLINE : OFF");
    fp_out << buf;

    sprintf(buf, "\n\nINPUT: GLOBAL IMAGE PARAMETER UNCERTAINTIES\n===========================================\n");
    fp_out << buf;
    (m_dGlobalLatitudeAprioriSigma == Isis::Null) ? sprintf(buf,"\n               POINT LATITUDE SIGMA: N/A"):
        sprintf(buf,"\n               POINT LATITUDE SIGMA: %lf (meters)", m_dGlobalLatitudeAprioriSigma);
    fp_out << buf;
    (m_dGlobalLongitudeAprioriSigma == Isis::Null) ? sprintf(buf,"\n              POINT LONGITUDE SIGMA: N/A"):
        sprintf(buf,"\n              POINT LONGITUDE SIGMA: %lf (meters)", m_dGlobalLongitudeAprioriSigma);
    fp_out << buf;
    (m_dGlobalRadiusAprioriSigma == Isis::Null) ? sprintf(buf,"\n                 POINT RADIUS SIGMA: N/A"):
        sprintf(buf,"\n                 POINT RADIUS SIGMA: %lf (meters)", m_dGlobalRadiusAprioriSigma);
    fp_out << buf;

    if (m_nNumberCamPosCoefSolved < 1 || m_dGlobalSpacecraftPositionAprioriSigma[0] == Isis::Null)
      sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: N/A");
    else
      sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: %lf (meters)",m_dGlobalSpacecraftPositionAprioriSigma[0]);
    fp_out << buf;

    if (m_nNumberCamPosCoefSolved < 2 || m_dGlobalSpacecraftPositionAprioriSigma[1] == Isis::Null)
      sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: N/A");
    else
      sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: %lf (m/s)",m_dGlobalSpacecraftPositionAprioriSigma[1]);
    fp_out << buf;

    if (m_nNumberCamPosCoefSolved < 3 || m_dGlobalSpacecraftPositionAprioriSigma[2] == Isis::Null)
      sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: N/A");
    else
      sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: %lf (m/s/s)",m_dGlobalSpacecraftPositionAprioriSigma[2]);
    fp_out << buf;

    if (m_nNumberCamAngleCoefSolved < 1 || m_dGlobalCameraAnglesAprioriSigma[0] == Isis::Null)
      sprintf(buf,"\n                CAMERA ANGLES SIGMA: N/A");
    else
      sprintf(buf,"\n                CAMERA ANGLES SIGMA: %lf (dd)",m_dGlobalCameraAnglesAprioriSigma[0]);
    fp_out << buf;

    if (m_nNumberCamAngleCoefSolved < 2 || m_dGlobalCameraAnglesAprioriSigma[1] == Isis::Null)
      sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: N/A");
    else
      sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: %lf (dd/s)",m_dGlobalCameraAnglesAprioriSigma[1]);
    fp_out << buf;

    if (m_nNumberCamAngleCoefSolved < 3 || m_dGlobalCameraAnglesAprioriSigma[2] == Isis::Null)
      sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: N/A");
    else
      sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: %lf (dd/s/s)",m_dGlobalCameraAnglesAprioriSigma[2]);
    fp_out << buf;
*/

    if (m_bundleSettings->solveTargetBody()) {
      sprintf(buf, "\n\nINPUT: TARGET BODY OPTIONS\n==============================\n");
      fp_out << buf;

      if (m_bundleTargetBody->solvePoleRA() && m_bundleTargetBody->solvePoleDec()) {
        sprintf(buf,"\n                             POLE: RIGHT ASCENSION");
        fp_out << buf;
        sprintf(buf,"\n                                 : DECLINATION\n");
        fp_out << buf;
      }
      else if (m_bundleTargetBody->solvePoleRA()) {
        sprintf(buf,"\n                             POLE: RIGHT ASCENSION\n");
        fp_out << buf;
      }
      else if (m_bundleTargetBody->solvePoleDec()) {
        sprintf(buf,"\n                             POLE: DECLINATION\n");
        fp_out << buf;
      }

      if (m_bundleTargetBody->solvePM() || m_bundleTargetBody->solvePMVelocity()
          || m_bundleTargetBody->solvePMAcceleration()) {
        sprintf(buf,"\n                   PRIME MERIDIAN: W0 (OFFSET)");
        fp_out << buf;

        if (m_bundleTargetBody->solvePMVelocity()) {
          sprintf(buf,"\n                                 : WDOT (SPIN RATE)");
          fp_out << buf;
        }
        if (m_bundleTargetBody->solvePMAcceleration()) {
          sprintf(buf,"\n                               :W ACCELERATION");
          fp_out << buf;
        }
      }

      if (m_bundleTargetBody->solveTriaxialRadii() || m_bundleTargetBody->solveMeanRadius()) {
        if (m_bundleTargetBody->solveMeanRadius()) {
          sprintf(buf,"\n                          RADII: MEAN");
          fp_out << buf;
        }
        else if (m_bundleTargetBody->solveTriaxialRadii()) {
          sprintf(buf,"\n                          RADII: TRIAXIAL");
          fp_out << buf;
        }
      }
    }

    sprintf(buf, "\n\nJIGSAW: RESULTS\n===============\n");
    fp_out << buf;
    sprintf(buf, "\n                         Images: %6d",nImages);
    fp_out << buf;
    sprintf(buf, "\n                         Points: %6d",nValidPoints);
    fp_out << buf;

    sprintf(buf, "\n                 Total Measures: %6d",
                                     (m_bundleResults.numberObservations()
                                      + m_bundleResults.numberRejectedObservations()) / 2);
    fp_out << buf;

    sprintf(buf, "\n             Total Observations: %6d",
                                 m_bundleResults.numberObservations()
                                 + m_bundleResults.numberRejectedObservations());
    fp_out << buf;

    sprintf(buf, "\n              Good Observations: %6d", m_bundleResults.numberObservations());
    fp_out << buf;

    sprintf(buf, "\n          Rejected Observations: %6d",
                              m_bundleResults.numberRejectedObservations());
    fp_out << buf;

    if (m_bundleResults.numberConstrainedPointParameters() > 0) {
      sprintf(buf, "\n   Constrained Point Parameters: %6d",
                         m_bundleResults.numberConstrainedPointParameters());
      fp_out << buf;
    }

    if (m_bundleResults.numberConstrainedImageParameters() > 0) {
      sprintf(buf, "\n   Constrained Image Parameters: %6d",
                         m_bundleResults.numberConstrainedImageParameters());
      fp_out << buf;
    }

    if (m_bundleResults.numberConstrainedTargetParameters() > 0) {
      sprintf(buf, "\n  Constrained Target Parameters: %6d",
                         m_bundleResults.numberConstrainedTargetParameters());
      fp_out << buf;
    }

    sprintf(buf, "\n                       Unknowns: %6d",
                                           m_bundleResults.numberUnknownParameters());
    fp_out << buf;

    if (nInnerConstraints > 0) {
      sprintf(buf, "\n      Inner Constraints: %6d", nInnerConstraints);
     fp_out << buf;
    }

    if (nDistanceConstraints > 0) {
      sprintf(buf, "\n   Distance Constraints: %d", nDistanceConstraints);
      fp_out << buf;
    }

    sprintf(buf, "\n             Degrees of Freedom: %6d", nDegreesOfFreedom);
    fp_out << buf;

    sprintf(buf, "\n           Convergence Criteria: %6.3g",
                               m_bundleSettings->convergenceCriteriaThreshold());
    fp_out << buf;

    if (nConvergenceCriteria == 1) {
      sprintf(buf, "(Sigma0)");
      fp_out << buf;
    }

    sprintf(buf, "\n                     Iterations: %6d", m_nIteration);
    fp_out << buf;

    if (m_nIteration >= m_bundleSettings->convergenceCriteriaMaximumIterations()) {
      sprintf(buf, "(Maximum reached)");
      fp_out << buf;
    }

    sprintf(buf, "\n                         Sigma0: %30.20lf\n", m_bundleResults.sigma0());
    fp_out << buf;
    sprintf(buf, " Error Propagation Elapsed Time: %6.4lf (seconds)\n",
                   m_bundleResults.elapsedTimeErrorProp());
    fp_out << buf;
    sprintf(buf, "             Total Elapsed Time: %6.4lf (seconds)\n",
                               m_bundleResults.elapsedTime());
    fp_out << buf;
    if (m_bundleResults.numberObservations() + m_bundleResults.numberRejectedObservations()
         > 100) {  //if there was enough data to calculate percentiles and box plot data
      sprintf(buf, "\n           Residual Percentiles:\n");
      fp_out << buf;

    // residual prob distribution values are calculated/printed
    // even if there is no maximum likelihood estimation
      try {
        for (int bin = 1;bin < 34;bin++) {
          //double quan =
          //    m_bundleResults.residualsCumulativeProbabilityDistribution().value(double(bin)/100);
          double cumProb = double(bin) / 100.0;
          double resValue =
               m_bundleResults.residualsCumulativeProbabilityDistribution().value(cumProb);
          double resValue33 =
              m_bundleResults.residualsCumulativeProbabilityDistribution().value(cumProb + 0.33);
          double resValue66 =
              m_bundleResults.residualsCumulativeProbabilityDistribution().value(cumProb + 0.66);
          sprintf(buf, "                 Percentile %3d: %+8.3lf"
                       "                 Percentile %3d: %+8.3lf"
                       "                 Percentile %3d: %+8.3lf\n",
                                         bin,      resValue,
                                         bin + 33, resValue33,
                                         bin + 66, resValue66);
          fp_out << buf;
        }
      }
      catch (IException &e) {
        QString msg = "Faiiled to output residual percentiles for bundleout";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
      try {
        sprintf(buf, "\n              Residual Box Plot:");
        fp_out << buf;
        sprintf(buf, "\n                        minimum: %+8.3lf",
                m_bundleResults.residualsCumulativeProbabilityDistribution().min());
        fp_out << buf;
        sprintf(buf, "\n                     Quartile 1: %+8.3lf",
                m_bundleResults.residualsCumulativeProbabilityDistribution().value(0.25));
        fp_out << buf;
        sprintf(buf, "\n                         Median: %+8.3lf",
                m_bundleResults.residualsCumulativeProbabilityDistribution().value(0.50));
        fp_out << buf;
        sprintf(buf, "\n                     Quartile 3: %+8.3lf",
                m_bundleResults.residualsCumulativeProbabilityDistribution().value(0.75));
        fp_out << buf;
        sprintf(buf, "\n                        maximum: %+8.3lf\n",
                m_bundleResults.residualsCumulativeProbabilityDistribution().max());
        fp_out << buf;
      }
      catch (IException &e) {
        QString msg = "Faiiled to output residual box plot for bundleout";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }

    sprintf(buf, "\nIMAGE MEASURES SUMMARY\n==========================\n\n");
    fp_out << buf;

    int nMeasures;
    int nRejectedMeasures;
    int nUsed;

    for (int i = 0; i < nImages; i++) {
      // imageIndex(i) retrieves index into the normal equations matrix
      // for Image(i)
      double rmsSampleResiduals = m_bundleResults.rmsImageSampleResiduals()[i].Rms();
      double rmsLineResiduals = m_bundleResults.rmsImageLineResiduals()[i].Rms();
      double rmsLandSResiduals = m_bundleResults.rmsImageResiduals()[i].Rms();

      nMeasures = m_pCnet->GetNumberOfValidMeasuresInImage(m_pSnList->serialNumber(i));
      nRejectedMeasures =
          m_pCnet->GetNumberOfJigsawRejectedMeasuresInImage(m_pSnList->serialNumber(i));

      nUsed = nMeasures - nRejectedMeasures;

      if (nUsed == nMeasures) {
        sprintf(buf, "%s   %5d of %5d %6.3lf %6.3lf %6.3lf\n",
                m_pSnList->fileName(i).toLatin1().data(),
                (nMeasures-nRejectedMeasures), nMeasures,
                rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
      }
      else {
        sprintf(buf, "%s   %5d of %5d* %6.3lf %6.3lf %6.3lf\n",
                m_pSnList->fileName(i).toLatin1().data(),
                (nMeasures-nRejectedMeasures), nMeasures,
                rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
      }
      fp_out << buf;
    }

    return true;
  }


  /**
   * output bundle results to file with error propagation
   */
  bool BundleAdjust::outputText() {

    QString ofname = "bundleout.txt";
    ofname = m_bundleSettings->outputFilePrefix() + ofname;

    std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
    if (!fp_out) {
      return false;
    }

    char buf[1056];
    BundleObservation *observation = NULL;

    int nObservations = m_bundleObservations.size();

    outputHeader(fp_out);

    bool berrorProp = false;
    if (m_bundleResults.converged() && m_bundleSettings->errorPropagation()) {
      berrorProp = true;
    }

    // output target body header if solving for target
    if (m_bundleSettings->solveTargetBody()) {
      sprintf(buf, "\nTARGET BODY\n==========================\n");
      fp_out << buf;

      sprintf(buf, "\n   Target         Initial              Total               Final             Initial           Final\n"
              "Parameter         Value              Correction            Value             Accuracy          Accuracy\n");
      fp_out << buf;

      QString targetString =
          m_bundleTargetBody->formatBundleOutputString(berrorProp);
      fp_out << (const char*)targetString.toLatin1().data();
    }

    // output image exterior orientation header
    sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION\n==========================\n");
    fp_out << buf;

    QMap<QString, QStringList> imagesAndParameters;

    if (m_bundleSettings->solveTargetBody()) {
      imagesAndParameters.insert( "target", m_bundleTargetBody->parameterList() );
    }

    for (int i = 0; i < nObservations; i++) {

      //if ( m_bundleResults.numberHeldImages() > 0 && m_pHeldSnList->hasSerialNumber(m_pSnList->serialNumber(i)) )
      //    bHeld = true;

      observation = m_bundleObservations.at(i);
      if (!observation) {
        continue;
      }

      int nImages = observation->size();
      for (int j = 0; j < nImages; j++) {
        BundleImage *image = observation->at(j);
        sprintf(buf, "\nImage Full File Name: %s\n", image->fileName().toLatin1().data());
        fp_out << buf;
        sprintf(buf, "\nImage Serial Number: %s\n", image->serialNumber().toLatin1().data());
        fp_out << buf;
      }

      sprintf(buf, "\n    Image         Initial              Total               Final             Initial           Final\n"
              "Parameter         Value              Correction            Value             Accuracy          Accuracy\n");
      fp_out << buf;

      QString observationString =
          observation->formatBundleOutputString(berrorProp);
      fp_out << (const char*)observationString.toLatin1().data();

      // Build list of images and parameters for correlation matrix.
      foreach ( QString image, observation->imageNames() ) {
        imagesAndParameters.insert( image, observation->parameterList() );
      }
    }
        
    // Save list of images and their associated parameters for CorrelationMatrix to use in ice.
    m_bundleResults.setCorrMatImgsAndParams(imagesAndParameters);

    // Save list of images and their associated parameters for CorrelationMatrix to use in ice.
    m_bundleResults.setCorrMatImgsAndParams(imagesAndParameters);

    // output point uncertainty statistics if error propagation is on
    if (berrorProp) {
      sprintf(buf, "\n\n\nPOINTS UNCERTAINTY SUMMARY\n==========================\n\n");
      fp_out << buf;
      sprintf(buf, " RMS Sigma Latitude(m)%20.8lf\n",
              m_bundleResults.sigmaLatitudeStatisticsRms());
      fp_out << buf;
      sprintf(buf, " MIN Sigma Latitude(m)%20.8lf at %s\n",
              m_bundleResults.minSigmaLatitudeDistance().meters(),
              m_bundleResults.minSigmaLatitudePointId().toLatin1().data());
      fp_out << buf;
      sprintf(buf, " MAX Sigma Latitude(m)%20.8lf at %s\n\n",
              m_bundleResults.maxSigmaLatitudeDistance().meters(),
              m_bundleResults.maxSigmaLatitudePointId().toLatin1().data());
      fp_out << buf;
      sprintf(buf, "RMS Sigma Longitude(m)%20.8lf\n",
              m_bundleResults.sigmaLongitudeStatisticsRms());
      fp_out << buf;
      sprintf(buf, "MIN Sigma Longitude(m)%20.8lf at %s\n",
              m_bundleResults.minSigmaLongitudeDistance().meters(),
              m_bundleResults.minSigmaLongitudePointId().toLatin1().data());
      fp_out << buf;
      sprintf(buf, "MAX Sigma Longitude(m)%20.8lf at %s\n\n",
              m_bundleResults.maxSigmaLongitudeDistance().meters(),
              m_bundleResults.maxSigmaLongitudePointId().toLatin1().data());
      fp_out << buf;
      if ( m_bundleSettings->solveRadius() ) {
        sprintf(buf, "   RMS Sigma Radius(m)%20.8lf\n",
                m_bundleResults.sigmaRadiusStatisticsRms());
        fp_out << buf;
        sprintf(buf, "   MIN Sigma Radius(m)%20.8lf at %s\n",
                m_bundleResults.minSigmaRadiusDistance().meters(),
                m_bundleResults.minSigmaRadiusPointId().toLatin1().data());
        fp_out << buf;
        sprintf(buf, "   MAX Sigma Radius(m)%20.8lf at %s\n",
                m_bundleResults.maxSigmaRadiusDistance().meters(),
                m_bundleResults.maxSigmaRadiusPointId().toLatin1().data());
        fp_out << buf;
      }
      else {
        sprintf(buf, "   RMS Sigma Radius(m)                 N/A\n");
        fp_out << buf;
        sprintf(buf, "   MIN Sigma Radius(m)                 N/A\n");
        fp_out << buf;
        sprintf(buf, "   MAX Sigma Radius(m)                 N/A\n");
        fp_out << buf;
      }
    }

    // output point summary data header
    sprintf(buf, "\n\nPOINTS SUMMARY\n==============\n%103sSigma          Sigma              Sigma\n"
            "           Label         Status     Rays    RMS        Latitude       Longitude          Radius"
            "        Latitude       Longitude          Radius\n", "");
    fp_out << buf;

    int nPoints = m_bundleControlPoints.size();
    for (int i = 0; i < nPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_bundleControlPoints.at(i);

      QString pointSummaryString =
          bundleControlPoint->formatBundleOutputSummaryString(berrorProp);
      fp_out << (const char*)pointSummaryString.toLatin1().data();
    }

    // output point detail data header
    sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
    fp_out << buf;

    for (int i = 0; i < nPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_bundleControlPoints.at(i);

      QString pointDetailString =
          bundleControlPoint->formatBundleOutputDetailString(berrorProp, m_dRTM);
      fp_out << (const char*)pointDetailString.toLatin1().data();
    }

    fp_out.close();

    return true;
  }


  /**
   * output point data to csv file
   */
  bool BundleAdjust::outputPointsCSV() {
    char buf[1056];

    QString ofname = "bundleout_points.csv";
    ofname = m_bundleSettings->outputFilePrefix() + ofname;

    std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
    if (!fp_out) {
      return false;
    }

    int nPoints = m_bundleControlPoints.size();

    double dLat, dLon, dRadius;
    double dX, dY, dZ;
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    QString strStatus;
    double cor_lat_m;
    double cor_lon_m;
    double cor_rad_m;
    int nMeasures, nRejectedMeasures;
    double dResidualRms;

    // print column headers
    if (m_bundleSettings->errorPropagation()) {
      sprintf(buf, "Point,Point,Accepted,Rejected,Residual,3-d,3-d,3-d,Sigma,"
              "Sigma,Sigma,Correction,Correction,Correction,Coordinate,"
              "Coordinate,Coordinate\nID,,,,,Latitude,Longitude,Radius,"
              "Latitude,Longitude,Radius,Latitude,Longitude,Radius,X,Y,Z\n"
              "Label,Status,Measures,Measures,RMS,(dd),(dd),(km),(m),(m),(m),"
              "(m),(m),(m),(km),(km),(km)\n");
    }
    else {
      sprintf(buf, "Point,Point,Accepted,Rejected,Residual,3-d,3-d,3-d,"
              "Correction,Correction,Correction,Coordinate,Coordinate,"
              "Coordinate\n,,,,,Latitude,Longitude,Radius,Latitude,"
              "Longitude,Radius,X,Y,Z\nLabel,Status,Measures,Measures,"
              "RMS,(dd),(dd),(km),(m),(m),(m),(km),(km),(km)\n");
    }
    fp_out << buf;

    for (int i = 0; i < nPoints; i++) {
      BundleControlPointQsp bundlecontrolpoint = m_bundleControlPoints.at(i);

      const ControlPoint *point = bundlecontrolpoint->rawControlPoint();

      if (!point) {
        continue;
      }

      if (point->IsRejected()) {
        continue;
      }

      dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
      dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
      dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();
      dX = point->GetAdjustedSurfacePoint().GetX().kilometers();
      dY = point->GetAdjustedSurfacePoint().GetY().kilometers();
      dZ = point->GetAdjustedSurfacePoint().GetZ().kilometers();
      nMeasures = point->GetNumMeasures();
      nRejectedMeasures = point->GetNumberOfRejectedMeasures();
      dResidualRms = point->GetResidualRms();

      // point corrections and initial sigmas
      boost::numeric::ublas::bounded_vector< double, 3 > corrections = bundlecontrolpoint->corrections();
      cor_lat_m = corrections[0]*m_dRTM;
      cor_lon_m = corrections[1]*m_dRTM*cos(dLat*Isis::DEG2RAD);
      cor_rad_m  = corrections[2]*1000.0;

      if (point->GetType() == ControlPoint::Fixed) {
        strStatus = "FIXED";
      }
      else if (point->GetType() == ControlPoint::Constrained) {
        strStatus = "CONSTRAINED";
      }
      else if (point->GetType() == ControlPoint::Free) {
        strStatus = "FREE";
      }
      else {
        strStatus = "UNKNOWN";
      }

      if (m_bundleSettings->errorPropagation()) {
        dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
        dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
        dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();

        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
                     "%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                point->GetId().toLatin1().data(), strStatus.toLatin1().data(), nMeasures,
                nRejectedMeasures, dResidualRms, dLat, dLon, dRadius, dSigmaLat, dSigmaLong,
                dSigmaRadius, cor_lat_m, cor_lon_m, cor_rad_m, dX, dY, dZ);
      }
      else
        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
                     "%16.8lf,%16.8lf\n",
                point->GetId().toLatin1().data(), strStatus.toLatin1().data(), nMeasures,
                nRejectedMeasures, dResidualRms, dLat, dLon, dRadius, cor_lat_m, cor_lon_m,
                cor_rad_m, dX, dY, dZ);

      fp_out << buf;
    }

    fp_out.close();

    return true;
  }


  /**
   * output image coordinate residuals to comma-separated-value
   * file
   */
  bool BundleAdjust::outputResiduals() {
    char buf[1056];

    QString ofname = "residuals.csv";
    ofname = m_bundleSettings->outputFilePrefix() + ofname;

    std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
    if (!fp_out) {
      return false;
    }

    // output column headers
    sprintf(buf, ",,,x image,y image,Measured,Measured,sample,line,Residual Vector\n");
    fp_out << buf;
    sprintf(buf, "Point,Image,Image,coordinate,coordinate,"
                 "Sample,Line,residual,residual,Magnitude\n");
    fp_out << buf;
    sprintf(buf, "Label,Filename,Serial Number,(mm),(mm),"
                 "(pixels),(pixels),(pixels),(pixels),(pixels),Rejected\n");
    fp_out << buf;

    int nImageIndex;

    // printf("output residuals!!!\n");

    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored()) {
        continue;
      }

      int nObservations = point->GetNumMeasures();
      for (int j = 0; j < nObservations; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored()) {
          continue;
        }

        Camera *pCamera = measure->Camera();
        if (!pCamera) {
          continue;
        }

        // Determine the image index
        nImageIndex = m_pSnList->serialNumberIndex(measure->GetCubeSerialNumber());

        if (measure->IsRejected()) {
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,*\n",
                  point->GetId().toLatin1().data(),
                  m_pSnList->fileName(nImageIndex).toLatin1().data(),
                  m_pSnList->serialNumber(nImageIndex).toLatin1().data(),
                  measure->GetFocalPlaneMeasuredX(),
                  measure->GetFocalPlaneMeasuredY(),
                  measure->GetSample(),
                  measure->GetLine(),
                  measure->GetSampleResidual(),
                  measure->GetLineResidual(),
                  measure->GetResidualMagnitude());
        }
        else {
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                  point->GetId().toLatin1().data(),
                  m_pSnList->fileName(nImageIndex).toLatin1().data(),
                  m_pSnList->serialNumber(nImageIndex).toLatin1().data(),
                  measure->GetFocalPlaneMeasuredX(),
                  measure->GetFocalPlaneMeasuredY(),
                  measure->GetSample(),
                  measure->GetLine(),
                  measure->GetSampleResidual(),
                  measure->GetLineResidual(),
                  measure->GetResidualMagnitude());
        }
        fp_out << buf;
      }
    }

    fp_out.close();

    return true;
  }

  /**
   * output image data to csv file
   */
  bool BundleAdjust::outputImagesCSV() {
/*
    char buf[1056];

    QString ofname = "bundleout_images.csv";
    ofname = m_bundleSettings.outputFilePrefix() + ofname;

    std::ofstream fp_out(ofname.toLatin1().data(), std::ios::out);
    if (!fp_out)
      return false;

    // setup column headers
    std::vector<QString> output_columns;

    output_columns.push_back("Image,");

    output_columns.push_back("rms,");
    output_columns.push_back("rms,");
    output_columns.push_back("rms,");

    char strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
    std::ostringstream ostr;
    int ncoeff = 1;
    if (m_nNumberCamPosCoefSolved > 0)
      ncoeff = m_nNumberCamPosCoefSolved;

    for (int i = 0; i < ncoeff; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (ncoeff == 1)
          output_columns.push_back("X,");
        else {
          QString str = "X(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
    }
    strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
    for (int i = 0; i < ncoeff; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (ncoeff == 1)
          output_columns.push_back("Y,");
        else {
          QString str = "Y(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
    }
    strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
    for (int i = 0; i < ncoeff; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (ncoeff == 1) {
          output_columns.push_back("Z,");
        }
        else {
          QString str = "Z(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
      if (!m_bundleSettings->solveTwist()) {
        break;
      }
    }

    strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
    for (int i = 0; i < m_nNumberCamAngleCoefSolved; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (m_nNumberCamAngleCoefSolved == 1)
          output_columns.push_back("RA,");
        else {
          QString str = "RA(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
    }
    strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
    for (int i = 0; i < m_nNumberCamAngleCoefSolved; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (m_nNumberCamAngleCoefSolved == 1)
          output_columns.push_back("DEC,");
        else {
          QString str = "DEC(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
    }
    strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
    for (int i = 0; i < m_nNumberCamAngleCoefSolved; i++) {
      if (i == 0) {
        ostr << strcoeff;
      }
      else if (i == 1) {
        ostr << strcoeff << "t";
      }
      else {
        ostr << strcoeff << "t" << i;
      }
      for (int j = 0; j < 5; j++) {
        if (m_nNumberCamAngleCoefSolved == 1 || !m_bundleSettings->solveTwist()) {
          output_columns.push_back("TWIST,");
        }
        else {
          QString str = "TWIST(";
          str += ostr.str().c_str();
          str += "),";
          output_columns.push_back(str);
        }
      }
      ostr.str("");
      strcoeff--;
      if (!m_bundleSettings->solveTwist())
        break;
    }

    // print first column header to buffer and output to file
    int ncolumns = output_columns.size();
    for (int i = 0; i < ncolumns; i++) {
      QString str = output_columns.at(i);
      sprintf(buf, "%s", (const char*)str.toLatin1().data());
      fp_out << buf;
    }
    sprintf(buf, "\n");
    fp_out << buf;

    output_columns.clear();
    output_columns.push_back("Filename,");

    output_columns.push_back("sample res,");
    output_columns.push_back("line res,");
    output_columns.push_back("total res,");

    int nparams = 3;
    if (m_nNumberCamPosCoefSolved)
      nparams = 3 * m_nNumberCamPosCoefSolved;

    int numCameraAnglesSolved = 2;
    if (m_bundleSettings->solveTwist()) numCameraAnglesSolved++;
    nparams += numCameraAnglesSolved*m_nNumberCamAngleCoefSolved;
    if (!m_bundleSettings->solveTwist()) nparams += 1; // Report on twist only
    for (int i = 0; i < nparams; i++) {
      output_columns.push_back("Initial,");
      output_columns.push_back("Correction,");
      output_columns.push_back("Final,");
      output_columns.push_back("Apriori Sigma,");
      output_columns.push_back("Adj Sigma,");
    }

    // print second column header to buffer and output to file
    ncolumns = output_columns.size();
    for (int i = 0; i < ncolumns; i++) {
      QString str = output_columns.at(i);
      sprintf(buf, "%s", (const char*)str.toLatin1().data());
      fp_out << buf;
    }
    sprintf(buf, "\n");
    fp_out << buf;

    Camera *pCamera = NULL;
    SpicePosition *pSpicePosition = NULL;
    SpiceRotation *pSpiceRotation = NULL;

    int nImages = images();
    double dSigma = 0.;
    int nIndex = 0;
    //bool bHeld = false;
    std::vector<double> coefX(m_nNumberCamPosCoefSolved);
    std::vector<double> coefY(m_nNumberCamPosCoefSolved);
    std::vector<double> coefZ(m_nNumberCamPosCoefSolved);
    std::vector<double> coefRA(m_nNumberCamAngleCoefSolved);
    std::vector<double> coefDEC(m_nNumberCamAngleCoefSolved);
    std::vector<double> coefTWI(m_nNumberCamAngleCoefSolved);
    std::vector<double> angles;

    output_columns.clear();

    // data structure to contain adjusted image parameter sigmas for CHOLMOD error propagation only
    boost::numeric::ublas::vector<double> vImageAdjustedSigmas;

    std::vector<double> BFP(3);

    for (int i = 0; i < nImages; i++) {

      //if (m_bundleResults.numberHeldImages() > 0 &&
      //    m_pHeldSnList->hasSerialNumber(m_pSnList->serialNumber(i)) )
      //  bHeld = true;

      pCamera = m_pCnet->Camera(i);
      if (!pCamera)
        continue;

      // imageIndex(i) retrieves index into the normal equations matrix for
      //  Image(i)
      nIndex = imageIndex(i) ;

      if (m_bundleSettings->errorPropagation()
          && m_bundleResults.converged()
          && m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
        vImageAdjustedSigmas = m_Image_AdjustedSigmas.at(i);
      }

      pSpicePosition = pCamera->instrumentPosition();
      if (!pSpicePosition) {
        continue;
      }

      pSpiceRotation = pCamera->instrumentRotation();
      if (!pSpiceRotation) {
          continue;
      }

      // for frame cameras we directly retrieve the J2000 Exterior Pointing
      // (i.e. position and orientation angles). For others (linescan, radar)
      //  we retrieve the polynomial coefficients from which the Exterior
      // Pointing parameters are derived.
      if (m_bundleSettings->instrumentPositionSolveOption() > 0) {
        pSpicePosition->GetPolynomial(coefX, coefY, coefZ);
      }
      else { // not solving for position so report state at center of image
        std::vector <double> coordinate(3);
        coordinate = pSpicePosition->GetCenterCoordinate();

        coefX.push_back(coordinate[0]);
        coefY.push_back(coordinate[1]);
        coefZ.push_back(coordinate[2]);
      }

      if (m_bundleSettings->instrumentPointingSolveOption() > 0)
        pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
//          else { // frame camera
      else if (pCamera->GetCameraType() != 3) {
// This is for m_bundleSettings->instrumentPointingSolveOption() = BundleSettings::NoPointingFactors (except Radar which
// has no pointing) and no polynomial fit has occurred
        angles = pSpiceRotation->GetCenterAngles();
        coefRA.push_back(angles.at(0));
        coefDEC.push_back(angles.at(1));
        coefTWI.push_back(angles.at(2));
      }

      // clear column vector
      output_columns.clear();

      // add filename
      output_columns.push_back(m_pSnList->fileName(i).toLatin1().data());

      // add rms of sample, line, total image coordinate residuals
      output_columns.push_back(
          toString(m_bundleResults.rmsImageSampleResiduals()[i].Rms()));
      output_columns.push_back(
          toString(m_bundleResults.rmsImageLineResiduals()[i].Rms()));
      output_columns.push_back(
          toString(m_bundleResults.rmsImageResiduals()[i].Rms()));

      int nSigmaIndex = 0;
      if (m_nNumberCamPosCoefSolved > 0) {
        for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {

            if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
            }
          }

          output_columns.push_back(toString(coeQStringfX[0] - m_imageCorrections(nIndex)));
          output_columns.push_back(toString(m_imageCorrections(nIndex)));
          output_columns.push_back(toString(coefX[j]));
          output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            output_columns.push_back(toString(dSigma));
          }
          else {
            output_columns.push_back("N/A");
          }
          nIndex++;
          nSigmaIndex++;
        }
        for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
            }
          }

          output_columns.push_back(toString(coefY[0] - m_imageCorrections(nIndex)));
          output_columns.push_back(toString(m_imageCorrections(nIndex)));
          output_columns.push_back(toString(coefY[j]));
          output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            output_columns.push_back(
                toString(dSigma));
          }
          else {
            output_columns.push_back("N/A");
          }
          nIndex++;
          nSigmaIndex++;
        }
        for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
            }
          }

          output_columns.push_back(toString(coefZ[0] - m_imageCorrections(nIndex)));
          output_columns.push_back(toString(m_imageCorrections(nIndex)));
          output_columns.push_back(toString(coefZ[j]));
          output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            output_columns.push_back(toString(dSigma));
          }
          else {
            output_columns.push_back("N/A");
          }
          nIndex++;
          nSigmaIndex++;
        }
      }
      else {
        output_columns.push_back(toString(coefX[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back(toString(coefX[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back("N/A");
        output_columns.push_back(toString(coefY[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back(toString(coefY[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back("N/A");
        output_columns.push_back(toString(coefZ[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back(toString(coefZ[0]));
        output_columns.push_back(toString(0));
        output_columns.push_back("N/A");
      }

      if (m_nNumberCamAngleCoefSolved > 0) {
        for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
            }
          }

          output_columns.push_back(toString((coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG));
          output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
          output_columns.push_back(toString(coefRA[j] * RAD2DEG));
          output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            output_columns.push_back(toString(dSigma * RAD2DEG));
          }
          else {
            output_columns.push_back("N/A");
          }
          nIndex++;
          nSigmaIndex++;
        }
        for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
            }
            else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
            }
          }

          output_columns.push_back(toString((coefDEC[j] - m_imageCorrections(nIndex)) * RAD2DEG));
          output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
          output_columns.push_back(toString(coefDEC[j] * RAD2DEG));
          output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

          if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
            output_columns.push_back(toString(dSigma * RAD2DEG));
          }
          else {
            output_columns.push_back("N/A");
          }
          nIndex++;
          nSigmaIndex++;
        }
        if (!m_bundleSettings->solveTwist()) {
          output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
          output_columns.push_back(toString(0.0));
          output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
          output_columns.push_back(toString(0.0));
          output_columns.push_back("N/A");
        }
        else {
          for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

            if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
              if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              }
              else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
                dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleResults.sigma0();
              }
              else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleResults.sigma0();
              }
            }

            output_columns.push_back(toString((coefTWI[j] - m_imageCorrections(nIndex)) * RAD2DEG));
            output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
            output_columns.push_back(toString(coefTWI[j] * RAD2DEG));
            output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

            if (m_bundleSettings->errorPropagation() && m_bundleResults.converged()) {
              output_columns.push_back(toString(dSigma * RAD2DEG));
            }
            else {
              output_columns.push_back("N/A");
            }
            nIndex++;
            nSigmaIndex++;
          }
        }
      }

      else if (pCamera->GetCameraType() != 3) {
        output_columns.push_back(toString(coefRA[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back(toString(coefRA[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back("N/A");
        output_columns.push_back(toString(coefDEC[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back(toString(coefDEC[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back("N/A");
        output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back("N/A");
      }

      // print column vector to buffer and output to file
      int ncolumns = output_columns.size();
      for (int i = 0; i < ncolumns; i++) {
        QString str = output_columns.at(i);

        if (i < ncolumns- 1) {
          sprintf(buf, "%s,", (const char*)str.toLatin1().data());
        }
        else {
          sprintf(buf, "%s", (const char*)str.toLatin1().data());
        }
        fp_out << buf;
      }
      sprintf(buf, "\n");
      fp_out << buf;
    }

    fp_out.close();
*/
    return true;
  }





#if 0
  QString BundleAdjust::formatPolynomialTermString(bool padded, int coefficientIndex,
                                                   char coefficientConstant) {

    QString str = "";
    str += coefficientConstant;
//    QString str = QString::fromAscii(&coefficientConstant);

    QString padding = "";
    if (padded) {
      padding = " ";
    }

    if (coefficientIndex == 0) {
      str = padding + padding + str;
    }
    else if (coefficientIndex == 1) {
      str = padding + str + "t";
    }
    else {
      str = str + "t" + coefficientIndex;
    }
    return str;
  }


  double BundleAdjust::solveMethodSigma(int normalEquationIndex, int sigmaIndex, int imageIndex) {
    double sigma = 0.0;
    if (m_bundleSettings->solveMethod() == BundleSettings::OldSparse) {
      gmm::row_matrix< gmm::rsvector< double > > lsqCovMatrix = m_pLsq->GetCovarianceMatrix();
      sigma = sqrt((double)(lsqCovMatrix(normalEquationIndex, normalEquationIndex)));
    }
    else if (m_bundleSettings->solveMethod() == BundleSettings::Sparse) {
      vector <double> imageAdjustedSigmas = m_Image_AdjustedSigmas.at(imageIndex);
      sigma = sqrt(imageAdjustedSigmas[sigmaIndex]) * m_bundleResults.sigma0();
    }
    else if (m_bundleSettings->solveMethod() == BundleSettings::SpecialK) {
      sigma = sqrt((double)(m_Normals(normalEquationIndex, normalEquationIndex))) * m_bundleResults.sigma0();
    }
    return sigma;
  }
#endif
  bool BundleAdjust::isConverged() {
    return m_bundleResults.converged();
  }



  /**
   * Slot for deltack and jigsaw to output the bundle status.
   */
  void BundleAdjust::outputBundleStatus(QString status) {
    status += "\n";
    printf(status.toStdString().c_str());
  }



  /**
   * Compute Bundle statistics. 
   *  
   * Sets: 
   * m_rmsImageSampleResiduals 
   * m_rmsImageLineResiduals 
   * m_rmsImageResiduals 
   * 
   * m_rmsImageXSigmas     UNUSED???
   * m_rmsImageYSigmas     UNUSED???
   * m_rmsImageZSigmas     UNUSED???
   * m_rmsImageRASigmas    UNUSED???
   * m_rmsImageDECSigmas   UNUSED???
   * m_rmsImageTWISTSigmas UNUSED???
   *  
   * m_maxSigmaLatitude 
   * m_maxSigmaLatitudePointId 
   * m_maxSigmaLongitude 
   * m_maxSigmaLongitudePointId 
   * m_maxSigmaRadius 
   * m_maxSigmaRadiusPointId
   *                       
   * m_minSigmaLatitude   
   * m_minSigmaLatitudePointId  
   * m_minSigmaLongitude  
   * m_minSigmaLongitudePointId 
   * m_minSigmaRadius     
   * m_minSigmaRadiusPointId    
   *  
   * m_rmsSigmaLat
   * m_rmsSigmaLon
   * m_rmsSigmaRad
   *  
   *  
   *  Needed:
   * SerialNumberList *snList, ControlNet *cnet, bool errorPropagation,
   * bool solveRadius 
   *
   *
   */
  bool BundleAdjust::computeBundleStatistics() {

    // use qvectors so that we can set the size.
    // this will be useful later when adding data.
    // data may added out of index order
    int numberImages = m_pSnList->size();
    QVector<Statistics> rmsImageSampleResiduals(numberImages);
    QVector<Statistics> rmsImageLineResiduals(numberImages);
    QVector<Statistics> rmsImageResiduals(numberImages);

    int numObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < numObjectPoints; i++) {

      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored()) {
        continue;
      }

      if (point->IsRejected()) {
        continue;
      }

      int numMeasures = point->GetNumMeasures();
      for (int j = 0; j < numMeasures; j++) {

        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored()) {
          continue;
        }

        if (measure->IsRejected()) {
          continue;
        }

        double sampleResidual = fabs(measure->GetSampleResidual());
        double lineResidual = fabs(measure->GetLineResidual());

        // Determine the index for this measure's serial number
        int imageIndex = m_pSnList->serialNumberIndex(measure->GetCubeSerialNumber());

        // add residual data to the statistics object at the appropriate serial number index
        rmsImageSampleResiduals[imageIndex].AddData(sampleResidual);
        rmsImageLineResiduals[imageIndex].AddData(lineResidual);
        rmsImageResiduals[imageIndex].AddData(lineResidual);
        rmsImageResiduals[imageIndex].AddData(sampleResidual);
      }
    }


    if (m_bundleSettings->errorPropagation()) {

      // initialize lat/lon/rad boundaries
      Distance minSigmaLatDist;
      QString  minSigmaLatPointId = "";
      
      Distance maxSigmaLatDist;
      QString  maxSigmaLatPointId = "";
      
      Distance minSigmaLonDist;
      QString  minSigmaLonPointId = "";
      
      Distance maxSigmaLonDist;
      QString  maxSigmaLonPointId = "";
      
      Distance minSigmaRadDist;
      QString  minSigmaRadPointId = "";
      
      Distance maxSigmaRadDist;
      QString  maxSigmaRadPointId = "";
      
      // compute stats for point sigmas
      Statistics sigmaLatStats;
      Statistics sigmaLonStats;
      Statistics sigmaRadStats;

      Distance sigmaLatDist, sigmaLonDist, sigmaRadDist;

      int nPoints = m_pCnet->GetNumPoints();
      // initialize max and min values to those from first valid point
      for (int i = 0; i < nPoints; i++) {

        const ControlPoint *point = m_pCnet->GetPoint(i);
        if (point->IsIgnored()) {
          continue;
        }

        maxSigmaLatDist = point->GetAdjustedSurfacePoint().GetLatSigmaDistance();;
        minSigmaLatDist = maxSigmaLatDist;

        maxSigmaLonDist = point->GetAdjustedSurfacePoint().GetLonSigmaDistance();;
        minSigmaLonDist = maxSigmaLonDist;

        maxSigmaLatPointId = point->GetId();
        maxSigmaLonPointId = maxSigmaLatPointId;
        minSigmaLatPointId = maxSigmaLatPointId;
        minSigmaLonPointId = maxSigmaLatPointId;

        if (m_bundleSettings->solveRadius()) {
          maxSigmaRadDist = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma();
          minSigmaRadDist = maxSigmaRadDist;

          maxSigmaRadPointId = maxSigmaLatPointId;
          minSigmaRadPointId = maxSigmaLatPointId;
        }
        break;
      }

      for (int i = 0; i < nPoints; i++) {

        const ControlPoint *point = m_pCnet->GetPoint(i);
        if (point->IsIgnored()) {
          continue;
        }

        sigmaLatDist = point->GetAdjustedSurfacePoint().GetLatSigmaDistance();
        sigmaLonDist = point->GetAdjustedSurfacePoint().GetLonSigmaDistance();
        sigmaRadDist = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma();

        sigmaLatStats.AddData(sigmaLatDist.meters());
        sigmaLonStats.AddData(sigmaLonDist.meters());
        sigmaRadStats.AddData(sigmaRadDist.meters());

        if (sigmaLatDist > maxSigmaLatDist) {
          maxSigmaLatDist = sigmaLatDist;
          maxSigmaLatPointId = point->GetId();
        }
        if (sigmaLonDist > maxSigmaLonDist) {
          maxSigmaLonDist = sigmaLonDist;
          maxSigmaLonPointId = point->GetId();
        }
        if (m_bundleSettings->solveRadius()) {
          if (sigmaRadDist > maxSigmaRadDist) {
            maxSigmaRadDist = sigmaRadDist;
            maxSigmaRadPointId = point->GetId();
          }
        }
        if (sigmaLatDist < minSigmaLatDist) {
          minSigmaLatDist = sigmaLatDist;
          minSigmaLatPointId = point->GetId();
        }
        if (sigmaLonDist < minSigmaLonDist) {
          minSigmaLonDist = sigmaLonDist;
          minSigmaLonPointId = point->GetId();
        }
        if (m_bundleSettings->solveRadius()) {
          if (sigmaRadDist < minSigmaRadDist) {
            minSigmaRadDist = sigmaRadDist;
            minSigmaRadPointId = point->GetId();
          }
        }
      }

      // update bundle results 
      m_bundleResults.resizeSigmaStatisticsVectors(numberImages);

      m_bundleResults.setSigmaLatitudeRange(minSigmaLatDist, maxSigmaLatDist,
                                               minSigmaLatPointId, maxSigmaLatPointId);

      m_bundleResults.setSigmaLongitudeRange(minSigmaLonDist, maxSigmaLonDist,
                                                minSigmaLonPointId, maxSigmaLonPointId);

      m_bundleResults.setSigmaRadiusRange(minSigmaRadDist, maxSigmaRadDist,
                                             minSigmaRadPointId, maxSigmaRadPointId);

      m_bundleResults.setRmsFromSigmaStatistics(sigmaLatStats.Rms(),
                                                sigmaLonStats.Rms(),
                                                sigmaRadStats.Rms());
    }
    m_bundleResults.setRmsImageResidualLists(rmsImageLineResiduals.toList(),
                                                rmsImageSampleResiduals.toList(),
                                                rmsImageResiduals.toList());

    return true;
  }

}
