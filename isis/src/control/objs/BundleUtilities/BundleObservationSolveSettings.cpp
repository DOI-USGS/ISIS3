#include "BundleObservationSolveSettings.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QUuid>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include "BundleImage.h"
#include "Camera.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


namespace Isis {

  /**
   * Constructor with default parameter initializations.
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(QObject *parent)
      : QObject(parent) {
    initialize();
  }



  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(Project *project, 
                                                                 XmlStackedHandlerReader *xmlReader,
                                                                 QObject *parent)
      : QObject(parent) {
    initialize();
    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));
  }



  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(FileName xmlFile,
                                                                 Project *project, 
                                                                 XmlStackedHandlerReader *xmlReader,
                                                                 QObject *parent)
      : QObject(parent) {

    initialize();
    QString xmlPath = xmlFile.expanded();
    QFile qXmlFile(xmlPath);
    if (!qXmlFile.open(QFile::ReadOnly) ) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with read access").arg(xmlPath),
                       _FILEINFO_);
    }

    QXmlInputSource xmlInputSource(&qXmlFile);
    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));
    bool success = xmlReader->parse(xmlInputSource);
    if (!success) {
      throw IException(IException::Unknown, 
                       QString("Failed to parse xml file, [%1]").arg(xmlPath),
                        _FILEINFO_);
    }
  }



  /**
   * Copy constructor.
   */
//  BundleObservationSolveSettings::BundleObservationSolveSettings(const BundleObservationSolveSettings &other)
//      : m_id(new QUuid(other.m_id->toString())),
//        m_instrumentId(other.m_instrumentId),
//        m_instrumentPointingSolveOption(other.m_instrumentPointingSolveOption),
//        m_solveTwist(other.m_solveTwist),
//        m_ckDegree(other.m_ckDegree),
//        m_ckSolveDegree(other.m_ckSolveDegree),
//        m_numberCamAngleCoefSolved(other.m_numberCamAngleCoefSolved),
//        m_solvePointingPolynomialOverExisting(other.m_solvePointingPolynomialOverExisting),
//        m_anglesAprioriSigma(other.m_anglesAprioriSigma),
//        m_pointingInterpolationType(other.m_pointingInterpolationType),
//        m_instrumentPositionSolveOption(other.m_instrumentPositionSolveOption),
//        m_spkDegree(other.m_spkDegree),
//        m_spkSolveDegree(other.m_spkSolveDegree),
//        m_numberCamPosCoefSolved(other.m_numberCamPosCoefSolved),
//        m_solvePositionOverHermiteSpline(other.m_solvePositionOverHermiteSpline),
//        m_positionAprioriSigma(other.m_positionAprioriSigma),
//        m_positionInterpolationType(other.m_positionInterpolationType) {
//  }
  BundleObservationSolveSettings::BundleObservationSolveSettings(const BundleObservationSolveSettings &other) {

    m_id = NULL;
    m_id = new QUuid(other.m_id->toString());
     // TODO: add check to all copy constructors (verify other.xxx is not null) and operator= ??? or intit all variables in all constructors

    m_instrumentId = other.m_instrumentId;
    m_instrumentPointingSolveOption = other.m_instrumentPointingSolveOption;
    m_solveTwist = other.m_solveTwist;
    m_ckDegree = other.m_ckDegree;
    m_ckSolveDegree = other.m_ckSolveDegree;
    m_numberCamAngleCoefSolved = other.m_numberCamAngleCoefSolved;
    m_solvePointingPolynomialOverExisting = other.m_solvePointingPolynomialOverExisting;
    m_anglesAprioriSigma = other.m_anglesAprioriSigma;
    m_pointingInterpolationType = other.m_pointingInterpolationType;
    m_instrumentPositionSolveOption = other.m_instrumentPositionSolveOption;
    m_spkDegree = other.m_spkDegree;
    m_spkSolveDegree = other.m_spkSolveDegree;
    m_numberCamPosCoefSolved = other.m_numberCamPosCoefSolved;
    m_solvePositionOverHermiteSpline = other.m_solvePositionOverHermiteSpline;
    m_positionAprioriSigma = other.m_positionAprioriSigma;
    m_positionInterpolationType = other.m_positionInterpolationType;
  }



  /**
   * Destructor.
   */
  BundleObservationSolveSettings::~BundleObservationSolveSettings() {

    delete m_id;
    m_id = NULL;

  }



  // TODO: CHECK THIS!!! equals operator
  BundleObservationSolveSettings 
      &BundleObservationSolveSettings::operator=(const BundleObservationSolveSettings &other) {
    if (&other != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(other.m_id->toString());
      
      m_instrumentId = other.m_instrumentId;
      
      // position related
      m_instrumentPositionSolveOption = other.m_instrumentPositionSolveOption;
      m_spkDegree = other.m_spkDegree;
      m_spkSolveDegree = other.m_spkSolveDegree;
      m_numberCamPosCoefSolved = other.m_numberCamPosCoefSolved;
      m_solvePositionOverHermiteSpline = other.m_solvePositionOverHermiteSpline;
      m_positionInterpolationType = other.m_positionInterpolationType;
      m_positionAprioriSigma = other.m_positionAprioriSigma;
      
      // pointing related
      m_instrumentPointingSolveOption = other.m_instrumentPointingSolveOption;
      m_solveTwist = other.m_solveTwist;
      m_ckDegree = other.m_ckDegree;
      m_ckSolveDegree = other.m_ckSolveDegree;
      m_numberCamAngleCoefSolved = other.m_numberCamAngleCoefSolved;
      m_solvePointingPolynomialOverExisting = other.m_solvePointingPolynomialOverExisting;
      m_pointingInterpolationType = other.m_pointingInterpolationType;
      m_anglesAprioriSigma = other.m_anglesAprioriSigma;
    }

    return *this;

  }



  void BundleObservationSolveSettings::initialize() {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());
    
    m_instrumentId = "";

    // Spacecraft Position Options
    m_instrumentPositionSolveOption = NoPositionFactors;
    m_spkDegree = 2;
    m_spkSolveDegree = -1;// ??? default to match no position factors is -1
    m_solvePositionOverHermiteSpline = false;
    m_positionInterpolationType = SpicePosition::PolyFunction;
    m_positionAprioriSigma.clear();

    // Camera Pointing Options
    m_instrumentPointingSolveOption = AnglesOnly;
    m_solveTwist = true;
    m_ckDegree = 2;
    m_ckSolveDegree = 0; // ??? default to match angles only is 0
    m_solvePointingPolynomialOverExisting = false;
    m_pointingInterpolationType = SpiceRotation::PolyFunction;
    m_numberCamAngleCoefSolved = 1;
    m_anglesAprioriSigma.clear();
  }



  // =============================================================================================//
  // =============================================================================================//
  // =============================================================================================//



  /**
   * set bundle solve parameters for an observation
   */
  bool BundleObservationSolveSettings::setFromPvl(PvlGroup& scParameterGroup) {



//??? use new setters




    // group name must be instrument id
    m_instrumentId = (QString)scParameterGroup.nameKeyword();

    // If CKDEGREE is not specified, then a default of 2 is used
    if (scParameterGroup.hasKeyword("CKDEGREE")) {
      m_ckDegree = (int)(scParameterGroup.findKeyword("CKDEGREE"));
    }

    // If CKSOLVEDEGREE is not specified, then a default of 2 is used -------jwb----- why ??? why not match camsolve option ???
    if (scParameterGroup.hasKeyword("CKSOLVEDEGREE")) {
      m_ckSolveDegree = (int) (scParameterGroup.findKeyword("CKSOLVEDEGREE"));
    }

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

    m_anglesAprioriSigma.clear();

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

    m_positionAprioriSigma.clear();

    // If TWIST is not specified, then a default of YES is used
    if (scParameterGroup.hasKeyword("TWIST")) {
      QString parval = (QString)scParameterGroup.findKeyword("TWIST");
      parval = parval.toUpper();
      if (parval == "TRUE" || parval == "YES") { // is this necessary ??? i think pvl and toString can handle it...
        m_solveTwist = true;
      }
      else if (parval == "FALSE" || parval == "NO") {
        m_solveTwist = false;
      }
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
        m_anglesAprioriSigma.append((double)(scParameterGroup.findKeyword("CAMERA_ANGLES_SIGMA")));
      if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA"))
        m_anglesAprioriSigma.append(
            (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA")));
      if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA"))
        m_anglesAprioriSigma.append(
            (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA")));
    }

    if (ssolve != "NONE") {
      if (scParameterGroup.hasKeyword("SPACECRAFT_POSITION_SIGMA"))
        m_positionAprioriSigma.append((double)(scParameterGroup.findKeyword("SPACECRAFT_POSITION_SIGMA")));
      if (scParameterGroup.hasKeyword("SPACECRAFT_VELOCITY_SIGMA"))
        m_positionAprioriSigma.append((double)(scParameterGroup.findKeyword("SPACECRAFT_VELOCITY_SIGMA")));
      if (scParameterGroup.hasKeyword("SPACECRAFT_ACCELERATION_SIGMA"))
        m_positionAprioriSigma.append((double)(scParameterGroup.findKeyword("SPACECRAFT_ACCELERATION_SIGMA")));
    }

    return true;
  }



  /**
   * TODO
   */
  void BundleObservationSolveSettings::setInstrumentId(QString instrumentId) {
    m_instrumentId = instrumentId;
  }



  /**
   * TODO
   */
  QString BundleObservationSolveSettings::instrumentId() const {
    return m_instrumentId;
  }



  // =============================================================================================//
  // ======================== Camera Pointing Options ============================================//
  // =============================================================================================//
  BundleObservationSolveSettings::InstrumentPointingSolveOption
      BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPointingFactors;
    }
    else if (option.compare("NoPointingFactors", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPointingFactors;
    }
    else if (option.compare("ANGLES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesOnly;
    }
    else if (option.compare("AnglesOnly", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocity;
    }
    else if (option.compare("AnglesAndVelocity", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocityAcceleration;
    }
    else if (option.compare("AnglesVelocityAndAcceleration", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AnglesVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AllPointingCoefficients;
    }
    else if (option.compare("AllPolynomialCoefficients", Qt::CaseInsensitive) == 0) {
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



  
  void BundleObservationSolveSettings::setInstrumentPointingSettings(
                                           InstrumentPointingSolveOption option, 
                                           bool solveTwist, 
                                           int ckDegree, 
                                           int ckSolveDegree,
                                           bool solvePolynomialOverExisting, 
                                           double anglesAprioriSigma, 
                                           double angularVelocityAprioriSigma, 
                                           double angularAccelerationAprioriSigma) {

    // automatically set the solve option and ck degree to the user entered values
    m_instrumentPointingSolveOption = option;
    m_ckDegree = ckDegree;

    // the ck solve degree entered is only used if we are solving for all coefficients, otherwise
    // it matches the appropriate polynomial size. also, we will only solve for polynomial over
    // the existing if we are solving for all coefficients ??? VERIFY

    if (option == AllPointingCoefficients) {
      m_ckSolveDegree = ckSolveDegree;
    }
    else {
        m_ckSolveDegree = ((int) option) - 1;
    }

    // we are solving for (solve degree + 1) coefficients
    // this is the maximum number of apriori sigmas allowed
    m_numberCamAngleCoefSolved = m_ckSolveDegree + 1;

    m_anglesAprioriSigma.clear();
    if (m_numberCamAngleCoefSolved > 0) {
      m_anglesAprioriSigma.append(anglesAprioriSigma);

      if (m_numberCamAngleCoefSolved > 1) {
        m_anglesAprioriSigma.append(angularVelocityAprioriSigma);

        if (m_numberCamAngleCoefSolved > 2) {
          m_anglesAprioriSigma.append(angularAccelerationAprioriSigma);
        }
      }
    }

    m_solveTwist = solveTwist; // dependent on solve option???

    // Set the SpiceRotation interpolation type enum appropriately 
    m_solvePointingPolynomialOverExisting = solvePolynomialOverExisting;
    if (m_solvePointingPolynomialOverExisting) {
      m_pointingInterpolationType = SpiceRotation::PolyFunctionOverSpice;
    }
    else {
      m_pointingInterpolationType = SpiceRotation::PolyFunction;
    }
  
  }



  BundleObservationSolveSettings::InstrumentPointingSolveOption
      BundleObservationSolveSettings::instrumentPointingSolveOption() const {
    return m_instrumentPointingSolveOption;
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
  int BundleObservationSolveSettings::ckDegree() const {
    return m_ckDegree;
  }



  /**
   * TODO
   */
  int BundleObservationSolveSettings::ckSolveDegree()const {
    return m_ckSolveDegree;
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
  bool BundleObservationSolveSettings::solvePolyOverPointing() const {
    return m_solvePointingPolynomialOverExisting;
  }



  /**
   * TODO
   */
  QList<double> BundleObservationSolveSettings::aprioriPointingSigmas() const {
    return m_anglesAprioriSigma;
  }



  /**
   * TODO
   */
  SpiceRotation::Source BundleObservationSolveSettings::pointingInterpolationType() const {
    return m_pointingInterpolationType;
  }


  // =============================================================================================//
  // ======================== Spacecraft Position Options ========================================//
  // =============================================================================================//
  BundleObservationSolveSettings::InstrumentPositionSolveOption
      BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(QString option) {
    if (option.compare("NONE", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPositionFactors;
    }
    else if (option.compare("NoPositionFactors", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::NoPositionFactors;
    }
    else if (option.compare("POSITIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionOnly;
    }
    else if (option.compare("PositionOnly", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionOnly;
    }
    else if (option.compare("VELOCITIES", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocity;
    }
    else if (option.compare("PositionAndVelocity", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocity;
    }
    else if (option.compare("ACCELERATIONS", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocityAcceleration;
    }
    else if (option.compare("PositionVelocityAndAcceleration", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::PositionVelocityAcceleration;
    }
    else if (option.compare("ALL", Qt::CaseInsensitive) == 0) {
      return BundleObservationSolveSettings::AllPositionCoefficients;
    }
    else if (option.compare("AllPolynomialCoefficients", Qt::CaseInsensitive) == 0) {
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
  void BundleObservationSolveSettings::setInstrumentPositionSettings(
                                           InstrumentPositionSolveOption option, 
                                           int spkDegree, 
                                           int spkSolveDegree, 
                                           bool positionOverHermite, 
                                           double positionAprioriSigma, 
                                           double velocityAprioriSigma, 
                                           double accelerationAprioriSigma) {
    // automatically set the solve option and spk degree to the user entered values
    m_instrumentPositionSolveOption = option;
    m_spkDegree = spkDegree;

    // the spk solve degree entered is only used if we are solving for all coefficients, otherwise
    // it matches the appropriate polynomial size. also, we will only solve for position over a
    // Hermite spline if we are solving for all coefficients ??? VERIFY
    if (option == AllPositionCoefficients) {
      m_spkSolveDegree = spkSolveDegree;
    }
    else {
      m_spkSolveDegree = ((int) option) - 1;
    }

    // we are solving for (solve degree + 1) coefficients
    // this is the maximum number of apriori sigmas allowed
    m_numberCamPosCoefSolved = m_spkSolveDegree + 1;

    m_positionAprioriSigma.clear();
    if (m_numberCamPosCoefSolved > 0) {
      m_positionAprioriSigma.append(positionAprioriSigma);
      if (m_numberCamPosCoefSolved > 1) {
        m_positionAprioriSigma.append(velocityAprioriSigma);
        if (m_numberCamPosCoefSolved > 2) {
          m_positionAprioriSigma.append(accelerationAprioriSigma);
        }
      }
    }

    // Set the SpicePosition interpolation type enum appropriately 
    m_solvePositionOverHermiteSpline = positionOverHermite;
    if (m_solvePositionOverHermiteSpline) {
      m_positionInterpolationType = SpicePosition::PolyFunctionOverHermiteConstant;
    }
    else {
      m_positionInterpolationType = SpicePosition::PolyFunction;
    }

  }



  BundleObservationSolveSettings::InstrumentPositionSolveOption
      BundleObservationSolveSettings::instrumentPositionSolveOption() const {
    return m_instrumentPositionSolveOption;
  }



  int BundleObservationSolveSettings::spkDegree() const {
    return m_spkDegree;
  }



  int BundleObservationSolveSettings::spkSolveDegree() const {
    return m_spkSolveDegree;
  }



  /**
   * TODO
   */
  int BundleObservationSolveSettings::numberCameraPositionCoefficientsSolved() const {
    return m_numberCamPosCoefSolved;
  }



  bool BundleObservationSolveSettings::solvePositionOverHermite() const {
    return m_solvePositionOverHermiteSpline;
  }



  /**
   * TODO
   */
  QList<double> BundleObservationSolveSettings::aprioriPositionSigmas() const {
    return m_positionAprioriSigma;
  }



  SpicePosition::Source BundleObservationSolveSettings::positionInterpolationType() const {
    return m_positionInterpolationType;
  }

  // =============================================================================================//
  // =============================================================================================//
  // =============================================================================================//

  PvlObject BundleObservationSolveSettings::pvlObject(QString name) const {

    QString pvlName = "";;
    if (name == "") {
      pvlName = m_instrumentId;
    }
    else {
      pvlName = name;
    }
    PvlObject pvl(pvlName);

    pvl += PvlKeyword("InstrumentPointingSolveOption", 
                      instrumentPointingSolveOptionToString(m_instrumentPointingSolveOption));
    if (m_instrumentPointingSolveOption > NoPointingFactors) {
      pvl += PvlKeyword("SolveTwist", toString(m_solveTwist));
      pvl += PvlKeyword("CKDegree", toString(m_ckDegree));
      pvl += PvlKeyword("CKSolveDegree", toString(m_ckSolveDegree));
      pvl += PvlKeyword("NumberAngleCoefficientsSolved", toString(m_numberCamAngleCoefSolved));
      pvl += PvlKeyword("SolvePointingPolynomialOverExisting", 
                        toString(m_solvePointingPolynomialOverExisting)); 
      PvlKeyword angleSigmas("AngleAprioriSigmas");
      if (m_anglesAprioriSigma.size() > 0) {
        for (int i = 0; i < m_anglesAprioriSigma.size(); i++) {
          if (m_anglesAprioriSigma[i] > 0) {
            angleSigmas.addValue(toString(m_anglesAprioriSigma[i]));
          }
          else {
            angleSigmas.addValue("N/A");
          }
        }
      }
      else {
        angleSigmas.addValue("N/A");
      }
      pvl += angleSigmas;
      
      pvl += PvlKeyword("InstrumentPointingInterpolationType",
                      toString((int)m_pointingInterpolationType));  // TODO: omit from pvl if pointing option == None ??? 
    }
    else {
      pvl += PvlKeyword("SolveTwist", "N/A");  // TODO: omit from pvl if pointing option == None ??? 
      pvl += PvlKeyword("CKDegree", "N/A");  // TODO: omit from pvl if pointing option == None ??? 
      pvl += PvlKeyword("CKSolveDegree", "N/A"); // or omit from pvl ??? 
      pvl += PvlKeyword("NumberAngleCoefficientsSolved", "N/A");
      pvl += PvlKeyword("SolvePointingPolynomialOverExisting", "N/A");
      pvl += PvlKeyword("AngleAprioriSigmas", "N/A");  // TODO: omit from pvl if pointing option == None ??? 
      pvl += PvlKeyword("InstrumentPointingInterpolationType", "N/A");
    }


    // position

    pvl += PvlKeyword("InstrumentPositionSolveOption", 
                      instrumentPositionSolveOptionToString(m_instrumentPositionSolveOption));
    if (m_instrumentPositionSolveOption != NoPositionFactors) {
      pvl += PvlKeyword("SPKDegree", toString(m_spkDegree));
      pvl += PvlKeyword("SPKSolveDegree", toString(m_spkSolveDegree));
      pvl += PvlKeyword("NumberPositionCoefficientsSolved", toString(m_numberCamPosCoefSolved));
      pvl += PvlKeyword("SolvePositionOverHermiteSpline", toString(m_solvePositionOverHermiteSpline));
    
      PvlKeyword positionSigmas("PositionAprioriSigmas");
      for (int i = 0; i < m_positionAprioriSigma.size(); i++) {
        if (m_positionAprioriSigma[i] > 0) {
          positionSigmas.addValue(toString(m_positionAprioriSigma[i]));
        }
        else {
          positionSigmas.addValue("N/A");
        }
    
      }
      pvl += positionSigmas;
    
      pvl += PvlKeyword("InstrumentPositionInterpolationType",
                        toString((int)m_positionInterpolationType));
    }
    else {
      pvl += PvlKeyword("SPKDegree", "N/A");
      pvl += PvlKeyword("SPKSolveDegree", "N/A");
      pvl += PvlKeyword("NumberPositionCoefficientsSolved", "N/A");
      pvl += PvlKeyword("SolvePositionOverHermiteSpline", "N/A");
      pvl += PvlKeyword("PositionAprioriSigmas", "N/A");
      pvl += PvlKeyword("InstrumentPositionInterpolationType", "N/A");
    }
    return pvl;
  }



  void BundleObservationSolveSettings::save(QXmlStreamWriter &stream, const Project *project) const {  // TODO: does xml stuff need project???

    stream.writeStartElement("bundleObservationSolveSettings");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("instrumentId", m_instrumentId);

    // pointing related
    stream.writeStartElement("instrumentPointingOptions");
    stream.writeAttribute("solveOption", 
                           instrumentPointingSolveOptionToString(m_instrumentPointingSolveOption));
    stream.writeAttribute("solveTwist", toString(m_solveTwist));
    stream.writeAttribute("degree", toString(m_ckDegree));
    stream.writeAttribute("solveDegree", toString(m_ckSolveDegree));
    stream.writeAttribute("numberCoefSolved", toString(m_numberCamAngleCoefSolved));
    stream.writeAttribute("solveOverExisting", toString(m_solvePointingPolynomialOverExisting));
    stream.writeAttribute("interpolationType", toString(m_pointingInterpolationType));
    stream.writeStartElement("aprioriPointingSigmas");
    if (m_anglesAprioriSigma.size() > 0) {
      stream.writeAttribute("angles", toString(m_anglesAprioriSigma[0]));
    }
    else {
      stream.writeAttribute("angles", "N/A");
    }
    if (m_anglesAprioriSigma.size() > 1) {
      stream.writeAttribute("angularVelocity", toString(m_anglesAprioriSigma[1]));
    }
    else {
      stream.writeAttribute("angularVelocity", "N/A");
    }
    if (m_anglesAprioriSigma.size() > 2) {
      stream.writeAttribute("angularAcceleration", toString(m_anglesAprioriSigma[2]));
    }
    else {
      stream.writeAttribute("angularAcceleration", "N/A");
    }
    stream.writeEndElement();// end aprioriPointingSigmas
    stream.writeEndElement();// end instrumentPointingOptions

    // position related
    stream.writeStartElement("instrumentPositionOptions");
    stream.writeAttribute("solveOption", 
                           instrumentPositionSolveOptionToString(m_instrumentPositionSolveOption));
    stream.writeAttribute("degree", toString(m_spkDegree));
    stream.writeAttribute("solveDegree", toString(m_spkSolveDegree));
    stream.writeAttribute("numberCoefSolved", toString(m_numberCamPosCoefSolved));
    stream.writeAttribute("solveOverHermiteSpline", toString(m_solvePositionOverHermiteSpline));
    stream.writeAttribute("interpolationType", toString(m_positionInterpolationType));
    stream.writeStartElement("aprioriPositionSigmas");
    if (m_positionAprioriSigma.size() > 0) {
      stream.writeAttribute("position", toString(m_positionAprioriSigma[0]));
    }
    else {
      stream.writeAttribute("position", "N/A");
    }
    if (m_positionAprioriSigma.size() > 1) {
      stream.writeAttribute("velocity", toString(m_positionAprioriSigma[1]));
    }
    else {
      stream.writeAttribute("velocity", "N/A");
    }
    if (m_positionAprioriSigma.size() > 2) {
      stream.writeAttribute("acceleration", toString(m_positionAprioriSigma[2]));
    }
    else {
      stream.writeAttribute("acceleration", "N/A");
    }
    stream.writeEndElement();// end aprioriPositionSigmas
    stream.writeEndElement(); // end instrumentPositionOptions
    stream.writeEndElement(); // end bundleObservationSolveSettings

  }



  BundleObservationSolveSettings::XmlHandler::XmlHandler(BundleObservationSolveSettings *settings, 
                                                         Project *project) {  // TODO: does xml stuff need project???
    m_observationSettings = settings;
    m_project = project;  // TODO: does xml stuff need project???
    m_characters = "";
  }



  BundleObservationSolveSettings::XmlHandler::~XmlHandler() {
    delete m_project;  // TODO: does xml stuff need project???
    m_project = NULL;
  }
  


  bool BundleObservationSolveSettings::XmlHandler::startElement(const QString &namespaceURI, 
                                                                const QString &localName,
                                                                const QString &qName,
                                                                const QXmlAttributes &atts) {
    m_characters = "";
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "instrumentPointingOptions") {
        
        QString pointingSolveOption = atts.value("solveOption");
        if (!pointingSolveOption.isEmpty()) {
          m_observationSettings->m_instrumentPointingSolveOption
              = stringToInstrumentPointingSolveOption(pointingSolveOption);
        }

        QString solveTwist = atts.value("solveTwist");
        if (!solveTwist.isEmpty()) {
          m_observationSettings->m_solveTwist = toBool(solveTwist);
        }

        QString ckDegree = atts.value("degree");
        if (!ckDegree.isEmpty()) {
          m_observationSettings->m_ckDegree = toInt(ckDegree);
        }

        QString ckSolveDegree = atts.value("solveDegree");
        if (!ckSolveDegree.isEmpty()) {
          m_observationSettings->m_ckSolveDegree = toInt(ckSolveDegree);
        }

        QString numberCoefSolved = atts.value("numberCoefSolved");
        if (!numberCoefSolved.isEmpty()) {
          m_observationSettings->m_numberCamAngleCoefSolved = toInt(numberCoefSolved);
        }

        QString solveOverExisting = atts.value("solveOverExisting");
        if (!solveOverExisting.isEmpty()) {
          m_observationSettings->m_solvePointingPolynomialOverExisting = toBool(solveOverExisting);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_observationSettings->m_pointingInterpolationType = SpiceRotation::Source(toInt(interpolationType));
        }

      }
      else if (localName == "aprioriPointingSigmas") {
        QString anglesSigma = atts.value("angles");
        if (!anglesSigma.isEmpty() && anglesSigma != "N/A") {
          m_observationSettings->m_anglesAprioriSigma.append(toDouble(anglesSigma));
        }

        QString velocitySigma = atts.value("angularVelocity");
        if (!velocitySigma.isEmpty() && velocitySigma != "N/A") {
          m_observationSettings->m_anglesAprioriSigma.append(toDouble(velocitySigma));
        }

        QString accelerationSigma = atts.value("angularAcceleration");
        if (!accelerationSigma.isEmpty() && accelerationSigma != "N/A") {
          m_observationSettings->m_anglesAprioriSigma.append(toDouble(accelerationSigma));
        }
      }
      else if (localName == "instrumentPositionOptions") {
        
        QString positionSolveOption = atts.value("solveOption");
        if (!positionSolveOption.isEmpty()) {
          m_observationSettings->m_instrumentPositionSolveOption
              = stringToInstrumentPositionSolveOption(positionSolveOption);
        }

        QString spkDegree = atts.value("degree");
        if (!spkDegree.isEmpty()) {
          m_observationSettings->m_spkDegree = toInt(spkDegree);
        }

        QString spkSolveDegree = atts.value("solveDegree");
        if (!spkSolveDegree.isEmpty()) {
          m_observationSettings->m_spkSolveDegree = toInt(spkSolveDegree);
        }

        QString numberCoefSolved = atts.value("numberCoefSolved");
        if (!numberCoefSolved.isEmpty()) {
          m_observationSettings->m_numberCamPosCoefSolved = toInt(numberCoefSolved);
        }

        QString solveOverHermiteSpline = atts.value("solveOverHermiteSpline");
        if (!solveOverHermiteSpline.isEmpty()) {
          m_observationSettings->m_solvePositionOverHermiteSpline = toBool(solveOverHermiteSpline);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_observationSettings->m_positionInterpolationType = SpicePosition::Source(toInt(interpolationType));
        }
      }
      else if (localName == "aprioriPositionSigmas") {

        QString positionSigma = atts.value("position");
        if (!positionSigma.isEmpty() && positionSigma != "N/A") {
          m_observationSettings->m_positionAprioriSigma.append(toDouble(positionSigma));
        }

        QString velocitySigma = atts.value("velocity");
        if (!velocitySigma.isEmpty() && velocitySigma != "N/A") {
          m_observationSettings->m_positionAprioriSigma.append(toDouble(velocitySigma));
        }

        QString accelerationSigma = atts.value("acceleration");
        if (!accelerationSigma.isEmpty() && accelerationSigma != "N/A") {
          m_observationSettings->m_positionAprioriSigma.append(toDouble(accelerationSigma));
        }
      }
    }
    return true;
  }



  bool BundleObservationSolveSettings::XmlHandler::characters(const QString &ch) {
    m_characters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool BundleObservationSolveSettings::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
    if (localName == "id") {
      m_observationSettings->m_id = NULL;
      m_observationSettings->m_id = new QUuid(m_characters);
    }
    else if (localName == "instrumentId") {
      m_observationSettings->m_instrumentId = m_characters;
    }
    else if (localName == "bundleObservationSolveSettings") {
      // end tag for this entire class... how to get out??? 
      // call parse, as in Control List???
    }
    m_characters = "";
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }



  QDataStream &BundleObservationSolveSettings::write(QDataStream &stream) const {

    stream << m_id->toString()
           << m_instrumentId
           << (qint32)m_instrumentPointingSolveOption
           << m_solveTwist
           << (qint32)m_ckDegree
           << (qint32)m_ckSolveDegree
           << (qint32)m_numberCamAngleCoefSolved
           << m_solvePointingPolynomialOverExisting
           << m_anglesAprioriSigma
           << (qint32)m_pointingInterpolationType
           << (qint32)m_instrumentPositionSolveOption
           << (qint32)m_spkDegree
           << (qint32)m_spkSolveDegree
           << (qint32)m_numberCamPosCoefSolved 
           << m_solvePositionOverHermiteSpline
           << m_positionAprioriSigma
           << (qint32)m_positionInterpolationType;

    return stream;

  }



  QDataStream &BundleObservationSolveSettings::read(QDataStream &stream) {

    QString id;
    qint32 anglesSolveOption, ckDegree, ckSolveDegree, numCamAngleCoefSolved, anglesInterpType,
           positionSolveOption, spkDegree, spkSolveDegree, numCamPosCoefSolved, positionInterpType;

    stream >> id 
           >> m_instrumentId
           >> anglesSolveOption
           >> m_solveTwist
           >> ckDegree
           >> ckSolveDegree
           >> numCamAngleCoefSolved
           >> m_solvePointingPolynomialOverExisting
           >> m_anglesAprioriSigma
           >> anglesInterpType
           >> positionSolveOption
           >> spkDegree
           >> spkSolveDegree
           >> numCamPosCoefSolved
           >> m_solvePositionOverHermiteSpline
           >> m_positionAprioriSigma
           >> positionInterpType;

    delete m_id;
    m_id = NULL;
    m_id = new QUuid(id);

    m_instrumentPointingSolveOption = (InstrumentPointingSolveOption)anglesSolveOption;
    m_ckDegree = (int)ckDegree;
    m_ckSolveDegree = (int)ckSolveDegree;
    m_numberCamAngleCoefSolved = (int)numCamAngleCoefSolved;
    m_pointingInterpolationType = (SpiceRotation::Source)anglesInterpType;
    m_instrumentPositionSolveOption = (InstrumentPositionSolveOption)positionSolveOption;
    m_spkDegree = (int)spkDegree;
    m_spkSolveDegree = (int)spkSolveDegree;
    m_numberCamPosCoefSolved = (int)numCamPosCoefSolved;
    m_positionInterpolationType = (SpicePosition::Source)positionInterpType;

    return stream;

  }



  QDataStream &operator<<(QDataStream &stream, const BundleObservationSolveSettings &settings) {
    return settings.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, BundleObservationSolveSettings &settings) {
    return settings.read(stream);
  }

}
