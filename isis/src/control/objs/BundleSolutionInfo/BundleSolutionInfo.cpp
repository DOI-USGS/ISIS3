#include "BundleSolutionInfo.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QXmlStreamWriter>

#include "BundleResults.h"
#include "ControlList.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "FileName.h"
#include "ImageList.h"
#include "IString.h"
#include "iTime.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "StatCumProbDistDynCalc.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  /**
   * Constructor. Creates a BundleSolutionInfo.
   *
   * @param inputSettings The settings saved in BundleSolutionInfo
   * @param controlNetworkFileName The file name and path of the control network
   * @param outputStatistics The results of the BundleAdjust
   * @param parent The Qt-relationship parent
   */
  BundleSolutionInfo::BundleSolutionInfo(BundleSettingsQsp inputSettings,
                                         FileName controlNetworkFileName,
                                         BundleResults outputStatistics,
                                         QList<ImageList *> imgList,
                                         QObject *parent) : QObject(parent) {
    m_id = new QUuid(QUuid::createUuid());
    m_runTime = "";
    m_name = m_runTime;
    m_inputControlNetFileName = new FileName(controlNetworkFileName);
    m_outputControl = NULL;
    m_settings = inputSettings;
    m_statisticsResults = new BundleResults(outputStatistics);
    m_images = new QList<ImageList *>(imgList);
    m_adjustedImages = new QList<ImageList *>;
  }


  /**
   * Constructor. Creates a BundleSolutionInfo from disk.
   *
   * @param project The current project
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleSolutionInfo::BundleSolutionInfo(Project *project,
                                         XmlStackedHandlerReader *xmlReader,
                                         QObject *parent) : QObject(parent) {
                                         //TODO does xml stuff need project???
    m_id = new QUuid(QUuid::createUuid());
    m_runTime = "";
    m_name = m_runTime;
    m_inputControlNetFileName = NULL;
    m_outputControl = NULL;
    m_statisticsResults = NULL;
    // what about the rest of the member data ? should we set defaults ??? CREATE INITIALIZE METHOD
    m_images = new QList<ImageList *>;
    m_adjustedImages = new QList<ImageList *>;

    xmlReader->setErrorHandler(new XmlHandler(this, project));
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * Destructor
   */
  BundleSolutionInfo::~BundleSolutionInfo() {
    delete m_id;

    delete m_inputControlNetFileName;
    m_inputControlNetFileName = NULL;

    delete m_outputControl;
    m_outputControl = NULL;

    delete m_statisticsResults;
    m_statisticsResults = NULL;

    if (m_images != NULL) {
      delete m_images;
      m_images = NULL;
    }

    // if (m_adjustedImages != NULL) {
    //   qDeleteAll(*m_adjustedImages);
    //   m_adjustedImages->clear();
    //   delete m_adjustedImages;
    //   m_adjustedImages = NULL;
    // }
  }


  /**
   * Returns bundleout text filename.
   *
   * @return QString Bundleout text filename.
   */
  QString BundleSolutionInfo::savedBundleOutputFilename() {
    return m_txtBundleOutputFilename;
  }


  /**
   * Returns filename of output bundle images csv file.
   *
   * @return QString filename of output bundle images csv file.
   */
  QString BundleSolutionInfo::savedImagesFilename() {
    return m_csvSavedImagesFilename;
  }


  /**
   * Returns filename of output bundle points csv file.
   *
   * @return QString filename of output bundle points csv file.
   */
  QString BundleSolutionInfo::savedPointsFilename() {
    return m_csvSavedPointsFilename;
  }


  /**
   * Returns filename of output bundle residuals csv file.
   *
   * @return QString filename of output bundle residuals csv file.
   */
  QString BundleSolutionInfo::savedResidualsFilename() {
    return m_csvSavedResidualsFilename;
  }


  /**
   * Adds a list of images that were adjusted (their labels were updated).
   *
   * @param ImageList *images The list of images to add that had their labels updated.
   */
  void BundleSolutionInfo::addAdjustedImages(ImageList *images) {
    m_adjustedImages->append(images);
  }


  /**
   * Sets the stat results.
   *
   * @param statisticsResults The new BundleResults
   */
  void BundleSolutionInfo::setOutputStatistics(BundleResults statisticsResults) {
    delete m_statisticsResults;
    m_statisticsResults = NULL;
    m_statisticsResults = new BundleResults(statisticsResults);
  }


  /**
   * TODO: change description below to something more like english.
   *
   * Change the on-disk file name for the control network used to be where the control network
   * ought to be in the given project.
   *
   * This method is modelled after the updateFileName() methods in Image and Control. Those methods
   * close something (cubes for Image and a control net for control) but there is not a close
   * method in BundleSolutionInfo.
   *
   * @param project The project that this BundleSolutionInfo is stored in
   */
  void BundleSolutionInfo::updateFileName(Project *project) {

    //TODO do we need to close anything here?

    FileName oldInputFileName(*m_inputControlNetFileName);
    FileName newInputFileName(project->cnetRoot() + "/" +
                     oldInputFileName.dir().dirName() + "/" + oldInputFileName.name());
    *m_inputControlNetFileName = newInputFileName.expanded();

    FileName oldOutputFileName(m_outputControl->fileName());
    FileName newOutputFileName(project->cnetRoot() + "/" +
                     oldOutputFileName.dir().dirName() + "/" + oldOutputFileName.name());

    if (m_outputControl) {
      delete m_outputControl;
    }
    m_outputControl = new Control(newOutputFileName.expanded());
  }


  /**
   * Returns the list of images that were adjusted after a bundle. This can potentially be an
   * empty QList if no image labels were updated.
   *
   * @return QList<ImageList*> Returns the adjusted images (images with updated labels).
   */
  QList<ImageList *> BundleSolutionInfo::adjustedImages() const {
    return *m_adjustedImages;
  }


  /**
   * Get a unique, identifying string associated with this BundleSolutionInfo object.
   *
   * @return @b QString A unique ID for this BundleSolutionInfo object
   */
  QString BundleSolutionInfo::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }


  /**
   * Sets the run time, and the name if a name is not already set.
   *
   * @param runTime The run time.
   */
  void BundleSolutionInfo::setRunTime(QString runTime) {
    // ??? validate that a valid time has been given???
    // try {
    //   iTime time(runTime);
    // }
    // catch (...) {
    //   throw IException(IException::Unknown,
    //                    "Invalid bundle adjustment run time [" + runTime + ].",
    //                    _FILEINFO_);
    // }
    m_runTime = runTime;
    if (m_name == m_runTime || m_name == "") {
      m_name = runTime;
    }
  }


  /**
   * Returns the run time.
   *
   * @return @b QString The run time.
   */
  QString BundleSolutionInfo::runTime() const {
    return m_runTime;
  }


  /**
   * Returns the name of the input control network.
   *
   * @return @b QString The name of the input control network.
   */
  QString BundleSolutionInfo::inputControlNetFileName() const {
    return m_inputControlNetFileName->expanded();
  }


  /**
   * Returns the name of the output control network.
   *
   * @return @b QString The name of the output control network.
   */
  QString BundleSolutionInfo::outputControlNetFileName() const {
    return m_outputControl->fileName();
  }


  /**
   * Returns the name of the output control network.
   *
   * @return @b QString The name of the output control network.
   */
  void BundleSolutionInfo::setOutputControl(Control *outputControl) {
    m_outputControl = outputControl;
  }


  /**
   * Returns bundle output Control object.
   *
   * @return Control* Pointer to bundle output Control object.
   */
  Control *BundleSolutionInfo::control() const {
    return m_outputControl;
  }


  /**
   * Returns bundle settings.
   *
   * @return BundleSettingsQsp Bundle settings.
   */
  BundleSettingsQsp BundleSolutionInfo::bundleSettings() {
    return m_settings;
  }


  /**
   * Returns the bundle results.
   *
   * @throws IException::Unknown "Results for this bundle is NULL."
   *
   * @return @b BundleResults The bundle results.
   */
  BundleResults BundleSolutionInfo::bundleResults() {
    if (m_statisticsResults) {
      return *m_statisticsResults;
    }
    else {
      throw IException(IException::Unknown,
                       "Results for this bundle is NULL.",
                       _FILEINFO_);
    }
  }


  /**
   * Returns the images used in the bundle
   *
   * @return m_imageList The image list used in the bundle
   */
  QList<ImageList *> BundleSolutionInfo::imageList() {
    return *m_images;
  }


  /**
   * Sets the name of the bundle.
   *
   * @param name QString of the new name
   */
  void BundleSolutionInfo::setName(QString name) {
    m_name = name;
  }


  /**
   * Returns the name of the bundle. The name defaults to the id, unless the name has been set using
   * setName()
   *
   * @return QString Name of the bundle
   */
  QString BundleSolutionInfo::name() const {
    return m_name;
  }


  /**
   * @brief Outputs the header for the bundleout_images.csv file
   * @param fpOut The output file stream.
   * @return True if the write is successful, False otherwise.
   *
   * @internal
   *   @history 2016-12-08 Ian Humphrey - Removed conditions that handle TWIST headers differently
   *                           than the other headers. The number of TWIST headers will be the same
   *                           as each of the other angle headers. Fixes #4557.
   */
  bool BundleSolutionInfo::outputImagesCSVHeader(std::ofstream &fpOut) {

    if (!fpOut) {
      return false;
    }

    char buf[1056];

    // setup column headers
    std::vector<QString> outputColumns;

    outputColumns.push_back("Image,");
    outputColumns.push_back("rms,");
    outputColumns.push_back("rms,");
    outputColumns.push_back("rms,");

    BundleObservationSolveSettings obsSettings = m_settings->observationSolveSettings(0);

    int numberCamPosCoefSolved = obsSettings.numberCameraPositionCoefficientsSolved();
    int numberCamAngleCoefSolved  = obsSettings.numberCameraAngleCoefficientsSolved();

    int nCoeff = 1;
    if (numberCamPosCoefSolved > 0)
      nCoeff = numberCamPosCoefSolved;

    for (int i = 0; i < nCoeff; i++) {
      for (int j = 0; j < 5; j++) {
        if (nCoeff == 1)
          outputColumns.push_back("X,");
        else {
          QString str = "X(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
    }
    for (int i = 0; i < nCoeff; i++) {
      for (int j = 0; j < 5; j++) {
        if (nCoeff == 1)
          outputColumns.push_back("Y,");
        else {
          QString str = "Y(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
    }
    for (int i = 0; i < nCoeff; i++) {
      for (int j = 0; j < 5; j++) {
        if (nCoeff == 1) {
          outputColumns.push_back("Z,");
        }
        else {
          QString str = "Z(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
      if (!i)
        break;
    }

    for (int i = 0; i < numberCamAngleCoefSolved; i++) {
      for (int j = 0; j < 5; j++) {
        if (numberCamAngleCoefSolved == 1)
          outputColumns.push_back("RA,");
        else {
          QString str = "RA(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
    }
    for (int i = 0; i < numberCamAngleCoefSolved; i++) {
      for (int j = 0; j < 5; j++) {
        if (numberCamAngleCoefSolved == 1)
          outputColumns.push_back("DEC,");
        else {
          QString str = "DEC(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
    }
    for (int i = 0; i < numberCamAngleCoefSolved; i++) {
      for (int j = 0; j < 5; j++) {
        if (numberCamAngleCoefSolved == 1) {
          outputColumns.push_back("TWIST,");
        }
        else {
          QString str = "TWIST(t" + toString(i) + "),";
          outputColumns.push_back(str);
        }
      }
    }

    // print first column header to buffer and output to file
    int ncolumns = outputColumns.size();
    for (int i = 0; i < ncolumns; i++) {
      QString str = outputColumns.at(i);
      sprintf(buf, "%s", (const char*)str.toLatin1().data());
      fpOut << buf;
    }
    sprintf(buf, "\n");
    fpOut << buf;

    outputColumns.clear();
    outputColumns.push_back("Filename,");

    outputColumns.push_back("sample res,");
    outputColumns.push_back("line res,");
    outputColumns.push_back("total res,");

    // Initially account for X,Y,Z (3)
    int nparams = 3;
    // See how many position coeffients we solved for to make more headers (t0, t1, ...)
    if (numberCamPosCoefSolved)
      nparams = 3 * numberCamPosCoefSolved;

    // Initially account for RA,DEC,TWIST (3)
    int numCameraAnglesSolved = 3;
    // See how many angle coefficients we solved for to make more headers (t0, t1, ...)
    nparams += numCameraAnglesSolved*numberCamAngleCoefSolved;
    for (int i = 0; i < nparams; i++) {
      outputColumns.push_back("Initial,");
      outputColumns.push_back("Correction,");
      outputColumns.push_back("Final,");
      outputColumns.push_back("Apriori Sigma,");
      outputColumns.push_back("Adj Sigma,");
    }

    // print second column header to buffer and output to file
    ncolumns = outputColumns.size();
    for (int i = 0; i < ncolumns; i++) {
      QString str = outputColumns.at(i);
      sprintf(buf, "%s", (const char*)str.toLatin1().data());
      fpOut << buf;
    }
    sprintf(buf, "\n");
    fpOut << buf;

    return true;
  }



  /**
   * Output header for bundle results file.
   *
   * @param fpOut The output stream that the header will be sent to.
   *
   * @return @b bool If the header was successfully output to the output stream.
   *
   * @throws IException::Io "Failed to output residual percentiles for bundleout"
   * @throws IException::Io "Failed to output residual box plot for bundleout"
   *
   * @todo Determine how multiple sensor solve settings should be output.
   */
  bool BundleSolutionInfo::outputHeader(std::ofstream &fpOut) {

    if (!fpOut) {
      return false;
    }

    char buf[1056];
    int numObservations = m_statisticsResults->observations().size();
    int numImages = 0;
    for (int i = 0; i < numObservations; i++) {
      numImages += m_statisticsResults->observations().at(i)->size();
    }
    int numValidPoints = m_statisticsResults->outputControlNet()->GetNumValidPoints();
    int numInnerConstraints = 0;
    int numDistanceConstraints = 0;
    int numDegreesOfFreedom = m_statisticsResults->numberObservations()
                            + m_statisticsResults->numberConstrainedPointParameters()
                            + m_statisticsResults->numberConstrainedImageParameters()
                            + m_statisticsResults->numberConstrainedTargetParameters()
                            - m_statisticsResults->numberUnknownParameters();

    int convergenceCriteria = 1;

    sprintf(buf, "JIGSAW: BUNDLE ADJUSTMENT\n=========================\n");
    fpOut << buf;
    sprintf(buf, "\n                       Run Time: %s",
                  Isis::iTime::CurrentLocalTime().toLatin1().data());
    fpOut << buf;
    sprintf(buf, "\n               Network Filename: %s",
                  m_inputControlNetFileName->expanded().toLatin1().data());
    fpOut << buf;
    sprintf(buf, "\n                     Network Id: %s",
                  m_statisticsResults->outputControlNet()->GetNetworkId().toLatin1().data());
    fpOut << buf;
    sprintf(buf, "\n            Network Description: %s",\
                  m_statisticsResults->outputControlNet()->Description().toLatin1().data());
    fpOut << buf;
    sprintf(buf, "\n                         Target: %s",
                  m_statisticsResults->outputControlNet()->GetTarget().toLatin1().data());
    fpOut << buf;
    sprintf(buf, "\n\n                   Linear Units: kilometers");
    fpOut << buf;
    sprintf(buf, "\n                  Angular Units: decimal degrees");
    fpOut << buf;
    sprintf(buf, "\n\nINPUT: SOLVE OPTIONS\n====================\n");
    fpOut << buf;

    m_settings->solveObservationMode() ?
      sprintf(buf, "\n                   OBSERVATIONS: ON"):
      sprintf(buf, "\n                   OBSERVATIONS: OFF");
    fpOut << buf;

    m_settings->solveRadius() ?
      sprintf(buf, "\n                         RADIUS: ON"):
      sprintf(buf, "\n                         RADIUS: OFF");
    fpOut << buf;

    m_settings->solveTargetBody() ?
      sprintf(buf, "\n                    TARGET BODY: ON"):
      sprintf(buf, "\n                    TARGET BODY: OFF");
    fpOut << buf;

    m_settings->updateCubeLabel() ?
      sprintf(buf, "\n                         UPDATE: YES"):
      sprintf(buf, "\n                         UPDATE: NO");
    fpOut << buf;

    m_settings->errorPropagation() ?
      sprintf(buf, "\n              ERROR PROPAGATION: ON"):
      sprintf(buf, "\n              ERROR PROPAGATION: OFF");
    fpOut << buf;

    if (m_settings->outlierRejection()) {
      sprintf(buf, "\n              OUTLIER REJECTION: ON");
      fpOut << buf;
      sprintf(buf, "\n           REJECTION MULTIPLIER: %lf",
                    m_settings->outlierRejectionMultiplier());
      fpOut << buf;

    }
    else {
      sprintf(buf, "\n              OUTLIER REJECTION: OFF");
      fpOut << buf;
      sprintf(buf, "\n           REJECTION MULTIPLIER: N/A");
      fpOut << buf;
    }

    sprintf(buf, "\n\nMAXIMUM LIKELIHOOD ESTIMATION\n============================\n");
    fpOut << buf;

    for (int tier = 0; tier < 3; tier++) {
      if (tier < m_statisticsResults->numberMaximumLikelihoodModels()) {
        sprintf(buf, "\n                         Tier %d Enabled: TRUE", tier);
        fpOut << buf;
        sprintf(buf, "\n               Maximum Likelihood Model: %s",
                      MaximumLikelihoodWFunctions::modelToString(
                          m_statisticsResults->
                              maximumLikelihoodModelWFunc(tier).model()).toLatin1().data());
        fpOut << buf;
        sprintf(buf, "\n    Quantile used for tweaking constant: %lf",
                      m_statisticsResults->maximumLikelihoodModelQuantile(tier));
        fpOut << buf;
        sprintf(buf, "\n   Quantile weighted R^2 Residual value: %lf",
                      m_statisticsResults->maximumLikelihoodModelWFunc(tier).tweakingConstant());
        fpOut << buf;
        sprintf(buf, "\n       Approx. weighted Residual cutoff: %s",
                      m_statisticsResults->maximumLikelihoodModelWFunc(tier)
                          .weightedResidualCutoff().toLatin1().data());
        fpOut << buf;
        if (tier != 2) fpOut << "\n";
      }
      else {
        sprintf(buf, "\n                         Tier %d Enabled: FALSE", tier);
        fpOut << buf;
      }
    }

    sprintf(buf, "\n\nINPUT: CONVERGENCE CRITERIA\n===========================\n");
    fpOut << buf;
    sprintf(buf, "\n                         SIGMA0: %e",
                  m_settings->convergenceCriteriaThreshold());
    fpOut << buf;
    sprintf(buf, "\n             MAXIMUM ITERATIONS: %d",
                  m_settings->convergenceCriteriaMaximumIterations());
    fpOut << buf;

    //TODO Should it be checked that positionSigmas.size() == positionSolveDegree and
    //     pointingSigmas.size() == pointingSolveDegree somewhere? JAM

    //TODO How do we output this information when using multiple solve settings? JAM

    BundleObservationSolveSettings globalSettings = m_settings->observationSolveSettings(0);
    int pointingSolveDegree = globalSettings.numberCameraAngleCoefficientsSolved();
    QList<double> pointingSigmas = globalSettings.aprioriPointingSigmas();
    int positionSolveDegree = globalSettings.numberCameraPositionCoefficientsSolved();
    QList<double> positionSigmas = globalSettings.aprioriPositionSigmas();

    sprintf(buf, "\n\nINPUT: CAMERA POINTING OPTIONS\n==============================\n");
    fpOut << buf;
    switch (pointingSolveDegree) {
      case 0:
        sprintf(buf,"\n                          CAMSOLVE: NONE");
        break;
      case 1:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES");
        break;
      case 2:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES, VELOCITIES");
        break;
      case 3:
        sprintf(buf,"\n                          CAMSOLVE: ANGLES, VELOCITIES, ACCELERATIONS");
        break;
      default:
        sprintf(buf,"\n                          CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)"
                    "\n                          CKDEGREE: %d"
                    "\n                     CKSOLVEDEGREE: %d",
                pointingSolveDegree,
                globalSettings.ckDegree(),
                globalSettings.ckSolveDegree());
        break;
    }
    fpOut << buf;
    globalSettings.solveTwist() ?
        sprintf(buf, "\n                             TWIST: ON"):
        sprintf(buf, "\n                             TWIST: OFF");
    fpOut << buf;
    globalSettings.solvePolyOverPointing() ?
        sprintf(buf, "\n POLYNOMIAL OVER EXISTING POINTING: ON"):
        sprintf(buf, "\nPOLYNOMIAL OVER EXISTING POINTING : OFF");
    fpOut << buf;

    sprintf(buf, "\n\nINPUT: SPACECRAFT OPTIONS\n=========================\n");
    fpOut << buf;
    switch (positionSolveDegree) {
      case 0:
        sprintf(buf,"\n                        SPSOLVE: NONE");
        break;
      case 1:
        sprintf(buf,"\n                        SPSOLVE: POSITION");
        break;
      case 2:
        sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES");
        break;
      case 3:
        sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES, ACCELERATIONS");
        break;
      default:
        sprintf(buf,"\n                        SPSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)"
                    "\n                      SPKDEGREE: %d"
                    "\n                 SPKSOLVEDEGREE: %d",
                positionSolveDegree,
                globalSettings.spkDegree(),
                globalSettings.spkSolveDegree());
        break;
    }
    fpOut << buf;
    globalSettings.solvePositionOverHermite() ?
        sprintf(buf, "\n POLYNOMIAL OVER HERMITE SPLINE: ON"):
        sprintf(buf, "\nPOLYNOMIAL OVER HERMITE SPLINE : OFF");
    fpOut << buf;

    sprintf(buf, "\n\nINPUT: GLOBAL IMAGE PARAMETER UNCERTAINTIES\n===========================================\n");
    fpOut << buf;
    (m_settings->globalLatitudeAprioriSigma() == Isis::Null) ?
        sprintf(buf,"\n               POINT LATITUDE SIGMA: N/A"):
        sprintf(buf,"\n               POINT LATITUDE SIGMA: %lf (meters)",
                m_settings->globalLatitudeAprioriSigma());
    fpOut << buf;
    (m_settings->globalLongitudeAprioriSigma() == Isis::Null) ?
        sprintf(buf,"\n              POINT LONGITUDE SIGMA: N/A"):
        sprintf(buf,"\n              POINT LONGITUDE SIGMA: %lf (meters)",
                m_settings->globalLongitudeAprioriSigma());
    fpOut << buf;
    (m_settings->globalRadiusAprioriSigma() == Isis::Null) ?
        sprintf(buf,"\n                 POINT RADIUS SIGMA: N/A"):
        sprintf(buf,"\n                 POINT RADIUS SIGMA: %lf (meters)",
                m_settings->globalRadiusAprioriSigma());
    fpOut << buf;
    (positionSolveDegree < 1 || positionSigmas[0] == Isis::Null) ?
        sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: N/A"):
        sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: %lf (meters)",
                positionSigmas[0]);
    fpOut << buf;

    (positionSolveDegree < 2 || positionSigmas[1] == Isis::Null) ?
        sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: N/A"):
        sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: %lf (m/s)",
                positionSigmas[1]);
    fpOut << buf;

    (positionSolveDegree < 3 || positionSigmas[2] == Isis::Null) ?
        sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: N/A"):
        sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: %lf (m/s/s)",
                positionSigmas[2]);
    fpOut << buf;

    (pointingSolveDegree < 1 || pointingSigmas[0] == Isis::Null) ?
        sprintf(buf,"\n                CAMERA ANGLES SIGMA: N/A"):
        sprintf(buf,"\n                CAMERA ANGLES SIGMA: %lf (dd)",
                pointingSigmas[0]);
    fpOut << buf;

    (pointingSolveDegree < 2 || pointingSigmas[1] == Isis::Null) ?
        sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: N/A"):
        sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: %lf (dd/s)",
                pointingSigmas[1]);
    fpOut << buf;

    (pointingSolveDegree < 3 || pointingSigmas[2] == Isis::Null) ?
        sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: N/A"):
        sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: %lf (dd/s/s)",
                pointingSigmas[2]);
    fpOut << buf;

    if (m_settings->solveTargetBody()) {
      sprintf(buf, "\n\nINPUT: TARGET BODY OPTIONS\n==============================\n");
      fpOut << buf;

      if (m_settings->solvePoleRA() && m_settings->solvePoleDec()) {
        sprintf(buf,"\n                             POLE: RIGHT ASCENSION");
        fpOut << buf;
        sprintf(buf,"\n                                 : DECLINATION\n");
        fpOut << buf;
      }
      else if (m_settings->solvePoleRA()) {
        sprintf(buf,"\n                             POLE: RIGHT ASCENSION\n");
        fpOut << buf;
      }
      else if (m_settings->solvePoleDec()) {
        sprintf(buf,"\n                             POLE: DECLINATION\n");
        fpOut << buf;
      }

      if (m_settings->solvePM() || m_settings->solvePMVelocity()
          || m_settings->solvePMAcceleration()) {
        sprintf(buf,"\n                   PRIME MERIDIAN: W0 (OFFSET)");
        fpOut << buf;

        if (m_settings->solvePMVelocity()) {
          sprintf(buf,"\n                                 : WDOT (SPIN RATE)");
          fpOut << buf;
        }
        if (m_settings->solvePMAcceleration()) {
          sprintf(buf,"\n                               :W ACCELERATION");
          fpOut << buf;
        }
      }

      if (m_settings->solveTriaxialRadii() || m_settings->solveMeanRadius()) {
        if (m_settings->solveMeanRadius()) {
          sprintf(buf,"\n                            RADII: MEAN");
          fpOut << buf;
        }
        else if (m_settings->solveTriaxialRadii()) {
          sprintf(buf,"\n                            RADII: TRIAXIAL");
          fpOut << buf;
        }
      }
    }

    sprintf(buf, "\n\nJIGSAW: RESULTS\n===============\n");
    fpOut << buf;
    sprintf(buf, "\n                         Images: %6d",numImages);
    fpOut << buf;
    sprintf(buf, "\n                         Points: %6d",numValidPoints);
    fpOut << buf;

    sprintf(buf, "\n                 Total Measures: %6d",
                  (m_statisticsResults->numberObservations()
                      + m_statisticsResults->numberRejectedObservations()) / 2);
    fpOut << buf;

    sprintf(buf, "\n             Total Observations: %6d",
                  m_statisticsResults->numberObservations()
                      + m_statisticsResults->numberRejectedObservations());
    fpOut << buf;

    sprintf(buf, "\n              Good Observations: %6d",
                  m_statisticsResults->numberObservations());
    fpOut << buf;

    sprintf(buf, "\n          Rejected Observations: %6d",
                  m_statisticsResults->numberRejectedObservations());
    fpOut << buf;

    if (m_statisticsResults->numberConstrainedPointParameters() > 0) {
      sprintf(buf, "\n   Constrained Point Parameters: %6d",
                    m_statisticsResults->numberConstrainedPointParameters());
      fpOut << buf;
    }

    if (m_statisticsResults->numberConstrainedImageParameters() > 0) {
      sprintf(buf, "\n   Constrained Image Parameters: %6d",
                    m_statisticsResults->numberConstrainedImageParameters());
      fpOut << buf;
    }

    if (m_statisticsResults->numberConstrainedTargetParameters() > 0) {
      sprintf(buf, "\n  Constrained Target Parameters: %6d",
                    m_statisticsResults->numberConstrainedTargetParameters());
      fpOut << buf;
    }

    sprintf(buf, "\n                       Unknowns: %6d",
                  m_statisticsResults->numberUnknownParameters());
    fpOut << buf;

    if (numInnerConstraints > 0) {
      sprintf(buf, "\n      Inner Constraints: %6d", numInnerConstraints);
     fpOut << buf;
    }

    if (numDistanceConstraints > 0) {
      sprintf(buf, "\n   Distance Constraints: %d", numDistanceConstraints);
      fpOut << buf;
    }

    sprintf(buf, "\n             Degrees of Freedom: %6d", numDegreesOfFreedom);
    fpOut << buf;

    sprintf(buf, "\n           Convergence Criteria: %6.3g",
                               m_settings->convergenceCriteriaThreshold());
    fpOut << buf;

    if (convergenceCriteria == 1) {
      sprintf(buf, "(Sigma0)");
      fpOut << buf;
    }

    sprintf(buf, "\n                     Iterations: %6d", m_statisticsResults->iterations());
    fpOut << buf;

    if (m_statisticsResults->iterations() >= m_settings->convergenceCriteriaMaximumIterations()) {
      sprintf(buf, "(Maximum reached)");
      fpOut << buf;
    }

    sprintf(buf, "\n                         Sigma0: %30.20lf\n", m_statisticsResults->sigma0());
    fpOut << buf;
    sprintf(buf, " Error Propagation Elapsed Time: %6.4lf (seconds)\n",
                  m_statisticsResults->elapsedTimeErrorProp());
    fpOut << buf;
    sprintf(buf, "             Total Elapsed Time: %6.4lf (seconds)\n",
                  m_statisticsResults->elapsedTime());
    fpOut << buf;
    if (m_statisticsResults->numberObservations()
        + m_statisticsResults->numberRejectedObservations()
        > 100) {
      sprintf(buf, "\n           Residual Percentiles:\n");
      fpOut << buf;

    // residual prob distribution values are calculated/printed
    // even if there is no maximum likelihood estimation
      try {
        for (int bin = 1;bin < 34;bin++) {
          double cumProb = double(bin) / 100.0;
          double resValue =
              m_statisticsResults->
                  residualsCumulativeProbabilityDistribution().value(cumProb);
          double resValue33 =
              m_statisticsResults->
                  residualsCumulativeProbabilityDistribution().value(cumProb + 0.33);
          double resValue66 =
              m_statisticsResults->
                  residualsCumulativeProbabilityDistribution().value(cumProb + 0.66);
          sprintf(buf, "                 Percentile %3d: %+8.3lf"
                       "                 Percentile %3d: %+8.3lf"
                       "                 Percentile %3d: %+8.3lf\n",
                                         bin,      resValue,
                                         bin + 33, resValue33,
                                         bin + 66, resValue66);
          fpOut << buf;
        }
      }
      catch (IException &e) {
        QString msg = "Failed to output residual percentiles for bundleout";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
      try {
        sprintf(buf, "\n              Residual Box Plot:");
        fpOut << buf;
        sprintf(buf, "\n                        minimum: %+8.3lf",
                m_statisticsResults->residualsCumulativeProbabilityDistribution().min());
        fpOut << buf;
        sprintf(buf, "\n                     Quartile 1: %+8.3lf",
                m_statisticsResults->residualsCumulativeProbabilityDistribution().value(0.25));
        fpOut << buf;
        sprintf(buf, "\n                         Median: %+8.3lf",
                m_statisticsResults->residualsCumulativeProbabilityDistribution().value(0.50));
        fpOut << buf;
        sprintf(buf, "\n                     Quartile 3: %+8.3lf",
                m_statisticsResults->residualsCumulativeProbabilityDistribution().value(0.75));
        fpOut << buf;
        sprintf(buf, "\n                        maximum: %+8.3lf\n",
                m_statisticsResults->residualsCumulativeProbabilityDistribution().max());
        fpOut << buf;
      }
      catch (IException &e) {
        QString msg = "Failed to output residual box plot for bundleout";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }

    sprintf(buf, "\nIMAGE MEASURES SUMMARY\n==========================\n\n");
    fpOut << buf;

    int numMeasures;
    int numRejectedMeasures;
    int numUsed;
    int imageIndex = 0;

    for (int i = 0; i < numObservations; i++) {

      int numImagesInObservation = m_statisticsResults->observations().at(i)->size();

      for (int j = 0; j < numImagesInObservation; j++) {

        BundleImageQsp bundleImage = m_statisticsResults->observations().at(i)->at(j);

        double rmsSampleResiduals = m_statisticsResults->
                                        rmsImageSampleResiduals()[imageIndex].Rms();
        double rmsLineResiduals =   m_statisticsResults->
                                        rmsImageLineResiduals()[imageIndex].Rms();
        double rmsLandSResiduals =  m_statisticsResults->
                                        rmsImageResiduals()[imageIndex].Rms();

        numMeasures =         m_statisticsResults->outputControlNet()->
                                  GetNumberOfValidMeasuresInImage(
                                      bundleImage->serialNumber());

        numRejectedMeasures = m_statisticsResults->outputControlNet()->
                                  GetNumberOfJigsawRejectedMeasuresInImage(
                                      bundleImage->serialNumber());

        numUsed =             numMeasures - numRejectedMeasures;

        if (numUsed == numMeasures) {
          sprintf(buf, "%s   %5d of %5d %6.3lf %6.3lf %6.3lf\n",
                  bundleImage->fileName().toLatin1().data(),
                  (numMeasures-numRejectedMeasures), numMeasures,
                  rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
        }
        else {
          sprintf(buf, "%s   %5d of %5d* %6.3lf %6.3lf %6.3lf\n",
                  bundleImage->fileName().toLatin1().data(),
                  (numMeasures-numRejectedMeasures), numMeasures,
                  rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
        }
        fpOut << buf;
        imageIndex++;
      }
    }

    return true;
  }


  /**
   * @brief Outputs the bundleout_images.csv file which contains Jigsaw data about the images
   * within each observation.
   * @return True upon success, False if something went wrong.
   *
   * @internal
   *   @history 2016-12-01 Ian Humphrey - Added %s as second argument to sprintf() call to prevent
   *                           -Wformat-security warning. Since image->fileName().toLatin1().data()
   *                           returns a char* at runtime, the compiler does not know if it will
   *                           contain format specifiers and produces the mentioned warning.
   */
  bool BundleSolutionInfo::outputImagesCSV() {

    char buf[1056];
    int imgIndex = 0;

    QList<Statistics> rmsImageSampleResiduals = m_statisticsResults->rmsImageSampleResiduals();
    QList<Statistics> rmsImageLineResiduals = m_statisticsResults->rmsImageLineResiduals();
    QList<Statistics> rmsImageResiduals = m_statisticsResults->rmsImageResiduals();

    QString ofname = "bundleout_images.csv";
    ofname = m_settings->outputFilePrefix() + ofname;
    m_csvSavedImagesFilename = ofname;

    std::ofstream fpOut(ofname.toLatin1().data(), std::ios::out);
    if (!fpOut) {
      return false;
    }


    BundleObservationQsp observation;

    int nObservations = m_statisticsResults->observations().size();

    outputImagesCSVHeader(fpOut);

    bool errorProp = false;
    if (m_statisticsResults->converged() && m_settings->errorPropagation()) {
      errorProp = true;
    }

    for (int i = 0; i < nObservations;i++ ) {
      observation = m_statisticsResults->observations().at(i);

      if(!observation) {
        continue;
      }

      int numImages = observation->size();

      for (int j = 0; j < numImages; j++) {

        BundleImageQsp image = observation->at(j);


        sprintf(buf, "%s", image->fileName().toLatin1().data());
        fpOut << buf;
        sprintf(buf,",");
        fpOut << buf;

        fpOut << toString(rmsImageSampleResiduals[imgIndex].Rms()).toLatin1().data();
        sprintf(buf,",");
        fpOut << buf;

        fpOut << toString(rmsImageLineResiduals[imgIndex].Rms()).toLatin1().data();
        sprintf(buf,",");
        fpOut << buf;

        fpOut << toString(rmsImageResiduals[imgIndex].Rms()).toLatin1().data();
        sprintf(buf,",");
        fpOut << buf;


        QString observationString =
            observation->formatBundleOutputString(errorProp,true);

        //Removes trailing commas
        if (observationString.right(1)==",") {
            observationString.truncate(observationString.length()-1);
        }

        fpOut << (const char*) observationString.toLatin1().data();
        sprintf(buf,"\n");
        fpOut << buf;
        imgIndex++;

      }
  }

    fpOut.close();
    return true;
  }



  /**
   * Outputs a text file with the results of the BundleAdjust.
   *
   * @return @b bool If the text file was successfully output.
   */
  bool BundleSolutionInfo::outputText() {

    QString ofname = "bundleout.txt";
    ofname = m_settings->outputFilePrefix() + ofname;

    std::ofstream fpOut(ofname.toLatin1().data(), std::ios::out);
    if (!fpOut) {
      return false;
    }

    m_txtBundleOutputFilename = ofname;

    char buf[1056];
    BundleObservationQsp observation;

    int nObservations = m_statisticsResults->observations().size();

    outputHeader(fpOut);

    bool berrorProp = false;
    if (m_statisticsResults->converged() && m_settings->errorPropagation()) {
      berrorProp = true;
    }

    // output target body header if solving for target
    if (m_settings->solveTargetBody()) {
      sprintf(buf, "\nTARGET BODY\n==========================\n");
      fpOut << buf;

      sprintf(buf, "\n   Target         Initial              Total               "
                   "Final             Initial           Final\n"
                   "Parameter         Value              Correction           "
                   "Value             Accuracy          Accuracy\n");
      fpOut << buf;

      QString targetString =
          m_settings->bundleTargetBody()->formatBundleOutputString(berrorProp);
      fpOut << (const char*)targetString.toLatin1().data();
    }

    // output image exterior orientation header
    sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION\n==========================\n");
    fpOut << buf;

    QMap<QString, QStringList> imagesAndParameters;

    if (m_settings->solveTargetBody()) {
      imagesAndParameters.insert( "target", m_settings->bundleTargetBody()->parameterList() );
    }

    for (int i = 0; i < nObservations; i++) {

      observation = m_statisticsResults->observations().at(i);
      if (!observation) {
        continue;
      }

      int numImages = observation->size();
      for (int j = 0; j < numImages; j++) {
        BundleImageQsp image = observation->at(j);
        sprintf(buf, "\nImage Full File Name: %s\n", image->fileName().toLatin1().data());
        fpOut << buf;
        sprintf(buf, "\nImage Serial Number: %s\n", image->serialNumber().toLatin1().data());
        fpOut << buf;

        sprintf(buf, "\n    Image         Initial              Total               "
                     "Final             Initial           Final\n"
                     "Parameter         Value              Correction            "
                     "Value             Accuracy          Accuracy\n");
        fpOut << buf;

        QString observationString =
            observation->formatBundleOutputString(berrorProp);
        fpOut << (const char*)observationString.toLatin1().data();

        // Build list of images and parameters for correlation matrix.
        foreach ( QString image, observation->imageNames() ) {
          imagesAndParameters.insert( image, observation->parameterList() );
        }
      }
    }

    // Save list of images and their associated parameters for CorrelationMatrix to use in ice.
    m_statisticsResults->setCorrMatImgsAndParams(imagesAndParameters);

    // Save list of images and their associated parameters for CorrelationMatrix to use in ice.
    m_statisticsResults->setCorrMatImgsAndParams(imagesAndParameters);

    // output point uncertainty statistics if error propagation is on
    if (berrorProp) {
      sprintf(buf, "\n\n\nPOINTS UNCERTAINTY SUMMARY\n==========================\n\n");
      fpOut << buf;
      sprintf(buf, " RMS Sigma Latitude(m)%20.8lf\n",
              m_statisticsResults->sigmaLatitudeStatisticsRms());
      fpOut << buf;
      sprintf(buf, " MIN Sigma Latitude(m)%20.8lf at %s\n",
              m_statisticsResults->minSigmaLatitudeDistance().meters(),
              m_statisticsResults->minSigmaLatitudePointId().toLatin1().data());
      fpOut << buf;
      sprintf(buf, " MAX Sigma Latitude(m)%20.8lf at %s\n\n",
              m_statisticsResults->maxSigmaLatitudeDistance().meters(),
              m_statisticsResults->maxSigmaLatitudePointId().toLatin1().data());
      fpOut << buf;
      sprintf(buf, "RMS Sigma Longitude(m)%20.8lf\n",
              m_statisticsResults->sigmaLongitudeStatisticsRms());
      fpOut << buf;
      sprintf(buf, "MIN Sigma Longitude(m)%20.8lf at %s\n",
              m_statisticsResults->minSigmaLongitudeDistance().meters(),
              m_statisticsResults->minSigmaLongitudePointId().toLatin1().data());
      fpOut << buf;
      sprintf(buf, "MAX Sigma Longitude(m)%20.8lf at %s\n\n",
              m_statisticsResults->maxSigmaLongitudeDistance().meters(),
              m_statisticsResults->maxSigmaLongitudePointId().toLatin1().data());
      fpOut << buf;
      if ( m_settings->solveRadius() ) {
        sprintf(buf, "   RMS Sigma Radius(m)%20.8lf\n",
                m_statisticsResults->sigmaRadiusStatisticsRms());
        fpOut << buf;
        sprintf(buf, "   MIN Sigma Radius(m)%20.8lf at %s\n",
                m_statisticsResults->minSigmaRadiusDistance().meters(),
                m_statisticsResults->minSigmaRadiusPointId().toLatin1().data());
        fpOut << buf;
        sprintf(buf, "   MAX Sigma Radius(m)%20.8lf at %s\n",
                m_statisticsResults->maxSigmaRadiusDistance().meters(),
                m_statisticsResults->maxSigmaRadiusPointId().toLatin1().data());
        fpOut << buf;
      }
      else {
        sprintf(buf, "   RMS Sigma Radius(m)                 N/A\n");
        fpOut << buf;
        sprintf(buf, "   MIN Sigma Radius(m)                 N/A\n");
        fpOut << buf;
        sprintf(buf, "   MAX Sigma Radius(m)                 N/A\n");
        fpOut << buf;
      }
    }

    // output point summary data header
    sprintf(buf, "\n\nPOINTS SUMMARY\n==============\n%103s"
            "Sigma          Sigma              Sigma\n"
            "           Label         Status     Rays    RMS"
            "        Latitude       Longitude          Radius"
            "        Latitude       Longitude          Radius\n", "");
    fpOut << buf;

    int nPoints = m_statisticsResults->bundleControlPoints().size();
    for (int i = 0; i < nPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_statisticsResults->bundleControlPoints().at(i);

      QString pointSummaryString =
          bundleControlPoint->formatBundleOutputSummaryString(berrorProp);
      fpOut << (const char*)pointSummaryString.toLatin1().data();
    }

    // output point detail data header
    sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
    fpOut << buf;

    bool solveRadius = m_settings->solveRadius();

    for (int i = 0; i < nPoints; i++) {
      BundleControlPointQsp bundleControlPoint = m_statisticsResults->bundleControlPoints().at(i);

      QString pointDetailString =
          bundleControlPoint->formatBundleOutputDetailString(berrorProp,
                                                           m_statisticsResults->radiansToMeters(),
                                                           solveRadius);
      fpOut << (const char*)pointDetailString.toLatin1().data();
    }

    fpOut.close();

    return true;
  }


  /**
   * Outputs point data to a csv file.
   *
   * @return @b bool If the point data was successfully output.
   */
  bool BundleSolutionInfo::outputPointsCSV() {
    char buf[1056];

    QString ofname = "bundleout_points.csv";
    ofname = m_settings->outputFilePrefix() + ofname;
    m_csvSavedPointsFilename = ofname;

    std::ofstream fpOut(ofname.toLatin1().data(), std::ios::out);
    if (!fpOut) {
      return false;
    }

    int numPoints = m_statisticsResults->bundleControlPoints().size();

    double dLat, dLon, dRadius;
    double dX, dY, dZ;
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    QString strStatus;
    double cor_lat_m;
    double cor_lon_m;
    double cor_rad_m;
    int numMeasures, numRejectedMeasures;
    double dResidualRms;

    // print column headers
    if (m_settings->errorPropagation()) {
      sprintf(buf, ",,,,,3-d,3-d,3-d,Sigma,Sigma,Sigma,Correction,Correction,Correction,Coordinate,"
              "Coordinate,Coordinate\nPoint,Point,Accepted,Rejected,Residual,Latitude,Longitude,"
              "Radius,Latitude,Longitude,Radius,Latitude,Longitude,Radius,X,Y,Z\nLabel,Status,"
              "Measures,Measures,RMS,(dd),(dd),(km),(m),(m),(m),(m),(m),(m),(km),(km),(km)\n");
    }
    else {
      sprintf(buf, ",,,,,3-d,3-d,3-d,Correction,Correction,Correction,Coordinate,Coordinate,"
              "Coordinate\nPoint,Point,Accepted,Rejected,Residual,Latitude,Longitude,Radius,"
              "Latitude,Longitude,Radius,X,Y,Z\nLabel,Status,Measures,Measures,RMS,(dd),(dd),(km),"
              "(m),(m),(m),(km),(km),(km)\n");
    }
    fpOut << buf;

    for (int i = 0; i < numPoints; i++) {
      BundleControlPointQsp bundlecontrolpoint = m_statisticsResults->bundleControlPoints().at(i);

      if (!bundlecontrolpoint) {
        continue;
      }

      if (bundlecontrolpoint->isRejected()) {
        continue;
      }

      dLat              = bundlecontrolpoint->adjustedSurfacePoint().GetLatitude().degrees();
      dLon              = bundlecontrolpoint->adjustedSurfacePoint().GetLongitude().degrees();
      dRadius           = bundlecontrolpoint->adjustedSurfacePoint().GetLocalRadius().kilometers();
      dX                = bundlecontrolpoint->adjustedSurfacePoint().GetX().kilometers();
      dY                = bundlecontrolpoint->adjustedSurfacePoint().GetY().kilometers();
      dZ                = bundlecontrolpoint->adjustedSurfacePoint().GetZ().kilometers();
      numMeasures         = bundlecontrolpoint->numberOfMeasures();
      numRejectedMeasures = bundlecontrolpoint->numberOfRejectedMeasures();
      dResidualRms      = bundlecontrolpoint->residualRms();

      // point corrections and initial sigmas
      boost::numeric::ublas::bounded_vector< double, 3 > corrections = bundlecontrolpoint->
                                                                           corrections();
      cor_lat_m = corrections[0]*m_statisticsResults->radiansToMeters();
      cor_lon_m = corrections[1]*m_statisticsResults->radiansToMeters()*cos(dLat*Isis::DEG2RAD);
      cor_rad_m  = corrections[2]*1000.0;

      if (bundlecontrolpoint->type() == ControlPoint::Fixed) {
        strStatus = "FIXED";
      }
      else if (bundlecontrolpoint->type() == ControlPoint::Constrained) {
        strStatus = "CONSTRAINED";
      }
      else if (bundlecontrolpoint->type() == ControlPoint::Free) {
        strStatus = "FREE";
      }
      else {
        strStatus = "UNKNOWN";
      }

      if (m_settings->errorPropagation()) {
        dSigmaLat = bundlecontrolpoint->adjustedSurfacePoint().GetLatSigmaDistance().meters();
        dSigmaLong = bundlecontrolpoint->adjustedSurfacePoint().GetLonSigmaDistance().meters();
        dSigmaRadius = bundlecontrolpoint->adjustedSurfacePoint().GetLocalRadiusSigma().meters();

        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
                     "%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                bundlecontrolpoint->id().toLatin1().data(), strStatus.toLatin1().data(),
                numMeasures, numRejectedMeasures, dResidualRms, dLat, dLon, dRadius, dSigmaLat,
                dSigmaLong, dSigmaRadius, cor_lat_m, cor_lon_m, cor_rad_m, dX, dY, dZ);
      }
      else
        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
                     "%16.8lf,%16.8lf\n",
                bundlecontrolpoint->id().toLatin1().data(), strStatus.toLatin1().data(),
                numMeasures, numRejectedMeasures, dResidualRms, dLat, dLon, dRadius, cor_lat_m,
                cor_lon_m, cor_rad_m, dX, dY, dZ);

      fpOut << buf;
    }

    fpOut.close();

    return true;
  }


  /**
   * Outputs image coordinate residuals to a csv file.
   *
   * @return @b bool If the residuals were successfully output.
   */
  bool BundleSolutionInfo::outputResiduals() {
    char buf[1056];

    QString ofname = "residuals.csv";
    ofname = m_settings->outputFilePrefix() + ofname;
    m_csvSavedResidualsFilename = ofname;

    std::ofstream fpOut(ofname.toLatin1().data(), std::ios::out);
    if (!fpOut) {
      return false;
    }

    // output column headers

    sprintf(buf, ",,,x image,y image,Measured,Measured,sample,line,Residual Vector\n");
    fpOut << buf;
    sprintf(buf, "Point,Image,Image,coordinate,coordinate,"
                 "Sample,Line,residual,residual,Magnitude\n");
    fpOut << buf;
    sprintf(buf, "Label,Filename,Serial Number,(mm),(mm),"
                 "(pixels),(pixels),(pixels),(pixels),(pixels),Rejected\n");
    fpOut << buf;

    // Setup counts and pointers

    int numPoints = m_statisticsResults->bundleControlPoints().size();
    int numMeasures = 0;

    BundleControlPointQsp bundleControlPoint;
    BundleMeasureQsp bundleMeasure;

    for (int i = 0; i < numPoints; i++) {
      bundleControlPoint = m_statisticsResults->bundleControlPoints().at(i);
      numMeasures = bundleControlPoint->size();

      if (bundleControlPoint->rawControlPoint()->IsIgnored()) {
        continue;
      }

      for (int j = 0; j < numMeasures; j++) {
        bundleMeasure = bundleControlPoint->at(j);

        Camera *measureCamera = bundleMeasure->camera();
        if (!measureCamera) {
          continue;
        }

        if (bundleMeasure->isRejected()) {
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,*\n",
                  bundleControlPoint->id().toLatin1().data(),
                  bundleMeasure->parentBundleImage()->fileName().toLatin1().data(),
                  bundleMeasure->cubeSerialNumber().toLatin1().data(),
                  bundleMeasure->focalPlaneMeasuredX(),
                  bundleMeasure->focalPlaneMeasuredY(),
                  bundleMeasure->sample(),
                  bundleMeasure->line(),
                  bundleMeasure->sampleResidual(),
                  bundleMeasure->lineResidual(),
                  bundleMeasure->residualMagnitude());
        }
        else {
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                  bundleControlPoint->id().toLatin1().data(),
                  bundleMeasure->parentBundleImage()->fileName().toLatin1().data(),
                  bundleMeasure->cubeSerialNumber().toLatin1().data(),
                  bundleMeasure->focalPlaneMeasuredX(),
                  bundleMeasure->focalPlaneMeasuredY(),
                  bundleMeasure->sample(),
                  bundleMeasure->line(),
                  bundleMeasure->sampleResidual(),
                  bundleMeasure->lineResidual(),
                  bundleMeasure->residualMagnitude());
        }
        fpOut << buf;
      }
    }

    fpOut.close();

    return true;
  }


  /**
   * Saves the BundleSolutionInfo to the project
   *
   * Output format:
   *
   *
   * <image id="..." fileName="...">
   *   ...
   * </image>
   *
   * (fileName attribute is just the base name)
   *
   * @param stream The stream to which the BundleSolutionInfo will be saved
   * @param project The project to which this BundleSolutionInfo will be saved
   * @param newProjectRoot The location of the project root directory.
   */
  void BundleSolutionInfo::save(QXmlStreamWriter &stream, const Project *project,
                                FileName newProjectRoot) const {

    // TODO: comment below not clear, why is this done?
    // This is done for unitTest which has no Project
    // SHOULD WE BE CREATING A SERIALIZED PROJECT AS INPUT TO THIS UNIT TEST?
    QString relativePath;
    QString relativeBundlePath;
    FileName bundleSolutionInfoRoot;

    if (project) {
      bundleSolutionInfoRoot = FileName(Project::bundleSolutionInfoRoot(newProjectRoot.expanded()) +
                                      "/" + runTime());
      QString oldPath = project->bundleSolutionInfoRoot(project->projectRoot()) + "/" + runTime();
      QString newPath = project->bundleSolutionInfoRoot(newProjectRoot.toString()) + "/" + runTime();
      //  If project is being saved to new area, create directory and copy files
      if (oldPath != newPath) {
        //  Create project folder for BundleSolutionInfo
        QDir bundleDir(newPath);
        if (!bundleDir.mkpath(bundleDir.path())) {
          throw IException(IException::Io,
                           QString("Failed to create directory [%1]")
                             .arg(bundleSolutionInfoRoot.path()),
                           _FILEINFO_);
        }
        QString oldFile = oldPath + "/" + FileName(m_outputControl->fileName()).name();
        QString newFile = newPath + "/" + FileName(m_outputControl->fileName()).name();
        if (!QFile::copy(oldFile, newFile)) {
          throw IException(IException::Io,
                           QString("Failed to copy file [%1] to new file [%2]")
                             .arg(m_outputControl->fileName()).arg(newFile),
                           _FILEINFO_);
        }
        newFile = newPath + "/" + FileName(m_txtBundleOutputFilename).name();
        if (!QFile::copy(m_txtBundleOutputFilename, newFile)) {
          throw IException(IException::Io,
                           QString("Failed to copy file [%1] to new file [%2]")
                             .arg(m_txtBundleOutputFilename).arg(newFile),
                           _FILEINFO_);
        }
        newFile = newPath + "/" + FileName(m_csvSavedImagesFilename).name();
        if (!QFile::copy(m_csvSavedImagesFilename, newFile)) {
          throw IException(IException::Io,
                           QString("Failed to copy file [%1] to new file [%2]")
                             .arg(m_csvSavedImagesFilename).arg(newFile),
                           _FILEINFO_);        
        }
        newFile = newPath + "/" + FileName(m_csvSavedPointsFilename).name();
        if (!QFile::copy(m_csvSavedPointsFilename, newFile)) {
          throw IException(IException::Io,
                           QString("Failed to copy file [%1] to new file [%2]")
                             .arg(m_csvSavedPointsFilename).arg(newFile),
                           _FILEINFO_);        
        }
        newFile = newPath + "/" + FileName(m_csvSavedResidualsFilename).name();
        if (!QFile::copy(m_csvSavedResidualsFilename, newFile)) {
          throw IException(IException::Io,
                           QString("Failed to copy file [%1] to new file [%2]")
                             .arg(m_csvSavedResidualsFilename).arg(newFile),
                           _FILEINFO_);        
        }
      }

      // Create relativePath
      relativePath = m_inputControlNetFileName->expanded().remove(project->newProjectRoot());
      // Get rid of any preceding "/" , but add on ending "/"
      if (relativePath.startsWith("/")) {
        relativePath.remove(0,1);
      }

      // Create relativeBundlePath for bundleSolutionInfo
      relativeBundlePath = newPath.remove(project->newProjectRoot());
      // Get rid of any preceding "/" , but add on ending "/"
      if (relativeBundlePath.startsWith("/")) {
        relativeBundlePath.remove(0,1);
      }
      relativeBundlePath += "/";
    }

    // TODO: so, we can do the stuff below if project is NULL?

    stream.writeStartElement("bundleSolutionInfo");
    // save ID, cnet file name, and run time to stream
    stream.writeStartElement("generalAttributes");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("name", m_name);
    stream.writeTextElement("runTime", runTime());

    stream.writeTextElement("inputFileName",
                            relativePath);
    stream.writeTextElement("bundleOutTXT",
                            relativeBundlePath + FileName(m_txtBundleOutputFilename).name());
    stream.writeTextElement("imagesCSV",
                            relativeBundlePath + FileName(m_csvSavedImagesFilename).name());
    stream.writeTextElement("pointsCSV",
                            relativeBundlePath + FileName(m_csvSavedPointsFilename).name());
    stream.writeTextElement("residualsCSV",
                            relativeBundlePath + FileName(m_csvSavedResidualsFilename).name());
    stream.writeEndElement(); // end general attributes

    // save settings to stream
    m_settings->save(stream, project);

    // save statistics to stream
    m_statisticsResults->save(stream, project);

    if (project) {
      // save adjusted images lists to stream
      if (!m_adjustedImages->isEmpty()) {
        stream.writeStartElement("imageLists");
        for (int i = 0; i < m_adjustedImages->count(); i++) {
          m_adjustedImages->at(i)->save(stream, project, bundleSolutionInfoRoot);
        }
        stream.writeEndElement();
      }

      // save output control
      stream.writeStartElement("outputControl");
      m_outputControl->save(stream, project, relativeBundlePath);
      stream.writeEndElement();
    }

    stream.writeEndElement(); //end bundleSolutionInfo
  }


  /**
   * Create an XML Handler (reader) that can populate the BundleSolutionInfo class data. See
   *   BundleSolutionInfo::save() for the expected format.
   *
   * @param bundleSolutionInfo The bundle solution we're going to be initializing
   * @param project The project we are working in
   */
  BundleSolutionInfo::XmlHandler::XmlHandler(BundleSolutionInfo *bundleSolutionInfo,
                                             Project *project) {
    m_xmlHandlerBundleSolutionInfo = bundleSolutionInfo;
    m_xmlHandlerProject = project;
    m_xmlHandlerCharacters = "";
  }


  /**
   * Destructor
   */
  BundleSolutionInfo::XmlHandler::~XmlHandler() {
  }


  /**
   * Adds characters to m_xmlHandlerCharacters
   *
   * @param ch QString of characters to add
   *
   * @return @b bool Almost always true. Only false if the characters cannot be read
   */
  bool BundleSolutionInfo::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }


  /**
   * Handle an XML start element. This expects <image/> and <displayProperties/> elements.
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML.
   * @param qName ???
   * @param atts The attribute containing the keyword value for the given local name.
   *
   * @return @b bool True if we should continue reading the XML.
   */
  bool BundleSolutionInfo::XmlHandler::startElement(const QString &namespaceURI,
                                                    const QString &localName,
                                                    const QString &qName,
                                                    const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {

      if (localName == "bundleSettings") {
        m_xmlHandlerBundleSolutionInfo->m_settings =
            BundleSettingsQsp(new BundleSettings(m_xmlHandlerProject, reader()));
      }
      else if (localName == "bundleResults") {
        m_xmlHandlerBundleSolutionInfo->m_statisticsResults = new BundleResults(m_xmlHandlerProject,
                                                                                reader());
      }
      else if (localName == "imageList") {
        m_xmlHandlerBundleSolutionInfo->m_adjustedImages->append(
            new ImageList(m_xmlHandlerProject, reader()));
      }
      else if (localName == "outputControl") {
        FileName outputControlPath = FileName(m_xmlHandlerProject->bundleSolutionInfoRoot() + "/"
                                              + m_xmlHandlerBundleSolutionInfo->runTime());

        m_xmlHandlerBundleSolutionInfo->m_outputControl = new Control(outputControlPath, reader());
      }
    }
    return true;
  }


  /**
   * Handle an XML end element.
   *
   * @param namespaceURI ???
   * @param localName The keyword name given to the member variable in the XML.
   * @param qName ???
   *
   * @return @b bool Returns XmlStackedHandler's endElement()
   */
  bool BundleSolutionInfo::XmlHandler::endElement(const QString &namespaceURI,
                                                  const QString &localName,
                                                  const QString &qName) {
    // This is done for unitTest which has no Project
    QString projectRoot;
    if (m_xmlHandlerProject) {
      projectRoot = m_xmlHandlerProject->projectRoot() + "/";
    }

    if (localName == "id") {
      // all constructors assign a Uuid - we need to give it a one from the XML
      assert(m_xmlHandlerBundleSolutionInfo->m_id);
      delete m_xmlHandlerBundleSolutionInfo->m_id;
      m_xmlHandlerBundleSolutionInfo->m_id = new QUuid(m_xmlHandlerCharacters);
    }
    else if (localName == "name") {
      m_xmlHandlerBundleSolutionInfo->m_name = m_xmlHandlerCharacters;
    }
    else if (localName == "runTime") {
      m_xmlHandlerBundleSolutionInfo->m_runTime = m_xmlHandlerCharacters;
    }
    else if (localName == "inputFileName") {
      assert(m_xmlHandlerBundleSolutionInfo->m_inputControlNetFileName == NULL);
      m_xmlHandlerBundleSolutionInfo->m_inputControlNetFileName = new FileName(
        projectRoot + m_xmlHandlerCharacters);
    }
    else if (localName == "bundleOutTXT") {
      m_xmlHandlerBundleSolutionInfo->m_txtBundleOutputFilename =
        projectRoot + m_xmlHandlerCharacters;
    }
    else if (localName == "imagesCSV") {
      m_xmlHandlerBundleSolutionInfo->m_csvSavedImagesFilename = 
        projectRoot + m_xmlHandlerCharacters;
    }
    else if (localName == "pointsCSV") {
      m_xmlHandlerBundleSolutionInfo->m_csvSavedPointsFilename =
        projectRoot + m_xmlHandlerCharacters;
    }
    else if (localName == "residualsCSV") {
      m_xmlHandlerBundleSolutionInfo->m_csvSavedResidualsFilename =
        projectRoot + m_xmlHandlerCharacters;
    }

    m_xmlHandlerCharacters = "";
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
