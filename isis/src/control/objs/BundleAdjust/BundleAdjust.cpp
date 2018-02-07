#include "BundleAdjust.h"

// std lib
#include <iomanip>
#include <iostream>
#include <sstream>

// qt lib
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMutex>

// boost lib
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

// Isis lib
#include "Application.h"
#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundlePolynomialContinuityConstraint.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "BundleSolutionInfo.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "Control.h"
#include "ControlPoint.h"
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


  /**
   * Custom error handler for CHOLMOD.
   * If CHOLMOD encounters an error then this will be called.
   *
   * @param status The CHOLMOD error status.
   * @param file The name of the source code file where the error occured.
   * @param lineNumber The line number in file where the error occured.
   * @param message The error message.
   */
  static void cholmodErrorHandler(int nStatus,
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


  /**
   * Construct a BundleAdjust object from the given settings, control network file,
   * and cube list.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param cnetFile The filename of the control network to be used.
   * @param cubeList The list of filenames of the cubes to be adjusted.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             const QString &cnetFile,
                             const QString &cubeList,
                             bool printSummary) {
    m_abort = false;
    Progress progress;
    // initialize constructor dependent settings...
    // m_printSummary, m_cleanUp, m_cnetFileName, m_controlNet,
    // m_serialNumberList, m_bundleSettings
    m_printSummary = printSummary;
    m_cleanUp = true;
    m_cnetFileName = cnetFile;
    try {
      m_controlNet = ControlNetQsp( new ControlNet(cnetFile, &progress) );
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);
    m_serialNumberList = new SerialNumberList(cubeList);
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init(&progress);
  }


  /**
   * Construct a BundleAdjust object with held cubes.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param cnetFile The filename of the control network to be used.
   * @param cubeList The list of filenames of the cubes to be adjusted.
   * @param heldList The list of filenames of the held cubes.  Held cubes must be in both
   *                 heldList and cubeList.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             QString &cnetFile,
                             SerialNumberList &snlist,
                             bool printSummary) {
    // initialize constructor dependent settings...
    // m_printSummary, m_cleanUp, m_cnetFileName, m_controlNet,
    // m_serialNumberList, m_bundleSettings
    m_abort = false;
    Progress progress;
    m_printSummary = printSummary;
    m_cleanUp = false;
    m_cnetFileName = cnetFile;
    try {
      m_controlNet = ControlNetQsp( new ControlNet(cnetFile, &progress) );
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);
    m_serialNumberList = &snlist;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  /**
   * Constructs a BundleAdjust object using a Control object.
   * A new control network object will be created as a copy of the Control's control network.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param cnet The Control object whose control network will be copied.
   *             The Control will not be modified by the BundleAdjust.
   * @param snlist A serial number list containing the cubes to be adjusted.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             Control &cnet,
                             SerialNumberList &snlist,
                             bool printSummary) {
    // initialize constructor dependent settings...
    // m_printSummary, m_cleanUp, m_cnetFileName, m_controlNet,
    // m_serialNumberList, m_bundleSettings
    m_abort = false;
    Progress progress;
    m_printSummary = printSummary;
    m_cleanUp = false;
    m_cnetFileName = cnet.fileName();
    try {
      m_controlNet = ControlNetQsp( new ControlNet(cnet.fileName(), &progress) );
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);
    m_serialNumberList = &snlist;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  /**
   * Constructs a BundleAdjust object using a ControlNet object.
   * A copy of the ControlNet will be used.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param cnet The ControlNet that will be copied.  The original ControlNet
   *             will not be modified.
   * @param snlist A serial number list containing the cubes to be adjusted.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             ControlNet &cnet,
                             SerialNumberList &snlist,
                             bool printSummary) {
    // initialize constructor dependent settings...
    // m_printSummary, m_cleanUp, m_cnetFileName, m_controlNet,
    // m_serialNumberList, m_bundleSettings
    m_abort = false;
    m_printSummary = printSummary;
    m_cleanUp = false;
    m_cnetFileName = "";
    try {
      m_controlNet = ControlNetQsp( new ControlNet(cnet) );
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);
    m_serialNumberList = &snlist;
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  /**
   * Constructs a BundleAdjust from an already created ControlNet within a shared pointer.
   *
   * @param bundleSettings QSharedPointer to the bundle settings to use.
   * @param cnet QSharedPointer to the control net to adjust.
   * @param cubeList QString name of list of cubes to create serial numbers for.
   * @param printSummary Boolean indicating whether to print application output summary.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             ControlNetQsp cnet,
                             const QString &cubeList,
                             bool printSummary) {
    m_abort = false;
    m_printSummary = printSummary;
    m_cleanUp = false;
    m_cnetFileName = "";
    try {
      m_controlNet = cnet;
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);
    m_serialNumberList = new SerialNumberList(cubeList);
    m_bundleSettings = bundleSettings;
    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    init();
  }


  /**
   * Thread safe constructor.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param control The Control object whose control network will be copied.
   *                The Control will not be modified by the BundleAdjust.
   * @param snlist A serial number list containing the cubes to be adjusted.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             Control &control,
                             QList<ImageList *> imgLists,
                             bool printSummary) {
    m_bundleSettings = bundleSettings;

    m_abort = false;
    Progress progress;
    try {
      m_controlNet = ControlNetQsp( new ControlNet(control.fileName(), &progress) );
    }
    catch (IException &e) {
      throw;
    }
    m_bundleResults.setOutputControlNet(m_controlNet);

    m_imageLists = imgLists;

    // this is too slow and we need to get rid of the serial number list anyway
    // should be unnecessary as Image class has serial number
    // could hang on to image list until creating BundleObservations?
    m_serialNumberList = new SerialNumberList;

    foreach (ImageList *imgList, imgLists) {
      foreach (Image *image, *imgList) {
        m_serialNumberList->add(image->fileName());
//      m_serialNumberList->add(image->serialNumber(), image->fileName());
      }
    }

    m_bundleTargetBody = bundleSettings->bundleTargetBody();

    m_printSummary = printSummary;

    m_cleanUp = false;
    m_cnetFileName = control.fileName();

    init();
  }


  /**
   * Destroys BundleAdjust object, deallocates pointers (if we have ownership),
   * and frees variables from cholmod library.
   *
   * @internal
   *   @history 2016-10-13 Ian Humphrey - Removed deallocation of m_pHeldSnList, since this
   *                           member was removed. References #4293.
   */
  BundleAdjust::~BundleAdjust() {
    // If we have ownership
    if (m_cleanUp) {
      delete m_serialNumberList;
    }

    freeCHOLMODLibraryVariables();

  }


  /**
   * Initialize all solution parameters. This method is called
   * by constructors to
   * <ul>
   *   <li> initialize member variables                            </li>
   *   <li> set up the control net                                 </li>
   *   <li> get the cameras set up for all images                  </li>
   *   <li> clear JigsawRejected flags                             </li>
   *   <li> create a new BundleImages and add to BundleObservation </li>
   *   <li> set up vector of BundleControlPoints                   </li>
   *   <li> set parent observation for each BundleMeasure          </li>
   *   <li> use BundleSettings to set more parameters              </li>
   *   <li> set up matrix initializations                          </li>
   *   <li> initialize cholomod library variables                  </li>
   * </ul>
   *
   * @param progress A pointer to the progress of creating the cameras.
   *
   * @throws IException::Programmer "In BundleAdjust::init(): image is null."
   * @throws IException::Programmer "In BundleAdjust::init(): observation is null."
   *
   * @internal
   *   @history 2011-08-14 Debbie A. Cook - Opt out of network validation
   *                           for deltack network in order to allow
   *                           a single measure on a point
   *   @history 2016-10-13 Ian Humphrey - Removed verification of held images in the from list
   *                           and counting of the number of held images. References #4293.
   *
   *   @todo remove printf statements
   *   @todo answer comments with questions, TODO, ???, and !!!
   */
  void BundleAdjust::init(Progress *progress) {

    m_previousNumberImagePartials = 0;

    // initialize
    //
    // JWB
    // - some of these not originally initialized.. better values???
    m_iteration = 0;
    m_radiansToMeters = 0.0;
    m_metersToRadians = 0.0;
    m_rank = 0;
    m_iterationSummary = "";

    // Get the cameras set up for all images
    // NOTE - THIS IS NOT THE SAME AS "setImage" as called in BundleAdjust::computePartials
    // this call only does initializations; sets measure's camera pointer, etc
    // RENAME????????????
    m_controlNet->SetImages(*m_serialNumberList, progress);

    // clear JigsawRejected flags
    m_controlNet->ClearJigsawRejected();

    // initialize held variables
    int numImages = m_serialNumberList->size();

    // matrix stuff
    m_normalInverse.clear();
    m_RHS.clear();
    m_imageSolution.clear();

    // we don't want to call initializeCHOLMODLibraryVariables() here since mRank=0
    // m_cholmodCommon, m_sparseNormals are not initialized
    m_L = NULL;
    m_cholmodNormal = NULL;
    m_cholmodTriplet = NULL;

    // should we initialize objects m_xResiduals, m_yResiduals, m_xyResiduals

    // (must be a smarter way)
    // get target body radii and body specific conversion factors between radians and meters.
    // need validity checks and different conversion factors for lat and long
    // initialize m_bodyRadii
    m_bodyRadii[0] = m_bodyRadii[1] = m_bodyRadii[2] = Distance();
    Camera *cnetCamera = m_controlNet->Camera(0);
    if (cnetCamera) {
      cnetCamera->radii(m_bodyRadii);  // meters

      if (m_bodyRadii[0] >= Distance(0, Distance::Meters)) {
        m_metersToRadians = 0.001 / m_bodyRadii[0].kilometers(); // at equator
        m_radiansToMeters = 1.0 / m_metersToRadians;
        m_bundleResults.setRadiansToMeters(m_radiansToMeters);
      }
    }

    // set up BundleObservations and assign solve settings for each from BundleSettings class
    // TODO: code below should go into a separate method? Maybe initBundleUtilities().
    int normalsMatrixStartBlock = 0;
      for (int i = 0; i < numImages; i++) {

        Camera *camera = m_controlNet->Camera(i);
        QString observationNumber = m_serialNumberList->observationNumber(i);
        QString instrumentId = m_serialNumberList->spacecraftInstrumentId(i);
        QString serialNumber = m_serialNumberList->serialNumber(i);
        QString fileName = m_serialNumberList->fileName(i);

        // create a new BundleImage and add to new (or existing if observation mode is on)
        // BundleObservation
        BundleImageQsp image = BundleImageQsp(new BundleImage(camera, serialNumber, fileName));

        if (!image) {
          QString msg = "In BundleAdjust::init(): image " + fileName + "is null." + "\n";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        BundleObservationQsp observation =
            m_bundleObservations.addNew(image, observationNumber, instrumentId, m_bundleSettings);

        if (!observation) {
          QString msg = "In BundleAdjust::init(): observation "
                        + observationNumber + "is null." + "\n";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        // the observation stores the index to its associated SparseBlockColumnMatrix in
        // m_sparseNormals
        // TODO: I (Ken E.) think the individual segments should somehow be storing the index to
        // their associated SparseBlockColumnMatrix
        observation->setNormalsMatrixStartBlock(normalsMatrixStartBlock);
        normalsMatrixStartBlock += observation->numberPolynomialSegments();

        // initialize piecewise polynomial continuity constraints for time-dependent sensors if
        // necessary
        // TODO: can we let BundleObservation handle this?
        if (observation->numberPolynomialPositionSegments() > 1
            || observation->numberPolynomialPointingSegments() > 1) {

          BundlePolynomialContinuityConstraintQsp polyConstraint =
              BundlePolynomialContinuityConstraintQsp(
                new BundlePolynomialContinuityConstraint(observation));
          observation->setContinuityConstraints(polyConstraint);
        }
      }

      if (m_bundleSettings->solveTargetBody()) {
        m_bundleObservations.initializeBodyRotation();
      }

      // set up vector of BundleControlPoints
      int numControlPoints = m_controlNet->GetNumPoints();
      for (int i = 0; i < numControlPoints; i++) {
        ControlPoint *point = m_controlNet->GetPoint(i);
        if (point->IsIgnored()) {
          continue;
        }

        BundleControlPointQsp bundleControlPoint(new BundleControlPoint(point));
        m_bundleControlPoints.append(bundleControlPoint);

        bundleControlPoint->setWeights(m_bundleSettings, m_metersToRadians);

        // set parent observation for each BundleMeasure

        int numMeasures = bundleControlPoint->size();
        for (int j = 0; j < numMeasures; j++) {
          BundleMeasureQsp measure = bundleControlPoint->at(j);
          QString cubeSerialNumber = measure->cubeSerialNumber();

          BundleObservationQsp observation =
              m_bundleObservations.observationByCubeSerialNumber(cubeSerialNumber);
          BundleImageQsp image = observation->imageByCubeSerialNumber(cubeSerialNumber);

          measure->setParentObservation(observation);
          measure->setParentImage(image);
        }
      }

    //===========================================================================================//
    //==== Use the bundle settings to initialize more member variables and set up solutions =====//
    //===========================================================================================//

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

    int numberContinuityConstraints = m_bundleObservations.numberContinuityConstraintEquations();
    m_bundleResults.setNumberContinuityConstraintEquations(numberContinuityConstraints);

    //===========================================================================================//
    //=============== End Bundle Settings =======================================================//
    //===========================================================================================//

    //===========================================================================================//
    //======================== initialize matrices and more parameters ==========================//
    //===========================================================================================//

    // size of reduced normals matrix

    // TODO
    // this should be determined from BundleSettings
    // m_rank will be the sum of observation, target, and self-cal parameters
    // TODO
    m_rank = m_bundleObservations.numberParameters();

    if (m_bundleSettings->solveTargetBody()) {
      m_rank += m_bundleSettings->numberTargetBodyParameters();
    }

    int num3DPoints = m_bundleControlPoints.size();

    m_bundleResults.setNumberUnknownParameters(m_rank + 3 * num3DPoints);

    m_imageSolution.resize(m_rank);

    // initializations for cholmod
    initializeCHOLMODLibraryVariables();

    // initialize normal equations matrix
    initializeNormalEquationsMatrix();
  }


  /**
   * control network validation - on the very real chance that the net
   * has not been checked before running the bundle
   *
   * checks implemented for ...
   *  (1) images with 0 or 1 measures
   *
   * @return bool If the control network is valid.
   *
   * @throws IException::User "Images with one or less measures:"
   *
   * @internal
   *   @history  2011-08-04 Debbie A. Cook - Changed error message to
   *                            indicate it fails with one measure as
   *                            well as no measures.
   *   @history 2016-09-22 Ian Humphrey - Replaced statusUpdate signal emits with direct
   *                           calls to outputBundleStats() so the validation messages are
   *                           printed to stdout. References #4313.
   */
  bool BundleAdjust::validateNetwork() {
    outputBundleStatus("\nValidating network...");

    int imagesWithInsufficientMeasures = 0;
    QString msg = "Images with one or less measures:\n";
    int numObservations = m_bundleObservations.size();
    for (int i = 0; i < numObservations; i++) {
      int numImages = m_bundleObservations.at(i)->size();
      for (int j = 0; j < numImages; j++) {
        BundleImageQsp bundleImage = m_bundleObservations.at(i)->at(j);
        int numMeasures = m_controlNet->
                              GetNumberOfValidMeasuresInImage(bundleImage->serialNumber());

        if (numMeasures > 1) {
          continue;
        }

        imagesWithInsufficientMeasures++;
        msg += bundleImage->fileName() + ": " + toString(numMeasures) + "\n";
      }
    }

    if ( imagesWithInsufficientMeasures > 0 ) {
      throw IException(IException::User, msg, _FILEINFO_);
    }

    outputBundleStatus("Validation complete!...");

    return true;
  }


  /**
   * Initializations for CHOLMOD sparse matrix package.
   * Calls cholmod_start, sets m_cholmodCommon options.
   *
   * @return bool If the CHOLMOD library variables were successfully initialized.
   */
  bool BundleAdjust::initializeCHOLMODLibraryVariables() {
    if ( m_rank <= 0 ) {
      return false;
    }

    m_cholmodTriplet = NULL;

    cholmod_start(&m_cholmodCommon);

    // set user-defined cholmod error handler
    m_cholmodCommon.error_handler = cholmodErrorHandler;

    // testing not using metis
    m_cholmodCommon.nmethods = 1;
    m_cholmodCommon.method[0].ordering = CHOLMOD_AMD;

    return true;
  }


  /**
   * Initialize Normal Equations matrix (m_sparseNormals).
   *
   * @return bool.
   *
   * @todo Ken We are explicitly setting the start column for each SparseBlockColumn in the normal
   *           equations matrix below. Is it possible to make the m_sparseNormals matrix smart
   *           enough to set the start column of a column block automatically when it is added?
   *
   */
  bool BundleAdjust::initializeNormalEquationsMatrix() {

    int nBlockColumns = m_bundleObservations.numberPolynomialSegments();

    if (m_bundleSettings->solveTargetBody())
      nBlockColumns += 1;

    m_sparseNormals.setNumberOfColumns(nBlockColumns);

    m_sparseNormals.at(0)->setStartColumn(0);

    int nParameters = 0;
    int blockColumn = 0;
    if (m_bundleSettings->solveTargetBody()) {
      nParameters += m_bundleSettings->numberTargetBodyParameters();
      blockColumn = 1;
    }

    for (int i = 0; i < m_bundleObservations.size(); i++) {
      int positionParameters =
          m_bundleObservations.at(i)->numberPositionParametersPerSegment();

      int pointingParameters =
          m_bundleObservations.at(i)->numberPointingParametersPerSegment();

      int positionSegments = m_bundleObservations.at(i)->numberPolynomialPositionSegments();
      for (int j = 0; j < positionSegments; j++) {
        m_sparseNormals.at(blockColumn)->setStartColumn(nParameters);
        m_sparseNormals.at(blockColumn)->setObservationIndex(i);
        nParameters += positionParameters;
        blockColumn++;
      }
      int pointingSegments = m_bundleObservations.at(i)->numberPolynomialPointingSegments();
      for (int j = 0; j < pointingSegments; j++) {
        m_sparseNormals.at(blockColumn)->setStartColumn(nParameters);
        m_sparseNormals.at(blockColumn)->setObservationIndex(i);
        nParameters += pointingParameters;
        blockColumn++;
      }
    }

    return true;
  }


  /**
   * Initialize Normal Equations matrix (m_sparseNormals).
   *
   * @return bool.
   *
   * @todo Ken We are explicitly setting the start column for each SparseBlockColumn in the normal
   *           equations matrix below. Is it possible to make the m_sparseNormals matrix smart
   *           enough to set the start column of a column block automatically when it is added?
   *
   */
/*
  bool BundleAdjust::initializeNormalEquationsMatrix() {

    int nBlockColumns = m_bundleObservations.numberPolynomialSegments();

    if (m_bundleSettings->solveTargetBody())
      nBlockColumns += 1;

    m_sparseNormals.setNumberOfColumns(nBlockColumns);

    m_sparseNormals.at(0)->setStartColumn(0);

    int nParameters = 0;
    if (m_bundleSettings->solveTargetBody()) {
      nParameters += m_bundleSettings->numberTargetBodyParameters();
      m_sparseNormals.at(1)->setStartColumn(nParameters);

      int observation = 0;
      for (int i = 2; i < nBlockColumns; i++) {
        nParameters += m_bundleObservations.at(observation)->numberParameters();
        m_sparseNormals.at(i)->setStartColumn(nParameters);
        observation++;
      }
    }
    else {
      int blockColumn = 0;
      for (int i = 0; i < m_bundleObservations.size(); i++) {
        int positionParameters =
            m_bundleObservations.at(i)->numberPositionParametersPerSegment();

        int pointingParameters =
            m_bundleObservations.at(i)->numberPointingParametersPerSegment();

        int positionSegments = m_bundleObservations.at(i)->numberPolynomialPositionSegments();
        for (int j = 0; j < positionSegments; j++) {
          m_sparseNormals.at(blockColumn)->setStartColumn(nParameters);
          m_sparseNormals.at(blockColumn)->setObservationIndex(i);
          nParameters += positionParameters;
          blockColumn++;
        }
        int pointingSegments = m_bundleObservations.at(i)->numberPolynomialPointingSegments();
        for (int j = 0; j < pointingSegments; j++) {
          m_sparseNormals.at(blockColumn)->setStartColumn(nParameters);
          m_sparseNormals.at(blockColumn)->setObservationIndex(i);
          nParameters += pointingParameters;
          blockColumn++;
        }
      }
    }

    return true;
  }
*/

  /**
   * @brief Free CHOLMOD library variables.
   *
   * Frees m_cholmodTriplet, m_cholmodNormal, and m_L.
   * Calls cholmod_finish when complete.
   *
   * @return bool If the CHOLMOD library successfully cleaned up.
   */
  bool BundleAdjust::freeCHOLMODLibraryVariables() {

    cholmod_free_triplet(&m_cholmodTriplet, &m_cholmodCommon);
    cholmod_free_sparse(&m_cholmodNormal, &m_cholmodCommon);
    cholmod_free_factor(&m_L, &m_cholmodCommon);

    cholmod_finish(&m_cholmodCommon);

    return true;
  }


  /**
   * Compute the least squares bundle adjustment solution using Cholesky decomposition.
   *
   * @return BundleSolutionInfo A container with settings and results from the adjustment.
   *
   * @see BundleAdjust::solveCholesky
   *
   * @todo make solveCholesky return a BundleSolutionInfo object and delete this placeholder ???
   */
  BundleSolutionInfo BundleAdjust::solveCholeskyBR() {
    solveCholesky();
    return bundleSolveInformation();
  }


  /**
   * Flag to abort when bundle is threaded. Flag is set outside the bundle thread,
   * typically by the gui thread.
   */
  void BundleAdjust::abortBundle() {
    m_abort = true;
  }


  /**
   * Compute the least squares bundle adjustment solution using Cholesky decomposition.
   *
   * @return bool If the solution was successfully computed.
   *
   * @internal
   *   @history 2016-10-25 Ian Humphrey - Spacing and precision for Sigma0 and Elapsed Time match
   *                           ISIS production's jigsaw std output. References #4463."
   *   @history 2016-10-28 Ian Humphrey - Updated spacing for Error Propagation Complete message.
   *                           References #4463."
   *   @history 2016-11-16 Ian Humphrey - Modified catch block to throw the caught exception, so
   *                           a message box will appear to the user when running jigsaw in GUI
   *                           mode. Fixes #4483.
   */
  bool BundleAdjust::solveCholesky() {
    try {

      // throw error if a frame camera is included AND
      // if m_bundleSettings->solveInstrumentPositionOverHermiteSpline()
      // is set to true (can only use for line scan or radar)
  //    if (m_bundleSettings->solveInstrumentPositionOverHermiteSpline() == true) {
  //      int numImages = images();
  //      for (int i = 0; i < numImages; i++) {
  //        if (m_controlNet->Camera(i)->GetCameraType() == 0) {
  //          QString msg = "At least one sensor is a frame camera. "
  //                        "Spacecraft Option OVERHERMITE is not valid for frame cameras\n";
  //          throw IException(IException::User, msg, _FILEINFO_);
  //        }
  //      }
  //    }

      // Compute the apriori lat/lons for each nonheld point
      m_controlNet->ComputeApriori(); // original location

      // ken testing - if solving for target mean radius, set point radius to current mean radius
      // if solving for triaxial radii, set point radius to local radius
      // TODO: can we do this in the init() method?
      if (m_bundleTargetBody && m_bundleTargetBody->solveMeanRadius()) {
        int numControlPoints = m_bundleControlPoints.size();
        for (int i = 0; i < numControlPoints; i++) {
          BundleControlPointQsp point = m_bundleControlPoints.at(i);
          SurfacePoint surfacepoint = point->adjustedSurfacePoint();

          surfacepoint.ResetLocalRadius(m_bundleTargetBody->meanRadius());

          point->setAdjustedSurfacePoint(surfacepoint);
        }
      }

      if (m_bundleTargetBody && m_bundleTargetBody->solveTriaxialRadii()) {
        int numControlPoints = m_bundleControlPoints.size();
        for (int i = 0; i < numControlPoints; i++) {
          BundleControlPointQsp point = m_bundleControlPoints.at(i);
          SurfacePoint surfacepoint = point->adjustedSurfacePoint();

          Distance localRadius = m_bundleTargetBody->localRadius(surfacepoint.GetLatitude(),
                                                                 surfacepoint.GetLongitude());
          surfacepoint.ResetLocalRadius(localRadius);

          point->setAdjustedSurfacePoint(surfacepoint);
        }
      }

      m_iteration = 1;
      double vtpv = 0.0;
      double previousSigma0 = 0.0;

      // Set up formatting for status updates with doubles (e.g. Sigma0, Elapsed Time)
      int fieldWidth = 20;
      char format = 'f';
      int precision = 10;

      // start the clock
      clock_t solveStartClock = clock();

      for (;;) {

        emit iterationUpdate(m_iteration, m_bundleResults.sigma0());

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        emit statusUpdate( QString("starting iteration %1\n").arg(m_iteration) );

        clock_t iterationStartClock = clock();

        // zero normals (after iteration 0)
        if (m_iteration != 1) {
          m_sparseNormals.zeroBlocks();
        }

//        clock_t formNormalsClock1 = clock();
        // form normal equations
        if (!formNormalEquations()) {
          m_bundleResults.setConverged(false);
          break;
        }
//        clock_t formNormalsClock2 = clock();

//        double formNormalsTime = (formNormalsClock2 - formNormalsClock1)
//            / (double)CLOCKS_PER_SEC;

//        qDebug() << "BundleAdjust::formNormalEquations() elapsed time: " << formNormalsTime;

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        // solve the system
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

//        clock_t applyParameterCorrectionsClock1 = clock();
        // apply parameter corrections
        applyParameterCorrections();
//        clock_t applyParameterCorrectionsClock2 = clock();

//        double applyParameterCorrectionsTime = (applyParameterCorrectionsClock2 - applyParameterCorrectionsClock1)
//            / (double)CLOCKS_PER_SEC;

//        qDebug() << "BundleAdjust::applyParameterCorrections() elapsed time: " << applyParameterCorrectionsTime;

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

//        clock_t computeResidualsClock1 = clock();
        // compute residuals
        vtpv = computeResiduals();
//        clock_t computeResidualsClock2 = clock();

//        double computeResidualsTime = (computeResidualsClock2 - computeResidualsClock1)
//            / (double)CLOCKS_PER_SEC;

//        qDebug() << "BundleAdjust::computeResiduals() elapsed time: " << computeResidualsTime;

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

        // compute sigma0
        // (also called variance of unit weight, reference variance, variance factor, etc.)
        m_bundleResults.computeSigma0(vtpv, m_bundleSettings->convergenceCriteria());

        emit statusUpdate(QString("Iteration: %1")
                                  .arg(m_iteration));
        emit statusUpdate(QString("Sigma0: %1")
                                  .arg(m_bundleResults.sigma0(),
                                       fieldWidth,
                                       format,
                                       precision));
        emit statusUpdate(QString("Observations: %1")
                                  .arg(m_bundleResults.numberObservations()));
        emit statusUpdate(QString("Constrained Parameters:%1")
                                  .arg(m_bundleResults.numberConstrainedPointParameters()));
        if (m_bundleResults.numberContinuityConstraintEquations() > 0) {
          emit statusUpdate(QString("Continuity Constraint Equations:%1")
                                    .arg(m_bundleResults.numberContinuityConstraintEquations()));
        }
        emit statusUpdate(QString("Constrained Parameters:%1")
                                  .arg(m_bundleResults.numberConstrainedPointParameters()));
        emit statusUpdate(QString("Unknowns: %1")
                                  .arg(m_bundleResults.numberUnknownParameters()));
        emit statusUpdate(QString("Degrees of Freedom: %1")
                                  .arg(m_bundleResults.degreesOfFreedom()));
        emit iterationUpdate(m_iteration, m_bundleResults.sigma0());

        // check for convergence
        if (m_bundleSettings->convergenceCriteria() == BundleSettings::Sigma0) {
          if (fabs(previousSigma0 - m_bundleResults.sigma0())
                <= m_bundleSettings->convergenceCriteriaThreshold()) {
            // convergence detected

            // if maximum likelihood tiers are being processed,
            // check to see if there's another tier to go.
            if (m_bundleResults.maximumLikelihoodModelIndex()
                   < m_bundleResults.numberMaximumLikelihoodModels() - 1
                && m_bundleResults.maximumLikelihoodModelIndex()
                     < 2) {
              // TODO is this second condition redundant???
              // should BundleResults require num models <= 3, so num models - 1 <= 2
              if (m_bundleResults.numberMaximumLikelihoodModels()
                      > m_bundleResults.maximumLikelihoodModelIndex() + 1) {

                // If there is another tier left we will increment the index.
                m_bundleResults.incrementMaximumLikelihoodModelIndex();
              }
            }
            else { // otherwise iterations are complete
              m_bundleResults.setConverged(true);
              emit statusUpdate("Bundle has converged\n");
              break;
            }
          }
        }
        else {
          // bundleSettings.convergenceCriteria() == BundleSettings::ParameterCorrections
          int numConvergedParams = 0;
          int numImgParams = m_imageSolution.size();
          for (int ij = 0; ij < numImgParams; ij++) {
            if (fabs(m_imageSolution(ij)) > m_bundleSettings->convergenceCriteriaThreshold()) {
              break;
            }
            else
              numConvergedParams++;
          }

          if ( numConvergedParams == numImgParams ) {
            m_bundleResults.setConverged(true);
            emit statusUpdate("Bundle has converged");
            break;
          }
        }

        m_bundleResults.printMaximumLikelihoodTierInformation();
        clock_t iterationStopClock = clock();
        double iterationTime = (iterationStopClock - iterationStartClock)
                                / (double)CLOCKS_PER_SEC;
        emit statusUpdate( QString("End of Iteration %1").arg(m_iteration) );
        emit statusUpdate( QString("Elapsed Time: %1").arg(iterationTime,
                                                           fieldWidth,
                                                           format,
                                                           precision) );

        // check for maximum iterations
        if (m_iteration >= m_bundleSettings->convergenceCriteriaMaximumIterations()) {
          break;
        }

        // restart the dynamic calculation of the cumulative probility distribution of residuals
        // (in unweighted pixels) --so it will be up to date for the next iteration
        if (!m_bundleResults.converged()) {
          m_bundleResults.initializeResidualsProbabilityDistribution(101);
        }
        // TODO: is this necessary ???
        // probably all ready initialized to 101 nodes in bundle settings constructor...

        // if we're using CHOLMOD and still going, release cholmod_factor
        // (if we don't, memory leaks will occur), otherwise we need it for error propagation
        if (!m_bundleResults.converged() || !m_bundleSettings->errorPropagation()) {
          cholmod_free_factor(&m_L, &m_cholmodCommon);
        }

        iterationSummary();

        m_iteration++;

        previousSigma0 = m_bundleResults.sigma0();
      } // end of bundle iteration loop

      if (m_bundleResults.converged() && m_bundleSettings->errorPropagation()) {
        clock_t errorPropStartClock = clock();
        printf("Starting Error Propagation");
        errorPropagation();
        emit statusUpdate("\n\nError Propagation Complete");
        clock_t errorPropStopClock = clock();
        m_bundleResults.setElapsedTimeErrorProp((errorPropStopClock - errorPropStartClock)
                                                / (double)CLOCKS_PER_SEC);
      }

      clock_t solveStopClock = clock();
      m_bundleResults.setElapsedTime((solveStopClock - solveStartClock)
                                     / (double)CLOCKS_PER_SEC);

      wrapUp();

      m_bundleResults.setIterations(m_iteration);
      m_bundleResults.setObservations(m_bundleObservations);
      m_bundleResults.setBundleControlPoints(m_bundleControlPoints);

      BundleSolutionInfo *results = new BundleSolutionInfo(bundleSolveInformation());
      emit resultsReady(results);

      emit statusUpdate("\nBundle Complete");

      iterationSummary();
    }
    catch (IException &e) {
      m_bundleResults.setConverged(false);
      emit statusUpdate("\n aborting...");
      emit finished();
      QString msg = "Could not solve bundle adjust.";
      throw IException(e, e.errorType(), msg, _FILEINFO_);
    }

    emit finished();
    return true;
  }


  /**
   * Create a BundleSolutionInfo containing the settings and results from the bundle adjustment.
   *
   * @return BundleSolutionInfo A container with solve information from the adjustment.
   */
  BundleSolutionInfo BundleAdjust::bundleSolveInformation() {
    BundleSolutionInfo results(m_bundleSettings, FileName(m_cnetFileName), m_bundleResults, imageLists());
    results.setRunTime("");
    return results;
  }


  /**
   * Form the least-squares normal equations matrix.
   * Each BundleControlPoint stores its Q matrix and NIC vector.
   * The covariance matrix for each point will be stored in its adjusted surface point.
   *
   * @return bool
   *
   * @see BundleAdjust::formMeasureNormals
   * @see BundleAdjust::formPointNormals
   * @see BundleAdjust::formWeightedNormals
   */
  bool BundleAdjust::formNormalEquations() {
    bool status = false;

    m_bundleResults.setNumberObservations(0);// ???
    m_bundleResults.resetNumberConstrainedPointParameters();//???

    // Initialize auxiliary matrices and vectors.
    static LinearAlgebra::Matrix coeffTarget;
    static LinearAlgebra::Matrix coeffImagePosition;
    static LinearAlgebra::Matrix coeffImagePointing;
    static LinearAlgebra::Matrix coeffPoint3D(2, 3);
    static LinearAlgebra::Vector coeffRHS(2);
    static LinearAlgebra::MatrixUpperTriangular N22(3);
    SparseBlockColumnMatrix N12;
    static LinearAlgebra::Vector n2(3);
    LinearAlgebra::VectorCompressed n1(m_rank);

    m_RHS.resize(m_rank);

    // if solving for target body parameters, set size of coeffTarget
    // (note this size will not change through the adjustment).
    if (m_bundleSettings->solveTargetBody()) {
      int numTargetBodyParameters = m_bundleSettings->numberTargetBodyParameters();
      // TODO make sure numTargetBodyParameters is greater than 0
      coeffTarget.resize(2,numTargetBodyParameters);
    }

    // clear N12, n1, and nj
    N12.clear();
    n1.clear();
    m_RHS.clear();

    // clear static matrices
    coeffPoint3D.clear();
    coeffRHS.clear();
    N22.clear();
    n2.clear();

    // TODO timing tests
//    double cumulativeComputePartialsTime = 0.0;
//    double cumulativeFormMeasureNormalsTime = 0.0;
//    double cumulativeFormPointNormalsTime = 0.0;

    // loop over 3D points
    int numGood3DPoints = 0;
    int numRejected3DPoints = 0;
    int pointIndex = 0;
    int num3DPoints = m_bundleControlPoints.size();

    printf("\n");

    for (int i = 0; i < num3DPoints; i++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if (point->isRejected()) {
        numRejected3DPoints++;

        pointIndex++;
        continue;
      }

      if ( i != 0 ) {
        N22.clear();
        N12.wipe();
        n2.clear();
      }

      // loop over measures for this point
      int numMeasures = point->size();
      for (int j = 0; j < numMeasures; j++) {
        BundleMeasureQsp measure = point->at(j);

        // flagged as "JigsawFail" implies this measure has been rejected
        // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
        if (measure->isRejected()) {
          continue;
        }

//        clock_t computePartialsClock1 = clock();

        // segment solution
//        if (measure->parentBundleObservation()->numberPolynomialPositionSegments() > 1 ||
//            measure->parentBundleObservation()->numberPolynomialPointingSegments() > 1) {
          status = computePartials(coeffTarget, coeffImagePosition, coeffImagePointing,
                                   coeffPoint3D, coeffRHS, measure);
//        }
//        else {
//          status = computePartials(coeffTarget, coeffImage, coeffPoint3D, coeffRHS, measure);
//        }

//        clock_t computePartialsClock2 = clock();
//        cumulativeComputePartialsTime += (computePartialsClock2 - computePartialsClock1)
//            / (double)CLOCKS_PER_SEC;

        if (!status) {
          // TODO should status be set back to true? JAM
          // TODO this measure should be flagged as rejected.
          continue;
        }

        // update number of observations
        int numObs = m_bundleResults.numberObservations();
        m_bundleResults.setNumberObservations(numObs + 2);

//        clock_t formMeasureNormalsClock1 = clock();

        // segment solution
//        if (measure->parentBundleObservation()->numberPolynomialPositionSegments() > 1 ||
//            measure->parentBundleObservation()->numberPolynomialPointingSegments() > 1) {
        formMeasureNormals(N22, N12, n1, n2, coeffTarget, coeffImagePosition, coeffImagePointing,
                           coeffPoint3D, coeffRHS, measure);
//        }
//        else {
//          formMeasureNormals(N22, N12, n1, n2, coeffTarget, coeffImage, coeffPoint3D, coeffRHS,
//                             measure);
//        }

//        clock_t formMeasureNormalsClock2 = clock();
//        cumulativeFormMeasureNormalsTime += (formMeasureNormalsClock2 - formMeasureNormalsClock1)
//            / (double)CLOCKS_PER_SEC;

      } // end loop over this points measures

//      clock_t formPointNormalsClock1 = clock();
      formPointNormals(N22, N12, n2, m_RHS, point);
//      clock_t formPointNormalsClock2 = clock();

//      cumulativeFormPointNormalsTime += (formPointNormalsClock2 - formPointNormalsClock1)
//          / (double)CLOCKS_PER_SEC;

      pointIndex++;

      numGood3DPoints++;
    } // end loop over 3D points

//  qDebug() << "cumulative BundleAdjust::computePartials() Time: " << cumulativeComputePartialsTime;
//  qDebug() << "cumulative BundleAdjust::formMeasureNormals() Time: " << cumulativeFormMeasureNormalsTime;
//  qDebug() << "cumulative BundleAdjust::formPointNormals() Time: " << cumulativeFormPointNormalsTime;

  // form the reduced normal equations
//  clock_t formWeightedNormalsClock1 = clock();
    formWeightedNormals(n1, m_RHS);
//  clock_t formWeightedNormalsClock2 = clock();

//  double formWeightedNormalsTime = (formWeightedNormalsClock2 - formWeightedNormalsClock1)
//      / (double)CLOCKS_PER_SEC;

//  qDebug() << "BundleAdjust::formWeightedNormals Time: " << formWeightedNormalsTime;

  // finally if necessary, apply piecewise polynomial continuity constraints
    if (m_bundleResults.numberContinuityConstraintEquations() > 0) {

//    clock_t applyContinuityClock1 = clock();

      applyPolynomialContinuityConstraints();

//    clock_t applyContinuityClock2 = clock();

//    double applyContinuityTime = (applyContinuityClock2 - applyContinuityClock1)
//        / (double)CLOCKS_PER_SEC;

//    qDebug() << "applying Continuity Constraints: " << applyContinuityTime;
    }

    // update number of unknown parameters
    m_bundleResults.setNumberUnknownParameters(m_rank + 3 * numGood3DPoints);

    return status;
  }


  /**
   * Form the auxiliary normal equation matrices N22, N12, n1, and n2 for a measure.
   *
   * @param N22 The normal equation matrix for the point on the body.
   * @param N12 The normal equation matrix for the camera and the target body.
   * @param n1 The right hand side vector for the camera and the target body.
   * @param n2 The right hand side vector for the point on the body.
   * @param coeffTarget Target body partial derivative matrix.
   * @param coeffImagePosition Camera position partial derivative matrix.
   * @param coeffImagePointing Camera orientation partial derivatives matrix.
   * @param coeffPoint3D Control point lat, lon, and radius partial derivative matrix.
   * @param coeffRHS Measure right hand side vector.
   * @param measure QSharedPointer to current measure.
   *
   * @return bool If the matrices were successfully formed.
   *
   * @see BundleAdjust::formNormalEquations
   */
  bool BundleAdjust::formMeasureNormals(LinearAlgebra::MatrixUpperTriangular &N22,
                                        SparseBlockColumnMatrix &N12,
                                        LinearAlgebra::VectorCompressed &n1,
                                        LinearAlgebra::Vector &n2,
                                        LinearAlgebra::Matrix &coeffTarget,
                                        LinearAlgebra::Matrix &coeffImagePosition,
                                        LinearAlgebra::Matrix &coeffImagePointing,
                                        LinearAlgebra::Matrix &coeffPoint3D,
                                        LinearAlgebra::Vector &coeffRHS,
                                        BundleMeasureQsp      measure) {

    int positionBlockIndex = measure->positionNormalsBlockIndex();
    int pointingBlockIndex = measure->pointingNormalsBlockIndex();

    // if we are solving for target body parameters
    if (m_bundleSettings->solveTargetBody()) {
      int numTargetPartials = coeffTarget.size2();

      static vector<double> n1Target(numTargetPartials);
      n1Target.resize(numTargetPartials);
      n1Target.clear();

      // insert submatrix at column, row
      m_sparseNormals.insertMatrixBlock(0, 0, numTargetPartials, numTargetPartials);

      // contribution to N11 matrix for target body
      (*(*m_sparseNormals[0])[0]) += prod(trans(coeffTarget), coeffTarget);

      // solving for position
      if (positionBlockIndex >= 0) {
        // portion of N11 between target and image
        m_sparseNormals.insertMatrixBlock(positionBlockIndex, 0, numTargetPartials,
                                          coeffImagePosition.size2());

        (*(*m_sparseNormals[positionBlockIndex])[0]) += prod(trans(coeffTarget),coeffImagePosition);
      }

      // solving for pointing
      if (pointingBlockIndex >= 0) {
        // portion of N11 between target and image
        m_sparseNormals.insertMatrixBlock(pointingBlockIndex, 0, numTargetPartials,
                                          coeffImagePointing.size2());

        (*(*m_sparseNormals[pointingBlockIndex])[0]) += prod(trans(coeffTarget),coeffImagePointing);
      }

      // form N12 target portion
      N12.insertMatrixBlock(0, numTargetPartials, 3);
      *N12[0] += prod(trans(coeffTarget), coeffPoint3D);;

      // contribution to n1 vector
      vector_range<LinearAlgebra::VectorCompressed >
          n1_range (n1, range (0, numTargetPartials));

      n1_range += prod(trans(coeffTarget), coeffRHS);
    }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ below is ok (2015-06-03)
// TODO - if solving for target (and/or self-cal) have to use not observationIndex below but
// observationIndex plus 1 or 2

    // solving for position
    if (positionBlockIndex >= 0) {

      // insert submatrix into normal equations at positionBlockIndex, positionBlockIndex
      // if block is already there, no insertion is made
      m_sparseNormals.insertMatrixBlock(positionBlockIndex, positionBlockIndex,
                                        coeffImagePosition.size2(), coeffImagePosition.size2());

      // contribution to N11 matrix
      (*(*m_sparseNormals[positionBlockIndex])[positionBlockIndex]) +=
          prod(trans(coeffImagePosition), coeffImagePosition);

      // insert submatrix into N12 matrix
      // if block is already there, no insertion is made
      N12.insertMatrixBlock(positionBlockIndex, coeffImagePosition.size2(), 3);

      // contribution to N12 matrix
      *N12[positionBlockIndex] += prod(trans(coeffImagePosition), coeffPoint3D);

      // contribution to n1 vector
      // TODO confirm we are using compressed vector efficiently
      int sc = m_sparseNormals.at(positionBlockIndex)->startColumn();
      vector_range<LinearAlgebra::VectorCompressed >
          n1_range (n1, range (sc, sc+coeffImagePosition.size2()));

      n1_range += prod(trans(coeffImagePosition), coeffRHS);
    }

    // solving for pointing
    if (pointingBlockIndex >= 0) {

      // insert submatrix into normal equations at positionBlockIndex, positionBlockIndex
      // if block is already there, no insertion is made
      m_sparseNormals.insertMatrixBlock(pointingBlockIndex, pointingBlockIndex,
                                        coeffImagePointing.size2(), coeffImagePointing.size2());

      // contribution to N11 matrix
      (*(*m_sparseNormals[pointingBlockIndex])[pointingBlockIndex]) +=
          prod(trans(coeffImagePointing), coeffImagePointing);

      // insert submatrix into N12 matrix
      // if block is already there, no insertion is made
      N12.insertMatrixBlock(pointingBlockIndex, coeffImagePointing.size2(), 3);

      // contribution to N12 matrix
      *N12[pointingBlockIndex] += prod(trans(coeffImagePointing), coeffPoint3D);

      // contribution to n1 vector
      // TODO confirm we are using compressed vector efficiently
      int sc = m_sparseNormals.at(pointingBlockIndex)->startColumn();
      vector_range<LinearAlgebra::VectorCompressed >
          vr (n1, range (sc, sc+coeffImagePointing.size2()));

      vr += prod(trans(coeffImagePointing), coeffRHS);
    }

    // solving for position and pointing
    if (positionBlockIndex >= 0 && pointingBlockIndex >= 0) {

      // insert submatrix into normal equations at pointingBlockIndex, positionBlockIndex
      // if block is already there, no insertion is made
      m_sparseNormals.insertMatrixBlock(pointingBlockIndex, positionBlockIndex,
                                        coeffImagePosition.size2(), coeffImagePointing.size2());

      // contribution to N11 matrix
      (*(*m_sparseNormals[pointingBlockIndex])[positionBlockIndex]) +=
          prod(trans(coeffImagePosition), coeffImagePointing);
    }

    // form N22 matrix
    N22 += prod(trans(coeffPoint3D), coeffPoint3D);

    // form n2 vector
    n2 += prod(trans(coeffPoint3D), coeffRHS);

//    m_previousNumberImagePartials = numImagePartials;

    return true;
  }


  /**
   * Compute the Q matrix and NIC vector for a control point.  The inputs N22, N12, and n2
   * come from calling formMeasureNormals() with the control point's measures.
   * The Q matrix and NIC vector are stored in the BundleControlPoint.
   * R = N12 x Q is accumulated into m_sparseNormals.
   *
   * @param N22 Contribution to normal equations matrix for a control point.
   * @param N12 Contribution to normal equations matrix for images and target body.
   * @param n2 The right hand side vector for the point on the body.
   * @param nj The output right hand side vector.
   * @param bundleControlPoint The control point that the Q matrixs are NIC vector
   *                           are being formed for.
   *
   * @return bool If the matrices were successfully formed.
   *
   * @see BundleAdjust::formNormalEquations
   */
  bool BundleAdjust::formPointNormals(symmetric_matrix<double, upper>&N22,
                                      SparseBlockColumnMatrix &N12,
                                      vector<double> &n2,
                                      vector<double> &nj,
                                      BundleControlPointQsp &bundleControlPoint) {

    boost::numeric::ublas::bounded_vector<double, 3> &NIC = bundleControlPoint->nicVector();
    SparseBlockRowMatrix &Q = bundleControlPoint->cholmodQMatrix();

    NIC.clear();
    Q.zeroBlocks();

    // weighting of 3D point parameters
    boost::numeric::ublas::bounded_vector<double, 3> &weights
        = bundleControlPoint->weights();
    boost::numeric::ublas::bounded_vector<double, 3> &corrections
        = bundleControlPoint->corrections();

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

    // invert N22
    invert3x3(N22);

    // save upper triangular covariance matrix for error propagation
    SurfacePoint SurfacePoint = bundleControlPoint->adjustedSurfacePoint();
    SurfacePoint.SetSphericalMatrix(N22);
    bundleControlPoint->setAdjustedSurfacePoint(SurfacePoint);

    // form Q (this is N22{-1} * N12{T})
    productATransB(N22, N12, Q);

    // form product of N22(inverse) and n2; store in NIC
    NIC = prod(N22, n2);

    // accumulate -R directly into reduced normal equations
    productAB(N12, Q);

    // accumulate -nj
    accumProductAlphaAB(-1.0, Q, n2, nj);

    return true;
  }


  /**
   * Apply weighting for spacecraft position, velocity, acceleration and camera angles, angular
   * velocities, angular accelerations if so stipulated (legalese).
   *
   * @param n1 The right hand side vector for the camera and the target body.
   * @param nj The right hand side vector
   *
   * @return bool If the weights were successfully applied.
   *
   * @throws IException::Programmer "In BundleAdjust::formWeightedNormals(): target body normals
   *                                 matrix block is null."
   * @throws IException::Programmer "In BundleAdjust::formWeightedNormals(): position segment
   *                                 normals matrix block is null."
   * @throws IException::Programmer "In BundleAdjust::formWeightedNormals(): pointing segment
   *                                 normals matrix block is null."
   *
   * @see BundleAdjust::formNormalEquations
   */
  bool BundleAdjust::formWeightedNormals(compressed_vector<double> &n1,
                                         vector<double> &nj) {

    m_bundleResults.resetNumberConstrainedImageParameters();

    int n = 0;
    int blockIndex = 0;

    if (m_bundleSettings->solveTargetBody()) {
      LinearAlgebra::Matrix *diagonalBlock = m_sparseNormals.getBlock(0, 0);
      if (!diagonalBlock) {
        QString msg = "In BundleAdjust::formWeightedNormals(): target body matrix block is null.\n";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // get parameter weights for target body
      vector<double> weights = m_bundleTargetBody->parameterWeights();
      vector<double> corrections = m_bundleTargetBody->parameterCorrections();

      for (size_t i = 0; i < diagonalBlock->size1(); i++) {
        if (weights[i] > 0.0) {
          (*diagonalBlock)(i,i) += weights[i];
          nj[n] -= weights[i] * corrections(i);
          m_bundleResults.incrementNumberConstrainedTargetParameters(1);
        }
        n++;
      }
      blockIndex = 1;
    }

    for (int i = 0; i < m_bundleObservations.size(); i++) {

      BundleObservationQsp observation = m_bundleObservations.at(i);

      // get parameter weights and corrections for this observation
      LinearAlgebra::Vector weights = observation->parameterWeights();
      LinearAlgebra::Vector corrections = observation->parameterCorrections();

      // loop over position segments
      int weightIndex = 0;
      int positionSegments = observation->numberPolynomialPositionSegments();
      int pointingSegments = observation->numberPolynomialPointingSegments();
      int totalSegments = observation->numberPolynomialSegments();
      for (int j = 0; j < positionSegments; j++) {
        LinearAlgebra::Matrix *diagonalBlock = m_sparseNormals.getBlock(blockIndex, blockIndex);
        if (!diagonalBlock) {
          QString msg = "In BundleAdjust::formWeightedNormals(): "
                        "position segment normals matrix block is null.\n";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        for (size_t k = 0; k < diagonalBlock->size1(); k++) {
          if (weights[weightIndex] > 0.0) {
            (*diagonalBlock)(k,k) += weights[weightIndex];
            nj[n] -= weights[weightIndex] * corrections(weightIndex);
            m_bundleResults.incrementNumberConstrainedImageParameters(1);
          }
          weightIndex++;
          n++;
        }
        blockIndex++;
      }

      // loop over pointing segments
      if (pointingSegments > 0) {
        for (int j = positionSegments; j < totalSegments; j++) {
          LinearAlgebra::Matrix *diagonalBlock = m_sparseNormals.getBlock(blockIndex, blockIndex);
          if (!diagonalBlock) {
            QString msg = "In BundleAdjust::formWeightedNormals(): "
                          "pointing segment normals matrix block is null.\n";
            throw IException(IException::Programmer, msg, _FILEINFO_);
          }

          for (size_t k = 0; k < diagonalBlock->size1(); k++) {
            if (weights[weightIndex] > 0.0) {
              (*diagonalBlock)(k,k) += weights[weightIndex];
              nj[n] -= weights[weightIndex] * corrections(weightIndex);
              m_bundleResults.incrementNumberConstrainedImageParameters(1);
            }
            weightIndex++;
            n++;
          }
          blockIndex++;
        }
      }
    }

    // add n1 to nj
    nj += n1;

    return true;
  }


  /**
   * Add piecewise polynomial continuity constraints to normals equations
   *
   * @todo Do we need a general flag indicating there are time-dependent observations in the bundle
   *       that require application of piecewise polynomial continuity constraints?
   *       e.g. if all images are framing, we won't be applying
   *
   */
  void BundleAdjust::applyPolynomialContinuityConstraints() {

    int t = 0;

    LinearAlgebra::Vector continuityRHS;

    for (int i = 0; i < m_bundleObservations.size(); i++) {

      // get observation corresponding to diagonal block i
      // TODO: would it be advantageous for BundleObservation to contain a Qsp to its corresponding
      //       diagonal matrix block? Or for all blocks that correspond to the observation?
      // (Ken 2017-07-18 - maybe)
      BundleObservationQsp observation = m_bundleObservations.at(i);

      // skip if we aren't applying continuity constraints for this observation
      if (observation->numberContinuityConstraints() <= 0) {
        continue;
      }

      bool solveForPosition =
          (observation->solveSettings()->instrumentPositionSolveOption() >
           BundleObservationSolveSettings::PositionOnly) ? true : false;

      bool solveForPointing =
          (observation->solveSettings()->instrumentPointingSolveOption() >
           BundleObservationSolveSettings::AnglesOnly) ? true : false;

      int positionSegments = observation->numberPolynomialPositionSegments();
      int pointingSegments = observation->numberPolynomialPointingSegments();
      int startBlock = observation->normalsMatrixStartBlock();

      SparseBlockMatrix ccSpkBlock;
      SparseBlockMatrix ccCkBlock;
      if (solveForPosition) {
        ccSpkBlock = observation->continuityContraintSpkMatrix();
      }

      if (solveForPointing) {
        ccCkBlock = observation->continuityContraintCkMatrix();
      }

      // handle position blocks
      if (solveForPosition && positionSegments > 1) {
        for (int j = 0; j < positionSegments; j++) {

          int t = j+startBlock;

          LinearAlgebra::Matrix *block = m_sparseNormals.getBlock(t, t);

          *block += (*(*ccSpkBlock[j])[j]);

          if (j > 0) {
            int positionPartials = observation->numberPositionParametersPerSegment();
            m_sparseNormals.insertMatrixBlock(t, t-1, positionPartials, positionPartials);
            block = m_sparseNormals.getBlock(t, t-1);

            *block += (*(*ccSpkBlock[j])[j-1]);
          }
        }
      }

      // handle pointing blocks
      if (solveForPointing && pointingSegments > 1) {
        for (int j = 0; j < pointingSegments; j++) {

          int t = j+startBlock+positionSegments;

          LinearAlgebra::Matrix *block = m_sparseNormals.getBlock(t, t);

          *block += (*(*ccCkBlock[j])[j]);

          if (j > 0) {
            int pointingPartials = observation->numberPointingParametersPerSegment();
            m_sparseNormals.insertMatrixBlock(t, t-1, pointingPartials, pointingPartials);
            block = m_sparseNormals.getBlock(t, t-1);
            *block += (*(*ccCkBlock[j])[j-1]);
          }
        }
      }

      // add contribution from continuity constraints into m_RHS
      continuityRHS = observation->continuityRHS();

      int numParameters = observation->numberParameters();
      for (int j = 0; j < numParameters; j++) {
        m_RHS(j+t) += continuityRHS(j);
      }

      t += numParameters;

      continuityRHS.clear();
    }
  }


  /**
   * Perform the matrix multiplication v2 = alpha ( Q x v1 ).
   *
   * @param alpha A constant multiplier.
   * @param v2 The output vector.
   * @param Q A sparse block matrix.
   * @param v1 A vector.
   */
  void BundleAdjust::productAlphaAV(double alpha, bounded_vector<double,3> &v2,
                                    SparseBlockRowMatrix &Q,
                                    LinearAlgebra::Vector &v1) {

    QMapIterator< int, LinearAlgebra::Matrix * > Qit(Q);

    int subrangeStart, subrangeEnd;
    
    while ( Qit.hasNext() ) {
      Qit.next();

      int columnIndex = Qit.key();

      subrangeStart = m_sparseNormals.at(columnIndex)->startColumn();
      subrangeEnd = subrangeStart + Qit.value()->size2();
      
      v2 += alpha * prod(*(Qit.value()),subrange(v1,subrangeStart,subrangeEnd));
    }
  }


  /**
   * Perform the matrix multiplication Q = N22 x N12(transpose)
   *
   * @param N22 A symmetric matrix
   * @param N12 A sparse block matrix
   * @param Q The output sparse block matrix
   *
   * @see BundleAdjust::formPointNormals
   */
  bool BundleAdjust::productATransB(symmetric_matrix <double,upper> &N22,
                                    SparseBlockColumnMatrix &N12,
                                    SparseBlockRowMatrix &Q) {

    QMapIterator< int, LinearAlgebra::Matrix * > N12it(N12);

    while ( N12it.hasNext() ) {
      N12it.next();

      int rowIndex = N12it.key();

      // insert submatrix in Q at block "rowIndex"
      Q.insertMatrixBlock(rowIndex, 3, N12it.value()->size1());

      *(Q[rowIndex]) = prod(N22,trans(*(N12it.value())));
    }

    return true;
  }


  /**
   * Perform the matrix multiplication C = N12 x Q.
   * The result, C, is stored in m_sparseNormals.
   *
   * @param N12 A sparse block matrix.
   * @param Q A sparse block matrix
   *
   * @see BundleAdjust::formPointNormals
   */
  void BundleAdjust::productAB(SparseBlockColumnMatrix &N12,
                               SparseBlockRowMatrix &Q) {
    // iterators for N12 and Q
    QMapIterator<int, LinearAlgebra::Matrix*> N12it(N12);
    QMapIterator<int, LinearAlgebra::Matrix*> Qit(Q);

    // now multiply blocks and subtract from m_sparseNormals
    while ( N12it.hasNext() ) {
      N12it.next();

      int rowIndex = N12it.key();
      LinearAlgebra::Matrix *N12block = N12it.value();

      while ( Qit.hasNext() ) {
        Qit.next();

        int columnIndex = Qit.key();

        if ( rowIndex > columnIndex ) {
          continue;
        }

        LinearAlgebra::Matrix *Qblock = Qit.value();

        // insert submatrix at column, row
        m_sparseNormals.insertMatrixBlock(columnIndex, rowIndex,
                                          N12block->size1(), Qblock->size2());

        (*(*m_sparseNormals[columnIndex])[rowIndex]) -= prod(*N12block,*Qblock);
      }
      Qit.toFront();
    }
  }


  /**
   * Performs the matrix multiplication nj = nj + alpha (Q x n2).
   *
   * @param alpha A constant multiplier.
   * @param Q A sparse block matrix.
   * @param n2 A vector.
   * @param nj The output accumulation vector.
   *
   * @see BundleAdjust::formPointNormals
   */
  void BundleAdjust::accumProductAlphaAB(double alpha,
                                         SparseBlockRowMatrix &Q,
                                         vector<double> &n2,
                                         vector<double> &nj) {

    if (alpha == 0.0) {
      return;
    }

    int numParams;

    QMapIterator<int, LinearAlgebra::Matrix*> Qit(Q);

    while ( Qit.hasNext() ) {
      Qit.next();

      int columnIndex = Qit.key();
      LinearAlgebra::Matrix *Qblock = Qit.value();

      LinearAlgebra::Vector blockProduct = prod(trans(*Qblock),n2);

      numParams = m_sparseNormals.at(columnIndex)->startColumn();

      for (unsigned i = 0; i < blockProduct.size(); i++) {
        nj(numParams+i) += alpha*blockProduct(i);
      }
    }
  }


  /**
   * Compute the solution to the normal equations using the CHOLMOD library.
   *
   * @return bool If the solution was successfully computed.
   *
   * @throws IException::Programmer "CHOLMOD: Failed to load Triplet matrix"
   *
   * @see BundleAdjust::solveCholesky
   */
  bool BundleAdjust::solveSystem() {

    // load cholmod triplet
    if ( !loadCholmodTriplet() ) {
      QString msg = "CHOLMOD: Failed to load Triplet matrix";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // convert triplet to sparse matrix
    m_cholmodNormal = cholmod_triplet_to_sparse(m_cholmodTriplet,
                                                m_cholmodTriplet->nnz,
                                                &m_cholmodCommon);

    // analyze matrix
    // TODO should we analyze just 1st iteration?
    m_L = cholmod_analyze(m_cholmodNormal, &m_cholmodCommon);

    // create cholmod cholesky factor
    // CHOLMOD will choose LLT or LDLT decomposition based on the characteristics of the matrix.
    cholmod_factorize(m_cholmodNormal, m_L, &m_cholmodCommon);

    // check for "matrix not positive definite" error
    if (m_cholmodCommon.status == CHOLMOD_NOT_POSDEF) {
      QString msg = "Matrix NOT positive-definite: failure at column " + toString((int) m_L->minor);
    throw IException(IException::User, msg, _FILEINFO_);
      error(msg);
      emit(finished());
      return false;
    }

    // cholmod solution and right-hand side vectors
    cholmod_dense *x, *b;

    // initialize right-hand side vector
    b = cholmod_zeros(m_cholmodNormal->nrow, 1, m_cholmodNormal->xtype, &m_cholmodCommon);

    // copy right-hand side vector into b
    double *px = (double*)b->x;
    for (int i = 0; i < m_rank; i++) {
      px[i] = m_RHS[i];
    }

    // cholmod solve
    x = cholmod_solve(CHOLMOD_A, m_L, b, &m_cholmodCommon);

    // copy solution vector x out into m_imageSolution
    double *sx = (double*)x->x;
    for (int i = 0; i < m_rank; i++) {
      m_imageSolution[i] = sx[i];
    }

    // free cholmod structures
    cholmod_free_sparse(&m_cholmodNormal, &m_cholmodCommon);
    cholmod_free_dense(&b, &m_cholmodCommon);
    cholmod_free_dense(&x, &m_cholmodCommon);

    return true;
  }


  /**
   * @brief Load sparse normal equations matrix into CHOLMOD triplet.
   *
   * Blocks from the sparse block normal matrix are loaded into a CHOLMOD triplet.
   * Before the triplet can be used with CHOLMOD, it must be converted to a
   * CHOLMOD sparse matrix via cholmod_triplet_to_sparse.
   *
   * @return bool If the triplet was successfully formed.
   *
   * @see BundleAdjust::solveSystem
   */
  bool BundleAdjust::loadCholmodTriplet() {
    if ( m_iteration == 1 ) {
      int numElements = m_sparseNormals.numberOfElements();
      m_cholmodTriplet = cholmod_allocate_triplet(m_rank, m_rank, numElements,
                                                  -1, CHOLMOD_REAL, &m_cholmodCommon);

      if ( !m_cholmodTriplet ) {
        printf("Triplet allocation failure");
        return false;
      }

      m_cholmodTriplet->nnz = 0;
    }

    int *tripletColumns = (int*) m_cholmodTriplet->i;
    int *tripletRows = (int*) m_cholmodTriplet->j;
    double *tripletValues = (double*)m_cholmodTriplet->x;

    double entryValue;

    int numEntries = 0;

    int numBlockcolumns = m_sparseNormals.size();
    for (int columnIndex = 0; columnIndex < numBlockcolumns; columnIndex++) {

      SparseBlockColumnMatrix *normalsColumn = m_sparseNormals[columnIndex];

      if ( !normalsColumn ) {
        printf("SparseBlockColumnMatrix retrieval failure at column %d", columnIndex);
        return false;
      }

      int numLeadingColumns = normalsColumn->startColumn();

      QMapIterator< int, LinearAlgebra::Matrix * > it(*normalsColumn);

      while ( it.hasNext() ) {
        it.next();

        int rowIndex = it.key();

        // note: as the normal equations matrix is symmetric, the # of leading rows for a block is
        //       equal to the # of leading columns for a block column at the "rowIndex" position
        int numLeadingRows = m_sparseNormals.at(rowIndex)->startColumn();

        LinearAlgebra::Matrix *normalsBlock = it.value();
        if ( !normalsBlock ) {
          printf("matrix block retrieval failure at column %d, row %d", columnIndex, rowIndex);
          printf("Total # of block columns: %d", numBlockcolumns);
          printf("Total # of blocks: %d", m_sparseNormals.numberOfBlocks());
          return false;
        }

        if ( columnIndex == rowIndex )  {   // diagonal block (upper-triangular but stored square)
          for (unsigned ii = 0; ii < normalsBlock->size1(); ii++) {
            for (unsigned jj = ii; jj < normalsBlock->size2(); jj++) {
              entryValue = normalsBlock->at_element(ii,jj);
                int entryColumnIndex = jj + numLeadingColumns;
                int entryRowIndex = ii + numLeadingRows;

                if ( m_iteration == 1 ) {
                  tripletColumns[numEntries] = entryColumnIndex;
                  tripletRows[numEntries] = entryRowIndex;
                  m_cholmodTriplet->nnz++;
                }

                tripletValues[numEntries] = entryValue;

                numEntries++;
            }
          }
        }
        else {                // off-diagonal block (square)
          for (unsigned ii = 0; ii < normalsBlock->size1(); ii++) {
            for (unsigned jj = 0; jj < normalsBlock->size2(); jj++) {
              entryValue = normalsBlock->at_element(ii,jj);
                int entryColumnIndex = jj + numLeadingColumns;
                int entryRowIndex = ii + numLeadingRows;

                if ( m_iteration ==1 ) {
                  tripletColumns[numEntries] = entryRowIndex;
                  tripletRows[numEntries] = entryColumnIndex;
                  m_cholmodTriplet->nnz++;
                }

                tripletValues[numEntries] = entryValue;

                numEntries++;
            }
          }
        }
      }
    }

    return true;
  }


  /**
   * Dedicated quick inverse of 3x3 matrix
   *
   * @param m The 3x3 matrix to invert.  Overwritten with the inverse.
   *
   * @return bool If the matrix was inverted.
   *                 False usually means the matrix is not invertible.
   *
   * @see BundleAdjust::formPointNormals
   *
   * @todo Move to LinearAlgebra
   */
  bool BundleAdjust::invert3x3(LinearAlgebra::MatrixUpperTriangular &m) {
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
   * Compute partial derivatives and weighted residuals for a measure.
   * coeffTarget, coeffImagePosition, coeffImagePointing, coeffPoint3D, and coeffRHS will be filled
   * with the different partial derivatives.
   *
   * @param coeffTarget Target body partial derivatives matrix.
   * @param coeffImagePosition Camera position partial derivatives.
   * @param coeffImagePointing Camera pointing partial derivatives.
   * @param coeffPoint3D Control point lat, lon, and radius partial derivatives.
   * @param coeffRHS Measure right hand side vector.
   * @param measure QSharedPointer to BundleMeasure that partials are being computed for.
   *
   * @return bool If the partials were successfully computed.
   *
   * @throws IException::User "Unable to map apriori surface point for measure"
   */
      bool BundleAdjust::computePartials(LinearAlgebra::Matrix &coeffTarget,
                                         LinearAlgebra::Matrix &coeffImagePosition,
                                         LinearAlgebra::Matrix &coeffImagePointing,
                                         LinearAlgebra::Matrix &coeffPoint3D,
                                         LinearAlgebra::Vector &coeffRHS,
                                         BundleMeasureQsp measure) {
    // additional vectors
    std::vector<double> lookBWRTLat;
    std::vector<double> lookBWRTLon;
    std::vector<double> lookBWRTRad;

    Camera *measureCamera = NULL;

    double measuredX, computedX, measuredY, computedY;
    double deltaX, deltaY;
    double observationSigma;
    double observationWeight;

    measureCamera = measure->camera();

    // clear partial derivative matrices and vectors
    if (m_bundleSettings->solveTargetBody()) {
      coeffTarget.clear();
    }

    coeffImagePosition.clear();
    coeffImagePointing.clear();
    coeffPoint3D.clear();
    coeffRHS.clear();

    // no need to call SetImage for framing camera ( CameraType  = 0 )
    if (measureCamera->GetCameraType() != 0) {
      // Set the Spice to the measured point
      // TODO - can we explain this better?
      measureCamera->SetImage(measure->sample(), measure->line());
    }

    // we set the measures polynomial segment indices and position and pointing matrix blocks
    // once only, in the first iteration.
    // NOTE: for time dependent sensors, Camera::SetImage MUST be called prior to
    // setPolySegementIndices
    // TODO: should we do this in initialization? But SetImage would have to be called there for
    // time dependent sensors.
    if (m_iteration == 1) {
      measure->setPolySegmentIndices();
      if (m_bundleSettings->solveTargetBody()) {
        measure->setNormalsBlockIndices(1);
      }
      else {
        measure->setNormalsBlockIndices();
      }
    }

    BundleControlPoint *point = measure->parentControlPoint();
    SurfacePoint surfacePoint = point->adjustedSurfacePoint();

    // Compute the look vector in instrument coordinates based on time of observation and apriori
    // lat/lon/radius
    if (!(measureCamera->GroundMap()->GetXY(surfacePoint, &computedX, &computedY))) {
      QString msg = "Unable to map apriori surface point for measure ";
      msg += measure->cubeSerialNumber() + " on point " + point->id() + " into focal plane";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // partials for fixed point w/r lat, long, radius in Body-Fixed
    lookBWRTLat = measureCamera->GroundMap()->PointPartial(surfacePoint,
                                                           CameraGroundMap::WRT_Latitude);
    lookBWRTLon = measureCamera->GroundMap()->PointPartial(surfacePoint,
                                                           CameraGroundMap::WRT_Longitude);
    lookBWRTRad = measureCamera->GroundMap()->PointPartial(surfacePoint,
                                                           CameraGroundMap::WRT_Radius);

    int index = 0;
    if (m_bundleSettings->solveTargetBody()) {
      if (m_bundleSettings->solvePoleRA()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_RightAscension, 0,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleSettings->solvePoleRAVelocity()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_RightAscension, 1,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleSettings->solvePoleDec()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Declination, 0,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleSettings->solvePoleDecVelocity()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Declination, 1,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleSettings->solvePM()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Twist, 0,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleSettings->solvePMVelocity()) {
        measureCamera->GroundMap()->GetdXYdTOrientation(SpiceRotation::WRT_Twist, 1,
                                                        &coeffTarget(0, index),
                                                        &coeffTarget(1, index));
        index++;
      }

      if (m_bundleTargetBody->solveMeanRadius()) {
        std::vector<double> lookBWRTMeanRadius =
            measureCamera->GroundMap()->MeanRadiusPartial(surfacePoint,
                                                          m_bundleTargetBody->meanRadius());

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTMeanRadius, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;
      }

      if (m_bundleTargetBody->solveTriaxialRadii()) {

        std::vector<double> lookBWRTRadiusA =
            measureCamera->GroundMap()->EllipsoidPartial(surfacePoint,
                                                         CameraGroundMap::WRT_MajorAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusA, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;

        std::vector<double> lookBWRTRadiusB =
            measureCamera->GroundMap()->EllipsoidPartial(surfacePoint,
                                                         CameraGroundMap::WRT_MinorAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusB, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;

        std::vector<double> lookBWRTRadiusC =
            measureCamera->GroundMap()->EllipsoidPartial(surfacePoint,
                                                         CameraGroundMap::WRT_PolarAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusC, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;
      }
    }

    measure->parentBundleObservation()->computePartials(coeffImagePosition, coeffImagePointing);

    // partials for 3D point
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTLat,
                                             &coeffPoint3D(0, 0),
                                             &coeffPoint3D(1, 0));
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTLon,
                                             &coeffPoint3D(0, 1),
                                             &coeffPoint3D(1, 1));
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRad,
                                             &coeffPoint3D(0, 2),
                                             &coeffPoint3D(1, 2));

    // right-hand side (measured - computed)
    measuredX = measure->focalPlaneMeasuredX();
    measuredY = measure->focalPlaneMeasuredY();

    deltaX = measuredX - computedX;
    deltaY = measuredY - computedY;

    coeffRHS(0) = deltaX;
    coeffRHS(1) = deltaY;

    // residual prob distribution is calculated even if there is no maximum likelihood estimation
    double obsValue = deltaX / measureCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    obsValue = deltaY / measureCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    observationSigma = 1.4 * measureCamera->PixelPitch();
    observationWeight = 1.0 / observationSigma;

    if (m_bundleResults.numberMaximumLikelihoodModels()
          > m_bundleResults.maximumLikelihoodModelIndex()) {
      // if maximum likelihood estimation is being used
      double residualR2ZScore
                 = sqrt(deltaX * deltaX + deltaY * deltaY) / observationSigma / sqrt(2.0);
      //dynamically build the cumulative probability distribution of the R^2 residual Z Scores
      m_bundleResults.addProbabilityDistributionObservation(residualR2ZScore);
      int currentModelIndex = m_bundleResults.maximumLikelihoodModelIndex();
      observationWeight *= m_bundleResults.maximumLikelihoodModelWFunc(currentModelIndex)
                            .sqrtWeightScaler(residualR2ZScore);
    }

    // multiply coefficients by observation weight
    coeffImagePosition *= observationWeight;
    coeffImagePointing *= observationWeight;
    coeffPoint3D *= observationWeight;
    coeffRHS *= observationWeight;

    if (m_bundleSettings->solveTargetBody()) {
      coeffTarget *= observationWeight;
    }

    return true;
  }


  /**
   * apply parameter corrections for solution.
   */
  void BundleAdjust::applyParameterCorrections() {

    int t = 0;

    // TODO - update target body parameters if in solution
    // note these come before BundleObservation parameters in normal equations matrix
    if (m_bundleSettings->solveTargetBody()) {
      int numTargetBodyParameters = m_bundleTargetBody->numberParameters();

      m_bundleTargetBody->applyParameterCorrections(subrange(m_imageSolution,0,
                                                            numTargetBodyParameters));

      t += numTargetBodyParameters;
    }

    // Update spice for each BundleObservation
    int numObservations = m_bundleObservations.size();
    for (int i = 0; i < numObservations; i++) {
      BundleObservationQsp observation = m_bundleObservations.at(i);

      int numParameters = observation->numberParameters();

      observation->applyParameterCorrections(subrange(m_imageSolution,t,t+numParameters));

      if (m_bundleSettings->solveTargetBody()) {
        observation->updateBodyRotation();
      }

      t += numParameters;
    }
        
    // TODO: Below code should move into BundleControlPoint->updateParameterCorrections
    //       except, what about the productAlphaAV method?
    
    // Update lat/lon for each control point
    double latCorrection, lonCorrection, radCorrection;
    int pointIndex = 0;
    int numControlPoints = m_bundleControlPoints.size();
    for (int i = 0; i < numControlPoints; i++) {
      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if (point->isRejected()) {
          pointIndex++;
          continue;
      }

      // get NIC, Q, and correction vector for this point
      boost::numeric::ublas::bounded_vector< double, 3 > &NIC = point->nicVector();
      SparseBlockRowMatrix &Q = point->cholmodQMatrix();
      boost::numeric::ublas::bounded_vector< double, 3 > &corrections = point->corrections();

      // subtract product of Q and nj from NIC
      productAlphaAV(-1.0, NIC, Q, m_imageSolution);

      // get point parameter corrections
      latCorrection = NIC(0);
      lonCorrection = NIC(1);
      radCorrection = NIC(2);

      SurfacePoint surfacepoint = point->adjustedSurfacePoint();

      double pointLat = surfacepoint.GetLatitude().degrees();
      double pointLon = surfacepoint.GetLongitude().degrees();
      double pointRad = surfacepoint.GetLocalRadius().meters();

      pointLat += RAD2DEG * latCorrection;
      pointLon += RAD2DEG * lonCorrection;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (pointLat < -90.0) {
        pointLat = -180.0 - pointLat;
        pointLon = pointLon + 180.0;
      }
      if (pointLat > 90.0) {
        pointLat = 180.0 - pointLat;
        pointLon = pointLon + 180.0;
      }
      while (pointLon > 360.0) {
        pointLon = pointLon - 360.0;
      }
      while (pointLon < 0.0) {
        pointLon = pointLon + 360.0;
      }

      pointRad += 1000.*radCorrection;

      // sum and save corrections
      corrections(0) += latCorrection;
      corrections(1) += lonCorrection;
      corrections(2) += radCorrection;
           
      // ken testing - if solving for target body mean radius, set radius to current
      // mean radius value
      if (m_bundleTargetBody && (m_bundleTargetBody->solveMeanRadius()
          || m_bundleTargetBody->solveTriaxialRadii())) {
        if (m_bundleTargetBody->solveMeanRadius()) {
          surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                               Longitude(pointLon, Angle::Degrees),
                                               m_bundleTargetBody->meanRadius());
        }
        else if (m_bundleTargetBody->solveTriaxialRadii()) {
            Distance localRadius = m_bundleTargetBody->
                                       localRadius(Latitude(pointLat, Angle::Degrees),
                                                   Longitude(pointLon, Angle::Degrees));
            surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                                 Longitude(pointLon, Angle::Degrees),
                                                 localRadius);
        }
      }
      else {
        surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                             Longitude(pointLon, Angle::Degrees),
                                             Distance(pointRad, Distance::Meters));
      }

      point->setAdjustedSurfacePoint(surfacepoint);

      pointIndex++;

    } // end loop over point corrections
  }


  /**
   * This method computes the focal plane residuals for the measures.
   *
   * @return double Weighted sum of the squares of the residuals, vtpv.
   *
   * @internal
   *   @history 2012-01-18 Debbie A. Cook - Fixed the computation of vx
   *                           and vy to make sure they are focal
   *                           plane x and y residuals instead of
   *                           image sample and line residuals.
   */
  double BundleAdjust::computeResiduals() {
    double vtpv = 0.0;
    double vtpvControl = 0.0;
    double vtpvImage = 0.0;
    double weight;
    double v, vx, vy;

    // clear residual stats vectors
    m_xResiduals.Reset();
    m_yResiduals.Reset();
    m_xyResiduals.Reset();

    // vtpv for image coordinates
    int numObjectPoints = m_bundleControlPoints.size();

    for (int i = 0; i < numObjectPoints; i++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      point->computeResiduals();

      int numMeasures = point->numberOfMeasures();
      for (int j = 0; j < numMeasures; j++) {
        const BundleMeasureQsp measure = point->at(j);

        weight = 1.4 * (measure->camera())->PixelPitch();
        weight = 1.0 / weight;
        weight *= weight;

        vx = measure->focalPlaneMeasuredX() - measure->focalPlaneComputedX();
        vy = measure->focalPlaneMeasuredY() - measure->focalPlaneComputedY();

        // if rejected, don't include in statistics
        if (measure->isRejected()) {
          continue;
        }

        m_xResiduals.AddData(vx);
        m_yResiduals.AddData(vy);
        m_xyResiduals.AddData(vx);
        m_xyResiduals.AddData(vy);

        vtpv += vx * vx * weight + vy * vy * weight;
      }
    }

    // add vtpv from constrained 3D points
    int pointIndex = 0;
    for (int i = 0; i < numObjectPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_bundleControlPoints.at(i);

      // get weight and correction vector for this point
      boost::numeric::ublas::bounded_vector<double, 3> weights = bundleControlPoint->weights();
      boost::numeric::ublas::bounded_vector<double, 3> corrections =
                                                             bundleControlPoint->corrections();

      if ( weights(0) > 0.0 ) {
          vtpvControl += corrections(0) * corrections(0) * weights(0);
      }
      if ( weights(1) > 0.0 ) {
          vtpvControl += corrections(1) * corrections(1) * weights(1);
      }
      if ( weights(2) > 0.0 ) {
          vtpvControl += corrections(2) * corrections(2) * weights(2);
      }

      pointIndex++;
    }

    // add vtpv from constrained image parameters
    for (int i = 0; i < m_bundleObservations.size(); i++) {
      BundleObservationQsp observation = m_bundleObservations.at(i);

      // get weight and correction vector for this observation
      const LinearAlgebra::Vector &weights = observation->parameterWeights();
      const LinearAlgebra::Vector &corrections = observation->parameterCorrections();

      for (int j = 0; j < (int)corrections.size(); j++) {
        if (weights[j] > 0.0) {
          v = corrections[j];
          vtpvImage += v * v * weights[j];
        }
      }
    }

    // TODO - add vtpv from constrained target body parameters
    double vtpvTargetBody = 0.0;
    if ( m_bundleTargetBody) {
      vtpvTargetBody = m_bundleTargetBody->vtpv();
    }

   vtpv = vtpv + vtpvControl + vtpvImage + vtpvTargetBody;

    // Compute rms for all image coordinate residuals
    // separately for x, y, then x and y together
    m_bundleResults.setRmsXYResiduals(m_xResiduals.Rms(), m_yResiduals.Rms(), m_xyResiduals.Rms());

    return vtpv;
  }


  /**
   * Compute the residuals for each adjusted point
   * and store adjustment results in m_bundleResults.
   *
   * @return bool If the wrap up was successful.
   */
  bool BundleAdjust::wrapUp() {
    // compute residuals in pixels

    // vtpv for image coordinates
    // TODO: is this necessary? Residuals are computed (and hopefully stored at the end of each
    // iteration. So are we doing this twice?
    for (int i = 0;  i < m_bundleControlPoints.size(); i++) {
      BundleControlPointQsp point = m_bundleControlPoints.at(i);
      point->computeResiduals();
    }

    computeBundleStatistics();

    return true;
  }


  /**
   * @brief Compute rejection limit.
   *
   * Computes the median and the median absolute deviation (M.A.D.) of the residuals.
   * Then, sets the rejection limit in m_bundleResults to median + RejectionMultiplier * M.A.D.
   *
   * @return bool If the rejection limit was successfully computed and set.
   *
   * @TODO should this be in BundleResults?
   *
   * @internal
   *   @history 2016-10-25 Ian Humphrey - "Rejection Limit" is output to std out again immediately
   *                           after "mad." References #4463.
   */
  bool BundleAdjust::computeRejectionLimit() {
      double vx, vy;

      int numResiduals = m_bundleResults.numberObservations() / 2;

      std::vector<double> residuals;

      residuals.resize(numResiduals);

      // load magnitude (squared) of residual vector
      int residualIndex = 0;
      int numObjectPoints = m_bundleControlPoints.size();
      for (int i = 0; i < numObjectPoints; i++) {

          const BundleControlPointQsp point = m_bundleControlPoints.at(i);

          if ( point->isRejected() ) {
            continue;
          }

          int numMeasures = point->numberOfMeasures();
          for (int j = 0; j < numMeasures; j++) {

              const BundleMeasureQsp measure = point->at(j);

              if ( measure->isRejected() ) {
                  continue;
              }

              vx = measure->sampleResidual();
              vy = measure->lineResidual();

              residuals[residualIndex] = sqrt(vx*vx + vy*vy);

              residualIndex++;
          }
      }

      // sort vectors
      std::sort(residuals.begin(), residuals.end());

      double median;
      double medianDev;
      double mad;

      int midpointIndex = numResiduals / 2;

      if ( numResiduals % 2 == 0 ) {
        median = (residuals[midpointIndex - 1] + residuals[midpointIndex]) / 2;
      }
      else {
        median = residuals[midpointIndex];
      }

      // compute M.A.D.
      for (int i = 0; i < numResiduals; i++) {
          residuals[i] = fabs(residuals[i] - median);
      }

      std::sort(residuals.begin(), residuals.end());

      if ( numResiduals % 2 == 0 ) {
        medianDev = (residuals[midpointIndex - 1] + residuals[midpointIndex]) / 2;
      }
      else {
        medianDev = residuals[midpointIndex];
      }

      std::cout << "median deviation: " << medianDev << std::endl;

      mad = 1.4826 * medianDev;

      std::cout << "mad: " << mad << "\n";

      m_bundleResults.setRejectionLimit(median
                                        + m_bundleSettings->outlierRejectionMultiplier() * mad);

      std::cout << "Rejection Limit: " << m_bundleResults.rejectionLimit() << std::endl;

      return true;
  }


  /**
   * Flags outlier measures and control points.
   *
   * @return bool If the flagging was successful.
   *
   * @todo How should we handle points with few measures.
   */
  bool BundleAdjust::flagOutliers() {
    double vx, vy;
    int numRejected;
    int totalNumRejected = 0;

    int maxResidualIndex;
    double maxResidual;
    double sumSquares;
    double usedRejectionLimit = m_bundleResults.rejectionLimit();

    // TODO What to do if usedRejectionLimit is too low?

    int numComingBack = 0;

    int numObjectPoints = m_bundleControlPoints.size();
    for (int i = 0; i < numObjectPoints; i++) {
      BundleControlPointQsp point = m_bundleControlPoints.at(i);

      point->zeroNumberOfRejectedMeasures();

      numRejected = 0;
      maxResidualIndex = -1;
      maxResidual = -1.0;

      int numMeasures = point->numberOfMeasures();
      for (int j = 0; j < numMeasures; j++) {

        BundleMeasureQsp measure = point->at(j);

        vx = measure->sampleResidual();
        vy = measure->lineResidual();

        sumSquares = sqrt(vx*vx + vy*vy);

        // measure is good
        if ( sumSquares <= usedRejectionLimit ) {

          // was it previously rejected?
          if ( measure->isRejected() ) {
            printf("Coming back in: %s\r",point->id().toLatin1().data());
            numComingBack++;
            m_controlNet->DecrementNumberOfRejectedMeasuresInImage(measure->cubeSerialNumber());
          }

          measure->setRejected(false);
          continue;
        }

        // if it's still rejected, skip it
        if ( measure->isRejected() ) {
          numRejected++;
          totalNumRejected++;
          continue;
        }

        if ( sumSquares > maxResidual ) {
          maxResidual = sumSquares;
          maxResidualIndex = j;
        }
      }

      // no observations above the current rejection limit for this 3D point
      if ( maxResidual == -1.0 || maxResidual <= usedRejectionLimit ) {
          point->setNumberOfRejectedMeasures(numRejected);
          continue;
      }

      // this is another kluge - if we only have two observations
      // we won't reject (for now)
      if ((numMeasures - (numRejected + 1)) < 2) {
          point->setNumberOfRejectedMeasures(numRejected);
          continue;
      }

      // otherwise, we have at least one observation
      // for this point whose residual is above the
      // current rejection limit - we'll flag the
      // worst of these as rejected
      BundleMeasureQsp rejected = point->at(maxResidualIndex);
      rejected->setRejected(true);
      numRejected++;
      point->setNumberOfRejectedMeasures(numRejected);
      m_controlNet->IncrementNumberOfRejectedMeasuresInImage(rejected->cubeSerialNumber());
      totalNumRejected++;

      // do we still have sufficient remaining observations for this 3D point?
      if ( ( numMeasures-numRejected ) < 2 ) {
          point->setRejected(true);
          printf("Rejecting Entire Point: %s\r",point->id().toLatin1().data());
      }
      else
          point->setRejected(false);

    }

    int numberRejectedObservations = 2*totalNumRejected;

    printf("\nRejected Observations:%10d (Rejection Limit:%12.5lf)\n",
            numberRejectedObservations, usedRejectionLimit);
    m_bundleResults.setNumberRejectedObservations(numberRejectedObservations);

    std::cout << "Measures that came back: " << numComingBack << "\n" << std::endl;

    return true;
  }


  /**
  * This method returns the image list used in the bundle adjust. If a QList<ImageList *> was passed
  * into the constructor then it uses that list, otherwise it constructs the QList using the
  * m_serialNumberList
  *
  * @return QList<ImageList *> The ImageLists used for the bundle adjust
  */
  QList<ImageList *> BundleAdjust::imageLists() {

    if (m_imageLists.count() > 0) {
      return m_imageLists;
    }
    else if (m_serialNumberList->size() > 0) {
      ImageList *imgList = new ImageList;
      try {
        for (int i = 0; i < m_serialNumberList->size(); i++) {
          Image *image = new Image(m_serialNumberList->fileName(i));
          imgList->append(image);
          image->closeCube();
        }
        m_imageLists.append(imgList);
      }
      catch (IException &e) {
        QString msg = "Invalid image in serial number list\n";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    else {
      QString msg = "No images used in bundle adjust\n";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return m_imageLists;
  }


  /**
   * Error propagation for solution.
   *
   * @return bool If the error propagation was successful.
   *
   * @throws IException::User "Input data and settings are not sufficiently stable
   *                           for error propagation."
   *
   * @internal
   *   @history 2016-10-05 Ian Humphrey - Updated to check to see if bundle settings is allowing
   *                           us to create the inverse matrix correlation file. References #4315.
   *   @history 2016-10-28 Ian Humphrey - Added extra newline between Error Propagation: Inverse
   *                           Blocking and Filling point covariance messages. References #4463.
   *   @history 2017-11-01 Ken Edmundson - Modified for piecewise polynomial support.
   */
  bool BundleAdjust::errorPropagation() {

    // free unneeded memory
    cholmod_free_triplet(&m_cholmodTriplet, &m_cholmodCommon);
    cholmod_free_sparse(&m_cholmodNormal, &m_cholmodCommon);

    LinearAlgebra::Matrix T(3, 3);
    double sigmaLat, sigmaLon, sigmaRad;
    double t;

    double sigma0Squared = m_bundleResults.sigma0() * m_bundleResults.sigma0();

    int numObjectPoints = m_bundleControlPoints.size();

    std::string currentTime = iTime::CurrentLocalTime().toLatin1().data();
    printf("     Time: %s\n\n", currentTime.c_str());

    // create and initialize array of 3x3 matrices for all object points
    std::vector< symmetric_matrix<double> > pointCovariances(numObjectPoints,
                                                             symmetric_matrix<double>(3));
    for (int d = 0; d < numObjectPoints; d++) {
      pointCovariances[d].clear();
    }

    cholmod_dense *x;        // solution vector
    cholmod_dense *b;        // right-hand side (column vectors of identity)

    b = cholmod_zeros ( m_rank, 1, CHOLMOD_REAL, &m_cholmodCommon );
    double *pb = (double*)b->x;

    double *px = NULL;

    SparseBlockColumnMatrix inverseMatrix;

    // Create unique file name
    FileName matrixFile(m_bundleSettings->outputFilePrefix() + "inverseMatrix.dat");
    //???FileName matrixFile = FileName::createTempFile(m_bundleSettings.outputFilePrefix()
    //???                                               + "inverseMatrix.dat");
    // Create file handle
    QFile matrixOutput(matrixFile.expanded());

    // Check to see if creating the inverse correlation matrix is turned on
    if (m_bundleSettings->createInverseMatrix()) {
      // Open file to write to
      matrixOutput.open(QIODevice::WriteOnly);
    }
    QDataStream outStream(&matrixOutput);

    int i, j, k;
    int columnIndex = 0;
    int numColumns = 0;
    int numBlockColumns = m_sparseNormals.size();
    int observationIndex = -1;
    int sigmaColumn = 0;
    for (i = 0; i < numBlockColumns; i++) {
      // columns in this column block
      SparseBlockColumnMatrix *normalsColumn = m_sparseNormals.at(i);
      if (i == 0) {
        numColumns = normalsColumn->numberOfColumns();
        int numRows = normalsColumn->numberOfRows();
        inverseMatrix.insertMatrixBlock(i, numRows, numColumns);
        inverseMatrix.zeroBlocks();
      }
      else {
        if (normalsColumn->numberOfColumns() == numColumns) {
          int numRows = normalsColumn->numberOfRows();
          inverseMatrix.insertMatrixBlock(i, numRows, numColumns);
          inverseMatrix.zeroBlocks();
        }
        else {
          numColumns = normalsColumn->numberOfColumns();

          // reset inverseMatrix
          inverseMatrix.wipe();

          // insert blocks
          for (j = 0; j < (i+1); j++) {
            SparseBlockColumnMatrix *normalsRow = m_sparseNormals.at(j);
            int numRows = normalsRow->numberOfRows();

            inverseMatrix.insertMatrixBlock(j, numRows, numColumns);
          }
        }
      }

      int localCol = 0;

      // solve for inverse for nCols
      for (j = 0; j < numColumns; j++) {
        if ( columnIndex > 0 ) {
          pb[columnIndex - 1] = 0.0;
        }
        pb[columnIndex] = 1.0;

        x = cholmod_solve ( CHOLMOD_A, m_L, b, &m_cholmodCommon );
        px = (double*)x->x;
        int rp = 0;

        // store solution in corresponding column of inverse
        for (k = 0; k < inverseMatrix.size(); k++) {
          LinearAlgebra::Matrix *matrix = inverseMatrix.value(k);

          int sz1 = matrix->size1();

          for (int ii = 0; ii < sz1; ii++) {
            (*matrix)(ii,localCol) = px[ii + rp];
          }
          rp += matrix->size1();
        }

        columnIndex++;
        localCol++;

        cholmod_free_dense(&x,&m_cholmodCommon);
      }

      // save adjusted target body sigmas if solving for target
      if (m_bundleSettings->solveTargetBody() && i == 0) {
        vector< double > &adjustedSigmas = m_bundleTargetBody->adjustedSigmas();
        matrix< double > *targetCovMatrix = inverseMatrix.value(i);

        for (int z = 0; z < numColumns; z++)
          adjustedSigmas[z] = sqrt((*targetCovMatrix)(z,z))*m_bundleResults.sigma0();
      }
      // save adjusted image sigmas
      else {
        BundleObservationQsp observation;
        if (m_bundleSettings->solveTargetBody()) {
          observation = m_bundleObservations.at(i-1);
          sigmaColumn = 0;
        }
        else {
          // reset sigma column if observation index has changed
          if (normalsColumn->observationIndex() != observationIndex) {
            sigmaColumn = 0;
          }
          observationIndex = normalsColumn->observationIndex();
          observation = m_bundleObservations.at(observationIndex);
        }
        vector< double > &adjustedSigmas = observation->adjustedSigmas();
        matrix< double > *imageCovMatrix = inverseMatrix.value(i);
        for ( int z = 0; z < numColumns; z++) {
          adjustedSigmas[sigmaColumn] = sqrt((*imageCovMatrix)(z,z))*m_bundleResults.sigma0();
          sigmaColumn++;
        }
      }

      // Output the inverse matrix if requested
      if (m_bundleSettings->createInverseMatrix()) {
        outStream << inverseMatrix;
      }

      // now loop over all object points to sum contributions into 3x3 point covariance matrix
      int pointIndex = 0;
      for (j = 0; j < numObjectPoints; j++) {

        BundleControlPointQsp point = m_bundleControlPoints.at(pointIndex);
        if ( point->isRejected() ) {
          continue;
        }

        // only update point every 100 points
        if (j%100 == 0) {
            printf("\rError Propagation: Inverse Block %8d of %8d; Point %8d of %8d", i+1,
                   numBlockColumns,  j+1, numObjectPoints);

            emit iterationUpdate(i+1, j+1);
        }

        // get corresponding Q matrix
        // NOTE: we are getting a reference to the Q matrix stored
        //       in the BundleControlPoint for speed (without the & it is dirt slow)
        SparseBlockRowMatrix &Q = point->cholmodQMatrix();

        T.clear();

        // get corresponding point covariance matrix
        boost::numeric::ublas::symmetric_matrix<double> &covariance = pointCovariances[pointIndex];

        // get firstQBlock - index i is the key into Q for firstQBlock
        LinearAlgebra::Matrix *firstQBlock = Q.value(i);
        if (!firstQBlock) {
          pointIndex++;
          continue;
        }

        // iterate over Q
        // secondQBlock is current map value
        QMapIterator< int, LinearAlgebra::Matrix * > it(Q);
        while ( it.hasNext() ) {
          it.next();

          int nKey = it.key();

          if (it.key() > i) {
            break;
          }

          LinearAlgebra::Matrix *secondQBlock = it.value();

          if ( !secondQBlock ) {// should never be NULL
            continue;
          }

          LinearAlgebra::Matrix *inverseBlock = inverseMatrix.value(it.key());

          if ( !inverseBlock ) {// should never be NULL
            continue;
          }

          T = prod(*inverseBlock, trans(*firstQBlock));
          T = prod(*secondQBlock,T);

          if (nKey != i) {
            T += trans(T);
          }

          try {
            covariance += T;
          }

          catch (std::exception &e) {
            printf("\n\n");
            QString msg = "Input data and settings are not sufficiently stable "
                          "for error propagation.";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
        pointIndex++;
      }
    }

    if (m_bundleSettings->createInverseMatrix()) {
      // Close the file.
      matrixOutput.close();
      // Save the location of the "covariance" matrix
      m_bundleResults.setCorrMatCovFileName(matrixFile);
    }

    // can free sparse normals now
    m_sparseNormals.wipe();

    // free b (right-hand side vector)
    cholmod_free_dense(&b,&m_cholmodCommon);

    printf("\n\n");
    currentTime = Isis::iTime::CurrentLocalTime().toLatin1().data();
    printf("\rFilling point covariance matrices: Time %s", currentTime.c_str());
    printf("\n\n");

    // now loop over points again and set final covariance stuff
    int pointIndex = 0;
    for (j = 0; j < numObjectPoints; j++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(pointIndex);

      if ( point->isRejected() ) {
        continue;
      }

      if (j%100 == 0) {
        printf("\rError Propagation: Filling point covariance matrices %8d of %8d\r",j+1,
               numObjectPoints);
      }

      // get corresponding point covariance matrix
      boost::numeric::ublas::symmetric_matrix<double> &covariance = pointCovariances[pointIndex];

      // Ask Ken what is happening here...Setting just the sigmas is not very accurate
      // Shouldn't we be updating and setting the matrix???  TODO
      SurfacePoint SurfacePoint = point->adjustedSurfacePoint();

      sigmaLat = SurfacePoint.GetLatSigma().radians();
      sigmaLon = SurfacePoint.GetLonSigma().radians();
      sigmaRad = SurfacePoint.GetLocalRadiusSigma().meters();

      t = sigmaLat * sigmaLat + covariance(0, 0);
      Distance latSigmaDistance(sqrt(sigma0Squared * t) * m_radiansToMeters, Distance::Meters);

      t = sigmaLon * sigmaLon + covariance(1, 1);
      t = sqrt(sigma0Squared * t) * m_radiansToMeters;
      Distance lonSigmaDistance(
          t * cos(point->adjustedSurfacePoint().GetLatitude().radians()),
          Distance::Meters);

      t = sigmaRad*sigmaRad + covariance(2, 2);
      t = sqrt(sigma0Squared * t) * 1000.0;

      SurfacePoint.SetSphericalSigmasDistance(latSigmaDistance, lonSigmaDistance,
                                              Distance(t, Distance::Meters));

      point->setAdjustedSurfacePoint(SurfacePoint);

      pointIndex++;
    }

    return true;
  }


  /**
   * Returns a pointer to the output control network.
   *
   * @return ControlNetQsp A shared pointer to the output control network.
   */
  ControlNetQsp BundleAdjust::controlNet() {
    return m_controlNet;
  }


  /**
   * Returns a pointer to the serial number list.
   *
   * @return SerialNumberList* A pointer to the serial number list.
   */
  SerialNumberList *BundleAdjust::serialNumberList() {
    return m_serialNumberList;
  }


  /**
   * Returns the number of images.
   *
   * @return int The number of images.
   */
  int BundleAdjust::numberOfImages() const {
    return m_serialNumberList->size();
  }


  /**
   * Return the ith filename in the cube list file given to constructor.
   *
   * @param i The index of the cube.
   *
   * @return QString The filename of the cube.
   *
   * @todo: probably don't need this, can get from BundleObservation
   */
  QString BundleAdjust::fileName(int i) {
    return m_serialNumberList->fileName(i);
  }


  /**
   * Returns what iteration the BundleAdjust is currently on.
   *
   * @return double The current iteration number.
   */
  double BundleAdjust::iteration() const {
    return m_iteration;
  }


  /**
   * Return a table cmatrix for the ith cube in the cube list given to the constructor.
   *
   * @param i The index of the cube
   *
   * @return Table The InstrumentPointing table for the cube.
   */
  Table BundleAdjust::cMatrix(int i) {
    return m_controlNet->Camera(i)->instrumentRotation()->Cache("InstrumentPointing");
  }


  /**
   * Return a table spacecraft vector for the ith cube in the cube list given to the
   * constructor.
   *
   * @param i The index of the cube
   *
   * @return Table The InstrumentPosition table for the cube.
   */
  Table BundleAdjust::spVector(int i) {
    return m_controlNet->Camera(i)->instrumentPosition()->Cache("InstrumentPosition");
  }


  /**
   * Creates an iteration summary and an iteration group for the solution summary
   *
   * @internal
   *   @history 2016-10-18 Ian Humphrey - Always output Rejection_Measures keyword regardless
   *                           of outlier rejection so we can match ISIS jigsaw output.
   *                           Fixes #4461.
   */
  void BundleAdjust::iterationSummary() {
    QString iterationNumber;

    if ( m_bundleResults.converged() ) {
        iterationNumber = "Iteration" + toString(m_iteration) + ": Final";
    }
    else {
        iterationNumber = "Iteration" + toString(m_iteration);
    }

    PvlGroup summaryGroup(iterationNumber);

    summaryGroup += PvlKeyword("Sigma0",
                               toString( m_bundleResults.sigma0() ) );
    summaryGroup += PvlKeyword("Observations",
                               toString( m_bundleResults.numberObservations() ) );
    summaryGroup += PvlKeyword("Constrained_Point_Parameters",
                               toString( m_bundleResults.numberConstrainedPointParameters() ) );
    summaryGroup += PvlKeyword("Constrained_Image_Parameters",
                               toString( m_bundleResults.numberConstrainedImageParameters() ) );
    if (m_bundleSettings->bundleTargetBody()) {
      summaryGroup += PvlKeyword("Constrained_Target_Parameters",
                                toString( m_bundleResults.numberConstrainedTargetParameters() ) );
    }
    summaryGroup += PvlKeyword("Unknown_Parameters",
                               toString( m_bundleResults.numberUnknownParameters() ) );
    summaryGroup += PvlKeyword("Degrees_of_Freedom",
                               toString( m_bundleResults.degreesOfFreedom() ) );
    summaryGroup += PvlKeyword( "Rejected_Measures",
                                toString( m_bundleResults.numberRejectedObservations()/2) );

    if ( m_bundleResults.numberMaximumLikelihoodModels() >
         m_bundleResults.maximumLikelihoodModelIndex() ) {
      // if maximum likelihood estimation is being used

      summaryGroup += PvlKeyword("Maximum_Likelihood_Tier: ",
                                 toString( m_bundleResults.maximumLikelihoodModelIndex() ) );
      summaryGroup += PvlKeyword("Median_of_R^2_residuals: ",
                                 toString( m_bundleResults.maximumLikelihoodMedianR2Residuals() ) );
    }

    if ( m_bundleResults.converged() ) {
      summaryGroup += PvlKeyword("Converged", "TRUE");
      summaryGroup += PvlKeyword("TotalElapsedTime", toString( m_bundleResults.elapsedTime() ) );

      if (m_bundleSettings->errorPropagation()) {
        summaryGroup += PvlKeyword("ErrorPropagationElapsedTime",
                                   toString( m_bundleResults.elapsedTimeErrorProp() ) );
      }
    }

    std::ostringstream ostr;
    ostr << summaryGroup << std::endl;
    m_iterationSummary += QString::fromStdString( ostr.str() );
    if (m_printSummary) {
      Application::Log(summaryGroup);
    }
  }


  /**
   * Returns if the BundleAdjust converged.
   *
   * @return bool If the BundleAdjust converged.
   */
  bool BundleAdjust::isConverged() {
    return m_bundleResults.converged();
  }


  /**
   * Returns the iteration summary string.
   *
   * @return QString the iteration summary string.
   *
   * @see iterationSummary()
   */
  QString BundleAdjust::iterationSummaryGroup() const {
    return m_iterationSummary;
  }


  /**
   * Slot for deltack and jigsaw to output the bundle status.
   *
   * @param status The bundle status string to output.
   *
   * @internal
   *   @history 2016-12-01 Ian Humphrey - Added %s as first paramter to prevent a
   *                           -Wformat-security warning during the build.
   */
  void BundleAdjust::outputBundleStatus(QString status) {
    status += "\n";
    printf("%s", status.toStdString().c_str());
  }



  /**
   * @brief Compute Bundle statistics and store them in m_bundleResults.
   *
   * Sets:
   * m_rmsImageSampleResiduals
   * m_rmsImageLineResiduals
   * m_rmsImageResiduals
   *
   * m_rmsImageXSigmas
   * m_rmsImageYSigmas
   * m_rmsImageZSigmas
   * m_rmsImageRASigmas
   * m_rmsImageDECSigmas
   * m_rmsImageTWISTSigmas
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
   * @return bool If the statistics were successfully computed and stored.
   */
  bool BundleAdjust::computeBundleStatistics() {

    // use qvectors so that we can set the size.
    // this will be useful later when adding data.
    // data may added out of index order
    int numberImages = m_serialNumberList->size();
    QVector<Statistics> rmsImageSampleResiduals(numberImages);
    QVector<Statistics> rmsImageLineResiduals(numberImages);
    QVector<Statistics> rmsImageResiduals(numberImages);

    int numObjectPoints = m_bundleControlPoints.size();
    for (int i = 0; i < numObjectPoints; i++) {

      const BundleControlPointQsp point = m_bundleControlPoints.at(i);

      if (point->isRejected()) {
        continue;
      }

      int numMeasures = point->numberOfMeasures();
      for (int j = 0; j < numMeasures; j++) {

        const BundleMeasureQsp measure = point->at(j);

        if (measure->isRejected()) {
          continue;
        }

        double sampleResidual = fabs(measure->sampleResidual());
        double lineResidual = fabs(measure->lineResidual());

        // Determine the index for this measure's serial number
        int imageIndex = m_serialNumberList->serialNumberIndex(measure->cubeSerialNumber());

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

      int numPoints = m_bundleControlPoints.size();
      // initialize max and min values to those from first valid point
      for (int i = 0; i < numPoints; i++) {

        const BundleControlPointQsp point = m_bundleControlPoints.at(i);

        maxSigmaLatDist = point->adjustedSurfacePoint().GetLatSigmaDistance();;
        minSigmaLatDist = maxSigmaLatDist;

        maxSigmaLonDist = point->adjustedSurfacePoint().GetLonSigmaDistance();;
        minSigmaLonDist = maxSigmaLonDist;

        maxSigmaLatPointId = point->id();
        maxSigmaLonPointId = maxSigmaLatPointId;
        minSigmaLatPointId = maxSigmaLatPointId;
        minSigmaLonPointId = maxSigmaLatPointId;

        if (m_bundleSettings->solveRadius()) {
          maxSigmaRadDist = point->adjustedSurfacePoint().GetLocalRadiusSigma();
          minSigmaRadDist = maxSigmaRadDist;

          maxSigmaRadPointId = maxSigmaLatPointId;
          minSigmaRadPointId = maxSigmaLatPointId;
        }
        break;
      }

      for (int i = 0; i < numPoints; i++) {

        const BundleControlPointQsp point = m_bundleControlPoints.at(i);

        sigmaLatDist = point->adjustedSurfacePoint().GetLatSigmaDistance();
        sigmaLonDist = point->adjustedSurfacePoint().GetLonSigmaDistance();
        sigmaRadDist = point->adjustedSurfacePoint().GetLocalRadiusSigma();

        sigmaLatStats.AddData(sigmaLatDist.meters());
        sigmaLonStats.AddData(sigmaLonDist.meters());
        sigmaRadStats.AddData(sigmaRadDist.meters());

        if (sigmaLatDist > maxSigmaLatDist) {
          maxSigmaLatDist = sigmaLatDist;
          maxSigmaLatPointId = point->id();
        }
        if (sigmaLonDist > maxSigmaLonDist) {
          maxSigmaLonDist = sigmaLonDist;
          maxSigmaLonPointId = point->id();
        }
        if (m_bundleSettings->solveRadius()) {
          if (sigmaRadDist > maxSigmaRadDist) {
            maxSigmaRadDist = sigmaRadDist;
            maxSigmaRadPointId = point->id();
          }
        }
        if (sigmaLatDist < minSigmaLatDist) {
          minSigmaLatDist = sigmaLatDist;
          minSigmaLatPointId = point->id();
        }
        if (sigmaLonDist < minSigmaLonDist) {
          minSigmaLonDist = sigmaLonDist;
          minSigmaLonPointId = point->id();
        }
        if (m_bundleSettings->solveRadius()) {
          if (sigmaRadDist < minSigmaRadDist) {
            minSigmaRadDist = sigmaRadDist;
            minSigmaRadPointId = point->id();
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
