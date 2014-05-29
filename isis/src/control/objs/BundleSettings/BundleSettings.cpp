#include "BundleSettings.h"

#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "PvlGroup.h"

namespace Isis {

  BundleSettings::BundleSettings() {
    m_validateNetwork = true;

    m_solveMethod = Sparse;
    m_solveObservationMode = false;
    m_solveRadius          = false;
    m_updateCubeLabel      = false;
    m_errorPropagation     = false;

    m_outlierRejection     = false;
    m_outlierRejectionMultiplier = 3.0; // default to rejection = false, i.e. multiplier = 1.0      ???

    // Spacecraft Position Options
    m_observerPositionSolveOption = NoPositionFactors;
    m_spkDegree = 2; // -1
    m_spkSolveDegree = 2; // -1
    m_solvePositionOverHermiteSpline = false;

    // Camera Pointing Options
    m_observerOrientationSolveOption = AnglesOnly;
    m_solveTwist = true;
    m_ckDegree = 2; // 0 since we default to angles ???
    m_ckSolveDegree = 2; // 0 since we default to angles ???
    m_fitOrientationPolynomialOverExisting = false;

    // Convergence Criteria
    m_convergenceCriteria = BundleSettings::Sigma0;
    m_convergenceCriteriaThreshold = 1.0e-10;
    m_convergenceCriteriaMaximumIterations = 50;

    // Parameter Uncertainties (Weighting)
    m_globalLatitudeAprioriSigma = -1.0;
    m_globalLongitudeAprioriSigma = -1.0;
    m_globalRadiusAprioriSigma = -1.0;
    m_globalObserverPositionAprioriSigma = -1.0;
    m_globalObserverVelocityAprioriSigma = -1.0;
    m_globalObserverAccelerationAprioriSigma = -1.0;
    m_globalObserverOrientationAnglesAprioriSigma = -1.0;
    m_globalObserverOrientationVelocityAprioriSigma = -1.0;
    m_globalObserverOrientationAccelerationAprioriSigma = -1.0;

    // Maximum Likelihood Estimation Options no default in the constructor - must be set.

    // Self Calibration ??? (from cnetsuite only)

    // Target Body ??? (from cnetsuite only)




    // Output Options
    m_outputFilePrefix = "";
    m_createBundleOutputFile = true;
    m_createCSVPointsFile    = true;
    m_createResidualsFile    = true;

  }



  BundleSettings::~BundleSettings() {}



  void BundleSettings::setValidateNetwork(bool validate) {
    m_validateNetwork = validate;
  }



  bool BundleSettings::validateNetwork() {
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
    else if (method.compare("OLDSPARSE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::OldSparse;
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
    else if (method == OldSparse) return "OldSparse";
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



  // =============================================================================================//
  // ======================== Spacecraft Position Options ========================================//
  // =============================================================================================//
  BundleSettings::ObserverPositionSolveOption BundleSettings::stringToObserverPositionSolveOption(
      QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::NoPositionFactors;
    }
    else if (option.compare("POSITION", Qt::CaseInsensitive) == 0) {
      return BundleSettings::PositionOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleSettings::PositionVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleSettings::PositionVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleSettings::AllPositionCoefficients;
    }
    else {
      throw IException(IException::Programmer,
                          "Unknown bundle observer position solve option " + option + ".",
                          _FILEINFO_);
    }
  }



  QString BundleSettings::observerPositionSolveOptionToString(ObserverPositionSolveOption option) {
    if (option == NoPositionFactors)                 return "None";
    else if (option == PositionOnly)                 return "PositionOnly";
    else if (option == PositionVelocity)             return "PositionAndVelocity";
    else if (option == PositionVelocityAcceleration) return "PositionVelocityAndAcceleration";
    else if (option == AllPositionCoefficients)      return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown position solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }



//   void BundleSettings::setObserverPositionSolveOptions(ObserverPositionSolveOption option, 
//                                                        bool solveOverHermiteSpline,
//                                                        int spkDegree, int spkSolveDegree) {
//     m_observerPositionSolveOption = option;
//     if (m_observerPositionSolveOption != NoPositionFactors) {
//       m_solvePositionOverHermiteSpline = solveOverHermiteSpline;
//     }
// //    if (m_observerPositionSolveOption == AllPositionCoefficients) {
//       m_spkDegree = spkDegree;
//       m_spkSolveDegree = spkSolveDegree;
// //    }
// //    else {
// //      m_spkDegree = (int) (m_observerPositionSolveOption - 1);
// //      m_spkSolveDegree = (int) (m_observerPositionSolveOption - 1);
// //    }
//   }



  void BundleSettings::setObserverPositionSolveOptions(ObserverPositionSolveOption option, 
                                                    bool solveOverHermiteSpline,
                                                    int spkDegree, int spkSolveDegree,
                                                    double globalObserverPositionAprioriSigma,
                                                    double globalObserverVelocityAprioriSigma,
                                                    double globalObserverAccelerationAprioriSigma) {
    m_observerPositionSolveOption = option;
    if (m_observerPositionSolveOption != NoPositionFactors) {
      m_solvePositionOverHermiteSpline = solveOverHermiteSpline;
    }
//    if (m_observerPositionSolveOption == AllPositionCoefficients) {
    m_spkDegree = spkDegree;
    m_spkSolveDegree = spkSolveDegree;
//    }
//    else {
//      m_spkDegree = (int) (m_observerPositionSolveOption - 1);
//      m_spkSolveDegree = (int) (m_observerPositionSolveOption - 1);
//    }
    if (option > NoPositionFactors) {

      m_globalObserverPositionAprioriSigma = globalObserverPositionAprioriSigma;

      if (option > PositionOnly) {
        m_globalObserverVelocityAprioriSigma = globalObserverVelocityAprioriSigma;

        if (option > PositionVelocityAcceleration) {
          m_globalObserverAccelerationAprioriSigma = globalObserverAccelerationAprioriSigma;
        }
        else {
          m_globalObserverAccelerationAprioriSigma = -1.0;
        }

      }
      else {
        m_globalObserverVelocityAprioriSigma = -1.0;
      }
    }
    else {
      m_globalObserverPositionAprioriSigma = -1.0;
    }
  }



  BundleSettings::ObserverPositionSolveOption BundleSettings::observerPositionSolveOption() const {
    return m_observerPositionSolveOption;
  }



  bool BundleSettings::solveObserverPositionOverHermiteSpline() const {
    return m_solvePositionOverHermiteSpline;
  }
  


  int BundleSettings::spkDegree() const {
    return m_spkDegree;
  }



  int BundleSettings::spkSolveDegree() const {
    return m_spkSolveDegree;
  }



  double BundleSettings::globalObserverPositionAprioriSigma() const {
    return m_globalObserverPositionAprioriSigma;
  }



  double BundleSettings::globalObserverVelocityAprioriSigma() const {
    return m_globalObserverVelocityAprioriSigma;
  }



  double BundleSettings::globalObserverAccelerationAprioriSigma() const {
    return m_globalObserverAccelerationAprioriSigma;
  }



  // =============================================================================================//
  // ======================== Camera Pointing Options ============================================//
  // =============================================================================================//
  BundleSettings::ObserverOrientationSolveOption 
      BundleSettings::stringToObserverOrientationSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::NoOrientationFactors;
    }
    else if (option.compare("ANGLES", Qt::CaseInsensitive) == 0) {
      return BundleSettings::AnglesOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleSettings::AnglesVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleSettings::AnglesVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleSettings::AllOrientationCoefficients;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown bundle observer orientation solve option " + option + ".",
                       _FILEINFO_);
    }
  }



  QString BundleSettings::observerOrientationSolveOptionToString(
      ObserverOrientationSolveOption option) {
    if (option == NoOrientationFactors)            return "None";
    else if (option == AnglesOnly)                 return "AnglesOnly";
    else if (option == AnglesVelocity)             return "AnglesAndVelocity";
    else if (option == AnglesVelocityAcceleration) return "AnglesVelocityAndAcceleration";
    else if (option == AllOrientationCoefficients) return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown orientation solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }



//   void BundleSettings::setObserverOrientationSolveOptions(ObserverOrientationSolveOption option, 
//                                                           bool solveTwist, bool fitOverExisting,
//                                                           int ckDegree, int ckSolveDegree) {
//     m_observerOrientationSolveOption = option;
//     if (m_observerOrientationSolveOption != NoOrientationFactors){
//       m_solveTwist = solveTwist;
//       m_fitOrientationPolynomialOverExisting = fitOverExisting;
//     }
// //    if (m_observerOrientationSolveOption == AllOrientationCoefficients){
//       m_ckDegree = ckDegree;
//       m_ckSolveDegree = ckSolveDegree;
// //    }
// //    else {
// //      m_ckDegree = (int) (m_observerOrientationSolveOption - 1);
// //      m_ckSolveDegree = (int) (m_observerOrientationSolveOption - 1);
// //    }
//   }



  void BundleSettings::setObserverOrientationSolveOptions(ObserverOrientationSolveOption option,
                          bool solveTwist, bool fitOverExisting, int ckDegree, int ckSolveDegree,
                          double globalObserverOrientationAnglesAprioriSigma,
                          double globalObserverOrientationAngularVelocityAprioriSigma,
                          double globalObserverOrientationAngularAccelerationAprioriSigma) {
    m_observerOrientationSolveOption = option;
    if (m_observerOrientationSolveOption != NoOrientationFactors) {
      m_solveTwist = solveTwist;
      m_fitOrientationPolynomialOverExisting = fitOverExisting;
    }
//    if (m_observerOrientationSolveOption == AllOrientationCoefficients){
    m_ckDegree = ckDegree;
    m_ckSolveDegree = ckSolveDegree;
//    }
//    else {
//      m_ckDegree = (int) (m_observerOrientationSolveOption - 1);
//      m_ckSolveDegree = (int) (m_observerOrientationSolveOption - 1);
//    }
    if (option > NoOrientationFactors) {

      m_globalObserverOrientationAnglesAprioriSigma = globalObserverOrientationAnglesAprioriSigma;

      if (option > AnglesOnly) {
        m_globalObserverOrientationVelocityAprioriSigma = globalObserverOrientationAngularVelocityAprioriSigma;

        if (option > AnglesVelocity) {
          m_globalObserverOrientationAccelerationAprioriSigma 
              = globalObserverOrientationAngularAccelerationAprioriSigma;
        }
        else {
          m_globalObserverOrientationAccelerationAprioriSigma = -1.0;
        }

      }
      else {
        m_globalObserverOrientationVelocityAprioriSigma = -1.0;
      }
    }
    else {
      m_globalObserverOrientationAnglesAprioriSigma = -1.0;
    }
  }



  BundleSettings::ObserverOrientationSolveOption 
      BundleSettings::observerOrientationSolveOption() const {
    return m_observerOrientationSolveOption;
  }



  bool BundleSettings::solveTwist() const {
    return m_solveTwist;
  }
  


  bool BundleSettings::fitOrientationPolynomialOverExisting() const {
    return m_fitOrientationPolynomialOverExisting;
  }
  


  int BundleSettings::ckDegree() const {
    return m_ckDegree;
  }



  int BundleSettings::ckSolveDegree() const {
    return m_ckSolveDegree;
  }
  


  double BundleSettings::globalObserverOrientationAnglesAprioriSigma() const {
    return m_globalObserverOrientationAnglesAprioriSigma;
  }



  double BundleSettings::globalObserverOrientationAngularVelocityAprioriSigma() const {
    return m_globalObserverOrientationVelocityAprioriSigma;
  }



  double BundleSettings::globalObserverOrientationAngularAccelerationAprioriSigma() const {
    return m_globalObserverOrientationAccelerationAprioriSigma;
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
// 
// 
// 
//   void BundleSettings::setGlobalObserverPositionAprioriSigma(double sigma) {
//     m_globalObserverPositionAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalObserverVelocityAprioriSigma(double sigma) {
//     m_globalObserverVelocityAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalObserverAccelerationAprioriSigma(double sigma) {
//     m_globalObserverAccelerationAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalObserverOrientationAnglesAprioriSigma(double sigma) {
//     m_globalObserverOrientationAnglesAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalObserverOrientationAngularVelocityAprioriSigma(double sigma) {
//     m_globalObserverOrientationVelocityAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalObserverOrientationAngularAccelerationAprioriSigma(double sigma) {
//     m_globalObserverOrientationAccelerationAprioriSigma = sigma;
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

  PvlGroup BundleSettings::pvlGroup() const {
    PvlGroup group("BundleSettings");
    group += PvlKeyword("SolveMethod", solveMethodToString(m_solveMethod));
    group += PvlKeyword("SolveObservationMode", toString(m_solveObservationMode));
    group += PvlKeyword("SolveRadius", toString(m_solveRadius));
    group += PvlKeyword("UpdateCubeLabel", toString(m_updateCubeLabel));
    group += PvlKeyword("ErrorPropagation", toString(m_errorPropagation));
    group += PvlKeyword("OutlierRejection", toString(m_outlierRejection));
    if (m_outlierRejection) {
      group += PvlKeyword("OutlierMultiplier", toString(m_outlierRejectionMultiplier));
    }
    group += PvlKeyword("ObserverPositionSolveOption",
                        observerPositionSolveOptionToString(m_observerPositionSolveOption));
    group += PvlKeyword("SPKDegree", toString(m_spkDegree));
    group += PvlKeyword("SPKSolveDegree", toString(m_spkSolveDegree));
    group += PvlKeyword("SolvePositionOverHermiteSpline", toString(m_solvePositionOverHermiteSpline));
    group += PvlKeyword("ObserverOrientationSolveOption",
                        observerOrientationSolveOptionToString(m_observerOrientationSolveOption));
    group += PvlKeyword("CKDegree", toString(m_ckDegree));
    group += PvlKeyword("CKSolveDegree", toString(m_ckSolveDegree));
    group += PvlKeyword("SolveTwist", toString(m_solveTwist));
    group += PvlKeyword("FitOrientationPolynomialOverExisting",
                        toString(m_fitOrientationPolynomialOverExisting));
    group += PvlKeyword("ConvergenceCriteria", convergenceCriteriaToString(m_convergenceCriteria));
    group += PvlKeyword("ConvergenceCriteriaThreshold", toString(m_convergenceCriteriaThreshold));
    group += PvlKeyword("ConvergenceCriteriaMaximumIterations",
                        toString(m_convergenceCriteriaMaximumIterations));
    group += PvlKeyword("GlobalLatitudeAprioriSigma", toString(m_globalLatitudeAprioriSigma));
    group += PvlKeyword("GlobalLongitudeAprioriSigma", toString(m_globalLongitudeAprioriSigma));
    group += PvlKeyword("GlobalRadiiAprioriSigma", toString(m_globalRadiusAprioriSigma));
    group += PvlKeyword("GlobalObserverPositionAprioriSigma", toString(m_globalObserverPositionAprioriSigma));
    group += PvlKeyword("GlobalObserverVelocityAprioriSigma", toString(m_globalObserverVelocityAprioriSigma));
    group += PvlKeyword("GlobalObserverAccelerationAprioriSigma",
                        toString(m_globalObserverAccelerationAprioriSigma));
    group += PvlKeyword("GlobalObserverOrientationAnglesAprioriSigma",
                        toString(m_globalObserverOrientationAnglesAprioriSigma));
    group += PvlKeyword("GlobalObserverOrientationAngularVelocityAprioriSigma",
                        toString(m_globalObserverOrientationVelocityAprioriSigma));
    group += PvlKeyword("GlobalObserverOrientationAngularAccelerationAprioriSigma",
                        toString(m_globalObserverOrientationAccelerationAprioriSigma));
    group += PvlKeyword("FilePrefix", m_outputFilePrefix);
    group += PvlKeyword("CreateBundleOutputFile", toString(m_createBundleOutputFile));
    group += PvlKeyword("CreateCSVPointsFile", toString(m_createCSVPointsFile));
    group += PvlKeyword("CreateResidualsFile", toString(m_createResidualsFile));

    for (int i = 0;i < m_maximumLikelihood.size();i++) {
      group += PvlKeyword("MaximumLikelihoodModel", maximumLikelihoodModelToString(m_maximumLikelihood[i].first));
      group += PvlKeyword("Quantile", toString(m_maximumLikelihood[i].second));
    }

    return group;
  }

}
