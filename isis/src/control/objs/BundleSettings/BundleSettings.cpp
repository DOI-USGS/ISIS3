#include "BundleSettings.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QtGlobal> // qMax()
#include <QUuid>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include <H5Cpp.h>
#include <hdf5_hl.h>
#include <hdf5.h>

#include "BundleObservationSolveSettings.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  /**
   * Constructs a BundleSettings object with default values.
   */
  BundleSettings::BundleSettings(QObject *parent) : QObject(parent) {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());

    m_validateNetwork = true;

    m_solveMethod = Sparse;
    m_solveObservationMode = false;
    m_solveRadius          = false;
    m_updateCubeLabel      = false;
    m_errorPropagation     = false;

    m_outlierRejection     = false;
    m_outlierRejectionMultiplier = 1.0;

    // Parameter Uncertainties (Weighting)
    m_globalLatitudeAprioriSigma  = Isis::Null;
    m_globalLongitudeAprioriSigma = Isis::Null;
    m_globalRadiusAprioriSigma    = Isis::Null;

    BundleObservationSolveSettings defaultSolveSettings(NULL);
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

  }



  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleSettings::BundleSettings(Project *project, 
                                 XmlStackedHandlerReader *xmlReader,
                                 QObject *parent) : QObject(parent) {   // TODO: does xml stuff need project???
    m_id = NULL;
    // what about the rest of the member data ??? should we set defaults ??? CREATE INITIALIZE METHOD

    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));

  }



//  /**
//   * Construct this BundleSettings object from XML.
//   *
//   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
//   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
//   * @param parent The Qt-relationship parent
//   */
//  BundleSettings::BundleSettings(FileName xmlFile,
//                                 Project *project, 
//                                 XmlStackedHandlerReader *xmlReader,
//                                 QObject *parent)
//      : QObject(parent) {   // TODO: does xml stuff need project???
//
//
//    m_id = NULL;
//    // what about the rest of the member data ??? should we set defaults ???
//
//    QString xmlPath = xmlFile.expanded();
//    QFile qXmlFile(xmlPath);
//    if (!qXmlFile.open(QFile::ReadOnly) ) {
//      throw IException(IException::Io,
//                       QString("Unable to open xml file, [%1],  with read access").arg(xmlPath),
//                       _FILEINFO_);
//    }
//
//    QXmlInputSource xmlInputSource(&qXmlFile);
//
//    xmlReader->pushContentHandler(new XmlHandler(this, project));
//    xmlReader->setErrorHandler(new XmlHandler(this, project));
//    bool success = xmlReader->parse(xmlInputSource);
//    if (!success) {
//      throw IException(IException::Unknown, 
//                       QString("Failed to parse xml file, [%1]").arg(xmlPath),
//                        _FILEINFO_);
//    }
//  }



//  BundleSettings::BundleSettings(XmlStackedHandlerReader *xmlReader, QObject *parent) {
//    m_id = NULL;
//    xmlReader->pushContentHandler(new XmlHandler(this));
//    xmlReader->setErrorHandler(new XmlHandler(this));
//  }

  BundleSettings::BundleSettings(H5::CommonFG &locationObject, QString locationName) {
    openH5Group(locationObject, locationName);
  }

  /**
   * copy constructor
   */
  BundleSettings::BundleSettings(const BundleSettings &src)
      : m_id(new QUuid(src.m_id->toString())),
        m_validateNetwork(src.m_validateNetwork),
        m_solveMethod(src.m_solveMethod),
        m_solveObservationMode(src.m_solveObservationMode),
        m_solveRadius(src.m_solveRadius),
        m_updateCubeLabel(src.m_updateCubeLabel),
        m_errorPropagation(src.m_errorPropagation),
        m_outlierRejection(src.m_outlierRejection),
        m_outlierRejectionMultiplier(src.m_outlierRejectionMultiplier),
        m_globalLatitudeAprioriSigma(src.m_globalLatitudeAprioriSigma),
        m_globalLongitudeAprioriSigma(src.m_globalLongitudeAprioriSigma),
        m_globalRadiusAprioriSigma(src.m_globalRadiusAprioriSigma),
        m_observationSolveSettings(src.m_observationSolveSettings),
        m_convergenceCriteria(src.m_convergenceCriteria),
        m_convergenceCriteriaThreshold(src.m_convergenceCriteriaThreshold),
        m_convergenceCriteriaMaximumIterations(src.m_convergenceCriteriaMaximumIterations),
        m_maximumLikelihood(src.m_maximumLikelihood),
        m_outputFilePrefix(src.m_outputFilePrefix),
        m_createBundleOutputFile(src.m_createBundleOutputFile),
        m_createCSVFiles(src.m_createCSVFiles),
        m_createResidualsFile(src.m_createResidualsFile) {
  }



  BundleSettings::~BundleSettings() {    
    delete m_id;
    m_id = NULL;
  }



  BundleSettings &BundleSettings::operator=(const BundleSettings &src) {
    if (&src != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(src.m_id->toString());

      m_validateNetwork = src.m_validateNetwork;
      m_solveMethod = src.m_solveMethod;
      m_solveObservationMode = src.m_solveObservationMode;
      m_solveRadius = src.m_solveRadius;
      m_updateCubeLabel = src.m_updateCubeLabel;
      m_errorPropagation = src.m_errorPropagation;
      m_outlierRejection = src.m_outlierRejection;
      m_outlierRejectionMultiplier = src.m_outlierRejectionMultiplier;
      m_globalLatitudeAprioriSigma = src.m_globalLatitudeAprioriSigma;
      m_globalLongitudeAprioriSigma = src.m_globalLongitudeAprioriSigma;
      m_globalRadiusAprioriSigma = src.m_globalRadiusAprioriSigma;
      m_observationSolveSettings = src.m_observationSolveSettings;
      m_convergenceCriteria = src.m_convergenceCriteria;
      m_convergenceCriteriaThreshold = src.m_convergenceCriteriaThreshold;
      m_convergenceCriteriaMaximumIterations = src.m_convergenceCriteriaMaximumIterations;
      m_maximumLikelihood = src.m_maximumLikelihood;
      m_outputFilePrefix = src.m_outputFilePrefix;
      m_createBundleOutputFile = src.m_createBundleOutputFile;
      m_createCSVFiles = src.m_createCSVFiles;
      m_createResidualsFile = src.m_createResidualsFile;
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
    if ( !IsNullPixel(globalLatitudeAprioriSigma()) ) {
      pvl += PvlKeyword("GlobalLatitudeAprioriSigma", toString(globalLatitudeAprioriSigma()));
    }
    else {
      pvl += PvlKeyword("GlobalLatitudeAprioriSigma", "None");
    }
    if (!IsNullPixel(globalLongitudeAprioriSigma())) {
      pvl += PvlKeyword("GlobalLongitudeAprioriSigma", toString(globalLongitudeAprioriSigma()));
    }
    else {
      pvl += PvlKeyword("GlobalLongitudeAprioriSigma", "None");
    }
    if (m_solveRadius) {
      if ( !IsNullPixel(globalLongitudeAprioriSigma()) ) {
      pvl += PvlKeyword("GlobalRadiiAprioriSigma", toString(globalRadiusAprioriSigma()));
      }
      else {
        pvl += PvlKeyword("GlobalRadiiAprioriSigma", "None");
      }
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
      BundleObservationSolveSettings boss = observationSolveSettings(i);
      PvlObject bundleObsSolveSettingsPvl = boss.pvlObject();
      pvl += bundleObsSolveSettingsPvl;
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
  void BundleSettings::save(QXmlStreamWriter &stream, const Project *project) const {   // TODO: does xml stuff need project???
//#if 0
    // option 2
    stream.writeStartElement("bundleSettings");

    stream.writeStartElement("globalSettings");

    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("validateNetwork", toString(validateNetwork()));
    
    stream.writeStartElement("solveOptions");
    stream.writeAttribute("solveMethod", solveMethodToString(solveMethod()));
    stream.writeAttribute("solveObservationMode", toString(solveObservationMode()));
    stream.writeAttribute("solveRadius", toString(solveRadius()));
    stream.writeAttribute("updateCubeLabel", toString(updateCubeLabel()));
    stream.writeAttribute("errorPropagation", toString(errorPropagation()));
    stream.writeEndElement();

    stream.writeStartElement("aprioriSigmas");
    if (IsNullPixel(globalLatitudeAprioriSigma())) {
      stream.writeAttribute("latitude", "N/A");
    }
    else {
      stream.writeAttribute("latitude", toString(globalLatitudeAprioriSigma()));
    }
    if (IsNullPixel(globalLongitudeAprioriSigma())) {
      stream.writeAttribute("longitude", "N/A");
    }
    else {
      stream.writeAttribute("longitude", toString(globalLongitudeAprioriSigma()));
    }
    if (IsNullPixel(globalRadiusAprioriSigma())) {
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
    stream.writeAttribute("createBundleOutputFile", toString(createBundleOutputFile()));
    stream.writeAttribute("createCSVFiles", toString(createCSVFiles()));
    stream.writeAttribute("createResidualsFile", toString(createResidualsFile()));
    stream.writeEndElement();
    
    stream.writeEndElement(); // end global settings

    if (!m_observationSolveSettings.isEmpty()) {
      stream.writeStartElement("observationSolveSettingsList");
      for (int i = 0; i < m_observationSolveSettings.size(); i++) {
        m_observationSolveSettings[i].save(stream, project);   // TODO: does xml stuff need project???
      }
      stream.writeEndElement();
    }
    else {
      // throw error??? should not write if no observation settings... 
    }
//#endif



#if 0
    // option 3
    stream.writeStartDocument("1.0");
    //stream.writeStartDocument("BundleSettingsOption3");
    stream.writeStartElement("globalSettings");

    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("validateNetwork", toString(validateNetwork()));
    
    stream.writeStartElement("solveOptions");
    stream.writeTextElement("solveMethod", solveMethodToString(solveMethod()));
    stream.writeTextElement("solveObservationMode", toString(solveObservationMode()));
    stream.writeTextElement("solveRadius", toString(solveRadius()));
    stream.writeTextElement("updateCubeLabel", toString(updateCubeLabel()));
    stream.writeTextElement("errorPropagation", toString(errorPropagation()));
    stream.writeEndElement();

    stream.writeStartElement("aprioriSigmas");
    stream.writeTextElement("latitude", toString(globalLatitudeAprioriSigma()));
    stream.writeTextElement("longitude", toString(globalLongitudeAprioriSigma()));
    if (solveRadius()) {
      stream.writeTextElement("radius", toString(globalRadiusAprioriSigma()));
    }
    else {
      stream.writeTextElement("radius", "N/A");
    }
    stream.writeEndElement();
    
    stream.writeStartElement("outlierRejectionOptions");
    stream.writeTextElement("rejection", toString(outlierRejection()));
    if (outlierRejection()) {
      stream.writeTextElement("multiplier", toString(outlierRejectionMultiplier()));
    }
    else {
      stream.writeTextElement("multiplier", "N/A");
    }
    stream.writeEndElement();
    
    stream.writeStartElement("convergenceCriteriaOptions");
    stream.writeTextElement("convergenceCriteria", convergenceCriteriaToString(convergenceCriteria()));
    stream.writeTextElement("threshold", toString(convergenceCriteriaThreshold()));
    stream.writeTextElement("maximumIterations", toString(convergenceCriteriaMaximumIterations()));
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
    stream.writeTextElement("fileNamePrefix", outputFilePrefix());
    stream.writeTextElement("createBundleOutputFile", toString(createBundleOutputFile()));
    stream.writeTextElement("createCSVFiles", toString(createCSVFiles()));
    stream.writeTextElement("createResidualsFile", toString(createResidualsFile()));
    stream.writeEndElement();
    
    stream.writeEndElement(); // end global settings

    stream.writeStartElement("observationSolveSettingsList");
    stream.writeAttribute("listSize", toString(numberSolveSettings()));
    for (int i = 0; i < m_observationSolveSettings.size(); i++) {
      stream.writeStartElement("observationSolveSettings");
      m_observationSolveSettings[i].save(stream, project);  // TODO: does xml stuff need project???
      stream.writeEndElement();
    }
    stream.writeEndElement();
#endif
    stream.writeEndElement();
  }



  /**
   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
   *   BundleSettings::save() for the expected format.
   *
   * @param bundleSettings The BundleSettings we're going to be initializing
   * @param imageFolder The folder that contains the Cube
   */
  BundleSettings::XmlHandler::XmlHandler(BundleSettings *bundleSettings, Project *project) {  // TODO: does xml stuff need project???
    m_xmlHandlerBundleSettings = bundleSettings;
    m_xmlHandlerProject = project;  // TODO: does xml stuff need project???
    m_xmlHandlerCharacters = "";
    m_xmlHandlerObservationSettings.clear();
  }



//  /**
//   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
//   *   BundleSettings::save() for the expected format.
//   *
//   * @param bundleSettings The BundleSettings we're going to be initializing
//   * @param imageFolder The folder that contains the Cube
//   */
//  BundleSettings::XmlHandler::XmlHandler(BundleSettings *bundleSettings) {
//    m_xmlHandlerBundleSettings = bundleSettings;
//    m_xmlHandlerProject = NULL;  // TODO: does xml stuff need project???
//    m_xmlHandlerCharacters = "";
//    m_xmlHandlerObservationSettings.clear();
//  }



  BundleSettings::XmlHandler::~XmlHandler() {
    // do not delete these pointers...
    // we don't own them, do we??? project passed into StatCumProbDistDynCalc constructor as pointer and bundleSettings = this
    // delete m_xmlHandlerProject;   TODO: does xml stuff need project???
    m_xmlHandlerProject = NULL;
  }



  /**
   * Handle an XML start element. This method is called when the reader finds an open tag.
   * handle the read when the startElement with the name localName has been found.
   * 
   * @param qName The qualified name of the tag.
   * @param attributes The list of attributes for the tag.
   * @return If we should continue reading the XML (usually true).
   */
  bool BundleSettings::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                                const QString &qName, const QXmlAttributes &attributes) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, attributes)) {

      if (localName == "solveOptions") {

        QString solveMethodStr = attributes.value("solveMethod");
        if (!solveMethodStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveMethod = stringToSolveMethod(solveMethodStr);
        }

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
      }
      else if (localName == "aprioriSigmas") {

        QString globalLatitudeAprioriSigmaStr = attributes.value("latitude");
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
            m_xmlHandlerBundleSettings->m_outlierRejectionMultiplier = toDouble(outlierRejectionMultiplierStr);
          }
          else {
            m_xmlHandlerBundleSettings->m_outlierRejectionMultiplier = 1.0;
          }
        }
      }
      else if (localName == "convergenceCriteriaOptions") {

        QString convergenceCriteriaStr = attributes.value("convergenceCriteria");
        if (!convergenceCriteriaStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteria = stringToConvergenceCriteria(convergenceCriteriaStr);
        }

        QString convergenceCriteriaThresholdStr = attributes.value("threshold");
        if (!convergenceCriteriaThresholdStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaThreshold = toDouble(convergenceCriteriaThresholdStr);
        }

        QString convergenceCriteriaMaximumIterationsStr = attributes.value("maximumIterations");
        if (!convergenceCriteriaMaximumIterationsStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaMaximumIterations = toInt(convergenceCriteriaMaximumIterationsStr);
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

        QString createBundleOutputFileStr = attributes.value("createBundleOutputFile");
        if (!createBundleOutputFileStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createBundleOutputFile = toBool(createBundleOutputFileStr);
        }

        QString createCSVFilesStr = attributes.value("createCSVFiles");
        if (!createCSVFilesStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createCSVFiles = toBool(createCSVFilesStr);
        }

        QString createResidualsFileStr = attributes.value("createResidualsFile");
        if (!createResidualsFileStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createResidualsFile = toBool(createResidualsFileStr);
        }
      }
      else if (localName == "bundleObservationSolveSettings") {
        m_xmlHandlerObservationSettings.append(
            new BundleObservationSolveSettings(m_xmlHandlerProject, reader()));
      }
    }
    return true;
  }



  bool BundleSettings::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool BundleSettings::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
    if (!m_xmlHandlerCharacters.isEmpty()) {
      if (localName == "id") {
        m_xmlHandlerBundleSettings->m_id = NULL;
        m_xmlHandlerBundleSettings->m_id = new QUuid(m_xmlHandlerCharacters);
      }
      else if (localName == "validateNetwork") {
        m_xmlHandlerBundleSettings->m_validateNetwork = toBool(m_xmlHandlerCharacters);
      }
      else if (localName == "observationSolveSettingsList") {
        for (int i = 0; i < m_xmlHandlerObservationSettings.size(); i++) {
          m_xmlHandlerBundleSettings->m_observationSolveSettings.append(*m_xmlHandlerObservationSettings[i]);
        }
        m_xmlHandlerObservationSettings.clear();
      }
  
      m_xmlHandlerCharacters = "";
    }
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }



  bool BundleSettings::XmlHandler::fatalError(const QXmlParseException &exception) {
    qDebug() << "Parse error at line " << exception.lineNumber()
             << ", " << "column " << exception.columnNumber() << ": "
             << qPrintable(exception.message());
    return false;
  }



  QDataStream &BundleSettings::write(QDataStream &stream) const {

    stream << m_id->toString()
           << m_validateNetwork
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

    QString id;
    qint32 solveMethod, convergenceCriteria, convergenceCriteriaMaximumIterations;

    stream >> id 
           >> m_validateNetwork
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

    delete m_id;
    m_id = NULL;
    m_id = new QUuid(id);

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


  void BundleSettings::createH5Group(H5::CommonFG &locationObject, QString locationName) const {
    try {
      // Try block to detect exceptions raised by any of the calls inside it
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
//        H5::Exception::dontPrint();

        // create a settings group to add to the given H5 object
        QString settingsGroupName = locationName + "/BundleSettings"; 
        H5::Group settingsGroup = locationObject.createGroup(settingsGroupName.toAscii());

        // use H5S_SCALAR data space type for single valued spaces
        H5::DataSpace spc;
        Attribute att;
        #if 0
        hsize_t dims[1] = {2};
        H5::DataSpace simple1x2Space(1, dims);
        dims[0] = 1;
        H5::DataSpace simple1DSpace(1, dims);

        hsize_t dims2D[2] = {{2}, {2}};
        H5::DataSpace simple2x2Space(2, dims);
        dims2D[2] = {1, 1};
        H5::DataSpace simple2x1Space(2, dims);
        #endif

        /*
         * Add bool attributes as predefined data type PredType::NATIVE_HBOOL
         */ 
        int intFromBool = (int)m_validateNetwork;
        att = settingsGroup.createAttribute("validateNetwork", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_solveObservationMode;
        att = settingsGroup.createAttribute("solveObservationMode", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_solveRadius;
        att = settingsGroup.createAttribute("solveRadius", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_updateCubeLabel;
        att = settingsGroup.createAttribute("updateCubeLabel", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_errorPropagation;
        att = settingsGroup.createAttribute("errorPropagation", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_outlierRejection;
        att = settingsGroup.createAttribute("outlierRejection", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_createBundleOutputFile;
        att = settingsGroup.createAttribute("createBundleOutputFile", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_createCSVFiles;
        att = settingsGroup.createAttribute("createCSVFiles", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        intFromBool = (int)m_createResidualsFile;
        att = settingsGroup.createAttribute("createResidualsFile", PredType::NATIVE_HBOOL, spc);
        att.write(PredType::NATIVE_HBOOL, &intFromBool);

        /* 
         * Add enum attributes as predefined data type PredType::C_S1 (string)
         */ 
        QString enumToStringValue = "";
        int stringSize = 0;
        H5::StrType strDataType;

        enumToStringValue = solveMethodToString(m_solveMethod);
        stringSize = enumToStringValue.length();
        strDataType = H5::StrType(PredType::C_S1, stringSize); 
        att = settingsGroup.createAttribute("solveMethod", strDataType, spc);
        att.write(strDataType, enumToStringValue.toStdString());

        enumToStringValue = convergenceCriteriaToString(m_convergenceCriteria);
        stringSize = enumToStringValue.length();
        strDataType = H5::StrType(PredType::C_S1, stringSize); 
        att = settingsGroup.createAttribute("convergenceCriteria", strDataType, spc);
        att.write(strDataType, enumToStringValue.toStdString());

        /* 
         * Add string attributes as predefined data type PredType::C_S1 (string)
         */ 
        stringSize = qMax(m_outputFilePrefix.length(), 1); // if empty string, set size to 1
        strDataType = H5::StrType(PredType::C_S1, stringSize); 
        att = settingsGroup.createAttribute("outputFilePrefix", strDataType, spc);
        att.write(strDataType, m_outputFilePrefix.toStdString());

        /* 
         * Add double attributes as predefined data type PredType::NATIVE_DOUBLE
         */ 
        att = settingsGroup.createAttribute("outlierRejectionMultiplier",
                                            PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(PredType::NATIVE_DOUBLE, &m_outlierRejectionMultiplier);

        att = settingsGroup.createAttribute("globalLatitudeAprioriSigma", 
                                            PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(PredType::NATIVE_DOUBLE, &m_globalLatitudeAprioriSigma);

        att = settingsGroup.createAttribute("globalLongitudeAprioriSigma", 
                                            PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(PredType::NATIVE_DOUBLE, &m_globalLongitudeAprioriSigma);

        att = settingsGroup.createAttribute("globalRadiusAprioriSigma", 
                                            PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(PredType::NATIVE_DOUBLE, &m_globalRadiusAprioriSigma);

        att = settingsGroup.createAttribute("convergenceCriteriaThreshold", 
                                            PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(PredType::NATIVE_DOUBLE, &m_convergenceCriteriaThreshold);

        /* 
         * Add integer attributes as predefined data type PredType::NATIVE_INT
         */ 
        att = settingsGroup.createAttribute("convergenceCriteriaMaximumIterations", 
                                            PredType::NATIVE_INT,
                                            spc);
        att.write(PredType::NATIVE_INT, &m_convergenceCriteriaMaximumIterations);

        // Data sets??? tables???
        // ??? QList<BundleObservationSolveSettings> m_observationSolveSettings; // TODO: pointer???
        // ??? QList< QPair< MaximumLikelihoodWFunctions::Model, double > > m_maximumLikelihood; // TODO: pointer???
//        return locationObject; 
      }
      // catch failure caused by the Attribute operations
      catch( H5::AttributeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 ATTRIBUTE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSet operations
      catch( H5::DataSetIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SET exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSpace operations
      catch( H5::DataSpaceIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SPACE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataType operations
      catch( H5::DataTypeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA TYPE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the H5File operations
      catch( H5::FileIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 FILE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the Group operations
      catch( H5::GroupIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GROUP exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GENERAL exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    } 
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown, 
                       "Unable to save bundle settings information to an HDF5 group.",
                       _FILEINFO_);
    }
  }


  void BundleSettings::openH5Group(H5::CommonFG &locationObject, QString locationName) {
    try {
      // Try block to detect exceptions raised by any of the calls inside it
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
//        H5::Exception::dontPrint();

        // create a settings group to add to the given H5 object
        QString settingsGroupName = locationName + "/BundleSettings"; 
        H5::Group settingsGroup = locationObject.openGroup(settingsGroupName.toAscii());

        Attribute att;

        /* 
         * read bool attributes as predefined data type PredType::NATIVE_HBOOL
         */ 
        int boolAttValue = 0;
        att = settingsGroup.openAttribute("validateNetwork");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_validateNetwork = (bool)boolAttValue;

        att = settingsGroup.openAttribute("solveObservationMode");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_solveObservationMode = (bool)boolAttValue;

        att = settingsGroup.openAttribute("solveRadius");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_solveRadius = (bool)boolAttValue;

        att = settingsGroup.openAttribute("updateCubeLabel");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_updateCubeLabel = (bool)boolAttValue;

        att = settingsGroup.openAttribute("errorPropagation");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_errorPropagation = (bool)boolAttValue;

        att = settingsGroup.openAttribute("outlierRejection");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_outlierRejection = (bool)boolAttValue;

        att = settingsGroup.openAttribute("createBundleOutputFile");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_createBundleOutputFile = (bool)boolAttValue;

        att = settingsGroup.openAttribute("createCSVFiles");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_createCSVFiles = (bool)boolAttValue;

        att = settingsGroup.openAttribute("createResidualsFile");
        att.read(PredType::NATIVE_HBOOL, &boolAttValue);
        m_createResidualsFile = (bool)boolAttValue;

        /* 
         * read enum attributes as predefined data type PredType::C_S1 (string)
         */ 
        H5std_string strAttValue;
        H5::StrType strDataType;

        att = settingsGroup.openAttribute("solveMethod");
        strDataType = H5::StrType(PredType::C_S1, att.getStorageSize());
        att.read(strDataType, strAttValue);
        m_solveMethod = stringToSolveMethod(QString::fromStdString(strAttValue));

        att = settingsGroup.openAttribute("convergenceCriteria");
        strDataType = H5::StrType(PredType::C_S1, att.getStorageSize());
        att.read(strDataType, strAttValue);
        m_convergenceCriteria = stringToConvergenceCriteria(QString::fromStdString(strAttValue));

        /* 
         * read string attributes as predefined data type PredType::C_S1 (string)
         */ 
        att = settingsGroup.openAttribute("outputFilePrefix");
        strDataType = H5::StrType(PredType::C_S1, att.getStorageSize());
        att.read(strDataType, strAttValue);
        m_outputFilePrefix = QString::fromStdString(strAttValue);

        /* 
         * read double attributes as predefined data type PredType::NATIVE_DOUBLE
         */ 
        att = settingsGroup.openAttribute("outlierRejectionMultiplier");
        att.read(PredType::NATIVE_DOUBLE, &m_outlierRejectionMultiplier);

        att = settingsGroup.openAttribute("globalLatitudeAprioriSigma");
        att.read(PredType::NATIVE_DOUBLE, &m_globalLatitudeAprioriSigma);

        att = settingsGroup.openAttribute("globalLongitudeAprioriSigma");
        att.read(PredType::NATIVE_DOUBLE, &m_globalLongitudeAprioriSigma);

        att = settingsGroup.openAttribute("globalRadiusAprioriSigma");
        att.read(PredType::NATIVE_DOUBLE, &m_globalRadiusAprioriSigma);

        att = settingsGroup.openAttribute("convergenceCriteriaThreshold");
        att.read(PredType::NATIVE_DOUBLE, &m_convergenceCriteriaThreshold);

        /* 
         * read int attributes as predefined data type PredType::NATIVE_INT
         */ 
        att = settingsGroup.openAttribute("convergenceCriteriaMaximumIterations");
        att.read(PredType::NATIVE_INT, &m_convergenceCriteriaMaximumIterations);

        // Data sets??? tables???
        // ??? QList<BundleObservationSolveSettings> m_observationSolveSettings; // TODO: pointer???
        // ??? QList< QPair< MaximumLikelihoodWFunctions::Model, double > > m_maximumLikelihood; // TODO: pointer???
      }
      // catch failure caused by the Attribute operations
      catch( H5::AttributeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 ATTRIBUTE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSet operations
      catch( H5::DataSetIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SET exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSpace operations
      catch( H5::DataSpaceIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SPACE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataType operations
      catch( H5::DataTypeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA TYPE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the H5File operations
      catch( H5::FileIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 FILE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the Group operations
      catch( H5::GroupIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GROUP exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GENERAL exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    } 
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown, 
                       "Unable to read bundle settings information from an HDF5 group.",
                       _FILEINFO_);
    }
  }

}
