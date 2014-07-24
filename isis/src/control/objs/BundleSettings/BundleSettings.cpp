#include "BundleSettings.h"

#include <QDebug>
#include <QList>
#include <QString>

#include "BundleObservationSolveSettings.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include <QUuid>
#include <QXmlStreamWriter>
#include "XmlStackedHandlerReader.h"

namespace Isis {

  /**
   * Constructs a BundleSettings object with default values.
   */
  BundleSettings::BundleSettings() {
    m_id = NULL;

    m_validateNetwork = true;
    m_solveMethod = Sparse;
    m_solveObservationMode = false;
    m_solveRadius          = false;
    m_updateCubeLabel      = false;
    m_errorPropagation     = false;

    m_outlierRejection     = false;
    m_outlierRejectionMultiplier = 1.0;

    // Parameter Uncertainties (Weighting)
    m_globalLatitudeAprioriSigma = -1.0;
    m_globalLongitudeAprioriSigma = -1.0;
    m_globalRadiusAprioriSigma = -1.0;

    BundleObservationSolveSettings defaultSolveSettings;
    m_observationSolveSettings.append(defaultSolveSettings);

    // Convergence Criteria
    m_convergenceCriteria = BundleSettings::Sigma0;
    m_convergenceCriteriaThreshold = 1.0e-10;
    m_convergenceCriteriaMaximumIterations = 50;

    // Maximum Likelihood Estimation Options no default in the constructor - must be set.
    m_maximumLikelihood.clear();

    // Self Calibration ??? (from cnetsuite only)

    // Target Body ??? (from cnetsuite only)

    // Output Options
    m_outputFilePrefix = "";
    m_createBundleOutputFile = true;
    m_createCSVFiles = true;
    m_createResidualsFile    = true;

    m_id = new QUuid(QUuid::createUuid());
  }


  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleSettings::BundleSettings(Project *project, XmlStackedHandlerReader *xmlReader,
                                 QObject *parent) : QObject(parent) {
    m_id = NULL;

    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  BundleSettings::BundleSettings(XmlStackedHandlerReader *xmlReader, QObject *parent) {
//    m_propertiesUsed = 0;
//    m_propertyValues = new QMap<int, QVariant>;

    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  /**
   * copy constructor
   */
  BundleSettings::BundleSettings(const BundleSettings &other)
      : m_validateNetwork(other.m_validateNetwork),
        m_solveMethod(other.m_solveMethod),
        m_solveObservationMode(other.m_solveObservationMode),
        m_solveRadius(other.m_solveRadius),
        m_updateCubeLabel(other.m_updateCubeLabel),
        m_errorPropagation(other.m_errorPropagation),
        m_outlierRejection(other.m_outlierRejection),
        m_outlierRejectionMultiplier(other.m_outlierRejectionMultiplier),
        m_globalLatitudeAprioriSigma(other.m_globalLatitudeAprioriSigma),
        m_globalLongitudeAprioriSigma(other.m_globalLongitudeAprioriSigma),
        m_globalRadiusAprioriSigma(other.m_globalRadiusAprioriSigma),
        m_convergenceCriteria(other.m_convergenceCriteria),
        m_convergenceCriteriaThreshold(other.m_convergenceCriteriaThreshold),
        m_convergenceCriteriaMaximumIterations(other.m_convergenceCriteriaMaximumIterations),
        m_outputFilePrefix(other.m_outputFilePrefix),
        m_createBundleOutputFile(other.m_createBundleOutputFile),
        m_createCSVFiles(other.m_createCSVFiles),
        m_createResidualsFile(other.m_createResidualsFile) {
    
    for (int i = 0; i < other.m_maximumLikelihood.size(); i++) {
      m_maximumLikelihood.append(other.m_maximumLikelihood[i]);
    }

    m_observationSolveSettings = other.m_observationSolveSettings;

//    delete m_id;
    m_id = new QUuid(other.m_id->toString());
  }



  BundleSettings::~BundleSettings() {    
    delete m_id;
    m_id = NULL;

//    int nSolveSettings = m_observationSolveSettings.size();
//    for (int i = 0; i < nSolveSettings; i++) {
//      BundleObservationSolveSettings *settings = m_observationSolveSettings.at(i);
//      delete settings;
//    }
//    m_observationSolveSettings.clear();

  }



  BundleSettings &BundleSettings::operator=(const BundleSettings &other) {
    if (&other != this) {
      m_validateNetwork = other.m_validateNetwork;
      m_solveMethod = other.m_solveMethod;
      m_solveObservationMode = other.m_solveObservationMode;
      m_solveRadius = other.m_solveRadius;
      m_updateCubeLabel = other.m_updateCubeLabel;
      m_errorPropagation = other.m_errorPropagation;
      m_outlierRejection = other.m_outlierRejection;
      m_outlierRejectionMultiplier = other.m_outlierRejectionMultiplier;
      m_globalLatitudeAprioriSigma = other.m_globalLatitudeAprioriSigma;
      m_globalLongitudeAprioriSigma = other.m_globalLongitudeAprioriSigma;
      m_globalRadiusAprioriSigma = other.m_globalRadiusAprioriSigma;
      m_convergenceCriteria = other.m_convergenceCriteria;
      m_convergenceCriteriaThreshold = other.m_convergenceCriteriaThreshold;
      m_convergenceCriteriaMaximumIterations = other.m_convergenceCriteriaMaximumIterations;
      m_outputFilePrefix = other.m_outputFilePrefix;
      m_createBundleOutputFile = other.m_createBundleOutputFile;
      m_createCSVFiles = other.m_createCSVFiles;
      m_createResidualsFile = other.m_createResidualsFile;

      for (int i = 0;i < other.m_maximumLikelihood.size();i++) {
        m_maximumLikelihood.append(other.m_maximumLikelihood[i]);
      }

      m_observationSolveSettings = other.m_observationSolveSettings;

      delete m_id;
      m_id = new QUuid(other.m_id->toString());
    }
    return *this;
  }



  void BundleSettings::setValidateNetwork(bool validate) {
    m_validateNetwork = validate;
  }



  bool BundleSettings::validateNetwork() const {
    return m_validateNetwork;
  }



  // =============================================================================================//
  // ======================== Solve Options ======================================================//
  // =============================================================================================//
  BundleSettings::SolveMethod BundleSettings::stringToSolveMethod(QString method) {

    if (method.compare("SPARSE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::Sparse;
    }
    else if (method.compare("SPECIALK", Qt::CaseInsensitive) == 0) {
      return BundleSettings::SpecialK;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown bundle solve method [" + method + "].",
                       _FILEINFO_);
    }
  }



  QString BundleSettings::solveMethodToString(SolveMethod method) {
    if (method == Sparse)         return "Sparse";
    else if (method == SpecialK)  return "SpecialK";
    else throw IException(IException::Programmer,
                          "Unknown solve method enum [" + toString(method) + "].",
                          _FILEINFO_);
  }



  void BundleSettings::setSolveOptions(SolveMethod method, bool solveObservationMode,
                                       bool updateCubeLabel, bool errorPropagation, 
                                       bool solveRadius, 
                                       double globalLatitudeAprioriSigma, 
                                       double globalLongitudeAprioriSigma, 
                                       double globalRadiusAprioriSigma) {
    m_solveMethod = method;
    m_solveObservationMode = solveObservationMode;
    m_solveRadius = solveRadius;
    m_updateCubeLabel = updateCubeLabel;
    m_errorPropagation = errorPropagation;
    m_globalLatitudeAprioriSigma = globalLatitudeAprioriSigma;
    m_globalLongitudeAprioriSigma = globalLongitudeAprioriSigma;
    if (m_solveRadius) {
      m_globalRadiusAprioriSigma = globalRadiusAprioriSigma;
    }
    else {
      m_globalRadiusAprioriSigma = -1.0;
    }
  }



  void BundleSettings::setOutlierRejection(bool outlierRejection, double multiplier) {
    m_outlierRejection = outlierRejection;
    if (m_outlierRejection) {
      m_outlierRejectionMultiplier = multiplier;
    }
    else {
      m_outlierRejectionMultiplier = 1.0;
    }
  }



  void BundleSettings::setObservationSolveOptions(
      QList<BundleObservationSolveSettings> observationSolveSettings) {
    m_observationSolveSettings = observationSolveSettings;
  }



  BundleSettings::SolveMethod BundleSettings::solveMethod() const {
    return m_solveMethod;
  }



  bool BundleSettings::solveObservationMode() const {
    return m_solveObservationMode;
  }



  bool BundleSettings::solveRadius() const {
    return m_solveRadius;
  }



  bool BundleSettings::updateCubeLabel() const {
    return m_updateCubeLabel;
  }



  bool BundleSettings::errorPropagation() const {
    return m_errorPropagation;
  }


  
  bool BundleSettings::outlierRejection() const {
    return m_outlierRejection;
  }



  double BundleSettings::outlierRejectionMultiplier() const {
    return m_outlierRejectionMultiplier;
  }



  double BundleSettings::globalLatitudeAprioriSigma() const {
    return m_globalLatitudeAprioriSigma;
  }



  double BundleSettings::globalLongitudeAprioriSigma() const {
    return m_globalLongitudeAprioriSigma;
  }



  double BundleSettings::globalRadiusAprioriSigma() const {
    return m_globalRadiusAprioriSigma;
  }



  int BundleSettings::numberSolveSettings() const {
     return m_observationSolveSettings.size();
  }



  BundleObservationSolveSettings 
      BundleSettings::observationSolveSettings(QString instrumentId) const {

    for (int i = 0; i < numberSolveSettings(); i++) {
      if (m_observationSolveSettings[i].instrumentId() == instrumentId) {
        return m_observationSolveSettings[i];
      }
    }
    QString msg = "Unable to find BundleObservationSolveSettings with InstrumentId = ["
                  + instrumentId + "].";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }



  BundleObservationSolveSettings 
      BundleSettings::observationSolveSettings(int n) const { 

    if (n < numberSolveSettings()) {
      return m_observationSolveSettings[n]; 
    }
    QString msg = "Unable to find BundleObservationSolveSettings with index = ["
                  + toString(n) + "].";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }



  // =============================================================================================//
  // ======================== Convergence Criteria ===============================================//
  // =============================================================================================//
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



  QString 
      BundleSettings::convergenceCriteriaToString(BundleSettings::ConvergenceCriteria criteria) {
    if (criteria == Sigma0)                    return "Sigma0";
    else if (criteria == ParameterCorrections) return "ParameterCorrections";
    else  throw IException(IException::Programmer,
                           "Unknown convergence criteria enum [" + toString(criteria) + "].",
                           _FILEINFO_);
  }



  void BundleSettings::setConvergenceCriteria(BundleSettings::ConvergenceCriteria criteria, 
                                              double threshold, 
                                              int maximumIterations) {
    m_convergenceCriteria = criteria;
    m_convergenceCriteriaThreshold = threshold;
    m_convergenceCriteriaMaximumIterations = maximumIterations;
  }



  BundleSettings::ConvergenceCriteria BundleSettings::convergenceCriteria() const {
    return m_convergenceCriteria;
  }



  double BundleSettings::convergenceCriteriaThreshold() const {
    return m_convergenceCriteriaThreshold;
  }



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



  void BundleSettings::addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Model model, 
                                                          double maxModelCQuantile) {

    if (m_maximumLikelihood.size() == 0 && model > MaximumLikelihoodWFunctions::HuberModified) {
      QString msg = "For bundle adjustments with multiple maximum likelihood estimators, the first "
                    "model must be of type HUBER or HUBER_MODIFIED.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_maximumLikelihood.append(qMakePair(model, maxModelCQuantile));
  }


  
  QList< QPair< MaximumLikelihoodWFunctions::Model, double > >  
      BundleSettings::maximumLikelihoodEstimatorModels() const {
    return m_maximumLikelihood;
  }


  
  // =============================================================================================//
  // ======================== Self Calibration ??? (from cnetsuite only) =========================//
  // =============================================================================================//
  
  // =============================================================================================//
  // ======================== Target Body ??? (from cnetsuite only) ==============================//
  // =============================================================================================//
  
  // =============================================================================================//
  // ======================== Output Options ??? (from Jigsaw only) ==============================//
  // =============================================================================================//
  void BundleSettings::setOutputFiles(QString outputFilePrefix, bool createBundleOutputFile, 
                                      bool createCSVFiles, bool createResidualsFile) {
    m_outputFilePrefix = outputFilePrefix;
    m_createBundleOutputFile = createBundleOutputFile;
    m_createCSVFiles = createCSVFiles;
    m_createResidualsFile = createResidualsFile;
  }



  QString BundleSettings::outputFilePrefix() const {
    return m_outputFilePrefix;
  }



  bool BundleSettings::createBundleOutputFile() const {
    return m_createBundleOutputFile;
  }



  bool BundleSettings::createCSVFiles() const {
    return m_createCSVFiles;
  }



  bool BundleSettings::createResidualsFile() const {
    return m_createResidualsFile;
  }

  PvlObject BundleSettings::pvlObject(QString name) const {

    PvlObject pvl(name);

    // General Solve Options
    pvl += PvlKeyword("NetworkValidated", toString(validateNetwork()));
    pvl += PvlKeyword("SolveMethod", solveMethodToString(solveMethod()));
    pvl += PvlKeyword("SolveObservationMode", toString(solveObservationMode()));
    pvl += PvlKeyword("SolveRadius", toString(solveRadius()));
    pvl += PvlKeyword("UpdateCubeLabel", toString(updateCubeLabel()));
    pvl += PvlKeyword("ErrorPropagation", toString(errorPropagation()));
    pvl += PvlKeyword("OutlierRejection", toString(outlierRejection()));
    if (m_outlierRejection) {
      pvl += PvlKeyword("OutlierMultiplier", toString(outlierRejectionMultiplier()));
    }
    if (globalLatitudeAprioriSigma() >= 0) {
      pvl += PvlKeyword("GlobalLatitudeAprioriSigma", toString(globalLatitudeAprioriSigma()));
    }
    else {
      pvl += PvlKeyword("GlobalLatitudeAprioriSigma", "None");
    }
    if (globalLongitudeAprioriSigma() >= 0) {
      pvl += PvlKeyword("GlobalLongitudeAprioriSigma", toString(globalLongitudeAprioriSigma()));
    }
    else {
      pvl += PvlKeyword("GlobalLongitudeAprioriSigma", "None");
    }
    if (m_solveRadius) {
      pvl += PvlKeyword("GlobalRadiiAprioriSigma", toString(globalRadiusAprioriSigma()));
    }

    // Convergence Criteria
    pvl += PvlKeyword("ConvergenceCriteria", convergenceCriteriaToString(convergenceCriteria()));
    pvl += PvlKeyword("ConvergenceCriteriaThreshold", toString(convergenceCriteriaThreshold()));
    pvl += PvlKeyword("ConvergenceCriteriaMaximumIterations",
                        toString(convergenceCriteriaMaximumIterations()));

    // Output Options
    pvl += PvlKeyword("CreateBundleOutputFile", toString(createBundleOutputFile()));
    pvl += PvlKeyword("CreateCSVFiles", toString(createCSVFiles()));
    pvl += PvlKeyword("CreateResidualsFile", toString(createResidualsFile()));
    if (createBundleOutputFile() || createCSVFiles() || createResidualsFile()) {
      pvl += PvlKeyword("FilePrefix", outputFilePrefix());
    }

    // Maximum Likelihood Options
    PvlKeyword models("MaximumLikelihoodModels"); 
    if (m_maximumLikelihood.size() > 0) {

      models.addValue(MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihood[0].first));

      PvlKeyword quantiles("MaximumLikelihoodQuantiles", 
                           toString(m_maximumLikelihood[0].second));

      for (int i = 1; i < m_maximumLikelihood.size(); i++) {
        models.addValue(MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihood[i].first));
        quantiles.addValue(toString(m_maximumLikelihood[i].second));
      }
      pvl += models;
      pvl += quantiles;
    }
    else {
      models.addValue("None");
    }

    pvl += PvlKeyword("NumberObservationSolveSettings", toString(numberSolveSettings()));

    for (int i = 0; i < numberSolveSettings(); i++) {
// TODO: make this work... ASSERT failure in QList<T>::operator[]: "index out of range"
// TODO:       BundleObservationSolveSettings boss = m_observationSolveSettings[i];
// TODO:       BundleObservationSolveSettings boss = observationSolveSettings(i);
// TODO:      PvlObject bundleObsSolveSettingsPvl = boss.pvlObject();
// TODO:      pvl += bundleObsSolveSettingsPvl;
      pvl += PvlKeyword("ObservationSolveSettingsInstrumentId", 
                        m_observationSolveSettings[i].instrumentId());
    }
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
  void BundleSettings::save(QXmlStreamWriter &stream, const Project *project,
                            FileName newProjectRoot) const {
    QString boolStr;

    stream.writeStartElement("generalSettings");

    stream.writeAttribute("id", m_id->toString());

    m_validateNetwork ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("validateNetwork", boolStr);

    stream.writeAttribute("solveMethod", solveMethodToString(m_solveMethod));

    m_solveObservationMode ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("solveObservationMode", boolStr);

    m_solveRadius ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("solveRadius", boolStr);

    m_updateCubeLabel ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("updateCubeLabel", boolStr);

    m_errorPropagation ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("errorPropagation", boolStr);

    m_outlierRejection ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("outlierRejection", boolStr);

    stream.writeAttribute("outlierRejectionMultiplier",
                          IString(m_outlierRejectionMultiplier).ToQt());
    stream.writeAttribute("globalLatitudeAprioriSigma", IString(m_globalLatitudeAprioriSigma).ToQt());
    stream.writeAttribute("globalLongitudeAprioriSigma", IString(m_globalLongitudeAprioriSigma).ToQt());
    stream.writeAttribute("globalRadiusAprioriSigma", IString(m_globalRadiusAprioriSigma).ToQt());

    stream.writeAttribute("convergenceCriteria",
                          convergenceCriteriaToString(m_convergenceCriteria));
    stream.writeAttribute("convergenceCriteriaThreshold",
                          IString(m_convergenceCriteriaThreshold).ToQt());
    stream.writeAttribute("convergenceCriteriaMaximumIterations",
                          IString(m_convergenceCriteriaMaximumIterations).ToQt());

//    QList< QPair< MaximumLikelihoodWFunctions::Model, double > > m_maximumLikelihood;

    stream.writeAttribute("m_outputFilePrefix", m_outputFilePrefix);

    m_createBundleOutputFile ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("createBundleOutputFile", boolStr);

    m_createCSVFiles ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("createCSVFiles", boolStr);

    m_createResidualsFile ? boolStr = "true" : boolStr = "false";
    stream.writeAttribute("createResidualsFile", boolStr);

    stream.writeEndElement();
  }


  /**
   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
   *   BundleSettings::save() for the expected format.
   *
   * @param bundleSettings The BundleSettings we're going to be initializing
   * @param imageFolder The folder that contains the Cube
   */
  BundleSettings::XmlHandler::XmlHandler(BundleSettings *bundleSettings, Project *project) {
    m_bundleSettings = bundleSettings;
    m_project = project;
  }

  BundleSettings::XmlHandler::XmlHandler(BundleSettings *bundleSettings) {
    m_bundleSettings = bundleSettings;
    m_project = NULL;
  }

  /**
   * Handle an XML start element. This expects <image/> and <displayProperties/> elements.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool BundleSettings::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                       const QString &qName, const QXmlAttributes &atts) {
    m_characters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "generalSettings") {
        QString id = atts.value("id");

        QString validateNetworkStr = atts.value("validateNetwork");
        QString solveMethodStr = atts.value("solveMethod");
        QString solveObservationModeStr = atts.value("solveObservationMode");
        QString solveRadiusStr = atts.value("solveRadius");
        QString updateCubeLabelStr = atts.value("updateCubeLabel");
        QString errorPropagationStr = atts.value("errorPropagation");
        QString outlierRejectionStr = atts.value("outlierRejection");
        QString outlierRejectionMultiplierStr = atts.value("outlierRejectionMultiplier");
        QString globalLatitudeAprioriSigmaStr = atts.value("globalLatitudeAprioriSigma");
        QString globalLongitudeAprioriSigmaStr = atts.value("globalLongitudeAprioriSigma");
        QString globalRadiusAprioriSigmaStr = atts.value("globalRadiusAprioriSigma");
        QString convergenceCriteriaStr = atts.value("convergenceCriteria");
        QString convergenceCriteriaThresholdStr = atts.value("convergenceCriteriaThreshold");
        QString convergenceCriteriaMaximumIterationsStr = atts.value("convergenceCriteriaMaximumIterations");
        QString outputFilePrefixStr = atts.value("outputFilePrefix");
        QString createBundleOutputFileStr = atts.value("createBundleOutputFile");
        QString createCSVFilesStr = atts.value("createCSVFiles");
        QString createResidualsFileStr = atts.value("createResidualsFile");


        if (!id.isEmpty()) {
//          delete m_bundleSettings->m_id;
//          m_bundleSettings->m_id = NULL;
//          m_bundleSettings->m_id = new QUuid(id.toAscii());

//          int fred=1;
        }

        if (!validateNetworkStr.isEmpty()) {
          (validateNetworkStr == "true") ? m_bundleSettings->m_validateNetwork = true : m_bundleSettings->m_validateNetwork = false;
        }

        if (!solveMethodStr.isEmpty()) {
          m_bundleSettings->m_solveMethod = stringToSolveMethod(solveMethodStr);
        }

        if (!solveObservationModeStr.isEmpty()) {
          (solveObservationModeStr == "true") ?
                m_bundleSettings->m_solveObservationMode = true : m_bundleSettings->m_solveObservationMode = false;
        }

        if (!solveRadiusStr.isEmpty()) {
          (solveRadiusStr == "true") ?
                m_bundleSettings->m_solveRadius = true : m_bundleSettings->m_solveRadius = false;
        }

        if (!updateCubeLabelStr.isEmpty()) {
          (updateCubeLabelStr == "true") ? m_bundleSettings->m_updateCubeLabel = true :
              m_bundleSettings->m_updateCubeLabel = false;
        }

        if (!errorPropagationStr.isEmpty()) {
          (errorPropagationStr == "true") ? m_bundleSettings->m_errorPropagation = true :
              m_bundleSettings->m_errorPropagation = false;
        }

        if (!outlierRejectionStr.isEmpty()) {
          (outlierRejectionStr == "true") ? m_bundleSettings->m_outlierRejection = true :
              m_bundleSettings->m_outlierRejection = false;
        }

        if (!outlierRejectionMultiplierStr.isEmpty()) {
          m_bundleSettings->m_outlierRejectionMultiplier = outlierRejectionMultiplierStr.toDouble();
        }

        if (!globalLatitudeAprioriSigmaStr.isEmpty()) {
          m_bundleSettings->m_globalLatitudeAprioriSigma = globalLatitudeAprioriSigmaStr.toDouble();
        }

        if (!globalLongitudeAprioriSigmaStr.isEmpty()) {
          m_bundleSettings->m_globalLongitudeAprioriSigma =
              globalLongitudeAprioriSigmaStr.toDouble();
        }

        if (!globalRadiusAprioriSigmaStr.isEmpty()) {
          m_bundleSettings->m_globalRadiusAprioriSigma = globalRadiusAprioriSigmaStr.toDouble();
        }

        if (!convergenceCriteriaStr.isEmpty()) {
          m_bundleSettings->m_convergenceCriteria = stringToConvergenceCriteria(convergenceCriteriaStr);
        }

        if (!convergenceCriteriaThresholdStr.isEmpty()) {
          m_bundleSettings->m_convergenceCriteriaThreshold = convergenceCriteriaThresholdStr.toDouble();
        }

        if (!convergenceCriteriaMaximumIterationsStr.isEmpty()) {
          m_bundleSettings->m_convergenceCriteriaMaximumIterations = convergenceCriteriaMaximumIterationsStr.toInt();
        }

        if (!outputFilePrefixStr.isEmpty()) {
          m_bundleSettings->m_outputFilePrefix = outputFilePrefixStr;
        }

        if (!createBundleOutputFileStr.isEmpty()) {
          (createBundleOutputFileStr == "true") ?
              m_bundleSettings->m_createBundleOutputFile = true :
              m_bundleSettings->m_createBundleOutputFile = false;
        }

        if (!createCSVFilesStr.isEmpty()) {
          (createCSVFilesStr == "true") ?
              m_bundleSettings->m_createCSVFiles = true :
              m_bundleSettings->m_createCSVFiles = false;
        }

        if (!createResidualsFileStr.isEmpty()) {
          (createResidualsFileStr == "true") ?
              m_bundleSettings->m_createResidualsFile = true :
              m_bundleSettings->m_createResidualsFile = false;
        }

//      else if (localName == "displayProperties") {
//        m_image->m_displayProperties = new ImageDisplayProperties(reader());
//      }
    }
  }

    return true;
  }


  bool BundleSettings::XmlHandler::characters(const QString &ch) {
    m_characters += ch;

    return XmlStackedHandler::characters(ch);
  }


  bool BundleSettings::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
//    if (localName == "footprint" && !m_characters.isEmpty()) {
//      geos::io::WKTReader wktReader(&globalFactory);
//      m_image->m_footprint = PolygonTools::MakeMultiPolygon(
//          wktReader.read(m_characters.toStdString()));
//    }
//    else if (localName == "image" && !m_image->m_footprint) {
//      QMutex mutex;
//      m_image->initFootprint(&mutex);
//      m_image->closeCube();
//    }

//    m_characters = "";
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }

  QDataStream &BundleSettings::write(QDataStream &stream) const {

    stream << m_validateNetwork
           << (qint32)m_solveMethod
           << m_solveObservationMode
           << m_solveRadius
           << m_updateCubeLabel
           << m_errorPropagation
           << m_outlierRejection
           << m_outlierRejectionMultiplier
           << m_globalLatitudeAprioriSigma
           << m_globalLongitudeAprioriSigma
           << m_globalRadiusAprioriSigma
           << m_observationSolveSettings
           << (qint32)m_convergenceCriteria
           << m_convergenceCriteriaThreshold
           << (qint32)m_convergenceCriteriaMaximumIterations
           << m_maximumLikelihood
           << m_outputFilePrefix
           << m_createBundleOutputFile
           << m_createCSVFiles
           << m_createResidualsFile;

    return stream;

  }



  QDataStream &BundleSettings::read(QDataStream &stream) {

    qint32 solveMethod, convergenceCriteria, convergenceCriteriaMaximumIterations;

    stream >> m_validateNetwork
           >> solveMethod
           >> m_solveObservationMode
           >> m_solveRadius
           >> m_updateCubeLabel
           >> m_errorPropagation
           >> m_outlierRejection
           >> m_outlierRejectionMultiplier
           >> m_globalLatitudeAprioriSigma
           >> m_globalLongitudeAprioriSigma
           >> m_globalRadiusAprioriSigma
           >> m_observationSolveSettings
           >> convergenceCriteria
           >> m_convergenceCriteriaThreshold
           >> convergenceCriteriaMaximumIterations
           >> m_maximumLikelihood
           >> m_outputFilePrefix
           >> m_createBundleOutputFile
           >> m_createCSVFiles
           >> m_createResidualsFile;

    m_solveMethod = (BundleSettings::SolveMethod)solveMethod;
    m_convergenceCriteria = (BundleSettings::ConvergenceCriteria)convergenceCriteria;
    m_convergenceCriteriaMaximumIterations = (int)convergenceCriteriaMaximumIterations;

    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const BundleSettings &settings) {
    return settings.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, BundleSettings &settings) {
    return settings.read(stream);
  }

}
