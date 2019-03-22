 #include "BundleAdjust.h"

// std lib
#include <iomanip>
#include <iostream>
#include <sstream>

// qt lib
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFutureWatcher>
#include <QMutex>
#include <QtConcurrentRun>

// boost lib
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

// Isis lib
#include "Application.h"
#include "BundleLidarControlPoint.h"
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
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "MaximumLikelihoodWFunctions.h"
#include "SpecialPixel.h"
#include "StatCumProbDistDynCalc.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace boost::numeric::ublas;
//using namespace Isis;

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
      m_controlNet = ControlNetQsp( new ControlNet(cnetFile, &progress,
                                     bundleSettings->controlPointCoordTypeReports()) );
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
   * Construct a BundleAdjust object from the given settings, control network file,
   * cube list, and lidar point data.
   *
   * @param bundleSettings A shared pointer to the BundleSettings to be used.
   * @param cnetFile The filename of the control network to be used.
   * @param cubeList The list of filenames of the cubes to be adjusted.
   * @param lidarDataFile Lidar point dataset filename.
   * @param printSummary If summaries should be printed each iteration.
   */
  BundleAdjust::BundleAdjust(BundleSettingsQsp bundleSettings,
                             const QString &cnetFile,
                             const QString &cubeList,
                             const QString &lidarDataFile,
                             bool printSummary) {
    m_abort = false;
    Progress progress;
    // initialize constructor dependent settings...
    // m_printSummary, m_cleanUp, m_cnetFileName, m_controlNet,
    // m_serialNumberList, m_bundleSettings
    m_printSummary = printSummary;
    m_cleanUp = true;
    m_cnetFileName = cnetFile;
    m_lidarFileName = lidarDataFile;
    try {
      m_controlNet = ControlNetQsp( new ControlNet(cnetFile, &progress) );
    }
    catch (IException &e) {
      throw;
    }

    // read lidar point data file
    try {
      m_lidarDataSet = LidarDataQsp( new LidarData());
      m_lidarDataSet->read(lidarDataFile);
    }
    catch (IException &e) {
      throw;
    }

    m_bundleResults.setOutputControlNet(m_controlNet);
    m_bundleResults.setOutputLidarData(m_lidarDataSet);
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
    try {
      m_controlNet = ControlNetQsp( new ControlNet(control.fileName()) );
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
    emit(statusUpdate("Initialization"));
    m_previousNumberImagePartials = 0;

    // initialize
    //
    // JWB
    // - some of these not originally initialized.. better values???
    m_iteration = 0;
    m_rank = 0;
    m_iterationSummary = "";

    // Get the cameras set up for all images
    // NOTE - THIS IS NOT THE SAME AS "setImage" as called in BundleAdjust::computePartials
    // this call only does initializations; sets measure's camera pointer, etc
    // RENAME????????????
    m_controlNet->SetImages(*m_serialNumberList, progress);

    if (m_lidarDataSet) {
      // TODO: (KLE) document why we're (at the moment) required to use an existing control net to
      //       SetImages for the lidar data set. In my opinion this is a major drawback to this
      //       implementation, and a really good argument for a control net design that allows
      //       multiple point sources in the same net (e.g. photogrammetric, lidar, and other?
      //       types).
      m_lidarDataSet->SetImages(*(m_controlNet.data()), progress);
    }

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

    // should we initialize objects m_xResiduals, m_yResiduals, m_xyResiduals?

    // set up BundleObservations and assign solve settings for each from BundleSettings class
    for (int i = 0; i < numImages; i++) {
      Camera *camera = m_controlNet->Camera(i);
      QString observationNumber = m_serialNumberList->observationNumber(i);
      QString instrumentId = m_serialNumberList->spacecraftInstrumentId(i);
      QString serialNumber = m_serialNumberList->serialNumber(i);
      QString fileName = m_serialNumberList->fileName(i);

      // create a new BundleImage and add to new (or existing if observation mode) BundleObservation
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
    }

    // set up vector of BundleControlPoints
    int numControlPoints = m_controlNet->GetNumPoints();
    for (int i = 0; i < numControlPoints; i++) {
      ControlPoint *point = m_controlNet->GetPoint(i);
      if (point->IsIgnored()) {
        continue;
      }

      BundleControlPointQsp bundleControlPoint(new BundleControlPoint(m_bundleSettings, point));
      m_bundleControlPoints.append(bundleControlPoint);

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
        measure->setSigma(1.4);
      }

      point->ComputeApriori();
    }

    // set up vector of BundleLidarControlPoints
    int numLidarPoints = 0;
    if (m_lidarDataSet) {
      numLidarPoints = m_lidarDataSet->points().size();
    }
    for (int i = 0; i < numLidarPoints; i++) {
      LidarControlPointQsp lidarPoint = m_lidarDataSet->points().at(i);
      if (lidarPoint->IsIgnored()) {
        continue;
      }

      if (!lidarPoint->GetId().contains("Lidar7696")) {
        continue;
      }

      BundleLidarControlPointQsp bundleLidarPoint(new BundleLidarControlPoint(m_bundleSettings,
                                                                              lidarPoint));
      m_bundleLidarControlPoints.append(bundleLidarPoint);

      // set parent observation for each BundleMeasure
      int numMeasures = bundleLidarPoint->size();
      for (int j = 0; j < numMeasures; j++) {
        BundleMeasureQsp measure = bundleLidarPoint->at(j);
        QString cubeSerialNumber = measure->cubeSerialNumber();

        BundleObservationQsp observation =
            m_bundleObservations.observationByCubeSerialNumber(cubeSerialNumber);
        BundleImageQsp image = observation->imageByCubeSerialNumber(cubeSerialNumber);

        measure->setParentObservation(observation);
        measure->setParentImage(image);
        measure->setSigma(30.0*1.4);
      }

      // WHY ARE WE CALLING COMPUTE APRIORI FOR LIDAR POINTS?
      // ANSWER: Because the ::computeApriori method is also setting the focal plane measures, see
      // line 916 in ControlPoint.Constrained_Point_Parameters
      // This really stinks, maybe we should be setting the focal plane measures here, as part of
      // the BundleAdjust::init method? Or better yet as part of the BundleControlPoint constructor?
      // right now we have a kluge in the ControlPoint::setApriori method to not update the coordinates
      // of lidar points
      // Also, maybe we could address Brents constant complaint about points where we can't get a lat or
      // lon due to bad SPICE causing the bundle to fail
      lidarPoint->ComputeApriori();

      // initialize range constraints
      bundleLidarPoint->initializeRangeConstraints();
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

    // NOTE that this will now include lidar points if any
    int num3DPoints = m_bundleControlPoints.size() + m_bundleLidarControlPoints.size();

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

    outputBundleStatus("\nValidation complete!...\n");
    
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
      m_bundleObservations.at(i)->setNormalsMatrixStartBlock(blockColumn);

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
   * Compute the least squares bundle adjustment solution using Cholesky decomposition.
   *
   * @return BundleSolutionInfo A container with settings and results from the adjustment.
   *
   * @see solveCholesky()
   *
   * @todo make solveCholesky return a BundleSolutionInfo object and delete this placeholder ???
   */
  BundleSolutionInfo* BundleAdjust::solveCholeskyBR() {
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
    emit(statusBarUpdate("Solving"));
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

      // Only use target body solution options when using Latitudinal coordinates
      if (m_bundleTargetBody && m_bundleTargetBody->solveTriaxialRadii()
         && m_bundleSettings->controlPointCoordTypeBundle() == SurfacePoint::Latitudinal) {
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

      // Beginning of iterations
      m_iteration = 1;
      double vtpv = 0.0;
      double previousSigma0 = 0.0;
      
      // start the clock
      clock_t solveStartClock = clock();

      for (;;) {

        emit iterationUpdate(m_iteration);

        // testing
        if (m_abort) {
          m_bundleResults.setConverged(false);
          emit statusUpdate("\n aborting...");
          emit finished();
          return false;
        }
        // testing

        emit statusUpdate( QString("\nstarting iteration %1\n").arg(m_iteration) );

        clock_t iterationStartClock = clock();

        // zero normals (after iteration 0)
        if (m_iteration != 1) {
          m_sparseNormals.zeroBlocks();
        }

//        clock_t formNormalsClock1 = clock();
        // form normal equations for photogrammetric points
        if (!formNormalEquations()) {
          m_bundleResults.setConverged(false);
          break;
        }
//        clock_t formNormalsClock2 = clock();

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
          outputBundleStatus("\nsolve failed!");
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
        // Compute residuals for all measures on all points for this iteration.
        // The mm residuals are stored in each BundleMeasure and the pixel
        // residuals are stored in each ControlMeasure.
        emit(statusBarUpdate("Computing Residuals"));
        computeResiduals();

        // compute vtpv (weighted sum of squares of residuals
        vtpv = computeVtpv();

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

        // Sigma0 (or "sigma nought") is the standard deviation of an observation of unit weight.
        // Sigma0^2 is the variance of an observation of unit weight (also reference variance or
        // variance factor).
        m_bundleResults.computeSigma0(vtpv, m_bundleSettings->convergenceCriteria());

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
              emit statusBarUpdate("Converged");

              clock_t iterationStopClock = clock();
              m_iterationTime = (iterationStopClock - iterationStartClock)
                                      / (double)CLOCKS_PER_SEC;
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
            emit statusUpdate("Bundle has converged\n");
            emit statusBarUpdate("Converged");
            break;
          }
        }

        m_bundleResults.printMaximumLikelihoodTierInformation();
        clock_t iterationStopClock = clock();
        m_iterationTime = (iterationStopClock - iterationStartClock)
                                / (double)CLOCKS_PER_SEC;

        // check for maximum iterations
        if (m_iteration >= m_bundleSettings->convergenceCriteriaMaximumIterations()) {
          emit(statusBarUpdate("Max Iterations Reached"));
          break;
        }

        // restart the dynamic calculation of the cumulative probility distribution of residuals
        // (in unweighted pixels) --so it will be up to date for the next iteration
        if (!m_bundleResults.converged()) {
          m_bundleResults.initializeResidualsProbabilityDistribution(101);
        }
        // TODO: is this necessary ???
        // probably already initialized to 101 nodes in bundle settings constructor...

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
        
        outputBundleStatus("\nStarting Error Propagation");
        
        errorPropagation();
        emit statusUpdate("\n\nError Propagation Complete\n");
        clock_t errorPropStopClock = clock();
        m_bundleResults.setElapsedTimeErrorProp((errorPropStopClock - errorPropStartClock)
                                                / (double)CLOCKS_PER_SEC);
      }

      clock_t solveStopClock = clock();
      m_bundleResults.setElapsedTime((solveStopClock - solveStartClock)
                                     / (double)CLOCKS_PER_SEC);

      computeBundleStatistics();

      m_bundleResults.setIterations(m_iteration);
      m_bundleResults.setObservations(m_bundleObservations);
      m_bundleResults.setBundleControlPoints(m_bundleControlPoints);

      if (!m_bundleLidarControlPoints.isEmpty()) {
        m_bundleResults.setBundleLidarPoints(m_bundleLidarControlPoints);
      }

      emit resultsReady(bundleSolveInformation());

      emit statusUpdate("\nBundle Complete\n");

      iterationSummary();
    }
    catch (IException &e) {
      m_bundleResults.setConverged(false);
      emit statusUpdate("\n aborting...");
      emit statusBarUpdate("Failed to Converge");
      emit finished();
      QString msg = "Could not solve bundle adjust.";
      throw IException(e, e.errorType(), msg, _FILEINFO_);
    }

    emit finished();
    return true;
  }


  /**
   * Compute image measure residuals.
   */
  void BundleAdjust::computeResiduals() {

    // residuals for photogrammetric measures
    m_bundleControlPoints.computeMeasureResiduals();

    // residuals for lidar measures
    if (!m_bundleLidarControlPoints.isEmpty()) {
      emit(statusBarUpdate("Computing Lidar Measure Residuals"));
      m_bundleLidarControlPoints.computeMeasureResiduals();
    }
  }


  /**
   * Create a BundleSolutionInfo containing the settings and results from the bundle adjustment.
   *
   * @return @b BundleSolutionInfo A container with solve information from the adjustment. NOTE:
   *            Caller takes ownership and is responsible for memory management of returned
   *            BundleSolutionInfo raw pointer.
   *
   */
  BundleSolutionInfo *BundleAdjust::bundleSolveInformation() {
    BundleSolutionInfo *bundleSolutionInfo = new BundleSolutionInfo(m_bundleSettings,
                                                                    FileName(m_cnetFileName),
                                                                    FileName(m_lidarFileName),
                                                                    m_bundleResults,
                                                                    imageLists());
    bundleSolutionInfo->setRunTime("");
    return bundleSolutionInfo;
  }

  /**
   * Contribution to the normal equations matrix from photogrammetric points.
   *
   * @return bool
   *
   * @see formNormalEquations()
   * @see formMeasureNormals()
   * @see formPointNormals()
   * @see formWeightedNormals()
   */
  bool BundleAdjust::formNormalEquations() {

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

    // loop over 3D photogrammetry points
    int numObservations = 0;
    int numGood3DPoints = 0;
    int numRejected3DPoints = 0;
    int numConstrainedCoordinates = 0;
    int pointIndex = 0;
    int num3DPoints = m_bundleControlPoints.size();

    for (int i = 0; i < num3DPoints; i++) {
      emit(pointUpdate(i+1));
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

          bool status = computePartials(coeffTarget, coeffImagePosition, coeffImagePointing,
                                   coeffPoint3D, coeffRHS, *measure);
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

        // increment number of observations
        numObservations += 2;

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
      numConstrainedCoordinates += formPointNormals(N22, N12, n2, m_RHS, point);
//      clock_t formPointNormalsClock2 = clock();

      //      cumulativeFormPointNormalsTime += (formPointNormalsClock2 - formPointNormalsClock1)
//          / (double)CLOCKS_PER_SEC;

      pointIndex++;

      numGood3DPoints++;
    } // end loop over 3D points

    m_bundleResults.setNumberConstrainedPointParameters(numConstrainedCoordinates);
    m_bundleResults.setNumberImageObservations(numObservations);

    int numRejectedLidarPoints = 0.0;
    int numGoodLidarPoints = 0.0;
    numObservations = 0;
    numConstrainedCoordinates = 0;

    // loop over lidar points
    int numLidarPoints = m_bundleLidarControlPoints.size();
    m_numLidarConstraints = 0;

    for (int i = 0; i < numLidarPoints; i++) {
      emit(pointUpdate(i+1));
      BundleLidarControlPointQsp point = m_bundleLidarControlPoints.at(i);

      if (!point->id().contains("Lidar7696")) {
        continue;
      }

      if (point->isRejected()) {
        numRejectedLidarPoints++;

        pointIndex++;
        continue;
      }

      N22.clear();
      N12.wipe();
      n2.clear();

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

          bool status = computePartials(coeffTarget, coeffImagePosition, coeffImagePointing,
                                   coeffPoint3D, coeffRHS, *measure);
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

        // increment number of lidar image "measurement" observations
        numObservations += 2;

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

//        if (point->id().contains("Lidar7696")) {
//          m_numLidarConstraints += point->applyLidarRangeConstraint(m_sparseNormals, N22, N12, n1,
//                                                                    n2, measure);
//        }

      } // end loop over this points measures

//      clock_t formPointNormalsClock1 = clock();
      numConstrainedCoordinates += formLidarPointNormals(N22, N12, n2, m_RHS, point);
//      clock_t formPointNormalsClock2 = clock();

      //      cumulativeFormPointNormalsTime += (formPointNormalsClock2 - formPointNormalsClock1)
//          / (double)CLOCKS_PER_SEC;

      pointIndex++;

      numGoodLidarPoints++;
    } // end loop over lidar 3D points

    m_bundleResults.setNumberConstrainedLidarPointParameters(numConstrainedCoordinates);
    m_bundleResults.setNumberLidarImageObservations(numObservations);


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

    return true;
  }


  /**
   * Form the least-squares normal equations matrix.
   *
   * TODO: comments below belong in documentation for BundleControlPoint, not here
   *
   * Each BundleControlPoint stores its Q matrix and NIC vector.
   * Limiting error portion of each points covariance matrix is stored in its adjusted surface
   * point.
   *
   * @return bool
   *
   * @see formPhotoNormalEquations()
   * @see formLidarNormalEquations()
   * @see formMeasureNormals()
   * @see formPointNormals()
   * @see formWeightedNormals()
   */
/*
  bool BundleAdjust::formNormalEquations() {
    emit(statusBarUpdate("Forming Normal Equations"));

    // reset statistics for next iteration
    m_bundleResults.initializeNewIteration();

    outputBundleStatus("\n\n");

    int numGoodPhotoPoints = 0;
    int numGoodLidarPoints = 0;

    // process photogrammetric points
    numGoodPhotoPoints = formPhotoNormalEquations();
    if (numGoodPhotoPoints <= 0) {
      return false;
    }

    // process lidar points, if any
    if (!m_bundleLidarControlPoints.isEmpty() ) {
      emit(statusBarUpdate("Lidar Point Contribution to Normal Equations"));
      numGoodLidarPoints = formLidarNormalEquations();
      if (numGoodLidarPoints <= 0) {
        return false;
      }
    }

    // update number of unknown parameters
    m_bundleResults.setNumberUnknownParameters(m_rank + 3*(numGoodPhotoPoints+numGoodLidarPoints));

    return true;
  }
*/

  /**
   * Contribution to the normal equations matrix from photogrammetric points.
   *
   * @return bool
   *
   * @see formNormalEquations()
   * @see formMeasureNormals()
   * @see formPointNormals()
   * @see formWeightedNormals()
   */
  int BundleAdjust::formPhotoNormalEquations() {

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
    int numObservations = 0;
    int numGood3DPoints = 0;
    int numRejected3DPoints = 0;
    int numConstrainedCoordinates = 0;
    int pointIndex = 0;
    int num3DPoints = m_bundleControlPoints.size();

    for (int i = 0; i < num3DPoints; i++) {
      emit(pointUpdate(i+1));
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

          bool status = computePartials(coeffTarget, coeffImagePosition, coeffImagePointing,
                                   coeffPoint3D, coeffRHS, *measure);
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

        // increment number of observations
        numObservations += 2;

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
      numConstrainedCoordinates += formPointNormals(N22, N12, n2, m_RHS, point);
//      clock_t formPointNormalsClock2 = clock();

      //      cumulativeFormPointNormalsTime += (formPointNormalsClock2 - formPointNormalsClock1)
//          / (double)CLOCKS_PER_SEC;

      pointIndex++;

      numGood3DPoints++;
    } // end loop over 3D points

    m_bundleResults.setNumberConstrainedPointParameters(numConstrainedCoordinates);
    m_bundleResults.setNumberImageObservations(numObservations);

//  qDebug() << "cumulative computePartials() Time: " << cumulativeComputePartialsTime;
//  qDebug() << "cumulative formMeasureNormals() Time: " << cumulativeFormMeasureNormalsTime;
//  qDebug() << "cumulative formPointNormals() Time: " << cumulativeFormPointNormalsTime;

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

    return numGood3DPoints;
  }


  /**
   * Contribution to normal equations matrix from lidar points.
   *
   * For simultaneously acquired image and lidar observations includes range constraint between the
   * lidar point on the surface and the corresponding simultaneous image(s).
   *
   * There could be multiple simultaneous images acquired with a lidar observation, e.g. LROC NAC
   * left and right cameras.
   *
   * @return bool
   *
   * @see formNormalEquations()
   * @see formMeasureNormals()
   * @see formPointNormals()
   * @see formWeightedNormals()
   */
  int BundleAdjust::formLidarNormalEquations() {

    // TODO timing tests
//    double cumulativeComputePartialsTime = 0.0;
//    double cumulativeFormMeasureNormalsTime = 0.0;
//    double cumulativeFormPointNormalsTime = 0.0;

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

    // loop over 3D points
    int numObservations = 0;
    int numGoodLidarPoints = 0;
    int numRejectedLidarPoints = 0;
    int numConstrainedCoordinates = 0;
    int pointIndex = 0;
    int numLidarPoints = m_bundleLidarControlPoints.size();

    m_numLidarConstraints = 0;

    for (int i = 0; i < numLidarPoints; i++) {
      emit(pointUpdate(i+1));
      BundleLidarControlPointQsp point = m_bundleLidarControlPoints.at(i);

      if (!point->id().contains("Lidar7696")) {
        continue;
      }

      if (point->isRejected()) {
        numRejectedLidarPoints++;

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

          bool status = computePartials(coeffTarget, coeffImagePosition, coeffImagePointing,
                                   coeffPoint3D, coeffRHS, *measure);
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

        // increment number of lidar image "measurement" observations
        numObservations += 2;

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

//        if (point->id().contains("Lidar7696")) {
//          m_numLidarConstraints += point->applyLidarRangeConstraint(m_sparseNormals, N22, N12, n1,
//                                                                    n2, measure);
//        }

      } // end loop over this points measures

//      clock_t formPointNormalsClock1 = clock();
      numConstrainedCoordinates += formLidarPointNormals(N22, N12, n2, m_RHS, point);
//      clock_t formPointNormalsClock2 = clock();

      //      cumulativeFormPointNormalsTime += (formPointNormalsClock2 - formPointNormalsClock1)
//          / (double)CLOCKS_PER_SEC;

      pointIndex++;

      numGoodLidarPoints++;
    } // end loop over 3D points

    m_bundleResults.setNumberConstrainedLidarPointParameters(numConstrainedCoordinates);
    m_bundleResults.setNumberLidarImageObservations(numObservations);

//  qDebug() << "cumulative computePartials() Time: " << cumulativeComputePartialsTime;
//  qDebug() << "cumulative formMeasureNormals() Time: " << cumulativeFormMeasureNormalsTime;
//  qDebug() << "cumulative formPointNormals() Time: " << cumulativeFormPointNormalsTime;

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

    m_bundleResults.setNumberLidarRangeConstraints(m_numLidarConstraints);

    return numGoodLidarPoints;
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
   * @see formNormalEquations()
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
      *N12[0] += prod(trans(coeffTarget), coeffPoint3D);

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

      // insert submatrix into normal equations at pointingBlockIndex, pointingBlockIndex
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
   * @return int Number of constrained coordinates.
   *
   * @see BundleAdjust::formNormalEquations
   */
  int BundleAdjust::formPointNormals(symmetric_matrix<double, upper>&N22,
                                      SparseBlockColumnMatrix &N12,
                                      vector<double> &n2,
                                      vector<double> &nj,
                                      BundleControlPointQsp &bundleControlPoint) {

    boost::numeric::ublas::bounded_vector<double, 3> &NIC = bundleControlPoint->nicVector();
    SparseBlockRowMatrix &Q = bundleControlPoint->qMatrix();

    NIC.clear();
    Q.zeroBlocks();

    int numConstrainedCoordinates = 0;

    // weighting of 3D point parameters
    // Make sure weights are in the units corresponding to the bundle coordinate type
    boost::numeric::ublas::bounded_vector<double, 3> &weights
        = bundleControlPoint->weights();
    boost::numeric::ublas::bounded_vector<double, 3> &corrections
        = bundleControlPoint->corrections();

//    qDebug() << weights[0];
//    qDebug() << weights[1];
//    qDebug() << weights[2];
//    qDebug() << corrections[0];
//    qDebug() << corrections[1];
//    qDebug() << corrections[2];

    if (weights(0) > 0.0) {
      N22(0,0) += weights(0);
      n2(0) += (-weights(0) * corrections(0));
      numConstrainedCoordinates++;
    }

    if (weights(1) > 0.0) {
      N22(1,1) += weights(1);
      n2(1) += (-weights(1) * corrections(1));
      numConstrainedCoordinates++;
    }

    if (weights(2) > 0.0) {
      N22(2,2) += weights(2);
      n2(2) += (-weights(2) * corrections(2));
      numConstrainedCoordinates++;
    }

    // invert N22
    invert3x3(N22);

    // save upper triangular covariance matrix for error propagation
    SurfacePoint SurfacePoint = bundleControlPoint->adjustedSurfacePoint();
    SurfacePoint.SetMatrix(m_bundleSettings->controlPointCoordTypeBundle(), N22);
    bundleControlPoint->setAdjustedSurfacePoint(SurfacePoint);

    // form Q (this is N22{-1} * N12{T})
    productATransB(N22, N12, Q);

    // form product of N22(inverse) and n2; store in NIC
    NIC = prod(N22, n2);

    // accumulate -R directly into reduced normal equations
    productAB(N12, Q);

    // accumulate -nj
    accumProductAlphaAB(-1.0, Q, n2, nj);

    return numConstrainedCoordinates;
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
   * @return int Number of constrained coordinates.
   *
   * @see BundleAdjust::formNormalEquations
   */
  int BundleAdjust::formLidarPointNormals(symmetric_matrix<double, upper>&N22,
                                           SparseBlockColumnMatrix &N12,
                                           vector<double> &n2,
                                           vector<double> &nj,
                                           BundleLidarControlPointQsp &bundleLidarControlPoint) {

    boost::numeric::ublas::bounded_vector<double, 3> &NIC = bundleLidarControlPoint->nicVector();
    SparseBlockRowMatrix &Q = bundleLidarControlPoint->qMatrix();

    NIC.clear();
    Q.zeroBlocks();

    int numConstrainedCoordinates = 0;

    // weighting of 3D point parameters
    // Make sure weights are in the units corresponding to the bundle coordinate type
    boost::numeric::ublas::bounded_vector<double, 3> &weights
        = bundleLidarControlPoint->weights();
    boost::numeric::ublas::bounded_vector<double, 3> &corrections
        = bundleLidarControlPoint->corrections();

    qDebug() << weights[0];
    qDebug() << weights[1];
    qDebug() << weights[2];
    qDebug() << corrections[0];
    qDebug() << corrections[1];
    qDebug() << corrections[2];

    if (weights(0) > 0.0) {
      N22(0,0) += weights(0);
      n2(0) += (-weights(0) * corrections(0));
      numConstrainedCoordinates++;
    }

    if (weights(1) > 0.0) {
      N22(1,1) += weights(1);
      n2(1) += (-weights(1) * corrections(1));
      numConstrainedCoordinates++;
    }

    if (weights(2) > 0.0) {
      N22(2,2) += weights(2);
      n2(2) += (-weights(2) * corrections(2));
      numConstrainedCoordinates++;
    }

    // invert N22
    invert3x3(N22);

    // save upper triangular covariance matrix for error propagation
    SurfacePoint SurfacePoint = bundleLidarControlPoint->adjustedSurfacePoint();
    SurfacePoint.SetMatrix(m_bundleSettings->controlPointCoordTypeBundle(), N22);
    bundleLidarControlPoint->setAdjustedSurfacePoint(SurfacePoint);

    // form Q (this is N22{-1} * N12{T})
    productATransB(N22, N12, Q);

    // form product of N22(inverse) and n2; store in NIC
    NIC = prod(N22, n2);

    // accumulate -R directly into reduced normal equations
    productAB(N12, Q);

    // accumulate -nj
    accumProductAlphaAB(-1.0, Q, n2, nj);

    return numConstrainedCoordinates;
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
   *
   * Adding range constraint between laser altimeter ground point and camera station
   *
   * @author 2018-04-12 Ken Edmundson
   *
   * @internal
   *   @history 2018-04-12 Ken Edmundson - Original version
   */
  bool BundleAdjust::applyLidarRangeConstraint(LinearAlgebra::MatrixUpperTriangular& N22,
                                               SparseBlockColumnMatrix& N12,
                                               LinearAlgebra::VectorCompressed& n1,
                                               LinearAlgebra::Vector& n2,
                                               int numberImagePartials,
                                               BundleMeasureQsp measure,
                                               BundleControlPointQsp point) {
    int i;

    if (!point->id().contains("Lidar")) {
      return false;
    }

    QString cubeSerialNumber = measure->cubeSerialNumber();

    if (!((LidarControlPoint*)point->rawControlPoint())->isSimultaneous(cubeSerialNumber)) {
      return false;
    }

    double range = ((LidarControlPoint*)point->rawControlPoint())->range();
    double sigmaRange = ((LidarControlPoint*)point->rawControlPoint())->sigmaRange();

    int imageIndex = measure->positionNormalsBlockIndex();

    LinearAlgebra::Matrix coeff_range_image(1,numberImagePartials);
    LinearAlgebra::Matrix coeff_range_point3D(1,3);
    LinearAlgebra::Vector coeff_range_RHS(1);

    coeff_range_image.clear();
    coeff_range_point3D.clear();
    coeff_range_RHS.clear();

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

    // compute partial derivatives for camstation-to-range point condition

    // get ground point in body-fixed coordinates
    SurfacePoint adjustedSurfacePoint =
        measure->parentControlPoint()->rawControlPoint()->GetAdjustedSurfacePoint();
    double xPoint  = adjustedSurfacePoint.GetX().kilometers();
    double yPoint  = adjustedSurfacePoint.GetY().kilometers();
    double zPoint  = adjustedSurfacePoint.GetZ().kilometers();

    // get spacecraft position in J2000 coordinates
    std::vector<double> CameraJ2KXYZ(3);
    CameraJ2KXYZ = measure->camera()->instrumentPosition()->Coordinate();
    double xCameraJ2K  = CameraJ2KXYZ[0];
    double yCameraJ2K  = CameraJ2KXYZ[1];
    double zCameraJ2K  = CameraJ2KXYZ[2];

    // get spacecraft position in body-fixed coordinates
    std::vector<double> CameraBodyFixedXYZ(3);

    // "InstrumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
    // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
    CameraBodyFixedXYZ = measure->camera()->bodyRotation()->ReferenceVector(CameraJ2KXYZ);
    double xCamera  = CameraBodyFixedXYZ[0];
    double yCamera  = CameraBodyFixedXYZ[1];
    double zCamera  = CameraBodyFixedXYZ[2];

    // computed distance between spacecraft and point
    double dX = xCamera - xPoint;
    double dY = yCamera - yPoint;
    double dZ = zCamera - zPoint;
    double computed_distance = sqrt(dX*dX+dY*dY+dZ*dZ);

    // observed distance - computed distance
    double observed_computed = range - computed_distance;

    // get matrix that rotates spacecraft from J2000 to body-fixed
    std::vector<double> matrix_Target_to_J2K;
    matrix_Target_to_J2K = measure->camera()->bodyRotation()->Matrix();

    double m11 = matrix_Target_to_J2K[0];
    double m12 = matrix_Target_to_J2K[1];
    double m13 = matrix_Target_to_J2K[2];
    double m21 = matrix_Target_to_J2K[3];
    double m22 = matrix_Target_to_J2K[4];
    double m23 = matrix_Target_to_J2K[5];
    double m31 = matrix_Target_to_J2K[6];
    double m32 = matrix_Target_to_J2K[7];
    double m33 = matrix_Target_to_J2K[8];

    // partials w/r to image camera position in J2K
    // auxiliaries
    double a1 = m11*xCameraJ2K + m12*yCameraJ2K + m13*zCameraJ2K - xPoint;
    double a2 = m21*xCameraJ2K + m22*yCameraJ2K + m23*zCameraJ2K - yPoint;
    double a3 = m31*xCameraJ2K + m32*yCameraJ2K + m33*zCameraJ2K - zPoint;

    coeff_range_image(0,0) = (m11*a1 + m21*a2 + m31*a3)/computed_distance;
    coeff_range_image(0,1) = (m12*a1 + m22*a2 + m32*a3)/computed_distance;
    coeff_range_image(0,2) = (m13*a1 + m23*a2 + m33*a3)/computed_distance;

//    std::cout << coeff_range_image << std::endl;

    // partials w/r to point
    double lat    = adjustedSurfacePoint.GetLatitude().radians();
    double lon    = adjustedSurfacePoint.GetLongitude().radians();
    double radius = adjustedSurfacePoint.GetLocalRadius().kilometers();

    double sinlat = sin(lat);
    double coslat = cos(lat);
    double sinlon = sin(lon);
    double coslon = cos(lon);

    coeff_range_point3D(0,0) = radius*(sinlat*coslon*a1 + sinlat*sinlon*a2 - coslat*a3)/computed_distance;
    coeff_range_point3D(0,1) = radius*(coslat*sinlon*a1 - coslat*coslon*a2)/computed_distance;
    coeff_range_point3D(0,2) = -(coslat*coslon*a1 + coslat*sinlon*a2 + sinlat*a3)/computed_distance;

    // right hand side
    coeff_range_RHS(0) = observed_computed;

    // multiply coefficients by observation weight
    double dObservationWeight = 1.0/(sigmaRange*0.001); // converting sigma from meters to km
    coeff_range_image   *= dObservationWeight;
    coeff_range_point3D *= dObservationWeight;
    coeff_range_RHS     *= dObservationWeight;

    // form matrices to be added to normal equation auxiliaries
    static vector<double> n1_image(numberImagePartials);
    n1_image.clear();

    // form N11 for the condition partials for image
//    static symmetric_matrix<double, upper> N11(numberImagePartials);
//    N11.clear();

//    std::cout << "N11" << std::endl << N11 << std::endl;

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

//    N11 = prod(trans(coeff_range_image), coeff_range_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    int t = numberImagePartials * imageIndex;

    // insert submatrix at column, row
//    m_sparseNormals.insertMatrixBlock(imageIndex, imageIndex, numberImagePartials,
//                                      numberImagePartials);

    (*(*m_sparseNormals[imageIndex])[imageIndex])
        += prod(trans(coeff_range_image), coeff_range_image);

//    std::cout << (*(*m_sparseNormals[imageIndex])[imageIndex]) << std::endl;

    // form N12_Image
//    static matrix<double> N12_Image(numberImagePartials, 3);
//    N12_Image.clear();

//    N12_Image = prod(trans(coeff_range_image), coeff_range_point3D);

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
//    N12.insertMatrixBlock(imageIndex, numberImagePartials, 3);
    *N12[imageIndex] += prod(trans(coeff_range_image), coeff_range_point3D);

//  printf("N12\n");
//  std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(coeff_range_image), coeff_range_RHS);

//  std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < numberImagePartials; i++)
      n1(i + t) += n1_image(i);

    // form N22
    N22 += prod(trans(coeff_range_point3D), coeff_range_point3D);

//  std::cout << "N22" << std::endl << N22 << std::endl;
//  std::cout << "n2" << std::endl << n2 << std::endl;

    // form n2
    n2 += prod(trans(coeff_range_point3D), coeff_range_RHS);

//  std::cout << "n2" << std::endl << n2 << std::endl;

    m_numLidarConstraints++;

    return true;
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

//    qDebug() << m_RHS;

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
        outputBundleStatus("\nTriplet allocation failure\n");
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
        QString status = "\nSparseBlockColumnMatrix retrieval failure at column " + 
                         QString::number(columnIndex);
        outputBundleStatus(status);
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
          QString status = "\nmatrix block retrieval failure at column ";
          status.append(columnIndex);
          status.append(", row ");
          status.append(rowIndex);
          outputBundleStatus(status);
          status = "Total # of block columns: " + QString::number(numBlockcolumns);
          outputBundleStatus(status);
          status = "Total # of blocks: " + QString::number(m_sparseNormals.numberOfBlocks());
          outputBundleStatus(status);
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
   * Compute partial derivatives.
   * 
   * coeffTarget, coeffImagePosition, coeffImagePointing, and coeffPoint3D are filled
   * with partial derivatives. coeffRHS is filled with "measured - computed." This is
   * the difference between the measure and the corresponding value as computed via
   * back projection of the control point into the image.
   * 
   * @param coeffTarget Target body partial derivatives matrix.
   * @param coeffImagePosition Camera position partial derivatives.
   * @param coeffImagePointing Camera pointing partial derivatives.
   * @param coeffPoint3D Control point lat, lon, and radius partial derivatives.
   * @param coeffRHS Right hand side vector (measured - computed).
   * @param measure Image measure.
   * @param point Object point.
   * 
   * TODO: should measure and point be const?
   *
   * @return bool If partials were successfully computed.
   *
   * @throws IException::User "Unable to map apriori surface point for measure"
   */
  bool BundleAdjust::computePartials(matrix<double> &coeffTarget,
                                     matrix<double> &coeffImagePosition,
                                     matrix<double> &coeffImagePointing,
                                     matrix<double> &coeffPoint3D,
                                     vector<double> &coeffRHS,
                                     BundleMeasure &measure) {

    // These vectors are either body-fixed latitudinal (lat/lon/radius) or rectangular (x/y/z)
    // depending on the value of coordinate type in SurfacePoint
    std::vector<double> lookBWRTCoord1;
    std::vector<double> lookBWRTCoord2;
    std::vector<double> lookBWRTCoord3;

    BundleControlPoint *point = measure.parentControlPoint();

    double measuredX, computedX, measuredY, computedY;
    double deltaX, deltaY;

    Camera *measureCamera = NULL;
    measureCamera = measure.camera();

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
      // Set the Spice to the measured point. A framing camera exposes the entire image at one time.
      // It will have a single set of Spice for the entire image. Time dependent sensors may
      // populate a single image with multiple exposures, each with a unique set of Spice. SetImage
      // must be called repeatedly for these images to point to the Spice for the current pixel.
      measureCamera->SetImage(measure.sample(), measure.line());
    }

    // we set the measures polynomial segment indices and position and pointing matrix blocks
    // once only, in the first iteration.
    // NOTE: for time dependent sensors, Camera::SetImage MUST be called prior to
    // setPolySegmentIndices
    // TODO: should we do this in initialization? But SetImage would have to be called there for
    // time dependent sensors.
    if (m_iteration == 1) {
      measure.setPolySegmentIndices();
      measure.setNormalsBlockIndices();
    }

    // Compute the look vector in instrument coordinates based on time of observation and apriori
    // lat/lon/radius
    if (!(measureCamera->GroundMap()->GetXY(point->adjustedSurfacePoint(),
                                            &computedX, &computedY))) {
      QString msg = "Unable to map apriori surface point for measure ";
      msg += measure.cubeSerialNumber() + " on point " + point->id() + " into focal plane";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Retrieve the coordinate type (latitudinal or rectangular) and compute the partials for
    // the fixed point with respect to each coordinate in Body-Fixed
    SurfacePoint::CoordinateType type = m_bundleSettings->controlPointCoordTypeBundle();
    lookBWRTCoord1 = point->adjustedSurfacePoint().Partial(type, SurfacePoint::One);
    lookBWRTCoord2 = point->adjustedSurfacePoint().Partial(type, SurfacePoint::Two);
    lookBWRTCoord3 = point->adjustedSurfacePoint().Partial(type, SurfacePoint::Three);

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
            measureCamera->GroundMap()->MeanRadiusPartial(point->adjustedSurfacePoint(),
                                                          m_bundleTargetBody->meanRadius());

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTMeanRadius, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;
      }

      if (m_bundleTargetBody->solveTriaxialRadii()) {

        std::vector<double> lookBWRTRadiusA =
            measureCamera->GroundMap()->EllipsoidPartial(point->adjustedSurfacePoint(),
                                                         CameraGroundMap::WRT_MajorAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusA, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;

        std::vector<double> lookBWRTRadiusB =
            measureCamera->GroundMap()->EllipsoidPartial(point->adjustedSurfacePoint(),
                                                         CameraGroundMap::WRT_MinorAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusB, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;

        std::vector<double> lookBWRTRadiusC =
            measureCamera->GroundMap()->EllipsoidPartial(point->adjustedSurfacePoint(),
                                                         CameraGroundMap::WRT_PolarAxis);

        measureCamera->GroundMap()->GetdXYdPoint(lookBWRTRadiusC, &coeffTarget(0, index),
                                                 &coeffTarget(1, index));
        index++;
      }
    }

    measure.parentBundleObservation()->computePartials(coeffImagePosition, coeffImagePointing,
                                                       *measureCamera);

    // Complete partials calculations for 3D point (latitudinal or rectangular)
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTCoord1,
                                             &coeffPoint3D(0, 0),
                                             &coeffPoint3D(1, 0));
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTCoord2,
                                             &coeffPoint3D(0, 1),
                                             &coeffPoint3D(1, 1));
    measureCamera->GroundMap()->GetdXYdPoint(lookBWRTCoord3,
                                             &coeffPoint3D(0, 2),
                                             &coeffPoint3D(1, 2));

    // right-hand side (measured - computed)
    measuredX = measure.focalPlaneMeasuredX();
    measuredY = measure.focalPlaneMeasuredY();

    deltaX = measuredX - computedX;
    deltaY = measuredY - computedY;

    coeffRHS(0) = deltaX;
    coeffRHS(1) = deltaY;

    // residual prob distribution is calculated even if there is no maximum likelihood estimation
    double obsValue = deltaX / measureCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    obsValue = deltaY / measureCamera->PixelPitch();
    m_bundleResults.addResidualsProbabilityDistributionObservation(obsValue);

    double observationSigma = measure.sigma();
    double observationWeightSqrt = measure.weightSqrt();

    if (m_bundleResults.numberMaximumLikelihoodModels()
          > m_bundleResults.maximumLikelihoodModelIndex()) {
      // if maximum likelihood estimation is being used
      double residualR2ZScore
                 = sqrt(deltaX * deltaX + deltaY * deltaY) / observationSigma / sqrt(2.0);
      //dynamically build the cumulative probability distribution of the R^2 residual Z Scores
      m_bundleResults.addProbabilityDistributionObservation(residualR2ZScore);
      int currentModelIndex = m_bundleResults.maximumLikelihoodModelIndex();
      observationWeightSqrt *= m_bundleResults.maximumLikelihoodModelWFunc(currentModelIndex)
                            .sqrtWeightScaler(residualR2ZScore);
    }

    // multiply coefficients by observation weight
    coeffImagePosition *= observationWeightSqrt;
    coeffImagePointing *= observationWeightSqrt;
    coeffPoint3D *= observationWeightSqrt;
    coeffRHS *= observationWeightSqrt;

    if (m_bundleSettings->solveTargetBody()) {
      coeffTarget *= observationWeightSqrt;
    }

    return true;
  }


  /**
   * Apply parameter corrections for current iteration.
   */
  void BundleAdjust::applyParameterCorrections() {
    emit(statusBarUpdate("Updating Parameters"));
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
    // TODO: can we do this faster by threading with QtConcurrent::run?
    int numObservations = m_bundleObservations.size();
    for (int i = 0; i < numObservations; i++) {
      BundleObservationQsp observation = m_bundleObservations.at(i);

      int numParameters = observation->numberParameters();

      observation->applyParameterCorrections(subrange(m_imageSolution,t,t+numParameters),
                                             m_bundleSettings->solveTargetBody());

      t += numParameters;
    }
        
    // Apply corrections for photogrammetric control points
    // TODO: can we do these faster by threading with QtConcurrent::run?
    m_bundleControlPoints.applyParameterCorrections(m_sparseNormals, m_imageSolution,
                                                    m_bundleTargetBody);

    // Apply corrections for lidar points (if any)
    if (!m_bundleLidarControlPoints.isEmpty()) {
      m_bundleLidarControlPoints.applyParameterCorrections(m_sparseNormals, m_imageSolution,
                                                           m_bundleTargetBody);
    }
  }


  /**
   * Computes vtpv, the weighted sum of squares of residuals.
   *
   * @return double vtpv, the weighted sum of squares of residuals.
   *
   */
  double BundleAdjust::computeVtpv() {
    double vtpv = 0.0;
    double vtpvPhotoMeasures = 0.0;
    double vtpvLidarMeasures = 0.0;
    double vtpvPhotoControl = 0.0;
    double vtpvLidarControl = 0.0;
    double vtpvImage = 0.0;
    double vtpvRangeConstraints = 0.0;

    // x, y, and xy residual stats vectors
    Statistics xResiduals;
    Statistics yResiduals;
    Statistics xyResiduals;

    // vtpv from ...
    vtpvPhotoMeasures =
        m_bundleControlPoints.vtpvMeasureContribution();       // image measures
   vtpvLidarMeasures =
       m_bundleLidarControlPoints.vtpvMeasureContribution();   // lidar image measures
    vtpvPhotoControl =
        m_bundleControlPoints.vtpvContribution();              // constrained point parameters
    vtpvLidarControl =
        m_bundleLidarControlPoints.vtpvContribution();         // constrained lidar point parameters
    vtpvImage =
        m_bundleObservations.vtpvContribution();               // constrained image parameters
    double vtpvTargetBody = 0.0;
    if ( m_bundleTargetBody) {
      vtpvTargetBody =
          m_bundleTargetBody->vtpv();                          // constrained target body parameters
    }
//    vtpvRangeConstraints =
//        m_bundleLidarControlPoints.vtpvRangeContribution();    // lidar point range constraints

    vtpv = vtpvPhotoMeasures
         + vtpvLidarMeasures
         + vtpvPhotoControl
         + vtpvLidarControl
         + vtpvImage
         + vtpvRangeConstraints
         + vtpvTargetBody;

    qDebug() << "\n";
    qDebug() << "                        vtpv";
    qDebug() << "             Photo Residuals: " << vtpvPhotoMeasures;
    qDebug() << "             Lidar Residuals: " << vtpvLidarMeasures;
    qDebug() << "               Photo Control: " << vtpvPhotoControl;
    qDebug() << "               Lidar Control: " << vtpvLidarControl;
    qDebug() << "Constrained Image Parameters: " << vtpvImage;
    if ( m_bundleTargetBody) {
      qDebug() << "                  TargetBody: " << vtpvTargetBody;
    }
    qDebug() << "     Lidar Range Constraints: " << vtpvRangeConstraints;
    qDebug() << "                       Total: " << vtpv;


    // Compute rms for all image coordinate residuals
    // separately for x, y, then x and y together
    m_bundleResults.setRmsXYResiduals(xResiduals.Rms(), yResiduals.Rms(), xyResiduals.Rms());

    return vtpv;
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

      QString status = "\nmedian deviation: ";
      status.append(QString("%1").arg(medianDev));
      status.append("\n");
      outputBundleStatus(status);
      
      mad = 1.4826 * medianDev;

      status = "\nmad: ";
      status.append(QString("%1").arg(mad));
      status.append("\n");
      outputBundleStatus(status);
      
      m_bundleResults.setRejectionLimit(median
                                        + m_bundleSettings->outlierRejectionMultiplier() * mad);

      status = "\nRejection Limit: ";
      status.append(QString("%1").arg(m_bundleResults.rejectionLimit()));
      status.append("\n");
      outputBundleStatus(status);
      
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
    
    outputBundleStatus("\n");
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
            QString status = "Coming back in: ";
            status.append(QString("%1").arg(point->id().toLatin1().data()));
            status.append("\r");
            outputBundleStatus(status);
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
          QString status = "Rejecting Entire Point: ";
          status.append(QString("%1").arg(point->id().toLatin1().data()));
          status.append("\r");
          outputBundleStatus(status);
      }
      else
          point->setRejected(false);

    }

    int numberRejectedObservations = 2*totalNumRejected;

    QString status = "\nRejected Observations:";
    status.append(QString("%1").arg(numberRejectedObservations));
    status.append(" (Rejection Limit:");
    status.append(QString("%1").arg(usedRejectionLimit));
    status.append(")\n");
    outputBundleStatus(status);
    
    m_bundleResults.setNumberRejectedObservations(numberRejectedObservations);

    status = "\nMeasures that came back: ";
    status.append(QString("%1").arg(numComingBack));
    status.append("\n");
    outputBundleStatus(status);
         
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
   *   @history 2018-09-06 Debbie A. Cook and Ken Edmundson - (added to BundleXYZ 
   *                            branch on (2018-05-31).  Moved productAlphaAV and control point 
   *                            parameter correction code to BundleControlPoint.  Earlier revised 
   *                            errorPropagation to compute the sigmas via the variance/ 
   *                            covariance matrices instead of the sigmas.  This should produce 
   *                            more accurate results.  References #4649 and #501.
   */
  bool BundleAdjust::errorPropagation() {
    emit(statusBarUpdate("Error Propagation"));
    // free unneeded memory
    cholmod_free_triplet(&m_cholmodTriplet, &m_cholmodCommon);
    cholmod_free_sparse(&m_cholmodNormal, &m_cholmodCommon);

    LinearAlgebra::Matrix T(3, 3);
    // *** TODO *** 
    // Can any of the control point specific code be moved to BundleControlPoint?

    double sigma0Squared = m_bundleResults.sigma0() * m_bundleResults.sigma0();

    int numObjectPoints = m_bundleControlPoints.size();

    std::string currentTime = iTime::CurrentLocalTime().toLatin1().data();
    
    QString status = "     Time: ";
    status.append(currentTime.c_str());
    status.append("\n\n");
    outputBundleStatus(status); 
    
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
        emit(pointUpdate(j+1));
        BundleControlPointQsp point = m_bundleControlPoints.at(pointIndex);
        if ( point->isRejected() ) {
          continue;
        }

        // only update point every 100 points
        if (j%100 == 0) {
          QString status = "\rError Propagation: Inverse Block ";
          status.append(QString::number(i+1));
          status.append(" of ");
          status.append(QString::number(numBlockColumns));
          status.append("; Point ");
          status.append(QString::number(j+1));
          status.append(" of ");
          status.append(QString::number(numObjectPoints));
          outputBundleStatus(status);
        }

        // get corresponding Q matrix
        // NOTE: we are getting a reference to the Q matrix stored
        //       in the BundleControlPoint for speed (without the & it is dirt slow)
        SparseBlockRowMatrix &Q = point->qMatrix();

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
            outputBundleStatus("\n\n");
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

    outputBundleStatus("\n\n");
     
    currentTime = Isis::iTime::CurrentLocalTime().toLatin1().data();
    
    status = "\rFilling point covariance matrices: Time ";
    status.append(currentTime.c_str());
    outputBundleStatus(status);
    outputBundleStatus("\n\n");

    // now loop over points again and set final covariance stuff
    // *** TODO *** Can this loop go into BundleControlPoint
    int pointIndex = 0;
    for (j = 0; j < numObjectPoints; j++) {

      BundleControlPointQsp point = m_bundleControlPoints.at(pointIndex);

      if ( point->isRejected() ) {
        continue;
      }

      if (j%100 == 0) {
        status = "\rError Propagation: Filling point covariance matrices ";
        status.append(QString("%1").arg(j+1));
        status.append(" of ");
        status.append(QString("%1").arg(numObjectPoints));
        status.append("\r");
        outputBundleStatus(status);
      }

      // get corresponding point covariance matrix
      boost::numeric::ublas::symmetric_matrix<double> &covariance = pointCovariances[pointIndex];

      // Update and reset the matrix
      // Get the Limiting Error Propagation uncertainties:  sigmas for coordinate 1, 2, and 3 in meters
      // 
      SurfacePoint SurfacePoint = point->adjustedSurfacePoint();

      // Get the TEP by adding the corresponding members of pCovar and covariance      
      boost::numeric::ublas::symmetric_matrix <double,boost::numeric::ublas::upper> pCovar;
      
      if (m_bundleSettings->controlPointCoordTypeBundle() == SurfacePoint::Latitudinal) {
        pCovar = SurfacePoint.GetSphericalMatrix(SurfacePoint::Kilometers);
      }
      else {
        // Assume Rectangular coordinates 
        pCovar = SurfacePoint.GetRectangularMatrix(SurfacePoint::Kilometers);
      }
      pCovar += covariance;
      pCovar *= sigma0Squared;

      // debug lines
      // if (j < 3) {
      //   std::cout << " Adjusted surface point ..." << std::endl;
      //   std:: cout << "     sigmaLat (radians) = " << sqrt(pCovar(0,0)) << std::endl;
      //   std:: cout << "     sigmaLon (radians) = " << sqrt(pCovar(1,1)) << std::endl;
      //   std:: cout << "     sigmaRad (km) = " << sqrt(pCovar(2,2)) << std::endl;
      // std::cout <<  "      Adjusted matrix = " << std::endl;
      // std::cout << "       " << pCovar(0,0) << "   " << pCovar(0,1) << "   "
      //           << pCovar(0,2) << std::endl; 
      // std::cout << "        " << pCovar(1,0) << "   " << pCovar(1,1) << "   "
      //           << pCovar(1,2) << std::endl; 
      // std::cout << "        " << pCovar(2,0) << "   " << pCovar(2,1) << "   "
      //           << pCovar(2,2) << std::endl;
      // }
      // end debug
      
      // Distance units are km**2
      SurfacePoint.SetMatrix(m_bundleSettings->controlPointCoordTypeBundle(),pCovar);
      point->setAdjustedSurfacePoint(SurfacePoint);
      // // debug lines
      // if (j < 3) {
      //   boost::numeric::ublas::symmetric_matrix <double,boost::numeric::ublas::upper> recCovar;
      //   recCovar = SurfacePoint.GetRectangularMatrix(SurfacePoint::Meters);
      //   std:: cout << "     sigmaLat (meters) = " << 
      //     point->adjustedSurfacePoint().GetSigmaDistance(SurfacePoint::Latitudinal,
      //     SurfacePoint::One).meters() << std::endl;
      //   std:: cout << "     sigmaLon (meters) = " <<
      //     point->adjustedSurfacePoint().GetSigmaDistance(SurfacePoint::Latitudinal,
      //     SurfacePoint::Two).meters() << std::endl;
      //   std:: cout << "   sigmaRad (km) = " << sqrt(pCovar(2,2)) << std::endl;
      //   std::cout << "Rectangular matrix with radius in meters" << std::endl;
      //   std::cout << "       " << recCovar(0,0) << "   " << recCovar(0,1) << "   "
      //           << recCovar(0,2) << std::endl; 
      //   std::cout << "        " << recCovar(1,0) << "   " << recCovar(1,1) << "   "
      //           << recCovar(1,2) << std::endl; 
      //   std::cout << "        " << recCovar(2,0) << "   " << recCovar(2,1) << "   "
      //           << recCovar(2,2) << std::endl;
      // }
      // // end debug

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
   * Returns a pointer to the output lidar data file.
   *
   * @return LidarDataQsp A shared pointer to the output lidar data file.
   */
  LidarDataQsp BundleAdjust::lidarData() {
    return m_lidarDataSet;
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

    summaryGroup += PvlKeyword("Elapsed_Time",
                               toString( m_iterationTime ) );
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
    m_iterationSummary += QString::fromStdString(ostr.str());
    if (m_printSummary) {
      Application::Log(summaryGroup);
    }

    // emit summary group to screen
    outputBundleStatus(QString::fromStdString(ostr.str()));
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
   * Returns if the BundleAdjust has been aborted.
   *
   * @return @b bool If the BundleAdjust was aborted.
   */
  bool BundleAdjust::isAborted() {
    return m_abort;
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
   *   @history 2016-12-01 Ian Humphrey - Added %s as first parameter to prevent a
   *                           -Wformat-security warning during the build.
   */
  void BundleAdjust::outputBundleStatus(QString status) {
    if (QCoreApplication::applicationName() != "ipce") { 
      printf("%s", status.toStdString().c_str());
    }
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

    int numLidarPoints = m_bundleLidarControlPoints.size();
    for (int i = 0; i < numLidarPoints; i++) {

      const BundleLidarControlPointQsp point = m_bundleLidarControlPoints.at(i);

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

      // initialize body-fixed coordinate boundaries

      // Latitude or X
      Distance minSigmaCoord1Dist;
      QString  minSigmaCoord1PointId = "";
      
      Distance maxSigmaCoord1Dist;
      QString  maxSigmaCoord1PointId = "";
      
      // Longitude or Y
      Distance minSigmaCoord2Dist;
      QString  minSigmaCoord2PointId = "";

      Distance maxSigmaCoord2Dist;
      QString  maxSigmaCoord2PointId = "";

      // Radius or Z
      Distance minSigmaCoord3Dist;
      QString  minSigmaCoord3PointId = "";

      Distance maxSigmaCoord3Dist;
      QString  maxSigmaCoord3PointId = "";
      
      // compute stats for point sigmas
      Statistics sigmaCoord1Stats;
      Statistics sigmaCoord2Stats;
      Statistics sigmaCoord3Stats;

      Distance sigmaCoord1Dist, sigmaCoord2Dist, sigmaCoord3Dist;
      SurfacePoint::CoordinateType type = m_bundleSettings->controlPointCoordTypeReports();

      int numPoints = m_bundleControlPoints.size();
      // initialize max and min values to those from first valid point
      for (int i = 0; i < numPoints; i++) {

        const BundleControlPointQsp point = m_bundleControlPoints.at(i);

        maxSigmaCoord1Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                            SurfacePoint::One);
        minSigmaCoord1Dist = maxSigmaCoord1Dist;

        maxSigmaCoord2Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                            SurfacePoint::Two);
        minSigmaCoord2Dist = maxSigmaCoord2Dist;

        maxSigmaCoord1PointId = point->id();
        maxSigmaCoord2PointId = maxSigmaCoord1PointId;
        minSigmaCoord1PointId = maxSigmaCoord1PointId;
        minSigmaCoord2PointId = maxSigmaCoord1PointId;

        // Get stats for coordinate 3 if used
        if (m_bundleSettings->solveRadius() || type == SurfacePoint::Rectangular) {
          maxSigmaCoord3Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                              SurfacePoint::Three);
          minSigmaCoord3Dist = maxSigmaCoord3Dist;

          maxSigmaCoord3PointId = maxSigmaCoord1PointId;
          minSigmaCoord3PointId = maxSigmaCoord1PointId;
        }
        break;
      }

      for (int i = 0; i < numPoints; i++) {

        const BundleControlPointQsp point = m_bundleControlPoints.at(i);

        sigmaCoord1Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                         SurfacePoint::One);
        sigmaCoord2Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                         SurfacePoint::Two);
        sigmaCoord3Dist = point->adjustedSurfacePoint().GetSigmaDistance(type,
                                                                         SurfacePoint::Three);

        sigmaCoord1Stats.AddData(sigmaCoord1Dist.meters());
        sigmaCoord2Stats.AddData(sigmaCoord2Dist.meters());
        sigmaCoord3Stats.AddData(sigmaCoord3Dist.meters());

        if (sigmaCoord1Dist > maxSigmaCoord1Dist) {
          maxSigmaCoord1Dist = sigmaCoord1Dist;
          maxSigmaCoord1PointId = point->id();
        }
        if (sigmaCoord2Dist > maxSigmaCoord2Dist) {
          maxSigmaCoord2Dist = sigmaCoord2Dist;
          maxSigmaCoord2PointId = point->id();
        }
        if (m_bundleSettings->solveRadius() || type == SurfacePoint::Rectangular) {
          if (sigmaCoord3Dist > maxSigmaCoord3Dist) {
            maxSigmaCoord3Dist = sigmaCoord3Dist;
            maxSigmaCoord3PointId = point->id();
          }
        }
        if (sigmaCoord1Dist < minSigmaCoord1Dist) {
          minSigmaCoord1Dist = sigmaCoord1Dist;
          minSigmaCoord1PointId = point->id();
        }
        if (sigmaCoord2Dist < minSigmaCoord2Dist) {
          minSigmaCoord2Dist = sigmaCoord2Dist;
          minSigmaCoord2PointId = point->id();
        }
        if (m_bundleSettings->solveRadius() || type == SurfacePoint::Rectangular) {
          if (sigmaCoord3Dist < minSigmaCoord3Dist) {
            minSigmaCoord3Dist = sigmaCoord3Dist;
            minSigmaCoord3PointId = point->id();
          }
        }
      }

      // update bundle results
      m_bundleResults.resizeSigmaStatisticsVectors(numberImages);

      m_bundleResults.setSigmaCoord1Range(minSigmaCoord1Dist, maxSigmaCoord1Dist,
                                            minSigmaCoord1PointId, maxSigmaCoord1PointId);

      m_bundleResults.setSigmaCoord2Range(minSigmaCoord2Dist, maxSigmaCoord2Dist,
                                             minSigmaCoord2PointId, maxSigmaCoord2PointId);

      m_bundleResults.setSigmaCoord3Range(minSigmaCoord3Dist, maxSigmaCoord3Dist,
                                          minSigmaCoord3PointId, maxSigmaCoord3PointId);

      m_bundleResults.setRmsFromSigmaStatistics(sigmaCoord1Stats.Rms(),
                                                sigmaCoord2Stats.Rms(),
                                                sigmaCoord3Stats.Rms());
    }
    m_bundleResults.setRmsImageResidualLists(rmsImageLineResiduals.toList(),
                                             rmsImageSampleResiduals.toList(),
                                             rmsImageResiduals.toList());

    return true;
  }

}
