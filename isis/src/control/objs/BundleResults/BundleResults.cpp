#include "BundleResults.h"

#include <QDataStream>
#include <QDebug>
#include <QUuid>
#include <QXmlStreamWriter>

#include "BundleSettings.h"
#include "BundleStatistics.h"
#include "ControlNet.h"
#include "FileName.h"
#include "ImageList.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  BundleResults::BundleResults(BundleSettings inputSettings, FileName controlNetworkFileName, QObject *parent) : QObject(parent) {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());


    m_controlNetworkFileName = NULL;
    m_controlNetworkFileName = new FileName(controlNetworkFileName);

    m_settings = NULL;
    m_settings = new BundleSettings(inputSettings);

    m_statisticsResults = NULL;

    m_runTime = "";

  }


  
  BundleResults::BundleResults(const BundleResults &other)
      : m_id(new QUuid(other.m_id->toString())),
        m_controlNetworkFileName(new FileName(other.m_controlNetworkFileName->expanded())),
        m_settings(new BundleSettings(*other.m_settings)),
        m_statisticsResults(new BundleStatistics(*other.m_statisticsResults)) {
    // TODO: add m_images???
    // TODO: add m_runTime???

  }



  BundleResults::~BundleResults() {
    delete m_id;
    m_id = NULL;

    delete m_controlNetworkFileName;
    m_controlNetworkFileName = NULL;

    delete m_settings;
    m_settings = NULL;

    delete m_statisticsResults;
    m_statisticsResults = NULL;
  }


  
  BundleResults &BundleResults::operator=(const BundleResults &other) {

    if (&other != this) {

      delete m_id;
      m_id = NULL;
      m_id = new QUuid(other.m_id->toString());

      delete m_controlNetworkFileName;
      m_controlNetworkFileName = NULL;
      m_controlNetworkFileName = new FileName(other.m_controlNetworkFileName->expanded());

      delete m_settings;
      m_settings = NULL;
      m_settings = new BundleSettings(*other.m_settings);

      delete m_statisticsResults;
      m_statisticsResults = NULL;
      m_statisticsResults = new BundleStatistics(*other.m_statisticsResults);
    // TODO: add m_images???
    // TODO: add m_runTime???
    }
    return *this;
  }



  void BundleResults::setOutputStatistics(BundleStatistics statisticsResults) {
    delete m_statisticsResults;
    m_statisticsResults = NULL;
    m_statisticsResults = new BundleStatistics(statisticsResults);
  }




  PvlObject BundleResults::pvlObject(QString resultsName, QString settingsName, 
                                     QString statisticsName) {

    PvlObject pvl(resultsName);
    if (m_controlNetworkFileName->expanded() != "") {
      pvl += PvlKeyword("OutputControlNetwork", m_controlNetworkFileName->expanded());
    }
    pvl += m_settings->pvlObject(settingsName);
    pvl += m_statisticsResults->pvlObject(statisticsName);
    return pvl;

  }



  /**
   * Output format:
   *
   *
   * <image id="..." fileName="...">
   *   ...
   * </image>
   *
   * (fileName attribute is just the base name)
   */
  void BundleResults::save(QXmlStreamWriter &stream, const Project *project,
                            FileName newProjectRoot) const {

    stream.writeStartElement("bundleResults");
    // save ID, cnet file name, and run time to stream
    stream.writeStartElement("generalAttributes");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("runTime", m_runTime);
    stream.writeTextElement("fileName", m_controlNetworkFileName->expanded());
    stream.writeEndElement(); // end general attributes

    // save settings to stream
    m_settings->save(stream, project);

    // save statistics to stream
    m_statisticsResults->save(stream, project);

    // save image lists to stream
    stream.writeStartElement("images");
    stream.writeAttribute("numberImageLists", toString(m_images->size()));
    for (int i = 0; i < m_images->size(); i++) {
      m_images->at(i)->save(stream, project, newProjectRoot);
    }
    stream.writeEndElement(); // end images

    stream.writeEndElement(); //end bundleResults
  }



  /**
   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
   *   BundleSettings::save() for the expected format.
   *
   * @param bundleSettings The image we're going to be initializing
   * @param imageFolder The folder that contains the Cube
   */
  BundleResults::XmlHandler::XmlHandler(BundleResults *bundleResults, Project *project) {
    m_bundleResults = bundleResults;
    m_project = project;
  }



  BundleResults::XmlHandler::~XmlHandler() {
    delete m_project;
    m_project = NULL;
  }



  /**
   * Handle an XML start element. This expects <image/> and <displayProperties/> elements.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool BundleResults::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                       const QString &qName, const QXmlAttributes &atts) {
    m_characters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {

      if (localName == "generalAttributes") {
        QString identification = atts.value("id");
        if (!identification.isEmpty()) {
          delete m_bundleResults->m_id;
          m_bundleResults->m_id = NULL;
          m_bundleResults->m_id = new QUuid(identification);
        }

        QString dateTime = atts.value("runTime");
        if (!dateTime.isEmpty()) {
          m_bundleResults->m_runTime = dateTime; // TODO: is this necessary for qstrings???
        }

        QString name = atts.value("fileName");
        if (!name.isEmpty()) {
          delete m_bundleResults->m_controlNetworkFileName;
          m_bundleResults->m_controlNetworkFileName = NULL;
          m_bundleResults->m_controlNetworkFileName = new FileName(name);
        }
      }
      else if (localName == "bundleSettings") {
        delete m_bundleResults->m_settings;
        m_bundleResults->m_settings = NULL;
        m_bundleResults->m_settings = new BundleSettings(m_project, reader());
      }
      else if (localName == "bundleStatistics") {
        delete m_bundleResults->m_statisticsResults;
        m_bundleResults->m_statisticsResults = NULL;
        m_bundleResults->m_statisticsResults = new BundleStatistics(m_project, reader()); //TODO: need to add constructor for this???
      }
      else if (localName == "images") {
        QString numberImageLists = atts.value("numberImageLists");
        int listSize = toInt(numberImageLists);
        m_bundleResults->m_images->clear();
        for (int i = 0; i < listSize; i++) {
          m_bundleResults->m_images->append(new ImageList(m_project, reader())); 
        }
      }
    }
    return true;
  }



  /**
   * Get a unique, identifying string associated with this BundleResults object.
   *
   * @return A unique ID for this BundleResults object
   */
  QString BundleResults::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }


  void BundleResults::setRunTime(QString runTime) {
    m_runTime = runTime;
  }


  QString BundleResults::runTime() const {
    return m_runTime;
  }


  QString BundleResults::controlNetworkFileName() const {
    return m_controlNetworkFileName->expanded();
  }

  BundleSettings *BundleResults::bundleSettings() {
    return m_settings;
  }

  BundleStatistics *BundleResults::bundleStatistics() {
    return m_statisticsResults;
  }

  QDataStream &BundleResults::write(QDataStream &stream) const {
    stream << m_id->toString()
           << m_controlNetworkFileName->expanded()
           << *m_settings
           << *m_statisticsResults;
    // TODO: add m_images???
    // TODO: add m_runTime???
    return stream;
  }



  QDataStream &BundleResults::read(QDataStream &stream) {

    QString id;
    stream >> id;
    delete m_id;
    m_id = NULL;
    m_id = new QUuid(id);

    QString controlNetworkFileName;
    stream >> controlNetworkFileName;
    delete m_controlNetworkFileName;
    m_controlNetworkFileName = NULL;
    m_controlNetworkFileName = new FileName(controlNetworkFileName);

    BundleSettings settings;
    stream >> settings;
    delete m_settings;
    m_settings = NULL;
    m_settings = new BundleSettings(settings);

    BundleStatistics statisticsResults;
    stream >> statisticsResults;
    delete m_statisticsResults;
    m_statisticsResults = NULL;
    m_statisticsResults = new BundleStatistics(statisticsResults);

    // TODO: add m_images???
    // TODO: add m_runTime???
    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const BundleResults &bundleResults) {
    return bundleResults.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, BundleResults &bundleResults) {
    return bundleResults.read(stream);
  }

}


#if 0

/**
 * Output bundle results to file.
 */
bool BundleAdjust::output(int numberIterations, double radiansToMetersBodyConversion, ) {
  if (m_bundleSettings.createBundleOutputFile()) {
    if (m_bundleStatistics.converged() && m_bundleSettings.errorPropagation()) {
      outputWithErrorPropagation(numberIterations, radiansToMetersBodyConversion);
    }
    else {
      outputNoErrorPropagation(numberIterations, radiansToMetersBodyConversion);
    }
  }

  if (m_bundleSettings.createCSVPointsFile()) {
    outputPointsCSV(radiansToMetersBodyConversion, );
    outputImagesCSV();
  }

  if (m_bundleSettings.createResidualsFile()) {
    outputResiduals();
  }

  return true;
}


/**
 * Output header for bundle results file.
 *
 */
bool BundleAdjust::outputHeader(std::ofstream &fp_out, int numberIterations) {
  if (!fp_out) return false;

  char buf[1056];
  int nImages              = images();
  int nValidPoints         = m_pCnet->GetNumValidPoints();
  int nInnerConstraints    = 0;
  int nDistanceConstraints = 0;
  int nDegreesOfFreedom    = m_bundleStatistics.numberObservations()
                          + m_bundleStatistics.numberConstrainedPointParameters()
                          + m_bundleStatistics.numberConstrainedImageParameters()
                          - m_bundleStatistics.numberUnknownParameters(); // ??? same as bstat dof ???
                                                                          //??? m_bundleStatistics.computeDegreesOfFreedom();
                                                                          //??? int nDegreesOfFreedom = m_bundleStatistics.degreesOfFreedom();

  int nConvergenceCriteria = 1;

  sprintf(buf, "JIGSAW: BUNDLE ADJUSTMENT\n=========================\n");
  fp_out << buf;
  sprintf(buf, "\n                       Run Time: %s",
          Isis::iTime::CurrentLocalTime().toAscii().data());
  fp_out << buf;
  sprintf(buf, "\n               Network Filename: %s", m_strCnetFileName.toAscii().data());
  fp_out << buf;
  sprintf(buf, "\n                     Network Id: %s", m_pCnet->GetNetworkId().toAscii().data());
  fp_out << buf;
  sprintf(buf, "\n            Network Description: %s", m_pCnet->Description().toAscii().data());
  fp_out << buf;
  sprintf(buf, "\n                         Target: %s", m_pCnet->GetTarget().toAscii().data());
  fp_out << buf;
  sprintf(buf, "\n\n                   Linear Units: kilometers");
  fp_out << buf;
  sprintf(buf, "\n                  Angular Units: decimal degrees");
  fp_out << buf;
  sprintf(buf, "\n\nINPUT: SOLVE OPTIONS\n====================\n");
  fp_out << buf;

  m_bundleSettings.solveObservationMode() ?
      sprintf(buf, "\n                   OBSERVATIONS: ON") :
      sprintf(buf, "\n                   OBSERVATIONS: OFF");
  fp_out << buf;

  m_bundleSettings.solveRadius() ?
      sprintf(buf, "\n                         RADIUS: ON") :
      sprintf(buf, "\n                         RADIUS: OFF");
  fp_out << buf;

  m_bundleSettings.updateCubeLabel() ?
      sprintf(buf, "\n                         UPDATE: YES") :
      sprintf(buf, "\n                         UPDATE: NO");
  fp_out << buf;

  sprintf(buf, "\n                  SOLUTION TYPE: %s",
          BundleSettings::solveMethodToString(
              m_bundleSettings.solveMethod()).toUpper().toAscii().data());
  fp_out << buf;

  m_bundleSettings.errorPropagation() ?
      sprintf(buf, "\n              ERROR PROPAGATION: ON") :
      sprintf(buf, "\n              ERROR PROPAGATION: OFF");
  fp_out << buf;

  m_bundleSettings.outlierRejection() ?
      sprintf(buf, "\n              OUTLIER REJECTION: ON") :
      sprintf(buf, "\n              OUTLIER REJECTION: OFF");
  fp_out << buf;

  sprintf(buf, "\n           REJECTION MULTIPLIER: %lf",
          m_bundleSettings.outlierRejectionMultiplier());
  fp_out << buf;

  sprintf(buf, "\n\nMAXIMUM LIKELIHOOD ESTIMATION\n============================\n");
  fp_out << buf;

  for (int tier = 0;tier < 3;tier++) {
    if (tier < m_bundleStatistics.numberMaximumLikelihoodModels()) { // replace number of models variable with settings models.size()???
      sprintf(buf, "\n                         Tier %d Enabled: TRUE", tier);
      fp_out << buf;
      sprintf(buf, "\n               Maximum Likelihood Model: %s",
              MaximumLikelihoodWFunctions::modelToString(
                  m_bundleStatistics.maximumLikelihoodModelWFunc(tier).model()).toAscii().data()); // use bundle settings ???
//        sprintf(buf, "\n               Maximum Likelihood Model: %s",
//                BundleSettings::maximumLikelihoodModelToString(
//                    m_bundleStatistics.maximumLikelihoodModelWFunc(tier).model()).toAscii().data());
//        sprintf(buf, "\n               Maximum Likelihood Model: ");
//        fp_out << buf;
//        m_bundleStatistics.maximumLikelihoodModelWFunc(tier).maximumLikelihoodModel(buf);
//        fp_out << buf;
      sprintf(buf, "\n    Quantile used for tweaking constant: %lf",
              m_bundleStatistics.maximumLikelihoodModelQuantile(tier));
      fp_out << buf;
      sprintf(buf, "\n   Quantile weighted R^2 Residual value: %lf",
              m_bundleStatistics.maximumLikelihoodModelWFunc(tier).tweakingConstant());
      fp_out << buf;
      sprintf(buf, "\n       Approx. weighted Residual cutoff: ");
      fp_out << buf;
      m_bundleStatistics.maximumLikelihoodModelWFunc(tier).weightedResidualCutoff(buf);
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
          m_bundleSettings.convergenceCriteriaThreshold());
  fp_out << buf;
  sprintf(buf, "\n             MAXIMUM ITERATIONS: %d",
          m_bundleSettings.convergenceCriteriaMaximumIterations());
  fp_out << buf;
  sprintf(buf, "\n\nINPUT: CAMERA POINTING OPTIONS\n==============================\n");
  fp_out << buf;

  //??? this line could replace the switch/case below...   sprintf(buf, "\n                          CAMSOLVE: %s", BundleSettings::instrumentPointingSolveOptionToString(m_bundleSettings.instrumentPointingSolveOption()).toUpper().toAscii().data();
  switch (m_bundleSettings.instrumentPointingSolveOption()) {
    case BundleSettings::AnglesOnly:
      sprintf(buf, "\n                          CAMSOLVE: ANGLES");
      break;
    case BundleSettings::AnglesVelocity:
      sprintf(buf, "\n                          CAMSOLVE: ANGLES, VELOCITIES");
      break;
    case BundleSettings::AnglesVelocityAcceleration:
      sprintf(buf, "\n                          CAMSOLVE: ANGLES, VELOCITIES, ACCELERATIONS");
      break;
    case BundleSettings::AllPointingCoefficients:
      sprintf(buf, "\n                          CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)",
              m_bundleSettings.ckSolveDegree());
      break;
    case BundleSettings::NoPointingFactors:
      sprintf(buf, "\n                          CAMSOLVE: NONE");
      break;
    default:
      break;
  }
  fp_out << buf;

  m_bundleSettings.solveTwist() ?
      sprintf(buf, "\n                             TWIST: ON") :
      sprintf(buf, "\n                             TWIST: OFF");
  fp_out << buf;

  m_bundleSettings.fitInstrumentPointingPolynomialOverExisting() ?
      sprintf(buf, "\n POLYNOMIAL OVER EXISTING POINTING: ON") :
      sprintf(buf, "\nPOLYNOMIAL OVER EXISTING POINTING : OFF");
  fp_out << buf;

  sprintf(buf, "\n\nINPUT: SPACECRAFT OPTIONS\n=========================\n");
  fp_out << buf;

//??? this line could replace the switch/case below...    sprintf(buf, "\n                          SPSOLVE: %s", BundleSettings::instrumentPositionSolveOptionToString(m_bundleSettings.instrumentPositionSolveOption()).toUpper().toAscii().data();
  switch (m_bundleSettings.instrumentPositionSolveOption()) {
    case BundleSettings::NoPositionFactors:
      sprintf(buf, "\n                        SPSOLVE: NONE");
      break;
    case BundleSettings::PositionOnly:
      sprintf(buf, "\n                        SPSOLVE: POSITION");
      break;
    case BundleSettings::PositionVelocity:
      sprintf(buf, "\n                        SPSOLVE: POSITION, VELOCITIES");
      break;
    case BundleSettings::PositionVelocityAcceleration:
      sprintf(buf, "\n                        SPSOLVE: POSITION, VELOCITIES, ACCELERATIONS");
      break;
    case BundleSettings::AllPositionCoefficients:
      sprintf(buf, "\n                       CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)",
              m_bundleSettings.spkSolveDegree());
      break;
    default:
      break;
  }
  fp_out << buf;

  m_bundleSettings.solveInstrumentPositionOverHermiteSpline() ?
      sprintf(buf, "\n POLYNOMIAL OVER HERMITE SPLINE: ON") :
      sprintf(buf, "\nPOLYNOMIAL OVER HERMITE SPLINE : OFF");
  fp_out << buf;

  sprintf(buf, "\n\nINPUT: GLOBAL IMAGE PARAMETER UNCERTAINTIES"
          "\n===========================================\n");
  fp_out << buf;

  (m_bundleSettings.globalLatitudeAprioriSigma() == -1) ?
      sprintf(buf, "\n               POINT LATITUDE SIGMA: N/A") :
      sprintf(buf, "\n               POINT LATITUDE SIGMA: %lf (meters)",
              m_bundleSettings.globalLatitudeAprioriSigma());
  fp_out << buf;

  (m_bundleSettings.globalLongitudeAprioriSigma() == -1) ?
      sprintf(buf, "\n              POINT LONGITUDE SIGMA: N/A") :
      sprintf(buf, "\n              POINT LONGITUDE SIGMA: %lf (meters)",
              m_bundleSettings.globalLongitudeAprioriSigma());
  fp_out << buf;

  (m_bundleSettings.globalRadiusAprioriSigma() == -1) ?
      sprintf(buf, "\n                 POINT RADIUS SIGMA: N/A") :
      sprintf(buf, "\n                 POINT RADIUS SIGMA: %lf (meters)",
              m_bundleSettings.globalRadiusAprioriSigma());
  fp_out << buf;

  if (m_nNumberCamPosCoefSolved < 1
      || m_bundleSettings.globalInstrumentPositionAprioriSigma() == -1) {
    sprintf(buf, "\n          SPACECRAFT POSITION SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n          SPACECRAFT POSITION SIGMA: %lf (meters)",
            m_bundleSettings.globalInstrumentPositionAprioriSigma());
  }
  fp_out << buf;

  if (m_nNumberCamPosCoefSolved < 2
      || m_bundleSettings.globalInstrumentVelocityAprioriSigma() == -1) {
    sprintf(buf, "\n          SPACECRAFT VELOCITY SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n          SPACECRAFT VELOCITY SIGMA: %lf (m/s)",
            m_bundleSettings.globalInstrumentVelocityAprioriSigma());
  }
  fp_out << buf;

  if (m_nNumberCamPosCoefSolved < 3
      || m_bundleSettings.globalInstrumentAccelerationAprioriSigma() == -1) {
    sprintf(buf, "\n      SPACECRAFT ACCELERATION SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n      SPACECRAFT ACCELERATION SIGMA: %lf (m/s/s)",
            m_bundleSettings.globalInstrumentAccelerationAprioriSigma());
  }
  fp_out << buf;

  if (m_nNumberCamAngleCoefSolved < 1
      || m_bundleSettings.globalInstrumentPointingAnglesAprioriSigma() == -1) {
    sprintf(buf, "\n                CAMERA ANGLES SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n                CAMERA ANGLES SIGMA: %lf (dd)",
            m_bundleSettings.globalInstrumentPointingAnglesAprioriSigma());
  }
  fp_out << buf;

  if (m_nNumberCamAngleCoefSolved < 2
      || m_bundleSettings.globalInstrumentPointingAngularVelocityAprioriSigma() == -1) {
    sprintf(buf, "\n      CAMERA ANGULAR VELOCITY SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n      CAMERA ANGULAR VELOCITY SIGMA: %lf (dd/s)",
            m_bundleSettings.globalInstrumentPointingAngularVelocityAprioriSigma());
  }
  fp_out << buf;

  if (m_nNumberCamAngleCoefSolved < 3
      || m_bundleSettings.globalInstrumentPointingAngularAccelerationAprioriSigma() == -1) {
    sprintf(buf, "\n  CAMERA ANGULAR ACCELERATION SIGMA: N/A");
  }
  else {
    sprintf(buf, "\n  CAMERA ANGULAR ACCELERATION SIGMA: %lf (dd/s/s)",
            m_bundleSettings.globalInstrumentPointingAngularAccelerationAprioriSigma());
  }
  fp_out << buf;

  sprintf(buf, "\n\nJIGSAW: RESULTS\n===============\n");
  fp_out << buf;
  sprintf(buf, "\n                         Images: %6d", nImages);
  fp_out << buf;
  sprintf(buf, "\n                         Points: %6d", nValidPoints);
  fp_out << buf;

  sprintf(buf, "\n                 Total Measures: %6d",
          (m_bundleStatistics.numberObservations()
           + m_bundleStatistics.numberRejectedObservations()) / 2);
  fp_out << buf;

  sprintf(buf, "\n             Total Observations: %6d",
          m_bundleStatistics.numberObservations()
          + m_bundleStatistics.numberRejectedObservations());
  fp_out << buf;

  sprintf(buf, "\n              Good Observations: %6d", m_bundleStatistics.numberObservations());
  fp_out << buf;

  sprintf(buf, "\n          Rejected Observations: %6d",
          m_bundleStatistics.numberRejectedObservations());
  fp_out << buf;

  if (m_bundleStatistics.numberConstrainedPointParameters() > 0) {
    sprintf(buf, "\n   Constrained Point Parameters: %6d",
            m_bundleStatistics.numberConstrainedPointParameters());
    fp_out << buf;
  }

  if (m_bundleStatistics.numberConstrainedImageParameters() > 0) {
    sprintf(buf, "\n   Constrained Image Parameters: %6d",
            m_bundleStatistics.numberConstrainedImageParameters());
    fp_out << buf;
  }

  sprintf(buf, "\n                       Unknowns: %6d",
          m_bundleStatistics.numberUnknownParameters());
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
          m_bundleSettings.convergenceCriteriaThreshold());
  fp_out << buf;

  if (nConvergenceCriteria == 1) {
    sprintf(buf, "(Sigma0)");
    fp_out << buf;
  }

  sprintf(buf, "\n                     Iterations: %6d", numberIterations);
  fp_out << buf;

  if (numberIterations >= m_bundleSettings.convergenceCriteriaMaximumIterations()) {
    sprintf(buf, "(Maximum reached)");
    fp_out << buf;
  }

  sprintf(buf, "\n                         Sigma0: %30.20lf\n", m_bundleStatistics.sigma0());
  fp_out << buf;
  sprintf(buf, " Error Propagation Elapsed Time: %6.4lf (seconds)\n",
          m_bundleStatistics.elapsedTimeErrorProp());
  fp_out << buf;
  sprintf(buf, "             Total Elapsed Time: %6.4lf (seconds)\n",
          m_bundleStatistics.elapsedTime());
  fp_out << buf;
  if (m_bundleStatistics.numberObservations() + m_bundleStatistics.numberRejectedObservations()
      > 100) {  //if there was enough data to calculate percentiles and box plot data
    sprintf(buf, "\n           Residual Percentiles:\n");
    fp_out << buf;
    try {
      for (int bin = 1;bin < 34;bin++) {
        //double quan =
        //    m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(double(bin)/100);
        double cumProb  = double(bin) / 100.0;
        double resValue =
                          m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(cumProb);
        double resValue33 =
                            m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(cumProb + 0.33);
        double resValue66 =
                            m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(cumProb + 0.66);
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
              m_bundleStatistics.residualsCumulativeProbabilityDistribution().min());
      fp_out << buf;
      sprintf(buf, "\n                     Quartile 1: %+8.3lf",
              m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(0.25));
      fp_out << buf;
      sprintf(buf, "\n                         Median: %+8.3lf",
              m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(0.50));
      fp_out << buf;
      sprintf(buf, "\n                     Quartile 3: %+8.3lf",
              m_bundleStatistics.residualsCumulativeProbabilityDistribution().value(0.75));
      fp_out << buf;
      sprintf(buf, "\n                        maximum: %+8.3lf\n",
              m_bundleStatistics.residualsCumulativeProbabilityDistribution().max());
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

  for (int i = 0;i < nImages;i++) {
    // imageIndex(i) retrieves index into the normal equations matrix
    // for Image(i)
    double rmsSampleResiduals = m_bundleStatistics.rmsImageSampleResiduals()[i].Rms();
    double rmsLineResiduals   = m_bundleStatistics.rmsImageLineResiduals()[i].Rms();
    double rmsLandSResiduals  = m_bundleStatistics.rmsImageResiduals()[i].Rms();

    nMeasures = m_pCnet->GetNumberOfValidMeasuresInImage(m_pSnList->SerialNumber(i));
    nRejectedMeasures =
                        m_pCnet->GetNumberOfJigsawRejectedMeasuresInImage(m_pSnList->SerialNumber(i));

    nUsed = nMeasures - nRejectedMeasures;

    if (nUsed == nMeasures) {
      sprintf(buf, "%s   %5d of %5d %6.3lf %6.3lf %6.3lf\n",
              m_pSnList->FileName(i).toAscii().data(),
              (nMeasures - nRejectedMeasures), nMeasures,
              rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
    }
    else {
      sprintf(buf, "%s   %5d of %5d* %6.3lf %6.3lf %6.3lf\n",
              m_pSnList->FileName(i).toAscii().data(),
              (nMeasures - nRejectedMeasures), nMeasures,
              rmsSampleResiduals, rmsLineResiduals, rmsLandSResiduals);
    }
    fp_out << buf;
  }

  return true;
}


/**
 * output bundle results to file with error propagation
 */
bool BundleAdjust::outputWithErrorPropagation(int numberIterations, 
                                              double radiansToMetersBodyConversion) {

  QString ofname("bundleout.txt");
  if ( m_bundleSettings.outputFilePrefix().length() != 0 )
  ofname = m_bundleSettings.outputFilePrefix() + "_" + ofname;

  std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
  if (!fp_out)
  return false;

  char buf[1056];
  std::vector<double> coefX(m_nNumberCamPosCoefSolved);
  std::vector<double> coefY(m_nNumberCamPosCoefSolved);
  std::vector<double> coefZ(m_nNumberCamPosCoefSolved);
  std::vector<double> coefRA(m_nNumberCamAngleCoefSolved);
  std::vector<double> coefDEC(m_nNumberCamAngleCoefSolved);
  std::vector<double> coefTWI(m_nNumberCamAngleCoefSolved);
  std::vector<double> angles;
  Camera *pCamera = NULL;
  SpicePosition *pSpicePosition = NULL;
  SpiceRotation *pSpiceRotation = NULL;

  int nImages = images();
  double dSigma= 0;
  int nIndex = 0;

  gmm::row_matrix<gmm::rsvector<double> > lsqCovMatrix;

  if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
    lsqCovMatrix = m_pLsq->GetCovarianceMatrix();  // get reference to the covariance matrix
                                                   // from the least-squares object
  }

  // data structure to contain adjusted image parameter sigmas for CHOLMOD error propagation only
  vector<double> vImageAdjustedSigmas;

  outputHeader(fp_out);

  sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION\n==========================\n");
  fp_out << buf;

  for (int i = 0; i < nImages; i++) {

    //if ( m_bundleStatistics.numberHeldImages() > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) )
    //    bHeld = true;

    pCamera = m_pCnet->Camera(i);
    if (!pCamera) {
      continue;
    }

    // imageIndex(i) retrieves index into the normal equations matrix for Image(i)
    nIndex = imageIndex(i);

    if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
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

    // for frame cameras we directly retrieve the Exterior Pointing (i.e. position
    // and orientation angles). For others (linescan, radar) we retrieve the polynomial
    // coefficients from which the Exterior Pointing parameters are derived.
    if (m_bundleSettings.instrumentPositionSolveOption() > 0) {
      pSpicePosition->GetPolynomial(coefX, coefY, coefZ);
    }
    else { // not solving for position
      std::vector <double> coordinate(3);
      coordinate = pSpicePosition->GetCenterCoordinate();
      coefX.push_back(coordinate[0]);
      coefY.push_back(coordinate[1]);
      coefZ.push_back(coordinate[2]);
    }

    if (m_bundleSettings.instrumentPointingSolveOption() > 0) {
      pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
    }
    //          else { // frame camera
    else { // m_bundleSettings.instrumentPointingSolveOption() = BundleSettings::NoPointingFactors
           // and no polynomial fit has occurred
      angles = pSpiceRotation->GetCenterAngles();
      coefRA.push_back(angles.at(0));
      coefDEC.push_back(angles.at(1));
      coefTWI.push_back(angles.at(2));
    }

    sprintf(buf, "\nImage Full File Name: %s\n", m_pSnList->FileName(i).toAscii().data());
    fp_out << buf;
    sprintf(buf, "\nImage Serial Number: %s\n", m_pSnList->SerialNumber(i).toAscii().data());
    fp_out << buf;
    sprintf(buf, "\n    Image         Initial              Total               "
            "Final             Initial           Final\n"
            "Parameter         Value              Correction            "
            "Value             Accuracy          Accuracy\n");
    fp_out << buf;

    int nSigmaIndex = 0;

    if (m_nNumberCamPosCoefSolved > 0) {
      char strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      std::ostringstream ostr;
      // ??? QString coefficientString = formatPolynomialOutputString(true,
      // ???                                                          m_nNumberCamPosCoefSolved,
      // ???                                                          "X",
      // ???                                                          coefX,
      // ???                                                          m_dGlobalSpacecraftPositionAprioriSigma,
      // ???                                                          nIndex,
      // ???                                                          nSigmaIndex,
      // ???                                                          i);
      // ??? nIndex += m_nNumberCamPosCoefSolved;
      // ??? nSigmaIndex += m_nNumberCamPosCoefSolved;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) { // ack!!! this is already inside another loop with i ???

        if (j == 0) {                                                                             // method??? repeated 6 times...
          ostr << "  " << strcoeff;                                                               // method??? repeated 6 times...
        }                                                                                         // method??? repeated 6 times...
        else if (j == 1) {                                                                        // method??? repeated 6 times...
          ostr << " " << strcoeff << "t";                                                         // method??? repeated 6 times...
        }                                                                                         // method??? repeated 6 times...
        else {                                                                                    // method??? repeated 6 times...
          ostr << strcoeff << "t" << j;                                                           // method??? repeated 6 times...
        }                                                                                         // method??? repeated 6 times...

        if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {                                                                       // this piece of code is repeated 12 times... method???
          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));                                  // this piece of code is repeated 12 times... method???
        }                                                                                         // this piece of code is repeated 12 times... method???
        else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {                               // this piece of code is repeated 12 times... method???
          dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();                           // this piece of code is repeated 12 times... method???
        }                                                                                         // this piece of code is repeated 12 times... method???
        else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {                             // this piece of code is repeated 12 times... method???
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();                         // this piece of code is repeated 12 times... method???
        }

        if (j == 0) {
          sprintf(buf, "  X (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefX[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefX[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefX[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefX[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }

        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
        nSigmaIndex++;
      }
      // ??? coefficientString += formatPolynomialOutputString(true,
      // ???                                                          m_nNumberCamPosCoefSolved,
      // ???                                                          "Y",
      // ???                                               coefY,
      // ???                                               m_dGlobalSpacecraftPositionAprioriSigma,
      // ???                                               nIndex,
      // ???                                               nSigmaIndex,
      // ???                                               i);
      // ??? nIndex += m_nNumberCamPosCoefSolved;
      // ??? nSigmaIndex += m_nNumberCamPosCoefSolved;
      strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }

        if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
          dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
        }

        if (j == 0) {
          sprintf(buf, "  Y (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefY[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefY[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefY[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefY[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }

        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
        nSigmaIndex++;
      }
      // ??? coefficientString += formatPolynomialOutputString(true,
      // ???                                                          m_nNumberCamPosCoefSolved,
      // ???                                                          "Z",
      // ???                                               coefZ,
      // ???                                               m_dGlobalSpacecraftPositionAprioriSigma,
      // ???                                               nIndex,
      // ???                                               nSigmaIndex,
      // ???                                               i);
      // ??? nIndex += m_nNumberCamPosCoefSolved;
      // ??? nSigmaIndex += m_nNumberCamPosCoefSolved;
      strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }

        if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
          dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
        }

        if (j == 0) {
          sprintf(buf, "  Z (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefZ[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefZ[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(), coefZ[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefZ[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], dSigma);
        }

        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
        nSigmaIndex++;
      }
      // ??? fp_out << coefficientString.toAscii().data();
    }
    else {
      sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefX[0], 0.0, coefX[0], 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefY[0], 0.0, coefY[0], 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefZ[0], 0.0, coefZ[0], 0.0, "N/A");
      fp_out << buf;
    }

    if (m_nNumberCamAngleCoefSolved > 0) {
      char strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
      std::ostringstream ostr;
      for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }

        if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
          dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
        }

        if (j == 0) {
          sprintf(buf, " RA (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(),(coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                  m_imageCorrections(nIndex) * RAD2DEG, coefRA[j] * RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(),(coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                  m_imageCorrections(nIndex) * RAD2DEG, coefRA[j] * RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
        }

        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
        nSigmaIndex++;
      }
      strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }

        if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
          dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
        }
        else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
        }

        if (j == 0) {
          sprintf(buf, "DEC (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(),(coefDEC[j] - m_imageCorrections(nIndex))*RAD2DEG,
                  m_imageCorrections(nIndex)*RAD2DEG, coefDEC[j]*RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  ostr.str().c_str(),(coefDEC[j] - m_imageCorrections(nIndex))*RAD2DEG,
                  m_imageCorrections(nIndex)*RAD2DEG, coefDEC[j]*RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
        }

        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
        nSigmaIndex++;
      }

      if (!m_bundleSettings.solveTwist()) {
        sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
      }
      else {
        strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
        for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

          if (j == 0) {
            ostr << "  " << strcoeff;
          }
          else if (j == 1) {
            ostr << " " << strcoeff << "t";
          }
          else {
            ostr << strcoeff << "t" << j;
          }

          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }

          if (j == 0) {
            sprintf(buf, "TWI (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                    ostr.str().c_str(),(coefTWI[j] - m_imageCorrections(nIndex))*RAD2DEG,
                    m_imageCorrections(nIndex)*RAD2DEG, coefTWI[j]*RAD2DEG,
                    m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
          }
          else {
            sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                    ostr.str().c_str(),(coefTWI[j] - m_imageCorrections(nIndex))*RAD2DEG,
                    m_imageCorrections(nIndex)*RAD2DEG, coefTWI[j]*RAD2DEG,
                    m_dGlobalCameraAnglesAprioriSigma[j], dSigma * RAD2DEG);
          }

          fp_out << buf;
          ostr.str("");
          strcoeff--;
          nIndex++;
          nSigmaIndex++;
        }
      }
    }

    else {
      sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
    }
  }

  // output point data
  sprintf(buf, "\n\n\nPOINTS UNCERTAINTY SUMMARY\n==========================\n\n");
  fp_out << buf;
  sprintf(buf, " RMS Sigma Latitude(m)%20.8lf\n", m_bundleStatistics.rmsSigmaLat());
  fp_out << buf;
  sprintf(buf, " MIN Sigma Latitude(m)%20.8lf at %s\n",
          m_bundleStatistics.minSigmaLatitude(),
          m_bundleStatistics.minSigmaLatitudePointId().toAscii().data());
  fp_out << buf;
  sprintf(buf, " MAX Sigma Latitude(m)%20.8lf at %s\n\n",
          m_bundleStatistics.maxSigmaLatitude(),
          m_bundleStatistics.maxSigmaLatitudePointId().toAscii().data());
  fp_out << buf;
  sprintf(buf, "RMS Sigma Longitude(m)%20.8lf\n", m_bundleStatistics.rmsSigmaLon());
  fp_out << buf;
  sprintf(buf, "MIN Sigma Longitude(m)%20.8lf at %s\n",
          m_bundleStatistics.minSigmaLongitude(),
          m_bundleStatistics.minSigmaLongitudePointId().toAscii().data());
  fp_out << buf;
  sprintf(buf, "MAX Sigma Longitude(m)%20.8lf at %s\n\n",
          m_bundleStatistics.maxSigmaLongitude(),
          m_bundleStatistics.maxSigmaLongitudePointId().toAscii().data());
  fp_out << buf;
  if (m_bundleSettings.solveRadius()) {
    sprintf(buf, "   RMS Sigma Radius(m)%20.8lf\n", m_bundleStatistics.rmsSigmaRad());
    fp_out << buf;
    sprintf(buf, "   MIN Sigma Radius(m)%20.8lf at %s\n",
            m_bundleStatistics.minSigmaRadius(),
            m_bundleStatistics.minSigmaRadiusPointId().toAscii().data());
    fp_out << buf;
    sprintf(buf, "   MAX Sigma Radius(m)%20.8lf at %s\n",
            m_bundleStatistics.maxSigmaRadius(),
            m_bundleStatistics.maxSigmaRadiusPointId().toAscii().data());
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

  // output point data
  sprintf(buf, "\n\nPOINTS SUMMARY\n==============\n"
          "%103sSigma          Sigma              Sigma\n"
          "           Label         Status     Rays    RMS"
          "        Latitude       Longitude          Radius"
          "        Latitude       Longitude          Radius\n", "");
  fp_out << buf;

  int nRays = 0;
  double dLat, dLon, dRadius;
  double dSigmaLat, dSigmaLong, dSigmaRadius;
  double cor_lat_dd, cor_lon_dd, cor_rad_m;
  double cor_lat_m, cor_lon_m;
  double dLatInit, dLonInit, dRadiusInit;
  int nGoodRays;
  double dResidualRms;
  QString strStatus;
  int nPointIndex = 0;

  int nPoints = m_pCnet->GetNumPoints();
  for (int i = 0; i < nPoints; i++) {

    const ControlPoint *point = m_pCnet->GetPoint(i);
    if (point->IsIgnored())
    continue;

    nRays = point->GetNumMeasures();
    dResidualRms = point->GetResidualRms();
    dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
    dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
    dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().meters();
    dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
    dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
    dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();
    nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

    if (point->GetType() == ControlPoint::Fixed)
    strStatus = "FIXED";
    else if (point->GetType() == ControlPoint::Constrained)
    strStatus = "CONSTRAINED";
    else if (point->GetType() == ControlPoint::Free)
    strStatus = "FREE";
    else
    strStatus = "UNKNOWN";

    sprintf(buf, "%16s%15s%5d of %d%6.2lf%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf\n",
            point->GetId().toAscii().data(), strStatus.toAscii().data(), nGoodRays, nRays,
            dResidualRms, dLat, dLon, dRadius * 0.001, dSigmaLat, dSigmaLong, dSigmaRadius);
    fp_out << buf;
    nPointIndex++;
  }

  // output point data
  sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
  fp_out << buf;

  nPointIndex = 0;
  for (int i = 0; i < nPoints; i++) {

    const ControlPoint *point = m_pCnet->GetPoint(i);
    if ( point->IsIgnored() )
    continue;

    nRays = point->GetNumMeasures();
    dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
    dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
    dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().meters();
    dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
    dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
    dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();
    nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

    // point corrections and initial sigmas
    bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];
    bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

    cor_lat_dd = corrections[0] * Isis::RAD2DEG;
    cor_lon_dd = corrections[1] * Isis::RAD2DEG;
    cor_rad_m  = corrections[2] * 1000.0;

    cor_lat_m = corrections[0] * radiansToMetersBodyConversion;
    cor_lon_m = corrections[1] * radiansToMetersBodyConversion * cos(dLat*Isis::DEG2RAD);

    dLatInit = dLat - cor_lat_dd;
    dLonInit = dLon - cor_lon_dd;
    dRadiusInit = dRadius - (corrections[2] * 1000.0);

    if (point->GetType() == ControlPoint::Fixed)
    strStatus = "FIXED";
    else if (point->GetType() == ControlPoint::Constrained)
    strStatus = "CONSTRAINED";
    else if (point->GetType() == ControlPoint::Free)
    strStatus = "FREE";
    else
    strStatus = "UNKNOWN";

    sprintf(buf, " Label: %s\nStatus: %s\n  Rays: %d of %d\n",
            point->GetId().toAscii().data(), strStatus.toAscii().data(), nGoodRays, nRays);
    fp_out << buf;

    sprintf(buf, "\n     Point         Initial               Total               "
            "Total              Final             Initial             Final\n"
            "Coordinate          Value             Correction          "
            "Correction            Value             Accuracy          Accuracy\n"
            "                 (dd/dd/km)           (dd/dd/km)           (Meters)"
            "           (dd/dd/km)          (Meters)          (Meters)\n");
    fp_out << buf;

    sprintf(buf, "  LATITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n",
            dLatInit, cor_lat_dd, cor_lat_m, dLat, apriorisigmas[0], dSigmaLat);
    fp_out << buf;

    sprintf(buf, " LONGITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n",
            dLonInit, cor_lon_dd, cor_lon_m, dLon, apriorisigmas[1], dSigmaLong);
    fp_out << buf;

    sprintf(buf, "    RADIUS%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n\n",
            dRadiusInit * 0.001, corrections[2], cor_rad_m, dRadius * 0.001,
            apriorisigmas[2], dSigmaRadius);

    fp_out << buf;
    nPointIndex++;
  }

  fp_out.close();

  return true;
}



/**
 * output bundle results to file with no error propagation
 *
 * @internal
 * @history 2011-05-22 Debbie A. Cook - Added commas to make csv header lines
 *                      consistent for comparer
 * @history 2011-06-05 Debbie A. Cook - Fixed output of spacecraft position
 *                      when it is not part of the bundle
 * @history 2011-07-26 Debbie A. Cook - Omitted output of camera angles for
 *                      radar, which only has spacecraft position
 */
bool BundleAdjust::outputNoErrorPropagation(int numberIterations, 
                                              double radiansToMetersBodyConversion) {

  QString ofname("bundleout.txt");
  if (!m_bundleSettings.outputFilePrefix().isEmpty())
  ofname = m_bundleSettings.outputFilePrefix() + "_" + ofname;

  std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
  if (!fp_out)
  return false;

  char buf[1056];

  //bool bHeld = false;
  std::vector<double> coefX(m_nNumberCamPosCoefSolved);
  std::vector<double> coefY(m_nNumberCamPosCoefSolved);
  std::vector<double> coefZ(m_nNumberCamPosCoefSolved);
  std::vector<double> coefRA(m_nNumberCamAngleCoefSolved);
  std::vector<double> coefDEC(m_nNumberCamAngleCoefSolved);
  std::vector<double> coefTWI(m_nNumberCamAngleCoefSolved);
  std::vector<double> angles;
  Camera *pCamera = NULL;
  SpicePosition *pSpicePosition = NULL;
  SpiceRotation *pSpiceRotation = NULL;
  int nIndex = 0;
  int nImages = images();

  outputHeader(fp_out);

  sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION ***J2000***"
          "\n======================================\n");
  fp_out << buf;

  for (int i = 0; i < nImages; i++) {
    //if (m_bundleStatistics.numberHeldImages() > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))
    //  bHeld = true;

    pCamera = m_pCnet->Camera(i);
    if (!pCamera)
    continue;

    nIndex = imageIndex(i);

    pSpicePosition = pCamera->instrumentPosition();
    if (!pSpicePosition)
    continue;

    pSpiceRotation = pCamera->instrumentRotation();
    if (!pSpiceRotation)
    continue;

    // for frame cameras we directly retrieve the Exterior Pointing (i.e. position
    // and orientation angles). For others (linescan, radar) we retrieve the polynomial
    // coefficients from which the Exterior Pointing paramters are derived.
    // This is incorrect...Correction below
    // For all instruments we retrieve the polynomial coefficients from which the
    // Exterior Pointing parameters are derived.  For framing cameras, a single
    // coefficient for each coordinate is returned.
    if (m_bundleSettings.instrumentPositionSolveOption() > 0)
    pSpicePosition->GetPolynomial(coefX,coefY,coefZ);
    //      else { // frame camera
    else { // This is for m_bundleSettings.instrumentPositionSolveOption() = BundleSettings::None and no polynomial fit has occurred
      std::vector <double> coordinate(3);
      coordinate = pSpicePosition->GetCenterCoordinate();
      coefX.push_back(coordinate[0]);
      coefY.push_back(coordinate[1]);
      coefZ.push_back(coordinate[2]);
    }

    if (m_bundleSettings.instrumentPointingSolveOption() > 0) {
//          angles = pSpiceRotation->Angles(3,1,3);
      pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
    }
    else { // frame camera
      angles = pSpiceRotation->GetCenterAngles();
      coefRA.push_back(angles.at(0));
      coefDEC.push_back(angles.at(1));
      coefTWI.push_back(angles.at(2));
    }

    sprintf(buf, "\nImage Full File Name: %s\n", m_pSnList->FileName(i).toAscii().data());
    fp_out << buf;
    sprintf(buf, "\n Image Serial Number: %s\n", m_pSnList->SerialNumber(i).toAscii().data());
    fp_out << buf;
    sprintf(buf, "\n    Image         Initial              Total               "
            "Final             Initial           Final\n"
            "Parameter         Value              Correction            "
            "Value             Accuracy          Accuracy\n");
    fp_out << buf;

    if (m_nNumberCamPosCoefSolved > 0) {
      char strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      std::ostringstream ostr;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {
        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }
        if (j == 0) {
          sprintf(buf, "  X (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefX[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefX[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefX[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefX[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
      }
      strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {
        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }
        if (j == 0) {
          sprintf(buf, "  Y (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefY[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefY[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefY[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefY[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
      }
      strcoeff = 'a' + m_nNumberCamPosCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {
        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }
        if (j == 0) {
          sprintf(buf, "  Z (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefZ[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefZ[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), coefZ[j] - m_imageCorrections(nIndex),
                  m_imageCorrections(nIndex), coefZ[j],
                  m_dGlobalSpacecraftPositionAprioriSigma[j], "N/A");
        }
        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
      }
    }
    else {
      sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18s%18s\n",
              coefX[0], 0.0, coefX[0], "N/A", "N/A");
      fp_out << buf;
      sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18s%18s\n",
              coefY[0], 0.0, coefY[0], "N/A", "N/A");
      fp_out << buf;
      sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18s%18s\n",
              coefZ[0], 0.0, coefZ[0], "N/A", "N/A");
      fp_out << buf;
    }

    if (m_nNumberCamAngleCoefSolved > 0) {
      char strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
      std::ostringstream ostr;
      for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {
        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }
        if (j == 0) {
          sprintf(buf, " RA (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(),(coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                  m_imageCorrections(nIndex) * RAD2DEG, coefRA[j] * RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(),(coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                  m_imageCorrections(nIndex) * RAD2DEG, coefRA[j] * RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
        }
        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
      }
      strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
      for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {
        if (j == 0) {
          ostr << "  " << strcoeff;
        }
        else if (j == 1) {
          ostr << " " << strcoeff << "t";
        }
        else {
          ostr << strcoeff << "t" << j;
        }
        if (j == 0) {
          sprintf(buf, "DEC (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(), (coefDEC[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                  m_imageCorrections(nIndex) * RAD2DEG, coefDEC[j] * RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
        }
        else {
          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  ostr.str().c_str(),(coefDEC[j] - m_imageCorrections(nIndex))*RAD2DEG,
                  m_imageCorrections(nIndex)*RAD2DEG, coefDEC[j]*RAD2DEG,
                  m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
        }
        fp_out << buf;
        ostr.str("");
        strcoeff--;
        nIndex++;
      }
      if (!m_bundleSettings.solveTwist()) {
        sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
      }
      else {
        strcoeff = 'a' + m_nNumberCamAngleCoefSolved - 1;
        for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {
          if (j == 0) {
            ostr << "  " << strcoeff;
          }
          else if (j == 1) {
            ostr << " " << strcoeff << "t";
          }
          else {
            ostr << strcoeff << "t" << j;
          }
          if (j == 0) {
            sprintf(buf, "TWI (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                    ostr.str().c_str(), (coefTWI[j] - m_imageCorrections(nIndex)) * RAD2DEG,
                    m_imageCorrections(nIndex) * RAD2DEG, coefTWI[j] * RAD2DEG,
                    m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
          }
          else {
            sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                    ostr.str().c_str(),(coefTWI[j] - m_imageCorrections(nIndex))*RAD2DEG,
                    m_imageCorrections(nIndex)*RAD2DEG, coefTWI[j]*RAD2DEG,
                    m_dGlobalCameraAnglesAprioriSigma[j], "N/A");
          }
          fp_out << buf;
          ostr.str("");
          strcoeff--;
          nIndex++;
        }
      }
    }
    else {
      sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
      sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
              coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
      fp_out << buf;
    }
  }

  fp_out << "\n\n\n";

  // output point data
  sprintf(buf, "\nPOINTS SUMMARY\n==============\n%99sSigma           Sigma           Sigma\n"
          "           Label      Status     Rays   RMS"
          "        Latitude       Longitude          Radius"
          "        Latitude        Longitude       Radius\n", "");
  fp_out << buf;

  int nRays = 0;
  double dResidualRms;
  double dLat,dLon,dRadius;
  double cor_lat_dd,cor_lon_dd,cor_rad_m;
  double cor_lat_m,cor_lon_m;
  double dLatInit,dLonInit,dRadiusInit;
  int nGoodRays;

  QString strStatus;

  int nPoints = m_pCnet->GetNumPoints();
  for (int i = 0; i < nPoints; i++) {

    const ControlPoint* point = m_pCnet->GetPoint(i);
    if (point->IsIgnored())
    continue;

    nRays = point->GetNumMeasures();
    dResidualRms = point->GetResidualRms();
    dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
    dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
    dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().meters();
    nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

    if (point->GetType() == ControlPoint::Fixed)
    strStatus = "FIXED";
    else if (point->GetType() == ControlPoint::Constrained)
    strStatus = "CONSTRAINED";
    else if (point->GetType() == ControlPoint::Free)
    strStatus = "FREE";
    else
    strStatus = "UNKNOWN";

    sprintf(buf, "%16s%12s%4d of %d%6.2lf%16.8lf%16.8lf%16.8lf%11s%16s%16s\n",
            point->GetId().toAscii().data(), strStatus.toAscii().data(), nGoodRays, nRays,
            dResidualRms, dLat, dLon, dRadius * 0.001, "N/A", "N/A", "N/A");

    fp_out << buf;
  }

  sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
  fp_out << buf;

  int nPointIndex = 0;
  for (int i = 0; i < nPoints; i++) {

    const ControlPoint* point = m_pCnet->GetPoint(i);
    if (point->IsIgnored())
    continue;

    nRays = point->GetNumMeasures();
    dLat = point->GetAdjustedSurfacePoint().GetLatitude().degrees();
    dLon = point->GetAdjustedSurfacePoint().GetLongitude().degrees();
    dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().meters();
    nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

    // point corrections and initial sigmas
    bounded_vector<double,3>& corrections = m_Point_Corrections[nPointIndex];
    bounded_vector<double,3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

    cor_lat_dd = corrections[0]*Isis::RAD2DEG;
    cor_lon_dd = corrections[1]*Isis::RAD2DEG;
    cor_rad_m  = corrections[2]*1000.0;

    cor_lat_m = corrections[0]*radiansToMetersBodyConversion;
    cor_lon_m = corrections[1]*radiansToMetersBodyConversion*cos(dLat*Isis::DEG2RAD);

    dLatInit = dLat-cor_lat_dd;
    dLonInit = dLon-cor_lon_dd;
    dRadiusInit = dRadius-(corrections[2]*1000.0);

    if (point->GetType() == ControlPoint::Fixed)
    strStatus = "FIXED";
    else if (point->GetType() == ControlPoint::Constrained)
    strStatus = "CONSTRAINED";
    else if (point->GetType() == ControlPoint::Free)
    strStatus = "FREE";
    else
    strStatus = "UNKNOWN";

    sprintf(buf, " Label: %s\nStatus: %s\n  Rays: %d of %d\n",
            point->GetId().toAscii().data(),strStatus.toAscii().data(),nGoodRays,nRays);
    fp_out << buf;

    sprintf(buf, "\n     Point         Initial               Total               "
            "Total              Final             Initial             Final\n"
            "Coordinate          Value             Correction          "
            "Correction            Value             Accuracy          Accuracy\n"
            "                 (dd/dd/km)           (dd/dd/km)           (Meters)"
            "           (dd/dd/km)          (Meters)          (Meters)\n");
    fp_out << buf;

    sprintf(buf, "  LATITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n",
            dLatInit, cor_lat_dd, cor_lat_m, dLat, apriorisigmas[0], "N/A");
    fp_out << buf;

    sprintf(buf, " LONGITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n",
            dLonInit, cor_lon_dd, cor_lon_m, dLon, apriorisigmas[1], "N/A");
    fp_out << buf;

    sprintf(buf, "    RADIUS%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n\n",
            dRadiusInit*0.001, corrections[2], cor_rad_m, dRadius*0.001, apriorisigmas[2], "N/A");

    fp_out << buf;

    nPointIndex++;
  }

  fp_out.close();

  return true;
}


/**
 * output point data to csv file
 */
bool BundleAdjust::outputPointsCSV(double radiansToMetersBodyConversion) {
  char buf[1056];

  QString ofname("bundleout_points.csv");
  if (!m_bundleSettings.outputFilePrefix().isEmpty())
  ofname = m_bundleSettings.outputFilePrefix() + "_" + ofname;

  std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
  if (!fp_out)
  return false;

  int nPoints = m_pCnet->GetNumPoints();

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
  if (m_bundleSettings.errorPropagation()) {
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

  int nPointIndex = 0;
  for (int i = 0; i < nPoints; i++) {
    const ControlPoint *point = m_pCnet->GetPoint(i);

    if (!point) {
      continue;
    }

    if (point->IsIgnored() || point->IsRejected()) {
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
    bounded_vector<double,3>& corrections = m_Point_Corrections[nPointIndex];
    cor_lat_m = corrections[0]*radiansToMetersBodyConversion;
    cor_lon_m = corrections[1]*radiansToMetersBodyConversion*cos(dLat*Isis::DEG2RAD);
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

    if (m_bundleSettings.errorPropagation()) {
      dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
      dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
      dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();

      sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
              "%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
              point->GetId().toAscii().data(), strStatus.toAscii().data(), nMeasures,
              nRejectedMeasures, dResidualRms, dLat, dLon, dRadius, dSigmaLat, dSigmaLong,
              dSigmaRadius, cor_lat_m, cor_lon_m, cor_rad_m, dX, dY, dZ);
    }
    else
    sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,"
            "%16.8lf,%16.8lf\n",
            point->GetId().toAscii().data(), strStatus.toAscii().data(), nMeasures,
            nRejectedMeasures, dResidualRms, dLat, dLon, dRadius, cor_lat_m, cor_lon_m,
            cor_rad_m, dX, dY, dZ);

    fp_out << buf;

    nPointIndex++;
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

  QString ofname("residuals.csv");
  if (!m_bundleSettings.outputFilePrefix().isEmpty())
  ofname = m_bundleSettings.outputFilePrefix() + "_" + ofname;

  std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
  if (!fp_out)
  return false;

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
    if (point->IsIgnored())
    continue;

    int nObservations = point->GetNumMeasures();
    for (int j = 0; j < nObservations; j++) {
      const ControlMeasure *measure = point->GetMeasure(j);
      if (measure->IsIgnored())
      continue;

      Camera *pCamera = measure->Camera();
      if (!pCamera)
      continue;

      // Determine the image index
      nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());

      if (measure->IsRejected())
      sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,*\n",
              point->GetId().toAscii().data(),
              m_pSnList->FileName(nImageIndex).toAscii().data(),
              m_pSnList->SerialNumber(nImageIndex).toAscii().data(),
              measure->GetFocalPlaneMeasuredX(),
              measure->GetFocalPlaneMeasuredY(),
              measure->GetSample(),
              measure->GetLine(),
              measure->GetSampleResidual(),
              measure->GetLineResidual(),
              measure->GetResidualMagnitude());
      else
      sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
              point->GetId().toAscii().data(),
              m_pSnList->FileName(nImageIndex).toAscii().data(),
              m_pSnList->SerialNumber(nImageIndex).toAscii().data(),
              measure->GetFocalPlaneMeasuredX(),
              measure->GetFocalPlaneMeasuredY(),
              measure->GetSample(),
              measure->GetLine(),
              measure->GetSampleResidual(),
              measure->GetLineResidual(),
              measure->GetResidualMagnitude());
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
  char buf[1056];

  QString ofname("bundleout_images.csv");
  if (!m_bundleSettings.outputFilePrefix().isEmpty())
  ofname = m_bundleSettings.outputFilePrefix() + "_" + ofname;

  std::ofstream fp_out(ofname.toAscii().data(), std::ios::out);
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
    if (!m_bundleSettings.solveTwist()) {
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
      if (m_nNumberCamAngleCoefSolved == 1 || !m_bundleSettings.solveTwist()) {
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
    if (!m_bundleSettings.solveTwist())
    break;
  }

  // print first column header to buffer and output to file
  int ncolumns = output_columns.size();
  for (int i = 0; i < ncolumns; i++) {
    QString str = output_columns.at(i);
    sprintf(buf, "%s", (const char*)str.toAscii().data());
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
  if (m_bundleSettings.solveTwist()) numCameraAnglesSolved++;
  nparams += numCameraAnglesSolved*m_nNumberCamAngleCoefSolved;
  if (!m_bundleSettings.solveTwist()) nparams += 1; // Report on twist only
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
    sprintf(buf, "%s", (const char*)str.toAscii().data());
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

  gmm::row_matrix<gmm::rsvector<double> > lsqCovMatrix;
  if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse
      && m_bundleSettings.errorPropagation()) {
    // Get reference to the covariance matrix from the least-squares object
    lsqCovMatrix = m_pLsq->GetCovarianceMatrix();
  }

  // data structure to contain adjusted image parameter sigmas for CHOLMOD error propagation only
  vector<double> vImageAdjustedSigmas;

  std::vector<double> BFP(3);

  for (int i = 0; i < nImages; i++) {

    //if (m_bundleStatistics.numberHeldImages() > 0 &&
    //    m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) )
    //  bHeld = true;

    pCamera = m_pCnet->Camera(i);
    if (!pCamera)
    continue;

    // imageIndex(i) retrieves index into the normal equations matrix for
    //  Image(i)
    nIndex = imageIndex(i);

    if (m_bundleSettings.errorPropagation()
        && m_bundleStatistics.converged()
        && m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
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
    if (m_bundleSettings.instrumentPositionSolveOption() > 0) {
      pSpicePosition->GetPolynomial(coefX, coefY, coefZ);
    }
    else { // not solving for position so report state at center of image
      std::vector <double> coordinate(3);
      coordinate = pSpicePosition->GetCenterCoordinate();

      coefX.push_back(coordinate[0]);
      coefY.push_back(coordinate[1]);
      coefZ.push_back(coordinate[2]);
    }

    if (m_bundleSettings.instrumentPointingSolveOption() > 0)
    pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
//          else { // frame camera
    else if (pCamera->GetCameraType() != 3) {
// This is for m_bundleSettings.instrumentPointingSolveOption() = BundleSettings::NoPointingFactors (except Radar which
// has no pointing) and no polynomial fit has occurred
      angles = pSpiceRotation->GetCenterAngles();
      coefRA.push_back(angles.at(0));
      coefDEC.push_back(angles.at(1));
      coefTWI.push_back(angles.at(2));
    }

    // clear column vector
    output_columns.clear();

    // add filename
    output_columns.push_back(m_pSnList->FileName(i).toAscii().data());

    // add rms of sample, line, total image coordinate residuals
    output_columns.push_back(
                             toString(m_bundleStatistics.rmsImageSampleResiduals()[i].Rms()));
    output_columns.push_back(
                             toString(m_bundleStatistics.rmsImageLineResiduals()[i].Rms()));
    output_columns.push_back(
                             toString(m_bundleStatistics.rmsImageResiduals()[i].Rms()));

    int nSigmaIndex = 0;
    if (m_nNumberCamPosCoefSolved > 0) {
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {

          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }
        }

        output_columns.push_back(toString(coefX[0] - m_imageCorrections(nIndex)));
        output_columns.push_back(toString(m_imageCorrections(nIndex)));
        output_columns.push_back(toString(coefX[j]));
        output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          output_columns.push_back(toString(dSigma));
        }
        else {
          output_columns.push_back("N/A");
        }
        nIndex++;
        nSigmaIndex++;
      }
      for (int j = 0; j < m_nNumberCamPosCoefSolved; j++) {

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }
        }

        output_columns.push_back(toString(coefY[0] - m_imageCorrections(nIndex)));
        output_columns.push_back(toString(m_imageCorrections(nIndex)));
        output_columns.push_back(toString(coefY[j]));
        output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
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

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }
        }

        output_columns.push_back(toString(coefZ[0] - m_imageCorrections(nIndex)));
        output_columns.push_back(toString(m_imageCorrections(nIndex)));
        output_columns.push_back(toString(coefZ[j]));
        output_columns.push_back(toString(m_dGlobalSpacecraftPositionAprioriSigma[j]));

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
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

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }
        }

        output_columns.push_back(toString((coefRA[j] - m_imageCorrections(nIndex)) * RAD2DEG));
        output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
        output_columns.push_back(toString(coefRA[j] * RAD2DEG));
        output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          output_columns.push_back(toString(dSigma * RAD2DEG));
        }
        else {
          output_columns.push_back("N/A");
        }
        nIndex++;
        nSigmaIndex++;
      }
      for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
            dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
            dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
          }
          else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
            dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
          }
        }

        output_columns.push_back(toString((coefDEC[j] - m_imageCorrections(nIndex)) * RAD2DEG));
        output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
        output_columns.push_back(toString(coefDEC[j] * RAD2DEG));
        output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

        if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
          output_columns.push_back(toString(dSigma * RAD2DEG));
        }
        else {
          output_columns.push_back("N/A");
        }
        nIndex++;
        nSigmaIndex++;
      }
      if (!m_bundleSettings.solveTwist()) {
        output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back(toString(coefTWI[0]*RAD2DEG));
        output_columns.push_back(toString(0.0));
        output_columns.push_back("N/A");
      }
      else {
        for (int j = 0; j < m_nNumberCamAngleCoefSolved; j++) {

          if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
            if (m_bundleSettings.solveMethod() == BundleSettings::OldSparse) {
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            }
            else if (m_bundleSettings.solveMethod() == BundleSettings::Sparse) {
              dSigma = sqrt(vImageAdjustedSigmas[nSigmaIndex]) * m_bundleStatistics.sigma0();
            }
            else if (m_bundleSettings.solveMethod() == BundleSettings::SpecialK) {
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_bundleStatistics.sigma0();
            }
          }

          output_columns.push_back(toString((coefTWI[j] - m_imageCorrections(nIndex)) * RAD2DEG));
          output_columns.push_back(toString(m_imageCorrections(nIndex) * RAD2DEG));
          output_columns.push_back(toString(coefTWI[j] * RAD2DEG));
          output_columns.push_back(toString(m_dGlobalCameraAnglesAprioriSigma[j]));

          if (m_bundleSettings.errorPropagation() && m_bundleStatistics.converged()) {
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
        sprintf(buf, "%s,", (const char*)str.toAscii().data());
      }
      else {
        sprintf(buf, "%s", (const char*)str.toAscii().data());
      }
      fp_out << buf;
    }
    sprintf(buf, "\n");
    fp_out << buf;
  }

  fp_out.close();

  return true;
}

#endif
