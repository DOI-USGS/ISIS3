#include "BundleObservationSolveSettings.h"

#include "Camera.h"
#include "BundleImage.h"

namespace Isis {

  /**
   * constructor with default parameter initializations
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings() {

    // Spacecraft Position Options
    m_instrumentPositionSolveOption = NoPositionFactors;
    m_spkDegree = 2;
    m_spkSolveDegree = 2;
    m_solvePositionOverHermiteSpline = false;
    m_positionInterpolationType = SpicePosition::PolyFunction;

    // Camera Pointing Options
    m_instrumentPointingSolveOption = AnglesOnly;
    m_solveTwist = true;
    m_ckDegree = 2;
    m_ckSolveDegree = 2;
    m_solvePointingPolynomialOverExisting = false;
    m_pointingInterpolationType = SpiceRotation::PolyFunction;

    m_numberCamAngleCoefSolved = 0;
    m_ckDegree = 2;
    m_ckSolveDegree = 2;
    m_solveTwist = false;
    m_spkDegree = 2;
  }


  /**
   * destructor
   */
  BundleObservationSolveSettings::~BundleObservationSolveSettings() {
  }


  /**
   * copy constructor
   */
  BundleObservationSolveSettings::
      BundleObservationSolveSettings(const BundleObservationSolveSettings &src) {

    m_instrumentId = src.m_instrumentId;

    // position related
    m_instrumentPositionSolveOption = src.m_instrumentPositionSolveOption;
    m_spkDegree = src.m_spkDegree;
    m_spkSolveDegree = src.m_spkSolveDegree;
    m_numberCamPosCoefSolved = src.m_numberCamPosCoefSolved;
    m_solvePositionOverHermiteSpline = src.m_solvePositionOverHermiteSpline;
    m_positionInterpolationType = src.m_positionInterpolationType;
    m_positionAprioriSigma = src.m_positionAprioriSigma;

    // pointing related
    m_instrumentPointingSolveOption = src.m_instrumentPointingSolveOption;
    m_solveTwist = src.m_solveTwist;
    m_ckDegree = src.m_ckDegree;
    m_ckSolveDegree = src.m_ckSolveDegree;
    m_numberCamAngleCoefSolved = src.m_numberCamAngleCoefSolved;
    m_solvePointingPolynomialOverExisting = src.m_solvePointingPolynomialOverExisting;
    m_pointingInterpolationType = src.m_pointingInterpolationType;
    m_anglesAprioriSigma = src.m_anglesAprioriSigma;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setCKDegree(double ckDegree) {
    m_ckDegree = ckDegree;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setCKSolveDegree(double ckSolveDegree) {
    m_ckSolveDegree = ckSolveDegree;
  }

  // =============================================================================================//
  // ======================== Camera Pointing Options ============================================//
  // =============================================================================================//
  BundleObservationSolveSettings::InstrumentPointingSolveOption
      BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPointingFactors;
    }
    else if (option.compare("ANGLES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AllPointingCoefficients;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown bundle instrument pointing solve option " + option + ".",
                       _FILEINFO_);
    }
  }



  QString BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
      InstrumentPointingSolveOption option) {
    if (option == NoPointingFactors)               return "None";
    else if (option == AnglesOnly)                 return "AnglesOnly";
    else if (option == AnglesVelocity)             return "AnglesAndVelocity";
    else if (option == AnglesVelocityAcceleration) return "AnglesVelocityAndAcceleration";
    else if (option == AllPointingCoefficients)    return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown pointing solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setInstrumentPointingSolveOption(InstrumentPointingSolveOption option) {
    m_instrumentPointingSolveOption = option;

    if (option == NoPointingFactors) {
      m_numberCamAngleCoefSolved = 0;
    }
    else if (option == AnglesOnly) {
      m_numberCamAngleCoefSolved = 1;
    }
    else if (option == AnglesVelocity) {
      m_numberCamAngleCoefSolved = 2;
    }
    else if (option == AnglesVelocityAcceleration) {
      m_numberCamAngleCoefSolved = 3;
    }
    else if (option == AllPointingCoefficients) {
      m_numberCamAngleCoefSolved = m_ckSolveDegree + 1;
    }

    m_anglesAprioriSigma.resize(m_numberCamAngleCoefSolved);
    m_anglesAprioriSigma.fill(-1.0);
  }

  // =============================================================================================//
  // ======================== Spacecraft Position Options ========================================//
  // =============================================================================================//
  BundleObservationSolveSettings::InstrumentPositionSolveOption
      BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPositionFactors;
    }
    else if (option.compare("POSITIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AllPositionCoefficients;
    }
    else {
      throw IException(IException::Programmer,
                          "Unknown bundle instrument position solve option " + option + ".",
                          _FILEINFO_);
    }
  }


  QString BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
      InstrumentPositionSolveOption option) {
    if (option == NoPositionFactors)                 return "None";
    else if (option == PositionOnly)                 return "PositionOnly";
    else if (option == PositionVelocity)             return "PositionAndVelocity";
    else if (option == PositionVelocityAcceleration) return "PositionVelocityAndAcceleration";
    else if (option == AllPositionCoefficients)      return "AllPolynomialCoefficients";
    else throw IException(IException::Programmer,
                          "Unknown position solve option enum [" + toString(option) + "].",
                          _FILEINFO_);
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setAnglesAprioriSigma(double anglesAprioriSigma) {
    if (m_anglesAprioriSigma.size() < 1) {
      return;
      QString msg = "cmatrixSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_anglesAprioriSigma[0] = anglesAprioriSigma;
  }

  /**
   * TODO
   */
  void BundleObservationSolveSettings::setAngularVelocityAprioriSigma(double angularVelocityAprioriSigma) {
    if (m_anglesAprioriSigma.size() < 2) {
      QString msg = "cmatrixSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_anglesAprioriSigma[1] = angularVelocityAprioriSigma;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setAngularAccelerationAprioriSigma(double angularAccelerationAprioriSigma) {
    if (m_anglesAprioriSigma.size() < 3) {
      QString msg = "cmatrixSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_anglesAprioriSigma[2] = angularAccelerationAprioriSigma;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setSolveTwist(bool solveTwist) {
    m_solveTwist = solveTwist;
  }

  /**
   * TODO
   */
  void BundleObservationSolveSettings::setSolvePolyOverPointing(bool solvePolynomialOverExisting) {
    m_solvePointingPolynomialOverExisting = solvePolynomialOverExisting;

    if (solvePolynomialOverExisting)
      m_pointingInterpolationType = SpiceRotation::PolyFunctionOverSpice;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setSPKDegree(double spkDegree) {
    m_spkDegree = spkDegree;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setSPKSolveDegree(double spkSolveDegree) {
    m_spkSolveDegree = spkSolveDegree;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setInstrumentPositionSolveOption(InstrumentPositionSolveOption option) {
    m_instrumentPositionSolveOption = option;

    if (option == NoPositionFactors) {
      m_numberCamPosCoefSolved = 0;
    }
    else if (option == PositionOnly) {
      m_numberCamPosCoefSolved = 1;
    }
    else if (option == PositionVelocity) {
      m_numberCamPosCoefSolved = 2;
    }
    else if (option == PositionVelocityAcceleration) {
      m_numberCamPosCoefSolved = 3;
    }
    else if (option == AllPositionCoefficients) {
      m_numberCamPosCoefSolved = m_spkSolveDegree + 1;
    }

    m_positionAprioriSigma.resize(m_numberCamPosCoefSolved);
    m_positionAprioriSigma.fill(-1.0);
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setSolvePolyOverHermite(bool positionOverHermite) {
    m_solvePositionOverHermiteSpline = positionOverHermite;

    if (positionOverHermite)
      m_positionInterpolationType = SpicePosition::PolyFunctionOverHermiteConstant;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setPositionAprioriSigma(double positionAprioriSigma) {
    if (m_positionAprioriSigma.size() < 1) {
      return;
      QString msg = "positionSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_positionAprioriSigma[0] = positionAprioriSigma;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setVelocityAprioriSigma(double velocityAprioriSigma) {
    if (m_positionAprioriSigma.size() < 2) {
      QString msg = "positionSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_positionAprioriSigma[1] = velocityAprioriSigma;
  }


  /**
   * TODO
   */
  void BundleObservationSolveSettings::setAccelerationAprioriSigma(double accelerationAprioriSigma) {
    if (m_positionAprioriSigma.size() < 3) {
      QString msg = "positionSolveType has not been set";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_positionAprioriSigma[2] = accelerationAprioriSigma;
  }


  /**
   * TODO
   */
  QString BundleObservationSolveSettings::instrumentId() const {
    return m_instrumentId;
  }


  /**
   * TODO
   */
  bool BundleObservationSolveSettings::solveTwist() const {
    return m_solveTwist;
  }


  /**
   * TODO
   */
  bool BundleObservationSolveSettings::solvePolyOverPointing() const {
    return m_solvePointingPolynomialOverExisting;
  }


  /**
   * TODO
   */
  int BundleObservationSolveSettings::ckDegree() const {
    return m_ckDegree;
  }


  /**
   * TODO
   */
  int BundleObservationSolveSettings::ckSolveDegree()const {
    return m_ckSolveDegree;
  }

  bool BundleObservationSolveSettings::solvePositionOverHermite() const {
    return m_solvePositionOverHermiteSpline;
  }

  int BundleObservationSolveSettings::spkDegree() const {
    return m_spkDegree;
  }

  int BundleObservationSolveSettings::spkSolveDegree() const {
    return m_spkSolveDegree;
  }

  SpicePosition::Source BundleObservationSolveSettings::positionInterpolationType() const {
    return m_positionInterpolationType;
  }

  BundleObservationSolveSettings::InstrumentPositionSolveOption
      BundleObservationSolveSettings::instrumentPositionSolveOption() const {
    return m_instrumentPositionSolveOption;
  }


  BundleObservationSolveSettings::InstrumentPointingSolveOption
      BundleObservationSolveSettings::instrumentPointingSolveOption() const {
    return m_instrumentPointingSolveOption;
  }


  /**
   * TODO
   */
  SpiceRotation::Source BundleObservationSolveSettings::pointingInterpolationType() const {
    return m_pointingInterpolationType;
  }


  /**
   * TODO
   */
  int BundleObservationSolveSettings::numberCameraAngleCoefficientsSolved() const {
    return m_numberCamAngleCoefSolved;
  }


  /**
   * TODO
   */
  int BundleObservationSolveSettings::numberCameraPositionCoefficientsSolved() const {
    return m_numberCamPosCoefSolved;
  }


  /**
   * TODO
   */
  QVector<double> BundleObservationSolveSettings::aprioriPointingSigmas() const {
    return m_anglesAprioriSigma;
  }


  /**
   * TODO
   */
  QVector<double> BundleObservationSolveSettings::aprioriPositionSigmas() const {
    return m_positionAprioriSigma;
  }


  /**
   * set bundle solve parameters for an observation
   */
  bool BundleObservationSolveSettings::setFromPvl(PvlGroup& scParameterGroup) {

    // group name must be instrument id
    m_instrumentId = (QString)scParameterGroup.nameKeyword();

    // If CKDEGREE is not specified, then a default of 2 is used
    if (scParameterGroup.hasKeyword("CKDEGREE")) {
      m_ckDegree = (int)(scParameterGroup.findKeyword("CKDEGREE"));
    }

    // If CKSOLVEDEGREE is not specified, then a default of 2 is used
    if (scParameterGroup.hasKeyword("CKSOLVEDEGREE"))
      m_ckSolveDegree = (int) (scParameterGroup.findKeyword("CKSOLVEDEGREE"));

    // do we solve for No pointing, ANGLES only, ANGLES+ANGULAR VELOCITY, ANGLES+ANGULAR VELOCITY+
    // ANGULAR ACCELERATION, or a higher order polynomial
    QString csolve = "NONE";
    csolve = (QString)scParameterGroup.findKeyword("CAMSOLVE");
    csolve = csolve.toUpper();
    if (csolve == "NONE") {
      m_instrumentPointingSolveOption = NoPointingFactors;
      m_numberCamAngleCoefSolved = 0;
    }
    else if (csolve == "ANGLES") {
      m_instrumentPointingSolveOption = AnglesOnly;
      m_numberCamAngleCoefSolved = 1;
    }
    else if (csolve == "VELOCITIES") {
      m_instrumentPointingSolveOption = AnglesVelocity;
      m_numberCamAngleCoefSolved = 2;
    }
    else if (csolve == "ACCELERATIONS") {
      m_instrumentPointingSolveOption = AnglesVelocityAcceleration;
      m_numberCamAngleCoefSolved = 3;
    }
    else if (csolve == "ALL"){
      m_instrumentPointingSolveOption = AllPointingCoefficients;
      m_numberCamAngleCoefSolved = m_ckSolveDegree + 1;
    }

    m_anglesAprioriSigma.resize(m_numberCamAngleCoefSolved);
    m_anglesAprioriSigma.fill(-1.0);


    // If OVEREXISTING is not specified, then a default of NO is used
    if (scParameterGroup.hasKeyword("OVEREXISTING")) {
      QString parval = (QString)scParameterGroup.findKeyword("OVEREXISTING");
      parval = parval.toUpper();
      if (parval == "TRUE" || parval == "YES") {
        m_solvePointingPolynomialOverExisting = true;
        m_pointingInterpolationType = SpiceRotation::PolyFunctionOverSpice;
      }
      else if (parval == "FALSE" || parval == "NO") {
        m_solvePointingPolynomialOverExisting = false;
        m_pointingInterpolationType = SpiceRotation::PolyFunction;
      }
      else {
        QString msg = "The OVEREXISTING parameter must be set to TRUE or FALSE; YES or NO";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // If SPKDEGREE is not specified, then a default of 2 is used
    if (scParameterGroup.hasKeyword("SPKDEGREE"))
      m_spkDegree = (int)(scParameterGroup.findKeyword("SPKDEGREE"));

    // If SPKSOLVEDEGREE is not specified, then a default of 2 is used
    if (scParameterGroup.hasKeyword("SPKSOLVEDEGREE"))
      m_spkSolveDegree = (int)(scParameterGroup.findKeyword("SPKSOLVEDEGREE"));

    // do we solve for No position, POSITION only, POSITION+VELOCITY, POSITION+VELOCITY+
    // ACCELERATION, or a higher order polynomial
    QString ssolve = "NONE";
    ssolve = (QString)scParameterGroup.findKeyword("SPSOLVE");
    ssolve = ssolve.toUpper();
    if (ssolve == "NONE") {
      m_instrumentPositionSolveOption = NoPositionFactors;
      m_numberCamPosCoefSolved = 0;
    }
    else if (ssolve == "POSITION") {
      m_instrumentPositionSolveOption = PositionOnly;
      m_numberCamPosCoefSolved = 1;
    }
    else if (ssolve == "VELOCITIES") {
      m_instrumentPositionSolveOption = PositionVelocity;
      m_numberCamPosCoefSolved = 2;
    }
    else if (ssolve == "ACCELERATIONS") {
      m_instrumentPositionSolveOption = PositionVelocityAcceleration;
      m_numberCamPosCoefSolved = 3;
    }
    else if (csolve == "ALL"){
      m_instrumentPositionSolveOption = AllPositionCoefficients;
      m_numberCamPosCoefSolved = m_spkSolveDegree + 1;
    }

    m_positionAprioriSigma.resize(m_numberCamPosCoefSolved);
    m_positionAprioriSigma.fill(-1.0);

    // If TWIST is not specified, then a default of YES is used
    if (scParameterGroup.hasKeyword("TWIST")) {
      QString parval = (QString)scParameterGroup.findKeyword("TWIST");
      parval = parval.toUpper();
      if (parval == "TRUE" || parval == "YES")
        m_solveTwist = true;
      else if (parval == "FALSE" || parval == "NO")
        m_solveTwist = false;
      else {
        QString msg = "The TWIST parameter must be set to TRUE or FALSE; YES or NO";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // If OVERHERMITE is not specified, then a default of NO is used
    if (scParameterGroup.hasKeyword("OVERHERMITE")) {
      QString parval = (QString)scParameterGroup.findKeyword("OVERHERMITE");
      parval = parval.toUpper();
      if (parval == "TRUE" || parval == "YES") {
        m_solvePositionOverHermiteSpline = true;
        m_positionInterpolationType = SpicePosition::PolyFunctionOverHermiteConstant;
      }
      else if (parval == "FALSE" || parval == "NO") {
        m_solvePositionOverHermiteSpline = false;
        m_positionInterpolationType = SpicePosition::PolyFunction;
      }
      else {
        QString msg = "The OVERHERMITE parameter must be set to TRUE or FALSE; YES or NO";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (csolve != "NONE") {
      if (scParameterGroup.hasKeyword("CAMERA_ANGLES_SIGMA"))
        m_anglesAprioriSigma[0] = (double)(scParameterGroup.findKeyword("CAMERA_ANGLES_SIGMA"));
      if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA"))
        m_anglesAprioriSigma[1] =
            (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA"));
      if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA"))
        m_anglesAprioriSigma[2] =
            (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA"));
    }

    if (ssolve != "NONE") {
      if (scParameterGroup.hasKeyword("SPACECRAFT_POSITION_SIGMA"))
        m_positionAprioriSigma[0] = (double)(scParameterGroup.findKeyword("SPACECRAFT_POSITION_SIGMA"));
      if (scParameterGroup.hasKeyword("SPACECRAFT_VELOCITY_SIGMA"))
        m_positionAprioriSigma[1] = (double)(scParameterGroup.findKeyword("SPACECRAFT_VELOCITY_SIGMA"));
      if (scParameterGroup.hasKeyword("SPACECRAFT_ACCELERATION_SIGMA"))
        m_positionAprioriSigma[2] = (double)(scParameterGroup.findKeyword("SPACECRAFT_ACCELERATION_SIGMA"));
    }

    return true;
  }

}
