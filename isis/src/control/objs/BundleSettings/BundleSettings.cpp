#include "BundleSettings.h"

#include "BundleObservationSolveSettings.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "PvlGroup.h"

namespace Isis {

  /**
   * constructor
   */
  BundleSettings::BundleSettings() {
    m_validateNetwork = true;

    m_solveMethod = Sparse;
    m_solveObservationMode = false;
    m_solveRadius          = false;
    m_updateCubeLabel      = false;
    m_errorPropagation     = false;

    m_outlierRejection     = false;
    m_outlierRejectionMultiplier = 3.0; // default to rejection = false, i.e. multiplier = 1.0      ???

    // Convergence Criteria
    m_convergenceCriteria = BundleSettings::Sigma0;
    m_convergenceCriteriaThreshold = 1.0e-10;
    m_convergenceCriteriaMaximumIterations = 50;

    // Parameter Uncertainties (Weighting)
    m_globalLatitudeAprioriSigma = -1.0;
    m_globalLongitudeAprioriSigma = -1.0;
    m_globalRadiusAprioriSigma = -1.0;

    // Maximum Likelihood Estimation Options no default in the constructor - must be set.
    m_maximumLikelihood.clear();

    // Self Calibration ??? (from cnetsuite only)

    // Target Body ??? (from cnetsuite only)

    // Output Options
    m_outputFilePrefix = "";
    m_createBundleOutputFile = true;
    m_createCSVPointsFile    = true;
    m_createResidualsFile    = true;
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
        m_createCSVPointsFile(other.m_createCSVPointsFile),
        m_createResidualsFile(other.m_createResidualsFile) {
    
    for (int i = 0; i < other.m_maximumLikelihood.size(); i++) {
      m_maximumLikelihood.append(other.m_maximumLikelihood[i]);
    }

    m_observationSolveSettings = other.m_observationSolveSettings;


  }



  BundleSettings::~BundleSettings() {

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
      m_createCSVPointsFile = other.m_createCSVPointsFile;
      m_createResidualsFile = other.m_createResidualsFile;

      for (int i = 0;i < other.m_maximumLikelihood.size();i++) {
        m_maximumLikelihood.append(other.m_maximumLikelihood[i]);
      }

      m_observationSolveSettings = other.m_observationSolveSettings;
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
                       "Unknown bundle solve method " + method + ".",
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



  void BundleSettings::setSolveMethod(SolveMethod method) {
    m_solveMethod = method;
  }



  void BundleSettings::setSolveObservationMode(bool solveObservationMode) {
    m_solveObservationMode = solveObservationMode;
  }



  void BundleSettings::setSolveRadius(bool solveRadius) {
    m_solveRadius = solveRadius;
  }



  void BundleSettings::setUpdateCubeLabel(bool updateCubeLabel) {
    m_updateCubeLabel = updateCubeLabel;
  }



  void BundleSettings::setErrorPropagation(bool errorPropagation) {
    m_errorPropagation = errorPropagation;
  }



  void BundleSettings::setOutlierRejection(bool outlierRejection, double multiplier) {
    m_outlierRejection = outlierRejection;
    m_outlierRejectionMultiplier = multiplier;
  }



  void BundleSettings::setObservationSolveOptions(QVector<BundleObservationSolveSettings*>& observationSolveSettings) {
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

  BundleObservationSolveSettings* BundleSettings::observationSolveSettings(QString instrumentId) {
    int nsolveSettings = m_observationSolveSettings.size();

    for (int i = 0; i < nsolveSettings; i++) {
      if (m_observationSolveSettings.at(i)->instrumentId() == instrumentId) {
        return m_observationSolveSettings.at(i);
        break;
      }
    }

    return NULL;
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
                          "Unknown bundle convergence criteria " + criteria + ".",
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
  MaximumLikelihoodWFunctions::Model BundleSettings::stringToMaximumLikelihoodModel(
      QString model) {
    if (model.compare("HUBER", Qt::CaseInsensitive) == 0) {
      return MaximumLikelihoodWFunctions::Huber;
    }
    else if (model.compare("HUBER_MODIFIED", Qt::CaseInsensitive) == 0) {
      return MaximumLikelihoodWFunctions::HuberModified;
    }
    else if (model.compare("WELSCH", Qt::CaseInsensitive) == 0) {
      return MaximumLikelihoodWFunctions::Welsch;
    }
    else if (model.compare("CHEN", Qt::CaseInsensitive) == 0) {
      return MaximumLikelihoodWFunctions::Chen;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown bundle maximum likelihood model " + model + ".",
                       _FILEINFO_);
    }
  }



  QString BundleSettings::maximumLikelihoodModelToString(
      MaximumLikelihoodWFunctions::Model model) {
    if (model == MaximumLikelihoodWFunctions::Huber)              return "Huber";
    else if (model == MaximumLikelihoodWFunctions::HuberModified) return "HuberModified";
    else if (model == MaximumLikelihoodWFunctions::Welsch)        return "Welsh";
    else if (model == MaximumLikelihoodWFunctions::Chen)          return "Chen";
    else  throw IException(IException::Programmer,
                           "Unknown maximum likelihood model enum [" + toString(model) + "].",
                           _FILEINFO_);
  }



  void BundleSettings::addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Model model, 
                                                          double maxModelCQuantile) {
    if (m_maximumLikelihood.size() == 0 && model > MaximumLikelihoodWFunctions::HuberModified) {
      QString msg = "For bundle adjustments with multiple maximum likelihood estimators, the first "
                    "must be HUBER or HUBER_MODIFIED.";
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
                                      bool createCSVPointsFile, bool createResidualsFile) {
    m_outputFilePrefix = outputFilePrefix;
    m_createBundleOutputFile = createBundleOutputFile;
    m_createCSVPointsFile = createCSVPointsFile;
    m_createResidualsFile = createResidualsFile;
  }



  QString BundleSettings::outputFilePrefix() const {
    return m_outputFilePrefix;
  }



  bool BundleSettings::createBundleOutputFile() const {
    return m_createBundleOutputFile;
  }



  bool BundleSettings::createCSVPointsFile() const {
    return m_createCSVPointsFile;
  }



  bool BundleSettings::createResidualsFile() const {
    return m_createResidualsFile;
  }

  PvlObject BundleSettings::pvlObject(QString name) const {
    PvlObject object(name);

    // General Solve Options
    object += PvlKeyword("SolveMethod", solveMethodToString(m_solveMethod));
    object += PvlKeyword("SolveObservationMode", toString(m_solveObservationMode));
    object += PvlKeyword("SolveRadius", toString(m_solveRadius));
    object += PvlKeyword("UpdateCubeLabel", toString(m_updateCubeLabel));
    object += PvlKeyword("ErrorPropagation", toString(m_errorPropagation));
    object += PvlKeyword("OutlierRejection", toString(m_outlierRejection));
    if (m_outlierRejection) {
      object += PvlKeyword("OutlierMultiplier", toString(m_outlierRejectionMultiplier));
    }
    object += PvlKeyword("GlobalLatitudeAprioriSigma", toString(m_globalLatitudeAprioriSigma));
    object += PvlKeyword("GlobalLongitudeAprioriSigma", toString(m_globalLongitudeAprioriSigma));
    if (m_solveRadius) {
      object += PvlKeyword("GlobalRadiiAprioriSigma", toString(m_globalRadiusAprioriSigma));
    }

    // Convergence Criteria
    object += PvlKeyword("ConvergenceCriteria", convergenceCriteriaToString(m_convergenceCriteria));
    object += PvlKeyword("ConvergenceCriteriaThreshold", toString(m_convergenceCriteriaThreshold));
    object += PvlKeyword("ConvergenceCriteriaMaximumIterations",
                        toString(m_convergenceCriteriaMaximumIterations));

    // Output Options
    object += PvlKeyword("CreateBundleOutputFile", toString(m_createBundleOutputFile));
    object += PvlKeyword("CreateCSVPointsFile", toString(m_createCSVPointsFile));
    object += PvlKeyword("CreateResidualsFile", toString(m_createResidualsFile));
    if (m_createBundleOutputFile || m_createCSVPointsFile || m_createResidualsFile) {
      object += PvlKeyword("FilePrefix", m_outputFilePrefix);
    }

    // Maximum Likelihood Options
    PvlKeyword models("MaximumLikelihoodModels"); 
    if (m_maximumLikelihood.size() > 0) {

      models.addValue(MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihood[0].first));

//      PvlKeyword models("MaximumLikelihoodModels",
//                        MaximumLikelihoodWFunctions::modelToString(m_wFunc[0]->model()));

      PvlKeyword quantiles("MaximumLikelihoodQuantiles", 
                           toString(m_maximumLikelihood[0].second));

      for (int i = 1; i < m_maximumLikelihood.size(); i++) {
        models.addValue(MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihood[i].first));
        quantiles.addValue(toString(m_maximumLikelihood[i].second));
      }
      object += models;
      object += quantiles;
    }
    else {
      models.addValue("None");
    }

    return object;
  }

}
