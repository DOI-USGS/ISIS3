#include "BundleSettings.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
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
    m_globalLatitudeAprioriSigma = globalLatitudeAprioriSigma;
    m_globalLongitudeAprioriSigma = globalLongitudeAprioriSigma;
    if (m_solveRadius) {
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
    stream.writeAttribute("latitude", toString(globalLatitudeAprioriSigma()));
    stream.writeAttribute("longitude", toString(globalLongitudeAprioriSigma()));
    if (solveRadius()) {
      stream.writeAttribute("radius", toString(globalRadiusAprioriSigma()));
    }
    else {
      stream.writeAttribute("radius", "N/A");
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
   * @param atts The list of attributes for the tag.
   * @return If we should continue reading the XML (usually true).
   */
  bool BundleSettings::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                       const QString &qName, const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {

      if (localName == "solveOptions") {
        
        QString solveMethodStr = atts.value("solveMethod");
        if (!solveMethodStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveMethod = stringToSolveMethod(solveMethodStr);
        }

        QString solveObservationModeStr = atts.value("solveObservationMode");
        if (!solveObservationModeStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveObservationMode = toBool(solveObservationModeStr);
        }

        QString solveRadiusStr = atts.value("solveRadius");
        if (!solveRadiusStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_solveRadius = toBool(solveRadiusStr);
        }

        QString updateCubeLabelStr = atts.value("updateCubeLabel");
        if (!updateCubeLabelStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_updateCubeLabel = toBool(updateCubeLabelStr);
        }

        QString errorPropagationStr = atts.value("errorPropagation");
        if (!errorPropagationStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_errorPropagation = toBool(errorPropagationStr);
        }
      }
      else if (localName == "aprioriSigmas") {

        QString globalLatitudeAprioriSigmaStr = atts.value("latitude");
        if (!globalLatitudeAprioriSigmaStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_globalLatitudeAprioriSigma = toDouble(globalLatitudeAprioriSigmaStr);
        }

        QString globalLongitudeAprioriSigmaStr = atts.value("longitude");
        if (!globalLongitudeAprioriSigmaStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_globalLongitudeAprioriSigma =
              toDouble(globalLongitudeAprioriSigmaStr);
        }

        QString globalRadiusAprioriSigmaStr = atts.value("radius");
        if (!globalRadiusAprioriSigmaStr.isEmpty()) {
          if (globalRadiusAprioriSigmaStr != "N/A") {
            m_xmlHandlerBundleSettings->m_globalRadiusAprioriSigma = toDouble(globalRadiusAprioriSigmaStr);
          }
          else {
            m_xmlHandlerBundleSettings->m_globalRadiusAprioriSigma = Isis::Null;
          }
        }
      }
      else if (localName == "outlierRejectionOptions") {
        QString outlierRejectionStr = atts.value("rejection");
        if (!outlierRejectionStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_outlierRejection = toBool(outlierRejectionStr);
        }

        QString outlierRejectionMultiplierStr = atts.value("multiplier");
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

        QString convergenceCriteriaStr = atts.value("convergenceCriteria");
        if (!convergenceCriteriaStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteria = stringToConvergenceCriteria(convergenceCriteriaStr);
        }

        QString convergenceCriteriaThresholdStr = atts.value("threshold");
        if (!convergenceCriteriaThresholdStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaThreshold = toDouble(convergenceCriteriaThresholdStr);
        }

        QString convergenceCriteriaMaximumIterationsStr = atts.value("maximumIterations");
        if (!convergenceCriteriaMaximumIterationsStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_convergenceCriteriaMaximumIterations = toInt(convergenceCriteriaMaximumIterationsStr);
        }
      }
      else if (localName == "model") {
        QString type = atts.value("type");
        QString quantile = atts.value("quantile");
        if (!type.isEmpty() && !quantile.isEmpty()) {
        m_xmlHandlerBundleSettings->m_maximumLikelihood.append(
              qMakePair(MaximumLikelihoodWFunctions::stringToModel(type),
                        toDouble(quantile)));
        }
      }
      else if (localName == "outputFileOptions") {
        QString outputFilePrefixStr = atts.value("fileNamePrefix");
        if (!outputFilePrefixStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_outputFilePrefix = outputFilePrefixStr;
        }

        QString createBundleOutputFileStr = atts.value("createBundleOutputFile");
        if (!createBundleOutputFileStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createBundleOutputFile = toBool(createBundleOutputFileStr);
        }

        QString createCSVFilesStr = atts.value("createCSVFiles");
        if (!createCSVFilesStr.isEmpty()) {
          m_xmlHandlerBundleSettings->m_createCSVFiles = toBool(createCSVFilesStr);
        }

        QString createResidualsFileStr = atts.value("createResidualsFile");
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

  void BundleSettings::savehdf5(hid_t fileId, H5::Group settingsGroup) const {
  }
  void BundleSettings::savehdf5(hid_t settingsGroupId, QString objectName) const {
  #if 0
    
    // Try block to detect exceptions raised by any of the calls inside it
    try {
      /*
       * Turn off the auto-printing when failure occurs so that we can
       * handle the errors appropriately
       */
      H5::Exception::dontPrint();

    // passed in
    //   H5::Group bundleSettingsGroup = H5::Group(hdfFile.createGroup("/BundleSettings"));
      hsize_t dims[2];              // dataset dimensions

      //TODO: finish Correlation Matrix
      //Create a dataset with compression
      hid_t correlationGroupId = H5Gcreate(settingsGroupId, "/BundleSolutionInfo/BundleSettings/CorrelationMatrix", 0);//??? 
      QString location = objectName + "/CorrelationMatrix";
      H5LTset_attribute_string(correlationGroupId, location.toAscii(), "correlationFileName", 
                               correlationMatrix().correlationFileName().expanded());
      H5LTset_attribute_string(correlationGroupId, location.toAscii(), "covarianceFileName", 
                               correlationMatrix().covarianceFileName().expanded());
      //  // TODO: jb - how do we add
    //  // correlationMatrix().imagesAndParameters()???
    //  // QMapIterator<QString, QStringList> a list of images with their
    //  // corresponding parameters...
    //
    //  
    //  // H5::Group generalStatsInfoGroup = H5::Group(hdfFile.createGroup("/GeneralStatisticsInfo"));
    //  const int numFixedPoints = numberFixedPoints();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberFixedPoints", &numFixedPoints, 1);
    //  const int numIgnoredPoints = numberIgnoredPoints();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberIgnoredPoints", &numIgnoredPoints, 1);
    //  const int numHeldImages = numberHeldImages();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberHeldImages", &numHeldImages, 1);
    //  const double rejectionLimit = rejectionLimit();
    //  H5LTset_attribute_double(fileId, "/BundleSettings", "rejectionLimit", &rejectionLimit, 1);
    //  const int numRejectedObservations = numberRejectedObservations();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberRejectedObservations", &numRejectedObservations, 1);
    //  const int numObservations = numberObservations();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberObservations", &numObservations, 1);
    //  const int numImageParameters = numberImageParameters();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberImageParameters", &numImageParameters, 1);
    //  const int numConstrainedPointParameters = numberConstrainedPointParameters();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberConstrainedPointParameters", 
    //                        &numConstrainedPointParameters, 1);
    //  const int numConstrainedImageParameters = numberConstrainedImageParameters();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberConstrainedImageParameters", 
    //                        &numConstrainedImageParameters, 1);
    //  const int numUnknownParameters = numberUnknownParameters();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "numberUnknownParameters", &numUnknownParameters, 1);
    //  const int degreesOfFreedom = degreesOfFreedom();
    //  H5LTset_attribute_int(fileId, "/BundleSettings", "degreesOfFreedom", &degreesOfFreedom, 1);
    //  const double sigma0 = sigma0();
    //  H5LTset_attribute_double(fileId, "/BundleSettings", "sigma0", &sigma0, 1);
    //  bool converged = converged();// ???
    //  H5LTset_attribute_string(fileId, "/BundleSettings", "converged", toString(converged), 1);
    //  
    //  
    //  // Create an RMS group in the file
    //  H5::Group rms = H5::Group(hdfFile.createGroup("/RMS"));
    //  H5::Group rms = H5::Group(hdfFile.createGroup("/RMS/residuals"));
    //  // RMS and Sigma Scalars
    //  H5LTset_attribute_double(fileId, "/RMS/residuals", "x", rmsRx(), 1);
    //  H5LTset_attribute_double(fileId, "/RMS/residuals", "y", rmsRy(), 1);
    //  H5LTset_attribute_double(fileId, "/RMS/residuals", "xy", rmsRxy(), 1);
    //  H5::Group rms = H5::Group(hdfFile.createGroup("/RMS/sigmas"));
    //  const double sigmas [3] = {rmsSigmaLat(), rmsSigmaLon(), rmsSigmaRad()};
    //  H5LTset_attribute_double(fileId, "/RMS/sigmas", "latitude", rmsSigmaLat(), 1);
    //  H5LTset_attribute_double(fileId, "/RMS/sigmas", "longitude", rmsSigmaLon(), 1);
    //  H5LTset_attribute_double(fileId, "/RMS/sigmas", "radius", rmsSigmaRad(), 1);
    //  // RMS and Sigma Vectors - This is a ton of duplicate code where I would rather use a list of functions - are functions first class objects?
    //  const hsize_t residualsArraySize = rmsImageResiduals.size();
    //  H5::DataSpace residualDataspace(1, &residualsArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/ResidualsList", H5::PredType::NATIVE_FLOAT, residualDataspace));
    //  dataset.write(m_rmsImageResiduals, H5::PredType::NATIVE_FLOAT);
    //  // Write the residual vector
    //  const hsize_t sampleArraySize = rmsImageSampleResiduals.size();
    //  H5::DataSpace sampleDataspace(1, &sampleArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/SampleList", H5::PredType::NATIVE_FLOAT, sampleDataspace));
    //  dataset.write(m_rmsImageSampleResiduals, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t lineArraySize = rmsImageLineResiduals.size();
    //  H5::DataSpace lineDataSpace(1,&lineArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/LineList", H5::PredType::NATIVE_FLOAT, lineDataSpace));
    //  dataset.write(m_rmsImageLineResiduals, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t xSigmasArraySize = rmsImageXSigmas.size();
    //  H5::DataSpace xSigmaDataspace(1, &xSigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/xSigmas", H5::PredType::NATIVE_FLOAT, xSigmaDataspace));
    //  dataset.write(m_rmsImageXSigmas, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t ySigmasArraySize = rmsImageYSigmas.size();
    //  H5::DataSpace ySigmaDataspace(1, &ySigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/ySigmas", H5::PredType::NATIVE_FLOAT, ySigmaDataspace));
    //  dataset.write(m_rmsImageYSigmas, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t zSigmasArraySize = rmsImageZSigmas.size();
    //  H5::DataSpace zSigmaDataspace(1, &zSigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/zSigmas", H5::PredType::NATIVE_FLOAT, zSigmaDataspace));
    //  dataset.write(m_rmsImageZSigmas, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t raSigmasArraySize = rmsImageRASigmas.size();
    //  H5::DataSpace raSigmaDataspace(1, &raSigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/raSigmas", H5::PredType::NATIVE_FLOAT, raSigmaDataspace));
    //  dataset.write(m_rmsImageRASigmas, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t decSigmasArraySize = rmsImageDECSigmas.size();
    //  H5::DataSpace decSigmaDataspace(1, &decSigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/decSigmas", H5::PredType::NATIVE_FLOAT, decSigmaDataspace));
    //  dataset.write(m_rmsImageDECSigmas, H5::PredType::NATIVE_FLOAT);
    //  const hsize_t twistSigmasArraySize = rmsImageTWISTSigmas.size();
    //  H5::DataSpace twistSigmaDataspace(1, &twistSigmasArraySize);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/RMS/twistSigmas", H5::PredType::NATIVE_FLOAT, twistSigmaDataspace));
    //  dataset.write(m_rmsImageTWISTSigmas, H5::PredType::NATIVE_FLOAT);
    //  
    //  //Write elapsed time and error prop as attirbutes tagged to the root
    //  const double elapsedTime = elapsedTime();
    //  H5LTset_attribute_double(fileId, "/BundleSettings", "elapsedTime", &elapsedTime, 1);
    //  const double errorProp = lapsedTimeErrorProp();
    //  H5LTset_attribute_double(fileId, "/BundleSettings", "elapsedTimeErrorProp", &errorProp, 1);
    //  
    //  //Write a sigmas table
    //  static m_sigmaTable sigmatable[6] = { // JB - why static???
    //      {"minLat", minSigmaLatitudePointId(), minSigmaLatitude()},
    //      {"maxLat", maxSigmaLatitudePointId(), maxSigmaLatitude()},
    //      {"minLon", minSigmaLongitudePointId(), minSigmaLongitude()},
    //      {"MaxLon", maxSigmaLongitudePointId(), maxSigmaLongitude()},
    //      {"minRad", minSigmaRadiusPointId(), minSigmaRadius()},
    //      {"maxRad", maxSigmaRadiusPointId(), maxSigmaRadius()}
    //  };
    //  
    //  const int nfields = 3; //How many columns and their types - must be const so that field names can be created
    //  
    //  size_t part_offset[nfields] = {HOFFSET(m_sigmaTable, type),
    //      HOFFSET(m_sigmaTable, pid),
    //      HOFFSET(m_sigmaTable, value)};
    //  
    //  //Field names and types
    //  hid_t field_type[nfields];  //Setup for a string type for the pid
    //  hid_t string_type = H5Tcopy(H5T_C_S1);
    //  H5Tset_size(string_type, (size_t)64);
    //  field_type[0] = string_type;  //type
    //  field_type[1] = string_type; //pid
    //  field_type[2] = H5T_NATIVE_FLOAT;
    //  
    //  //How many rows?
    //  int nrecords = 6;
    //  // Field names
    //  const char *fieldnames[nfields] = {"valuetype", "pid", "value"};
    //  
    //  hsize_t chunksize = 10;
    //  int *filldata = NULL;
    //  int compress = 0;
    //  
    //  H5TBmake_table("MinMaxSigma",fileId, "minmaxsigma", (hsize_t)nfields, (hsize_t)nrecords, sizeof(m_sigmaTable), fieldnames, part_offset, field_type, chunksize, filldata, compress, sigmatable);
    //  
    //  
    //  H5::Group mlestatistics = H5::Group(hdfFile.createGroup("/MLEstimation"));
    //  //TODO: ML Estimation Items
    //  //The existing code iterates through - is this stored as a matrix somewhere?
    //  // Dimensions
    //  dims[1] = 4;
    //  dims[0]  = 100;
    //  
    //  H5::DataSpace cumProbDataspace( RANK, dims );
    //  H5::DataSet dataset = H5::DataSet(hdfFile.createDataSet("/MLEstimation/cumulativeProbabilityCalculator", H5::PredType::NATIVE_FLOAT, cumProbDataspace));
    //  //dataset.write(data, H5::PredType::NATIVE_FLAOT);
    //  
    //  
    //  dims[1] = 4;
    //  dims[0] = 100;
    //  H5::DataSpace resCumProbdataspace(RANK, dims);
    //  dataset = H5::DataSet(hdfFile.createDataSet("/MLEstimation/residualsCumulativeProbabilityCalculator", H5::PredType::NATIVE_FLOAT, resCumProbdataspace));
    //  //dataset.write(data, H5::PredType::NATIVE_FLAOT);
    //  
    }  // end of try block
    // catch failure caused by the H5File operations
    catch( H5::FileIException error ) {
      QString msg = QString(error.getCDetailMsg());
      IException hpfError(IException::Unknown, msg, _FILEINFO_);
      msg = "Unable to save BundleResults to hpf5 file. "
            "H5 exception handler has detected a file error.";
      throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
    }
    // catch failure caused by the DataSet operations
    catch( H5::DataSetIException error ) {
      QString msg = QString(error.getCDetailMsg());
      IException hpfError(IException::Unknown, msg, _FILEINFO_);
      msg = "Unable to save BundleResults to hpf5 file. "
            "H5 exception handler has detected a data set error.";
      throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataSpaceIException error ) {
      QString msg = QString(error.getCDetailMsg());
      IException hpfError(IException::Unknown, msg, _FILEINFO_);
      msg = "Unable to save BundleResults to hpf5 file. "
            "H5 exception handler has detected a data space error.";
      throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataTypeIException error ) {
      QString msg = QString(error.getCDetailMsg());
      IException hpfError(IException::Unknown, msg, _FILEINFO_);
      msg = "Unable to save BundleResults to hpf5 file. "
            "H5 exception handler has detected a data type error.";
      throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
    }
    return;
      
#endif
  }
}
