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

    m_globalLatitudeAprioriSigma  = -1.0;
    m_globalLongitudeAprioriSigma = -1.0;
    m_globalRadiusAprioriSigma    = -1.0;

    // Spacecraft Position Options
    m_instrumentPositionSolveOption = NoPositionFactors;
    m_solvePositionOverHermiteSpline = false;
    m_spkDegree      = 2; // -1
    m_spkSolveDegree = 2; // -1
    m_globalInstrumentPositionAprioriSigma     = -1.0;
    m_globalInstrumentVelocityAprioriSigma     = -1.0;
    m_globalInstrumentAccelerationAprioriSigma = -1.0;

    // Camera Pointing Options
    m_instrumentPointingSolveOption = AnglesOnly;
    m_solveTwist = true;
    m_fitInstrumentPointingPolynomialOverExisting = false;
    m_ckDegree      = 2; // 0 since we default to angles ???
    m_ckSolveDegree = 2; // 0 since we default to angles ???
    m_globalInstrumentPointingAnglesAprioriSigma       = -1.0;
    m_globalInstrumentPointingVelocityAprioriSigma     = -1.0;
    m_globalInstrumentPointingAccelerationAprioriSigma = -1.0;

    // Convergence Criteria
    m_convergenceCriteria = BundleSettings::Sigma0;
    m_convergenceCriteriaThreshold         = 1.0e-10;
    m_convergenceCriteriaMaximumIterations = 50;

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


  BundleSettings::BundleSettings(const BundleSettings &other) {
    m_validateNetwork = other.validateNetwork();

    m_solveMethod = other.solveMethod();
    m_solveObservationMode = other.solveObservationMode();
    m_solveRadius = other.solveRadius();
    m_updateCubeLabel = other.updateCubeLabel();
    m_errorPropagation = other.errorPropagation();

    m_outlierRejection = other.outlierRejection();
    m_outlierRejectionMultiplier = other.outlierRejectionMultiplier();

    m_globalLatitudeAprioriSigma = other.globalLatitudeAprioriSigma();
    m_globalLongitudeAprioriSigma = other.globalLongitudeAprioriSigma();
    m_globalRadiusAprioriSigma = other.globalRadiusAprioriSigma();

    // Spacecraft Position Options
    m_instrumentPositionSolveOption = other.instrumentPositionSolveOption();
    m_spkDegree = other.spkDegree();
    m_spkSolveDegree = other.spkSolveDegree();
    m_solvePositionOverHermiteSpline = other.solveInstrumentPositionOverHermiteSpline();
    m_globalInstrumentPositionAprioriSigma = other.globalInstrumentPositionAprioriSigma();
    m_globalInstrumentVelocityAprioriSigma = other.globalInstrumentVelocityAprioriSigma();
    m_globalInstrumentAccelerationAprioriSigma = other.globalInstrumentAccelerationAprioriSigma();

    // Camera Pointing Options
    m_instrumentPointingSolveOption = other.instrumentPointingSolveOption();
    m_solveTwist = other.solveTwist();
    m_ckDegree = other.ckDegree();
    m_ckSolveDegree = other.ckSolveDegree();
    m_fitInstrumentPointingPolynomialOverExisting
        = other.fitInstrumentPointingPolynomialOverExisting();
    m_globalInstrumentPointingAnglesAprioriSigma
        = other.globalInstrumentPointingAnglesAprioriSigma();
    m_globalInstrumentPointingVelocityAprioriSigma
        = other.globalInstrumentPointingAngularVelocityAprioriSigma();
    m_globalInstrumentPointingAccelerationAprioriSigma
        = other.globalInstrumentPointingAngularAccelerationAprioriSigma();

    // Convergence Criteria
    m_convergenceCriteria = other.convergenceCriteria();
    m_convergenceCriteriaThreshold = other.convergenceCriteriaThreshold();
    m_convergenceCriteriaMaximumIterations = other.convergenceCriteriaMaximumIterations();

    // Maximum Likelihood Estimation Options no default in the constructor - must be set.
    for (int i = 0; i < other.maximumLikelihoodEstimatorModels().size(); i++) {
      addMaximumLikelihoodEstimatorModel(other.maximumLikelihoodEstimatorModels()[i].first,
                                         other.maximumLikelihoodEstimatorModels()[i].second);
    }
    // Self Calibration ??? (from cnetsuite only)

    // Target Body ??? (from cnetsuite only)




    // Output Options
    m_outputFilePrefix = other.outputFilePrefix();
    m_createBundleOutputFile = other.createBundleOutputFile();
    m_createCSVPointsFile    = other.createCSVPointsFile();
    m_createResidualsFile    = other.createResidualsFile();

  }



  BundleSettings::~BundleSettings() {
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
  BundleSettings::InstrumentPositionSolveOption BundleSettings::stringToInstrumentPositionSolveOption(
      QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::NoPositionFactors;
    }
    else if (option.compare("POSITIONS", Qt::CaseInsensitive) == 0) {
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
                          "Unknown bundle instrument position solve option " + option + ".",
                          _FILEINFO_);
    }
  }



  QString BundleSettings::instrumentPositionSolveOptionToString(InstrumentPositionSolveOption option) {
    if (option == NoPositionFactors)                 return "None";
    else if (option == PositionOnly)                 return "PositionOnly";
    else if (option == PositionVelocity)             return "PositionAndVelocity";
    else if (option == PositionVelocityAcceleration) return "PositionVelocityAndAcceleration";
    else if (option == AllPositionCoefficients)      return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown position solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }



//   void BundleSettings::setInstrumentPositionSolveOptions(InstrumentPositionSolveOption option, 
//                                                        bool solveOverHermiteSpline,
//                                                        int spkDegree, int spkSolveDegree) {
//     m_instrumentPositionSolveOption = option;
//     if (m_instrumentPositionSolveOption != NoPositionFactors) {
//       m_solvePositionOverHermiteSpline = solveOverHermiteSpline;
//     }
// //    if (m_instrumentPositionSolveOption == AllPositionCoefficients) {
//       m_spkDegree = spkDegree;
//       m_spkSolveDegree = spkSolveDegree;
// //    }
// //    else {
// //      m_spkDegree = (int) (m_instrumentPositionSolveOption - 1);
// //      m_spkSolveDegree = (int) (m_instrumentPositionSolveOption - 1);
// //    }
//   }



  void BundleSettings::setInstrumentPositionSolveOptions(InstrumentPositionSolveOption option, 
                                                    bool solveOverHermiteSpline,
                                                    int spkDegree, int spkSolveDegree,
                                                    double globalInstrumentPositionAprioriSigma,
                                                    double globalInstrumentVelocityAprioriSigma,
                                                    double globalInstrumentAccelerationAprioriSigma) {
    m_instrumentPositionSolveOption = option;
    if (m_instrumentPositionSolveOption != NoPositionFactors) {
      m_solvePositionOverHermiteSpline = solveOverHermiteSpline;
    }
//    if (m_instrumentPositionSolveOption == AllPositionCoefficients) {
    m_spkDegree = spkDegree;
    m_spkSolveDegree = spkSolveDegree;
//    }
//    else {
//      m_spkDegree = (int) (m_instrumentPositionSolveOption - 1);
//      m_spkSolveDegree = (int) (m_instrumentPositionSolveOption - 1);
//    }
    if (option > NoPositionFactors) {

      m_globalInstrumentPositionAprioriSigma = globalInstrumentPositionAprioriSigma;

      if (option > PositionOnly) {
        m_globalInstrumentVelocityAprioriSigma = globalInstrumentVelocityAprioriSigma;

        if (option > PositionVelocityAcceleration) {
          m_globalInstrumentAccelerationAprioriSigma = globalInstrumentAccelerationAprioriSigma;
        }
        else {
          m_globalInstrumentAccelerationAprioriSigma = -1.0;
        }

      }
      else {
        m_globalInstrumentVelocityAprioriSigma = -1.0;
      }
    }
    else {
      m_globalInstrumentPositionAprioriSigma = -1.0;
    }
  }



  BundleSettings::InstrumentPositionSolveOption BundleSettings::instrumentPositionSolveOption() const {
    return m_instrumentPositionSolveOption;
  }



  bool BundleSettings::solveInstrumentPositionOverHermiteSpline() const {
    return m_solvePositionOverHermiteSpline;
  }
  


  int BundleSettings::spkDegree() const {
    return m_spkDegree;
  }



  int BundleSettings::spkSolveDegree() const {
    return m_spkSolveDegree;
  }



  double BundleSettings::globalInstrumentPositionAprioriSigma() const {
    return m_globalInstrumentPositionAprioriSigma;
  }



  double BundleSettings::globalInstrumentVelocityAprioriSigma() const {
    return m_globalInstrumentVelocityAprioriSigma;
  }



  double BundleSettings::globalInstrumentAccelerationAprioriSigma() const {
    return m_globalInstrumentAccelerationAprioriSigma;
  }



  // =============================================================================================//
  // ======================== Camera Pointing Options ============================================//
  // =============================================================================================//
  BundleSettings::InstrumentPointingSolveOption 
      BundleSettings::stringToInstrumentPointingSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleSettings::NoPointingFactors;
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
      return BundleSettings::AllPointingCoefficients;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown bundle instrument pointing solve option " + option + ".",
                       _FILEINFO_);
    }
  }



  QString BundleSettings::instrumentPointingSolveOptionToString(
      InstrumentPointingSolveOption option) {
    if (option == NoPointingFactors)            return "None";
    else if (option == AnglesOnly)                 return "AnglesOnly";
    else if (option == AnglesVelocity)             return "AnglesAndVelocity";
    else if (option == AnglesVelocityAcceleration) return "AnglesVelocityAndAcceleration";
    else if (option == AllPointingCoefficients) return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown pointing solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }



//   void BundleSettings::setInstrumentPointingSolveOptions(InstrumentPointingSolveOption option, 
//                                                           bool solveTwist, bool fitOverExisting,
//                                                           int ckDegree, int ckSolveDegree) {
//     m_instrumentPointingSolveOption = option;
//     if (m_instrumentPointingSolveOption != NoPointingFactors){
//       m_solveTwist = solveTwist;
//       m_fitInstrumentPointingPolynomialOverExisting = fitOverExisting;
//     }
// //    if (m_instrumentPointingSolveOption == AllPointingCoefficients){
//       m_ckDegree = ckDegree;
//       m_ckSolveDegree = ckSolveDegree;
// //    }
// //    else {
// //      m_ckDegree = (int) (m_instrumentPointingSolveOption - 1);
// //      m_ckSolveDegree = (int) (m_instrumentPointingSolveOption - 1);
// //    }
//   }



  void BundleSettings::setInstrumentPointingSolveOptions(InstrumentPointingSolveOption option,
                          bool solveTwist, bool fitOverExisting, int ckDegree, int ckSolveDegree,
                          double globalInstrumentPointingAnglesAprioriSigma,
                          double globalInstrumentPointingAngularVelocityAprioriSigma,
                          double globalInstrumentPointingAngularAccelerationAprioriSigma) {
    m_instrumentPointingSolveOption = option;
    if (m_instrumentPointingSolveOption != NoPointingFactors) {
      m_solveTwist = solveTwist;
      m_fitInstrumentPointingPolynomialOverExisting = fitOverExisting;
    }
//    if (m_instrumentPointingSolveOption == AllPointingCoefficients){
    m_ckDegree = ckDegree;
    m_ckSolveDegree = ckSolveDegree;
//    }
//    else {
//      m_ckDegree = (int) (m_instrumentPointingSolveOption - 1);
//      m_ckSolveDegree = (int) (m_instrumentPointingSolveOption - 1);
//    }
    if (option > NoPointingFactors) {

      m_globalInstrumentPointingAnglesAprioriSigma = globalInstrumentPointingAnglesAprioriSigma;

      if (option > AnglesOnly) {
        m_globalInstrumentPointingVelocityAprioriSigma = globalInstrumentPointingAngularVelocityAprioriSigma;

        if (option > AnglesVelocity) {
          m_globalInstrumentPointingAccelerationAprioriSigma 
              = globalInstrumentPointingAngularAccelerationAprioriSigma;
        }
        else {
          m_globalInstrumentPointingAccelerationAprioriSigma = -1.0;
        }

      }
      else {
        m_globalInstrumentPointingVelocityAprioriSigma = -1.0;
      }
    }
    else {
      m_globalInstrumentPointingAnglesAprioriSigma = -1.0;
    }
  }



  BundleSettings::InstrumentPointingSolveOption 
      BundleSettings::instrumentPointingSolveOption() const {
    return m_instrumentPointingSolveOption;
  }



  bool BundleSettings::solveTwist() const {
    return m_solveTwist;
  }
  


  bool BundleSettings::fitInstrumentPointingPolynomialOverExisting() const {
    return m_fitInstrumentPointingPolynomialOverExisting;
  }
  


  int BundleSettings::ckDegree() const {
    return m_ckDegree;
  }



  int BundleSettings::ckSolveDegree() const {
    return m_ckSolveDegree;
  }
  


  double BundleSettings::globalInstrumentPointingAnglesAprioriSigma() const {
    return m_globalInstrumentPointingAnglesAprioriSigma;
  }



  double BundleSettings::globalInstrumentPointingAngularVelocityAprioriSigma() const {
    return m_globalInstrumentPointingVelocityAprioriSigma;
  }



  double BundleSettings::globalInstrumentPointingAngularAccelerationAprioriSigma() const {
    return m_globalInstrumentPointingAccelerationAprioriSigma;
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
//   void BundleSettings::setGlobalInstrumentPositionAprioriSigma(double sigma) {
//     m_globalInstrumentPositionAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalInstrumentVelocityAprioriSigma(double sigma) {
//     m_globalInstrumentVelocityAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalInstrumentAccelerationAprioriSigma(double sigma) {
//     m_globalInstrumentAccelerationAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalInstrumentPointingAnglesAprioriSigma(double sigma) {
//     m_globalInstrumentPointingAnglesAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalInstrumentPointingAngularVelocityAprioriSigma(double sigma) {
//     m_globalInstrumentPointingVelocityAprioriSigma = sigma;
//   }
// 
// 
// 
//   void BundleSettings::setGlobalInstrumentPointingAngularAccelerationAprioriSigma(double sigma) {
//     m_globalInstrumentPointingAccelerationAprioriSigma = sigma;
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

    // General Solve Options
    group += PvlKeyword("SolveMethod", solveMethodToString(m_solveMethod));
    group += PvlKeyword("SolveObservationMode", toString(m_solveObservationMode));
    group += PvlKeyword("SolveRadius", toString(m_solveRadius));
    group += PvlKeyword("UpdateCubeLabel", toString(m_updateCubeLabel));
    group += PvlKeyword("ErrorPropagation", toString(m_errorPropagation));
    group += PvlKeyword("OutlierRejection", toString(m_outlierRejection));
    if (m_outlierRejection) {
      group += PvlKeyword("OutlierMultiplier", toString(m_outlierRejectionMultiplier));
    }
    group += PvlKeyword("GlobalLatitudeAprioriSigma", toString(m_globalLatitudeAprioriSigma));
    group += PvlKeyword("GlobalLongitudeAprioriSigma", toString(m_globalLongitudeAprioriSigma));
    if (m_solveRadius) {
      group += PvlKeyword("GlobalRadiiAprioriSigma", toString(m_globalRadiusAprioriSigma));
    }

    // Position Solve Options
    group += PvlKeyword("InstrumentPositionSolveOption",
                        instrumentPositionSolveOptionToString(m_instrumentPositionSolveOption));
    group += PvlKeyword("SPKDegree", toString(m_spkDegree));
    group += PvlKeyword("SPKSolveDegree", toString(m_spkSolveDegree));
    group += PvlKeyword("SolvePositionOverHermiteSpline", toString(m_solvePositionOverHermiteSpline));
    if (m_instrumentPositionSolveOption == 4) {
      group += PvlKeyword("GlobalInstrumentPositionAprioriSigma", toString(m_globalInstrumentPositionAprioriSigma));
      group += PvlKeyword("GlobalInstrumentVelocityAprioriSigma", toString(m_globalInstrumentVelocityAprioriSigma));
      group += PvlKeyword("GlobalInstrumentAccelerationAprioriSigma",
                          toString(m_globalInstrumentAccelerationAprioriSigma));
    }


    // Pointing Solve Options
    group += PvlKeyword("InstrumentPointingSolveOption",
                        instrumentPointingSolveOptionToString(m_instrumentPointingSolveOption));
    group += PvlKeyword("CKDegree", toString(m_ckDegree));
    group += PvlKeyword("CKSolveDegree", toString(m_ckSolveDegree));
    group += PvlKeyword("SolveTwist", toString(m_solveTwist));
    group += PvlKeyword("FitPointingPolynomialOverExisting",
                        toString(m_fitInstrumentPointingPolynomialOverExisting));
    if (m_instrumentPointingSolveOption == 4) {
      group += PvlKeyword("GlobalInstrumentPointingAnglesAprioriSigma", 
                          toString(m_globalInstrumentPointingAnglesAprioriSigma));
      group += PvlKeyword("GlobalInstrumentPointingAngularVelocityAprioriSigma",
                          toString(m_globalInstrumentPointingVelocityAprioriSigma));
      group += PvlKeyword("GlobalInstrumentPointingAngularAccelerationAprioriSigma",
                          toString(m_globalInstrumentPointingAccelerationAprioriSigma));
    }

    // Convergence Criteria
    group += PvlKeyword("ConvergenceCriteria", convergenceCriteriaToString(m_convergenceCriteria));
    group += PvlKeyword("ConvergenceCriteriaThreshold", toString(m_convergenceCriteriaThreshold));
    group += PvlKeyword("ConvergenceCriteriaMaximumIterations",
                        toString(m_convergenceCriteriaMaximumIterations));

    // Output Options
    group += PvlKeyword("CreateBundleOutputFile", toString(m_createBundleOutputFile));
    group += PvlKeyword("CreateCSVPointsFile", toString(m_createCSVPointsFile));
    group += PvlKeyword("CreateResidualsFile", toString(m_createResidualsFile));
    if (m_createBundleOutputFile || m_createCSVPointsFile || m_createResidualsFile) {
      group += PvlKeyword("FilePrefix", m_outputFilePrefix);
    }

    // Maximum Likelihood Options
    for (int i = 0;i < m_maximumLikelihood.size();i++) {
      group += PvlKeyword("MaximumLikelihoodModel", maximumLikelihoodModelToString(m_maximumLikelihood[i].first));
      group += PvlKeyword("Quantile", toString(m_maximumLikelihood[i].second));
    }

    return group;
  }

}
