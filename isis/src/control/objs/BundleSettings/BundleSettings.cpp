#include "BundleSettings.h"

#include <QDataStream>
#include <QDebug>
//#include <QFile> currently only used in commented code
#include <QList>
#include <QString>
#include <QtGlobal> // qMax()
#include <QUuid>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include "BundleObservationSolveSettings.h"
//#include "FileName.h"currently only used in commented code
#include "IException.h"
#include "IString.h"
#include "Project.h" // currently used for xml handler
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  /**
   * @brief Constructs a BundleSettings object.
   * Default values are set for all member variables. By default, BundleSettings allows creation
   * of the inverse correlation matrix file.
   *
   * @see createInverseMatrix()
   * @see setCreateInverseMatrix()
   */
  BundleSettings::BundleSettings() {
    init();
    BundleObservationSolveSettings defaultSolveSettings;
    m_observationSolveSettings.append(defaultSolveSettings);
  }

  /**
   * @brief Set Default vales for a BundleSettings object.
   * Note we call the default constructor to initialize the TargetBody information
   * that is not currently in the XML.
   */
  void BundleSettings::init() {
    m_validateNetwork = true;

    m_solveObservationMode = false;
    m_solveRadius          = false;
    m_updateCubeLabel      = false;
    m_errorPropagation     = false;
    m_createInverseMatrix  = false;

    m_outlierRejection     = false;
    m_outlierRejectionMultiplier = 1.0;

    // Parameter Uncertainties (Weighting)
    m_globalLatitudeAprioriSigma  = Isis::Null;
    m_globalLongitudeAprioriSigma = Isis::Null;
    m_globalRadiusAprioriSigma    = Isis::Null;

    // Convergence Criteria
    m_convergenceCriteria = BundleSettings::Sigma0;
    m_convergenceCriteriaThreshold = 1.0e-10;
    m_convergenceCriteriaMaximumIterations = 50;

    // Maximum Likelihood Estimation Options no default in the constructor - must be set.
    m_maximumLikelihood.clear();

    // Self Calibration ??? (from ipce only)

    // Target Body
    m_solveTargetBody = false;
//    m_solveTargetBodyPolePosition = false;
//    m_solveTargetBodyZeroMeridian = false;
//    m_solveTargetBodyRotationRate = false;
//    m_solveTargetBodyRadiusMethod = None;

    // Output Options
    m_outputFilePrefix = "";
  }


  /**
   * Construct a BundleSettings object from member data read from an XML file.
   *
   * @code
   *   FileName xmlFile("bundleSettingsFileName.xml");
   *
   *   QString xmlPath = xmlFile.expanded();
   *   QFile file(xmlPath);
   *   file.open(QFile::ReadOnly);
   *   XmlStackedHandlerReader reader;
   *   BundleSettings settings(project, reader);
   * @endcode
   *
   * @param project A pointer to the project where the Settings will be saved.
   * @param xmlReader The Content handler to parse the BundleSettings XML
   */
  BundleSettings::BundleSettings(Project *project,
                                 XmlStackedHandlerReader *xmlReader) {
    init();
    xmlReader->setErrorHandler(new XmlHandler(this, project));
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * This copy constructor sets this BundleSettings' member data to match
   * that of the 'other' given BundleSettings.
   *
   * @param other The BundleSettings object to be copied.
   */
  BundleSettings::BundleSettings(const BundleSettings &other)
      : m_validateNetwork(other.m_validateNetwork),
        m_solveObservationMode(other.m_solveObservationMode),
        m_solveRadius(other.m_solveRadius),
        m_updateCubeLabel(other.m_updateCubeLabel),
        m_errorPropagation(other.m_errorPropagation),
        m_createInverseMatrix(other.m_createInverseMatrix),
        m_outlierRejection(other.m_outlierRejection),
        m_outlierRejectionMultiplier(other.m_outlierRejectionMultiplier),
        m_globalLatitudeAprioriSigma(other.m_globalLatitudeAprioriSigma),
        m_globalLongitudeAprioriSigma(other.m_globalLongitudeAprioriSigma),
        m_globalRadiusAprioriSigma(other.m_globalRadiusAprioriSigma),
        m_observationSolveSettings(other.m_observationSolveSettings),
        m_convergenceCriteria(other.m_convergenceCriteria),
        m_convergenceCriteriaThreshold(other.m_convergenceCriteriaThreshold),
        m_convergenceCriteriaMaximumIterations(other.m_convergenceCriteriaMaximumIterations),
        m_maximumLikelihood(other.m_maximumLikelihood),
        m_solveTargetBody(other.m_solveTargetBody),
        m_bundleTargetBody(other.m_bundleTargetBody),
        m_outputFilePrefix(other.m_outputFilePrefix){
  }


  /**
   * Destroys the BundleSettings object.
   */
  BundleSettings::~BundleSettings() {
  }


  /**
   * Assignment operator to allow proper copying of the 'other' BundleSettings
   * object to this one.
   *
   * @param other The BundleSettings object to be copied.
   *
   * @return @b BundleSettings& A reference to the copied BundleSettings object.
   */
  BundleSettings &BundleSettings::operator=(const BundleSettings &other) {
    if (&other != this) {
      m_validateNetwork = other.m_validateNetwork;
      m_solveObservationMode = other.m_solveObservationMode;
      m_solveRadius = other.m_solveRadius;
      m_updateCubeLabel = other.m_updateCubeLabel;
      m_errorPropagation = other.m_errorPropagation;
      m_createInverseMatrix = other.m_createInverseMatrix;
      m_outlierRejection = other.m_outlierRejection;
      m_outlierRejectionMultiplier = other.m_outlierRejectionMultiplier;
      m_globalLatitudeAprioriSigma = other.m_globalLatitudeAprioriSigma;
      m_globalLongitudeAprioriSigma = other.m_globalLongitudeAprioriSigma;
      m_globalRadiusAprioriSigma = other.m_globalRadiusAprioriSigma;
      m_observationSolveSettings = other.m_observationSolveSettings;
      m_convergenceCriteria = other.m_convergenceCriteria;
      m_convergenceCriteriaThreshold = other.m_convergenceCriteriaThreshold;
      m_convergenceCriteriaMaximumIterations = other.m_convergenceCriteriaMaximumIterations;
      m_solveTargetBody = other.m_solveTargetBody;
      m_bundleTargetBody = other.m_bundleTargetBody;
      m_maximumLikelihood = other.m_maximumLikelihood;
      m_outputFilePrefix = other.m_outputFilePrefix;
    }
    return *this;
  }


  /**
   * Sets the internal flag to indicate whether to validate the network before
   * the bundle adjustment.
   *
   * @see BundleAdjust::validateNetwork()
   *
   * @param validate Indicates whether the network should be validated by
   *                 BundleAdjust.
   *
   */
  void BundleSettings::setValidateNetwork(bool validate) {
    m_validateNetwork = validate;
  }


  /**
   * This method is used to determine whether to validate the network before
   * the bundle adjustment.
   *
   * @see BundleAdjust::validateNetwork()
   *
   * @return @b bool Indicates whether the network should be validated by
   *                 BundleAdjust.
   *
   */
  bool BundleSettings::validateNetwork() const {
    return m_validateNetwork;
  }


  // =============================================================================================//
  // ======================== Solve Options ======================================================//
  // =============================================================================================//

  /**
   * Set the solve options for the bundle adjustment.
   *
   * @param solveObservationMode A boolean value indicating whether to solve for
   *                             observation mode.
   * @param updateCubeLabel A boolean value indicating whether to update the
   *                        cube labels after the bundle adjustment is
   *                        completed.
   * @param errorPropagation A boolean value indicating whether to use the
   *                         cholmod library's error propagation.
   * @param solveRadius A boolean value indicating whether to solve for radius.
   * @param globalLatitudeAprioriSigma The global a priori sigma for latitude.
   * @param globalLongitudeAprioriSigma The global a priori sigma for longitude.
   * @param globalRadiusAprioriSigma The global a priori sigma for radius.
   */
  void BundleSettings::setSolveOptions(bool solveObservationMode,
                                       bool updateCubeLabel,
                                       bool errorPropagation,
                                       bool solveRadius,
                                       double globalLatitudeAprioriSigma,
                                       double globalLongitudeAprioriSigma,
                                       double globalRadiusAprioriSigma) {
    m_solveObservationMode = solveObservationMode;
    m_solveRadius = solveRadius;
    m_updateCubeLabel = updateCubeLabel;
    m_errorPropagation = errorPropagation;

    if (globalLatitudeAprioriSigma > 0.0) { // otherwise, we leave as default Isis::Null
      m_globalLatitudeAprioriSigma = globalLatitudeAprioriSigma;
    }
    else {
      m_globalLatitudeAprioriSigma = Isis::Null;
    }

    if (globalLongitudeAprioriSigma > 0.0) {
      m_globalLongitudeAprioriSigma = globalLongitudeAprioriSigma;
    }
    else {
      m_globalLongitudeAprioriSigma = Isis::Null;
    }

    if (m_solveRadius && globalRadiusAprioriSigma > 0.0) {
      m_globalRadiusAprioriSigma = globalRadiusAprioriSigma;
    }
    else {
      m_globalRadiusAprioriSigma = Isis::Null;
    }
  }


  /**
   * Set the outlier rejection options for the bundle adjustment.
   *
   * @param outlierRejection Indicates whether to perform automatic outlier
   *                         rejection during the bundle adjustment.
   * @param mutliplier The outlier rejection multiplier.
   */
  void BundleSettings::setOutlierRejection(bool outlierRejection, double multiplier) {
    m_outlierRejection = outlierRejection;
    if (m_outlierRejection) {
      m_outlierRejectionMultiplier = multiplier;
    }
    else {
      m_outlierRejectionMultiplier = 1.0;
    }
  }


  /**
   * Add the list of solve options for each observation.
   *
   * @param observationSolveSettings A list of BundleObservationSolveSettings objects
   *                                 to indicate the settings for each observation of
   *                                 the bundle adjustment.
   */
  void BundleSettings::setObservationSolveOptions(
      QList<BundleObservationSolveSettings> obsSolveSettingsList) {
    m_observationSolveSettings = obsSolveSettingsList;
  }


  /**
   * Indicates if the settings will allow the inverse correlation matrix to be created.
   *
   * This method is used to determine if the inverse correlation matrix file will be created when
   * creating error propagation information in the bundle adjust. If error propagation is not
   * turned on, then the inverse correlation matrix file will not be created.
   *
   * @return @b bool Returns whether or now the inverse correlation matrix is allowed to be created.
   *
   * @see BundleAdjust::errorPropagation()
   */
  bool BundleSettings::createInverseMatrix() const {
    return (m_errorPropagation && m_createInverseMatrix);
  }


  /**
   * This method is used to determine whether outlier rejection will be
   * performed on this bundle adjustment.
   *
   * @return @b bool Indicates whether to perform automatic outlier
   *                 rejection during the bundle adjustment.
   */
  bool BundleSettings::outlierRejection() const {
    return m_outlierRejection;
  }


  /**
   * This method is used to determine whether this bundle adjustment will solve
   * for observation mode.
   *
   * @return @b bool Indicates whether to solve for observation mode.
   */
  bool BundleSettings::solveObservationMode() const {
    return m_solveObservationMode;
  }


  /**
   * This method is used to determine whether this bundle adjustment will solve
   * for radius.
   *
   * @return @b bool Indicates whether to solve for radius.
   */
  bool BundleSettings::solveRadius() const {
    return m_solveRadius;
  }


  /**
   * This method is used to determine whether this bundle
   * adjustment will update the cube labels.
   *
   * @return @b bool Indicates whether to update the cube labels after the bundle adjustment
   *                 is completed.
   */
  bool BundleSettings::updateCubeLabel() const {
    return m_updateCubeLabel;
  }


  /**
   * This method is used to determine whether this bundle adjustment will
   * perform error propagation.
   *
   * @return @b bool Indicates whether to perform error propagation.
   */
  bool BundleSettings::errorPropagation() const {
    return m_errorPropagation;
  }


  /**
   * Turn the creation of the inverse correlation matrix file on or off.
   *
   * Note that the inverse correlation matrix is created in BundleAdjust, and will only be created
   * if error propagation is turned on. By default, BundleSettings allows the inverse matrix to
   * be created. This requires stand-alone applications (e.g. jigsaw) to call this method
   * to turn of the correlation matrix creation.
   *
   * @param createMatrixFile Boolean indicating whether or not to allow the inverse matrix file to
   *                         be created.
   *
   * @see BundleAdjust::errorPropagation()
   */
  void BundleSettings::setCreateInverseMatrix(bool createMatrixFile) {
    m_createInverseMatrix = createMatrixFile;
  }


  /**
   * Retrieves the outlier rejection multiplier for the bundle adjustment.
   *
   * @return @b double The outlier rejection multiplier.
   */
  double BundleSettings::outlierRejectionMultiplier() const {
    return m_outlierRejectionMultiplier;
  }


  /**
   * Retrieves the global a priori sigma latitude value for this bundle
   * adjustment.
   *
   * @return @b double The global a priori sigma for latitude.
   */
  double BundleSettings::globalLatitudeAprioriSigma() const {
    return m_globalLatitudeAprioriSigma;
  }


  /**
   * Retrieves the global a priori sigma longitude value for this bundle
   * adjustment.
   *
   * @return @b double The global a priori sigma for longitude.
   */
  double BundleSettings::globalLongitudeAprioriSigma() const {
    return m_globalLongitudeAprioriSigma;
  }


  /**
   * Retrieves the global a priori sigma radius value for this bundle
   * adjustment.
   *
   * @return @b double The global a priori sigma for radius.
   */
  double BundleSettings::globalRadiusAprioriSigma() const {
    return m_globalRadiusAprioriSigma;
  }


  /**
   * Retrieves the number of observation solve settings.
   *
   * @return @b int The number of solve settings object for this run of the
   *                bundle adjustment.
   */
  int BundleSettings::numberSolveSettings() const {
     return m_observationSolveSettings.size();
  }


  /**
   * Retrieves solve settings for the observation corresponding to the given
   * observation number.
   *
   * @param observationNumber The observation number associated with the
   *                          BundleObservationSolveSettings object to be accessed.
   *
   * @return @b BundleObservationSolveSettings The observation settings object that contains
   *                                           the observation number passed.
   *
   * @throw IException::Unknown "Unable to find BundleObservationSolveSettings
   *                             for given observation number"
   */
  BundleObservationSolveSettings
      BundleSettings::observationSolveSettings(QString observationNumber) const {

    for (int i = 0; i < numberSolveSettings(); i++) {
      if (m_observationSolveSettings[i].observationNumbers().contains(observationNumber)) {
        return m_observationSolveSettings[i];
      }
    }
    QString msg = "Unable to find BundleObservationSolveSettings for observation number ["
                  + observationNumber + "].";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * Retrieves solve settings for the observation corresponding to the given
   * index.
   *
   * @param n The index of the BundleObservationSolveSettings object to be
   *          accessed.
   * @return @b BundleObservationSolveSettings The observation settings object corresponding
   *                                           to the given index.
   * @throw IException::Unknown "Unable to find BundleObservationSolveSettings
   *                             with given index"
   */
  BundleObservationSolveSettings
      BundleSettings::observationSolveSettings(int n) const {

    if (n >= 0 && n < numberSolveSettings()) {
      return m_observationSolveSettings[n];
    }
    QString msg = "Unable to find BundleObservationSolveSettings with index = ["
                  + toString(n) + "].";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  // =============================================================================================//
  // ======================== Convergence Criteria ===============================================//
  // =============================================================================================//

  /**
   * Converts the given string value to a BundleSettings::ConvergenceCriteria
   * enumeration. Currently accepted inputs are listed below. This method is
   * case insensitive.
   * <ul>
   *   <li>Sigma0</li>
   *   <li>ParameterCorrections</li>
   * </ul>
   *
   * @param criteria Convergence criteria name to be converted.
   *
   * @return @b ConvergenceCriteria The enumeration corresponding to the given name.
   *
   * @throw Isis::Exception::Programmer "Unknown bundle convergence criteria."
   */
  BundleSettings::ConvergenceCriteria
      BundleSettings::stringToConvergenceCriteria(QString criteria) {
    if (criteria.compare("SIGMA0", Qt::CaseInsensitive) == 0) {
      return BundleSettings::Sigma0;
    }
    else if (criteria.compare("PARAMETERCORRECTIONS", Qt::CaseInsensitive) == 0) {
      return BundleSettings::ParameterCorrections;
    }
    else throw IException(IException::Programmer,
                          "Unknown bundle convergence criteria [" + criteria + "].",
                          _FILEINFO_);
  }


  /**
   * Converts the given BundleSettings::ConvergenceCriteria enumeration to a string.
   * This method is used to print the type of convergence criteria used in
   * the bundle adjustment.
   *
   * @param criteria The ConvergenceCriteria enumeration to be converted.
   *
   * @return @b QString The name associated with the given convergence criteria.
   *
   * @throw Isis::Exception::Programmer "Unknown bundle convergence criteria enum."
   */
  QString BundleSettings::convergenceCriteriaToString(
              BundleSettings::ConvergenceCriteria criteria) {
    if (criteria == Sigma0)                    return "Sigma0";
    else if (criteria == ParameterCorrections) return "ParameterCorrections";
    else  throw IException(IException::Programmer,
                           "Unknown convergence criteria enum [" + toString(criteria) + "].",
                           _FILEINFO_);
  }


  /**
   * Set the convergence criteria options for the bundle adjustment.
   *
   * @param criteria An enumeration for the convergence criteria to be
   *                 used for this bundle adjustment.
   * @param threshold The convergence threshold for this bundle adjustment.
   * @param maximumIterations The maximum number of iterations allowed
   *                          before the bundle adjustment determines
   *                          that the data is not converging.
   */
  void BundleSettings::setConvergenceCriteria(BundleSettings::ConvergenceCriteria criteria,
                                              double threshold,
                                              int maximumIterations) {
    m_convergenceCriteria = criteria;
    m_convergenceCriteriaThreshold = threshold;
    m_convergenceCriteriaMaximumIterations = maximumIterations;
  }


  /**
   * Retrieves the convergence criteria to be used to solve the bundle
   * adjustment.
   *
   * @return @b ConvergenceCriteria The enumeration of the convergence criteria.
   */
  BundleSettings::ConvergenceCriteria BundleSettings::convergenceCriteria() const {
    return m_convergenceCriteria;
  }


  /**
   * Retrieves the convergence threshold to be used to solve the bundle
   * adjustment.
   *
   * @return @b double The threshold that determines convergence.
   */
  double BundleSettings::convergenceCriteriaThreshold() const {
    return m_convergenceCriteriaThreshold;
  }


  /**
   * Retrieves the maximum number of iterations allowed to solve the
   * bundle adjustment.
   *
   * @param maximumIterations The maximum number of iterations allowed
   *                          before the bundle adjustment determines
   *                          that the data is not converging.
   */
  int BundleSettings::convergenceCriteriaMaximumIterations() const {
    return m_convergenceCriteriaMaximumIterations;
  }



  // =============================================================================================//
  // ======================== Parameter Uncertainties (Weighting) ================================//
  // =============================================================================================//
//   void BundleSettings::setGlobalLatitudeAprioriSigma(double sigma) {
//     m_globalLatitudeAprioriSigma = sigma;
//   }
//
//
//
//   void BundleSettings::setGlobalLongitudeAprioriSigma(double sigma) {
//     m_globalLongitudeAprioriSigma = sigma;
//   }
//
//
//
//   void BundleSettings::setGlobalRadiiAprioriSigma(double sigma) {
//     m_globalRadiusAprioriSigma = sigma;
//   }


  // =============================================================================================//
  // ======================== Maximum Likelihood Estimation Options ==============================//
  // =============================================================================================//


  /**
   * Add a maximum likelihood estimator (MLE) model to the bundle adjustment.
   *
   * @param model The enumeration for the model to be used.
   * @param maxModelCQuantile The C-Quantile of the residual to be used to
   *                          compute the tweaking constant.
   *
   *
   * @throw Isis::Exception::Programmer "For bundle adjustments with multiple maximum
   *                                     likelihood estimators, the first model must be of
   *                                     type HUBER or HUBER_MODIFIED."
   */
  void BundleSettings::addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Model model,
                                                          double maxModelCQuantile) {

    if (m_maximumLikelihood.size() == 0 && model > MaximumLikelihoodWFunctions::HuberModified) {
      QString msg = "For bundle adjustments with multiple maximum likelihood estimators, the first "
                    "model must be of type HUBER or HUBER_MODIFIED.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_maximumLikelihood.append(qMakePair(model, maxModelCQuantile));
  }


  /**
   * Retrieves the list of maximum likelihood estimator (MLE) models with their
   * corresponding C-Quantiles.
   *
   * @return QList< QPair< MaximumLikelihoodWFunctions::Model, double > >
   *             The list of tuples of the form (model, quantile) to be used for the
   *             bundle adjustment.
   */
  QList< QPair< MaximumLikelihoodWFunctions::Model, double > >
      BundleSettings::maximumLikelihoodEstimatorModels() const {
    return m_maximumLikelihood;
  }


  // =============================================================================================//
  // ======================== Self Calibration ??? (from ipce only) =========================//
  // =============================================================================================//

  // =============================================================================================//
  // ======================== Target Body ??? (from ipce only) ==============================//
  // =============================================================================================//

  /**
   * Sets the target body for the bundle adjustment.
   *
   * @param bundleTargetBody A pointer to the BundleTargetBody object for the
   *                         bundle adjustment to be run.
   */
  void BundleSettings::setBundleTargetBody(BundleTargetBodyQsp bundleTargetBody) {
    m_bundleTargetBody = bundleTargetBody;
  }


  /**
   * Retrieves a pointer to target body information for the bundle adjustment.
   *
   * @return @b BundleTargetBodyQsp A pointer to the BundleTargetBody object for
   *                                the bundle adjustment to be run.
   */
  BundleTargetBodyQsp BundleSettings::bundleTargetBody() const {
    return m_bundleTargetBody;
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body pole position.
   *
   * @return @b bool Indicates whether to solve for target pole position.
   */
//  bool BundleSettings::solveTargetBodyPolePosition() const {
//    return m_solveTargetBodyPolePosition;
//  }


  /**
   * Retrieves the number of target body parameters. If the BundleTargetBody
   * associated with this bundle adjustment is NULL, this method returns 0.
   *
   * @return @b int The number of target body parameters.
   */
  int BundleSettings::numberTargetBodyParameters() const {
    if (!m_bundleTargetBody)
      return 0;

    return m_bundleTargetBody->numberParameters();
  }


  /**
   * This method is used to determine whether the bundle adjustment will solve
   * for target body.
   *
   * @return @b bool Indicates whether to solve for target body.
   */
  bool BundleSettings::solveTargetBody() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    else {
      return (m_bundleTargetBody->numberParameters() > 0);
    }
  }


  /**
   * This method is used to determine whether the bundle adjustment will
   * solve for target body pole right ascension.
   *
   * @see BundleTargetBody::solvePoleRA()
   *
   * @return @b bool Indicates whether to solve for target pole RA.
   */
  bool BundleSettings::solvePoleRA() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePoleRA();
  }


  /**
   * This method is used to determine whether the bundle adjustment will
   * solve for target body pole right ascension velocity.
   *
   * @see BundleTargetBody::solvePoleRAVelocity()
   *
   * @return @b bool Indicates whether to solve for target pole RA velocity.
   */
  bool BundleSettings::solvePoleRAVelocity() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePoleRAVelocity();
  }


  /**
   * This method is used to determine whether the bundle adjustment will
   * solve for target body pole declination.
   *
   * @see BundleTargetBody::solvePoleDeclination()
   *
   * @return @b bool Indicates whether to solve for target pole declination.
   */
  bool BundleSettings::solvePoleDec() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePoleDec();
  }


  /**
   * This method is used to determine whether the bundle adjustment will
   * solve for target body pole declination velocity.
   *
   * @see BundleTargetBody::solvePoleDeclinationVelocity()
   *
   * @return @b bool Indicates whether to solve for target pole declination velocity.
   */
  bool BundleSettings::solvePoleDecVelocity() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePoleDecVelocity();
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body prime meridian.
   *
   * @see BundleTargetBody::solvePM()
   *
   * @return @b bool Indicates whether to solve for target PM.
   */
  bool BundleSettings::solvePM() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePM();
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body prime meridian velocity.
   *
   * @see BundleTargetBody::solvePMVelocity()
   *
   * @return @b bool Indicates whether to solve for target PM velocity.
   */
  bool BundleSettings::solvePMVelocity() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePMVelocity();
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body prime meridian acceleration.
   *
   * @see BundleTargetBody::solvePMAcceleration()
   *
   * @return @b bool Indicates whether to solve for target PM acceleration.
   */
  bool BundleSettings::solvePMAcceleration() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solvePMAcceleration();
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body triaxial radii.
   *
   * @see BundleTargetBody::solveTriaxialRadii()
   *
   * @return @b bool Indicates whether to solve for target triaxial radii.
   */
  bool BundleSettings::solveTriaxialRadii() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solveTriaxialRadii();
  }


  /**
   * This method is used to determine whether the bundle adjustment
   * will solve for target body mean radius.
   *
   * @see BundleTargetBody::solveMeanRadius()
   *
   * @return @b bool Indicates whether to solve for target mean radius.
   */
  bool BundleSettings::solveMeanRadius() const {
    if (!m_bundleTargetBody) {
      return false;
    }
    return m_bundleTargetBody->solveMeanRadius();
  }


//  void BundleSettings::setTargetBodySolveOptions(bool solveTargetBodyPolePosition,
//                                                 double aprioriRaPole, double sigmaRaPole,
//                                                 double aprioriDecPole, double sigmaDecPole,
//                                                 bool solveTargetBodyZeroMeridian,
//                                                 double aprioriW0, double sigmaW0,
//                                                 bool solveTargetBodyRotationRate,
//                                                 double aprioriWDot, double sigmaWDot,
//                                                 TargetRadiiSolveMethod solveRadiiMethod,
//                                                 double aprioriRadiusA, double sigmaRadiusA,
//                                                 double aprioriRadiusB, double sigmaRadiusB,
//                                                 double aprioriRadiusC, double sigmaRadiusC,
//                                                 double aprioriMeanRadius, double sigmaMeanRadius) {


//    m_solveTargetBody = true;
//    m_solveTargetBodyPolePosition = solveTargetBodyPolePosition;
//    m_aprioriRaPole = aprioriRaPole;
//    m_sigmaRaPole = sigmaRaPole;
//    m_aprioriDecPole = aprioriDecPole;
//    m_sigmaDecPole = sigmaDecPole;
//    m_solveTargetBodyZeroMeridian =solveTargetBodyZeroMeridian;
//    m_aprioriW0 = aprioriW0;
//    m_sigmaW0 = sigmaW0;
//    m_solveTargetBodyRotationRate = solveTargetBodyRotationRate;
//    m_aprioriWDot = aprioriWDot;
//    m_sigmaWDot = sigmaWDot;
//    m_solveTargetBodyRadiusMethod = solveRadiiMethod;
//    m_aprioriRadiusA = aprioriRadiusA;
//    m_sigmaRadiusA = sigmaRadiusA;
//    m_aprioriRadiusB = aprioriRadiusB;
//    m_sigmaRadiusB = sigmaRadiusB;
//    m_aprioriRadiusC = aprioriRadiusC;
//    m_sigmaRadiusC = sigmaRadiusC;
//    m_aprioriMeanRadius = aprioriMeanRadius;
//    m_sigmaMeanRadius = sigmaMeanRadius;

//    m_bundleTargetBody->setSolveSettings(solveTargetBodyPolePosition, aprioriRaPole, sigmaRaPole,
//                                         aprioriDecPole, sigmaDecPole, solveTargetBodyZeroMeridian,
//                                         aprioriW0, sigmaW0, solveTargetBodyRotationRate,
//                                         aprioriWDot, sigmaWDot, solveRadiiMethod, aprioriRadiusA,
//                                         sigmaRadiusA, aprioriRadiusB, sigmaRadiusB, aprioriRadiusC,
//                                         sigmaRadiusC, aprioriMeanRadius, sigmaMeanRadius);
//  }


  // =============================================================================================//
  // ========================= Output Options (from Jigsaw only) ================================//
  // =============================================================================================//
  /**
   * Set the output file prefix for the bundle adjustment.
   *
   * @param outputFilePrefix A string containing a prefix and/or directory path
   */
  void BundleSettings::setOutputFilePrefix(QString outputFilePrefix) {
    m_outputFilePrefix = outputFilePrefix;
  }


  /**
   * Retrieve the output file prefix. This string will be
   * appended to all of the output files created by the bundle
   * adjustment.
   *
   * @return @b QString A string containing a prefix and/or
   *                    directory path to be appended to all
   *                    output files.
   */
  QString BundleSettings::outputFilePrefix() const {
    return m_outputFilePrefix;
  }


  /**
   * @brief This method is used to write a BundleSettings object in an XML format
   *
   * @param stream The stream to write serialized XML output
   * @param project The project that contains the settings
   */
  void BundleSettings::save(QXmlStreamWriter &stream, const Project *project) const {
    stream.writeStartElement("bundleSettings");

    stream.writeStartElement("globalSettings");

    stream.writeTextElement("validateNetwork", toString(validateNetwork()));

    stream.writeStartElement("solveOptions");
    stream.writeAttribute("solveObservationMode", toString(solveObservationMode()));
    stream.writeAttribute("solveRadius", toString(solveRadius()));
    stream.writeAttribute("updateCubeLabel", toString(updateCubeLabel()));
    stream.writeAttribute("errorPropagation", toString(errorPropagation()));
    stream.writeAttribute("createInverseMatrix", toString(createInverseMatrix()));
    stream.writeEndElement();

    stream.writeStartElement("aprioriSigmas");
    if (IsSpecial(globalLatitudeAprioriSigma())) {
      stream.writeAttribute("latitude", "N/A");
    }
    else {
      stream.writeAttribute("latitude", toString(globalLatitudeAprioriSigma()));
    }
    if (IsSpecial(globalLongitudeAprioriSigma())) {
      stream.writeAttribute("longitude", "N/A");
    }
    else {
      stream.writeAttribute("longitude", toString(globalLongitudeAprioriSigma()));
    }
    if (IsSpecial(globalRadiusAprioriSigma())) {
      stream.writeAttribute("radius", "N/A");
    }
    else {
      stream.writeAttribute("radius", toString(globalRadiusAprioriSigma()));
    }
    stream.writeEndElement();

    stream.writeStartElement("outlierRejectionOptions");
    stream.writeAttribute("rejection", toString(outlierRejection()));
    if (outlierRejection()) {
      stream.writeAttribute("multiplier", toString(outlierRejectionMultiplier()));
    }
    else {
      stream.writeAttribute("multiplier", "N/A");
    }
    stream.writeEndElement();

    stream.writeStartElement("convergenceCriteriaOptions");
    stream.writeAttribute("convergenceCriteria",
                          convergenceCriteriaToString(convergenceCriteria()));
    stream.writeAttribute("threshold",
                          toString(convergenceCriteriaThreshold()));
    stream.writeAttribute("maximumIterations",
                          toString(convergenceCriteriaMaximumIterations()));
    stream.writeEndElement();

    stream.writeStartElement("maximumLikelihoodEstimation");
    for (int i = 0; i < m_maximumLikelihood.size(); i++) {
      stream.writeStartElement("model");
      stream.writeAttribute("type",
                          MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihood[i].first));
      stream.writeAttribute("quantile", toString(m_maximumLikelihood[i].second));
      stream.writeEndElement();
    }
    stream.writeEndElement();

    stream.writeStartElement("outputFileOptions");
    stream.writeAttribute("fileNamePrefix", outputFilePrefix());
    stream.writeEndElement();

    stream.writeEndElement(); // end global settings

    if (!m_observationSolveSettings.isEmpty()) {
      stream.writeStartElement("observationSolveSettingsList");
      for (int i = 0; i < m_observationSolveSettings.size(); i++) {
        m_observationSolveSettings[i].save(stream, project);
      }
      stream.writeEndElement();
    }
    else {
      // throw error??? should not write if no observation settings...
    }
    stream.writeEndElement();
  }


  /**
   * @brief Create an XML Handler (reader) that can populate the BundleSettings class data.
   * See BundleSettings::save() for the expected format. This contructor is called
   * inside the BundleSettings constructor that takes an XmlStackedHandlerReader.
   *
   * @param bundleSettings The BundleSettings we're going to be initializing
   * @param project The project that contains the settings
   */
  BundleSettings::XmlHandler::XmlHandler(BundleSettings *bundleSettings, Project *project) {
    m_xmlHandlerBundleSettings = bundleSettings;
    m_xmlHandlerProject = project;
    m_xmlHandlerCharacters = "";
    m_xmlHandlerObservationSettings.clear();
  }


  /**
   * @brief Destroys BundleSettings::XmlHandler object.
   */
  BundleSettings::XmlHandler::~XmlHandler() {
  }


  /**
   * Handle an XML start element. This method is called when the reader finds an open tag.
   * handle the read when the startElement with the name localName has been found.
   *
   * @param qName SAX namespace for this tag
   * @param localName SAX local name
   * @param qName SAX qualified name of the tag.
   * @param attributes The list of attributes for the tag.
   *
   * @return @b bool Indicates whether to continue reading the XML (usually true).
   */
  bool BundleSettings::XmlHandler::startElement(const QString &namespaceURI,
                                                const QString &localName,
                                                const QString &qName,
                                                const QXmlAttributes &attributes) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, attributes)) {

      if (localName == "solveOptions") {

        QString solveObservationModeStr = attributes.value("solveObservationMode");
        if (!solveObservationModeStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveObservationMode = toBool(solveObservationModeStr);
        }

        QString solveRadiusStr = attributes.value("solveRadius");
        if (!solveRadiusStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveRadius = toBool(solveRadiusStr);
        }

        QString updateCubeLabelStr = attributes.value("updateCubeLabel");
        if (!updateCubeLabelStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_updateCubeLabel = toBool(updateCubeLabelStr);
        }

        QString errorPropagationStr = attributes.value("errorPropagation");
        if (!errorPropagationStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_errorPropagation = toBool(errorPropagationStr);
        }

        QString createInverseMatrixStr = attributes.value("createInverseMatrix");
        if (!createInverseMatrixStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createInverseMatrix = toBool(createInverseMatrixStr);
        }
      }
      else if (localName == "aprioriSigmas") {

        QString globalLatitudeAprioriSigmaStr = attributes.value("latitude");
        m_xmlHandlerBundleSettings->m_globalLatitudeAprioriSigma = Isis::Null;
        // TODO: why do I need to init this one and not other sigmas???
        if (!globalLatitudeAprioriSigmaStr.isEmpty()) {
          if (globalLatitudeAprioriSigmaStr == "N/A") {
            m_xmlHandlerBundleSettings->m_globalLatitudeAprioriSigma = Isis::Null;
          }
          else {
            m_xmlHandlerBundleSettings->m_globalLatitudeAprioriSigma
                = toDouble(globalLatitudeAprioriSigmaStr);
          }
        }

        QString globalLongitudeAprioriSigmaStr = attributes.value("longitude");
        if (!globalLongitudeAprioriSigmaStr.isEmpty()) {
          if (globalLongitudeAprioriSigmaStr == "N/A") {
            m_xmlHandlerBundleSettings->m_globalLongitudeAprioriSigma = Isis::Null;
          }
          else {
            m_xmlHandlerBundleSettings->m_globalLongitudeAprioriSigma
                = toDouble(globalLongitudeAprioriSigmaStr);
          }
        }

        QString globalRadiusAprioriSigmaStr = attributes.value("radius");
        if (!globalRadiusAprioriSigmaStr.isEmpty()) {
          if (globalRadiusAprioriSigmaStr == "N/A") {
            m_xmlHandlerBundleSettings->m_globalRadiusAprioriSigma = Isis::Null;
          }
          else {
            m_xmlHandlerBundleSettings->m_globalRadiusAprioriSigma
                = toDouble(globalRadiusAprioriSigmaStr);
          }
        }
      }
      else if (localName == "outlierRejectionOptions") {
        QString outlierRejectionStr = attributes.value("rejection");
        if (!outlierRejectionStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_outlierRejection = toBool(outlierRejectionStr);
        }

        QString outlierRejectionMultiplierStr = attributes.value("multiplier");
        if (!outlierRejectionMultiplierStr.isEmpty()) {
          if (outlierRejectionMultiplierStr != "N/A") {
            m_xmlHandlerBundleSettings->m_outlierRejectionMultiplier
                = toDouble(outlierRejectionMultiplierStr);
          }
          else {
            m_xmlHandlerBundleSettings->m_outlierRejectionMultiplier = 1.0;
          }
        }
      }
      else if (localName == "convergenceCriteriaOptions") {

        QString convergenceCriteriaStr = attributes.value("convergenceCriteria");
        if (!convergenceCriteriaStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteria
              = stringToConvergenceCriteria(convergenceCriteriaStr);
        }

        QString convergenceCriteriaThresholdStr = attributes.value("threshold");
        if (!convergenceCriteriaThresholdStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaThreshold
              = toDouble(convergenceCriteriaThresholdStr);
        }

        QString convergenceCriteriaMaximumIterationsStr = attributes.value("maximumIterations");
        if (!convergenceCriteriaMaximumIterationsStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaMaximumIterations
              = toInt(convergenceCriteriaMaximumIterationsStr);
        }
      }
      else if (localName == "model") {
        QString type = attributes.value("type");
        QString quantile = attributes.value("quantile");
        if (!type.isEmpty() && !quantile.isEmpty()) {
          m_xmlHandlerBundleSettings->m_maximumLikelihood.append(
              qMakePair(MaximumLikelihoodWFunctions::stringToModel(type),
                        toDouble(quantile)));
        }
      }
      else if (localName == "outputFileOptions") {
        QString outputFilePrefixStr = attributes.value("fileNamePrefix");
        if (!outputFilePrefixStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_outputFilePrefix = outputFilePrefixStr;
        }
      }
      else if (localName == "bundleObservationSolveSettings") {
        m_xmlHandlerObservationSettings.append(
            new BundleObservationSolveSettings(m_xmlHandlerProject, reader()));
      }
    }
    return true;
  }


  /**
   * @brief Add a character from an XML element to the content handler.
   *
   * @param ch charater from XML element
   * @return true
   */
  bool BundleSettings::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }


  /**
   * @brief Handle end tags for the BundleSettings serialized XML.
   *
   * @param namespaceURI URI of the specified tags namespce
   * @param localName SAX localName
   * @param qName SAX qualified name
   *
   * @return true
   */
  bool BundleSettings::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
    if (!m_xmlHandlerCharacters.isEmpty()) {
      if (localName == "validateNetwork") {
        m_xmlHandlerBundleSettings->m_validateNetwork = toBool(m_xmlHandlerCharacters);
      }
      else if (localName == "observationSolveSettingsList") {
        for (int i = 0; i < m_xmlHandlerObservationSettings.size(); i++) {
          m_xmlHandlerBundleSettings->m_observationSolveSettings.append(
              *m_xmlHandlerObservationSettings[i]);
        }
        m_xmlHandlerObservationSettings.clear();
      }

      m_xmlHandlerCharacters = "";
    }
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }


  /**
   * @brief Format an error message indicating a problem with BundleSettings.
   *
   * @param QXmlParseException Execption thrown by parser.
   * @return false
   */
  bool BundleSettings::XmlHandler::fatalError(const QXmlParseException &exception) {
    qDebug() << "Parse error with BundleSettings at line " << exception.lineNumber()
             << ", " << "column " << exception.columnNumber() << ": "
             << qPrintable(exception.message());
    return false;
  }
}
