#include "BundleObservationSolveSettings.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QXmlInputSource>
#include <QXmlStreamWriter>

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
  BundleObservationSolveSettings::BundleObservationSolveSettings() {
    initialize();
  }


  /**
   * Construct this BundleObservationSolveSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides -
   *                             /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(
                                                             Project *project,
                                                             XmlStackedHandlerReader *xmlReader) {
    initialize();
    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));
  }


#if 0
  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(
                                                             FileName xmlFile,
                                                             Project *project,
                                                             XmlStackedHandlerReader *xmlReader) {

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
#endif


//  BundleObservationSolveSettings::BundleObservationSolveSettings(const BundleObservationSolveSettings &other)
//      : m_id(new QUuid(other.m_id->toString())),
//        m_instrumentId(other.m_instrumentId),
//        m_instrumentPointingSolveOption(other.m_instrumentPointingSolveOption),
//        m_numberCamAngleCoefSolved(other.m_numberCamAngleCoefSolved),
//        m_ckDegree(other.m_ckDegree),
//        m_ckSolveDegree(other.m_ckSolveDegree),
//        m_solveTwist(other.m_solveTwist),
//        m_solvePointingPolynomialOverExisting(other.m_solvePointingPolynomialOverExisting),
//        m_anglesAprioriSigma(other.m_anglesAprioriSigma),
//        m_pointingInterpolationType(other.m_pointingInterpolationType),
//        m_instrumentPositionSolveOption(other.m_instrumentPositionSolveOption),
//        m_numberCamPosCoefSolved(other.m_numberCamPosCoefSolved),
//        m_spkDegree(other.m_spkDegree),
//        m_spkSolveDegree(other.m_spkSolveDegree),
//        m_solvePositionOverHermiteSpline(other.m_solvePositionOverHermiteSpline),
//        m_positionAprioriSigma(other.m_positionAprioriSigma),
//        m_positionInterpolationType(other.m_positionInterpolationType) {
//  }
  /**
   * Constructs a BundleObservationSolveSettings from another one.
   *
   * @param other The BundleObservationSolveSettings to copy
   */
  BundleObservationSolveSettings::BundleObservationSolveSettings(const BundleObservationSolveSettings &other) {

    m_id = NULL;
    m_id = new QUuid(other.m_id->toString());
     // TODO: add check to all copy constructors (verify other.xxx is not null) and operator= ???
     // or intit all variables in all constructors

    m_instrumentId = other.m_instrumentId;
    m_instrumentPointingSolveOption = other.m_instrumentPointingSolveOption;
    m_observationNumbers = other.m_observationNumbers;
    m_numberCamAngleCoefSolved = other.m_numberCamAngleCoefSolved;
    m_ckDegree = other.m_ckDegree;
    m_ckSolveDegree = other.m_ckSolveDegree;
    m_solveTwist = other.m_solveTwist;
    m_solvePointingPolynomialOverExisting = other.m_solvePointingPolynomialOverExisting;
    m_anglesAprioriSigma = other.m_anglesAprioriSigma;
    m_pointingInterpolationType = other.m_pointingInterpolationType;
    m_instrumentPositionSolveOption = other.m_instrumentPositionSolveOption;
    m_numberCamPosCoefSolved = other.m_numberCamPosCoefSolved;
    m_spkDegree = other.m_spkDegree;
    m_spkSolveDegree = other.m_spkSolveDegree;
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


  /**
   * Assigns the state of another BundleObservationSolveSettings to this one.
   *
   * @param other The other BundleObservationSolveSettings to assign state from
   *
   * @internal
   *   @todo Check this (assignment operator)
   */
  BundleObservationSolveSettings
      &BundleObservationSolveSettings::operator=(const BundleObservationSolveSettings &other) {
    if (&other != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(other.m_id->toString());

      m_instrumentId = other.m_instrumentId;
      m_observationNumbers = other.m_observationNumbers;

      // pointing related
      m_instrumentPointingSolveOption = other.m_instrumentPointingSolveOption;
      m_numberCamAngleCoefSolved = other.m_numberCamAngleCoefSolved;
      m_ckDegree = other.m_ckDegree;
      m_ckSolveDegree = other.m_ckSolveDegree;
      m_solveTwist = other.m_solveTwist;
      m_solvePointingPolynomialOverExisting = other.m_solvePointingPolynomialOverExisting;
      m_pointingInterpolationType = other.m_pointingInterpolationType;
      m_anglesAprioriSigma = other.m_anglesAprioriSigma;

      // position related
      m_instrumentPositionSolveOption = other.m_instrumentPositionSolveOption;
      m_numberCamPosCoefSolved = other.m_numberCamPosCoefSolved;
      m_spkDegree = other.m_spkDegree;
      m_spkSolveDegree = other.m_spkSolveDegree;
      m_solvePositionOverHermiteSpline = other.m_solvePositionOverHermiteSpline;
      m_positionInterpolationType = other.m_positionInterpolationType;
      m_positionAprioriSigma = other.m_positionAprioriSigma;

    }

    return *this;

  }


  /**
   * Initializes the default state of this BundleObservationSolveSettings.
   */
  void BundleObservationSolveSettings::initialize() {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());

    m_instrumentId = "";

    // Camera Pointing Options
    // Defaults:
    //     m_instrumentPointingSolveOption = AnglesOnly;
    //     m_numberCamAngleCoefSolved = 1; // AnglesOnly;
    //     m_ckDegree = 2;
    //     m_ckSolveDegree = 2;
    //     m_solveTwist = true;
    //     m_solvePointingPolynomialOverExisting = false;
    //     m_pointingInterpolationType = SpiceRotation::PolyFunction;
    //     m_anglesAprioriSigma.append(Isis::Null); // num cam angle
    //     coef = 1
    setInstrumentPointingSettings(AnglesOnly, true, 2, 2, false);

    // Spacecraft Position Options
    // Defaults:
    //     m_instrumentPositionSolveOption = NoPositionFactors;
    //     m_numberCamPosCoefSolved = 0; // NoPositionFactors;
    //     m_spkDegree = 2;
    //     m_spkSolveDegree = 2;
    //     m_solvePositionOverHermiteSpline = false;
    //     m_positionInterpolationType = SpicePosition::PolyFunction;
    //     m_positionAprioriSigma.clear();
    setInstrumentPositionSettings(NoPositionFactors, 2, 2, false);

  }


  // =============================================================================================//
  // =============================================================================================//
  // =============================================================================================//

  /**
   * Sets the instrument id for this observation.
   *
   * @param instrumentId QString instrument id
   */
  void BundleObservationSolveSettings::setInstrumentId(QString instrumentId) {
    m_instrumentId = instrumentId;
  }


  /**
   * Accesses the instrument id for this observation.
   *
   * @return @b QString Returns the instrument id for this observation
   */
  QString BundleObservationSolveSettings::instrumentId() const {
    return m_instrumentId;
  }


  /**
   * Associates an observation number with these solve settings.
   *
   * These solve settings are to be applied to any associated observations.
   *
   * @param observationNumber QString observation number to associate with these settings.
   */
  void BundleObservationSolveSettings::addObservationNumber(QString observationNumber) {
    m_observationNumbers.insert(observationNumber);
  }


  /**
   * Returns a list of observation numbers associated with these solve settings.
   *
   * @return @b QSet<QString> Returns a QSet containing the associated observation numbers.
   */
  QSet<QString> BundleObservationSolveSettings::observationNumbers() const {
    return m_observationNumbers;
  }


  // =============================================================================================//
  // ======================== Camera Pointing Options ============================================//
  // =============================================================================================//


  /**
   * Translates a QString InstrumentPointingSolveOption to its enumerated value.
   *
   * @param option QString representation of the instrument pointing solve option
   *
   * @throws IException::Unknown "Unknown bundle instrument point solve option."
   *
   * @return @b BundleObservationSolveSettings::InstrumentPointingSolveOption Returns the enumerated
   *     value of the instrument pointing solve option
   */
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
      throw IException(IException::Unknown,
                       "Unknown bundle instrument pointing solve option " + option + ".",
                       _FILEINFO_);
    }
  }


  /**
   * Tranlsates an enumerated InstrumentPointingSolveOption value to its string representation.
   *
   * @param option Enumerated InstrumentPointingSolveOption value
   *
   * @throws IException::Programmer "Unknown pointing solve option enum."
   *
   * @return @b QString Returns the QString representation of the passed
   *                    InstrumentPointingSolveOption
   */
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
   * Sets the instrument pointing settings.
   *
   * @param option Option for how to solve for instrument pointing
   * @param solveTwist Whether or not to solve for twist
   * @param ckDegree
   * @param ckSolveDegree
   * @param solvePolynomialOverExisting Indicates whether the polynomial will be fit over an
   *                                    existing pointing polynomial
   * @param anglesAprioriSigma A priori angle values
   * @param angularVelocityAprioriSigma A priori angular velocity
   * @param angularAccelerationAprioriSigma A priori angular acceleration
   */
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

    // the ck solve degree entered is only used if we are solving for all coefficients
    // otherwise it defaults to 2.
    if (option == AllPointingCoefficients) {
      // update spkDegree and spkSolveDegree
      m_ckDegree = ckDegree;
      m_ckSolveDegree = ckSolveDegree;

      // we are solving for (solve degree + 1) coefficients
      // this is the maximum number of apriori sigmas allowed
      m_numberCamAngleCoefSolved = m_ckSolveDegree + 1;
    }
    else {
      // let spkDegree and spkSolveDegree default to 2, 2
      m_ckDegree = 2;
      m_ckSolveDegree = 2;

      // solve for the appropriate number of coefficients, based on solve option enum
      m_numberCamAngleCoefSolved = ((int) option);
    }

    m_anglesAprioriSigma.clear();
    if (m_numberCamAngleCoefSolved > 0) {
      if (anglesAprioriSigma > 0.0) {
        m_anglesAprioriSigma.append(anglesAprioriSigma);
      }
      else {
        m_anglesAprioriSigma.append(Isis::Null);
      }

      if (m_numberCamAngleCoefSolved > 1) {
        if (angularVelocityAprioriSigma > 0.0) {
          m_anglesAprioriSigma.append(angularVelocityAprioriSigma);
        }
        else {
          m_anglesAprioriSigma.append(Isis::Null);
        }

        if (m_numberCamAngleCoefSolved > 2) {
          if (angularAccelerationAprioriSigma > 0.0) {
            m_anglesAprioriSigma.append(angularAccelerationAprioriSigma);
          }
          else {
            m_anglesAprioriSigma.append(Isis::Null);
          }
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


  /**
   * Accesses the instrument pointing solve option.
   *
   * @return @b BundleObservationSolveSettings::InstrumentPointingSolveOption Returns the
   *     instrument pointing solve option
   */
  BundleObservationSolveSettings::InstrumentPointingSolveOption
      BundleObservationSolveSettings::instrumentPointingSolveOption() const {
    return m_instrumentPointingSolveOption;
  }


  /**
   * Accesses the flag for solving for twist.
   *
   * @return @b bool Returns whether or not to solve for twist
   */
  bool BundleObservationSolveSettings::solveTwist() const {
    return m_solveTwist;
  }


  /**
   * Accesses the degree of polynomial fit to original camera angles (ckDegree).
   *
   * @return @b int Returns the degree of the polynomial fit to the original camera angles
   */
  int BundleObservationSolveSettings::ckDegree() const {
    return m_ckDegree;
  }


  /**
   * Accesses the degree of the camera angles polynomial being fit to in the bundle adjustment
   * (ckSolveDegree).
   *
   * @return @b int Returns the degree of the camera angles polynomial in the bundle adjustment
   */
  int BundleObservationSolveSettings::ckSolveDegree()const {
    return m_ckSolveDegree;
  }


  /**
   * Accesses the number of camera angle coefficients in the solution.
   *
   * @return @b int Returns the number of camera angle coefficients in the solution
   */
  int BundleObservationSolveSettings::numberCameraAngleCoefficientsSolved() const {
    return m_numberCamAngleCoefSolved;
  }


  /**
   * Whether or not the solve polynomial will be fit over the existing pointing polynomial.
   *
   * @return @b bool Indicates whether the polynomial will be fit over the existing pointing
   *                 polynomial
   */
  bool BundleObservationSolveSettings::solvePolyOverPointing() const {
    return m_solvePointingPolynomialOverExisting;
  }


  /**
   * Accesses the a priori pointing sigmas.
   *
   * @return @b QList<double> Returns a QList of the a priori pointing sigmas
   */
  QList<double> BundleObservationSolveSettings::aprioriPointingSigmas() const {
    return m_anglesAprioriSigma;
  }


  /**
   * Accesses the SpiceRotation interpolation type for the instrument pointing.
   *
   * @return @b SpiceRotation::Source Returns the SpiceRotation interpolation type for pointing
   */
  SpiceRotation::Source BundleObservationSolveSettings::pointingInterpolationType() const {
    return m_pointingInterpolationType;
  }


  // =============================================================================================//
  // ======================== Spacecraft Position Options ========================================//
  // =============================================================================================//


  /**
   * Translates a QString InstrumentPositionSolveOption to its enumerated value.
   *
   * @param option QString representation of an instrument position solve option
   *
   * @throws IExeption::Unknown "Unknown bundle instrument position solve option."
   *
   * @return @b BundleObservationSolveSettings::InstrumentPositionSolveOption Returns the enumerated
   *     value of the instrument position solve option
   */
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
      throw IException(IException::Unknown,
                          "Unknown bundle instrument position solve option " + option + ".",
                          _FILEINFO_);
    }
  }


  /**
   * Translates an enumerated InstrumentPositionSolveOption to its string representation.
   *
   * @param option Enumerated InstrumentPositionSolveOption value
   *
   * @throws IException::Programmer "Unknown position solve option enum."
   *
   * @return @b QString Returns the QString representation of the passed
   *                    InstrumentPointingSolveOption
   */
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
   * Sets the instrument pointing settings.
   *
   * @param option Option for how to solve the instrument position
   * @param spkDegree
   * @param spkSolveDegree
   * @param positionOverHermite Whether or not the polynomial will be fit over an existing Hermite
   *                            spline
   * @param positionAprioriSigma A priori position sigma
   * @param velocityAprioriSigma A priori velocity sigma
   * @param accelerationAprioriSigma A priori acceleration sigma
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

    // the spk solve degree entered is only used if we are solving for all coefficients
    // otherwise it defaults to 2.
    if (option == AllPositionCoefficients) {
      // update spkDegree and spkSolveDegree
      m_spkDegree = spkDegree;
      m_spkSolveDegree = spkSolveDegree;
      // we are solving for (solve degree + 1) coefficients
      // this is the maximum number of apriori sigmas allowed
      m_numberCamPosCoefSolved = m_spkSolveDegree + 1;
    }
    else {
      // let spkDegree and spkSolveDegree default to 2, 2
      m_spkDegree = 2;
      m_spkSolveDegree = 2;

      // solve for the appropriate number of coefficients, based on solve option enum
      m_numberCamPosCoefSolved = ((int) option);
    }

    m_positionAprioriSigma.clear();
    if (m_numberCamPosCoefSolved > 0) {
      if (positionAprioriSigma > 0.0) {
        m_positionAprioriSigma.append(positionAprioriSigma);
      }
      else {
        m_positionAprioriSigma.append(Isis::Null);
      }

      if (m_numberCamPosCoefSolved > 1) {
        if (velocityAprioriSigma > 0.0) {
          m_positionAprioriSigma.append(velocityAprioriSigma);
        }
        else {
          m_positionAprioriSigma.append(Isis::Null);
        }

        if (m_numberCamPosCoefSolved > 2) {
          if (accelerationAprioriSigma > 0.0) {
            m_positionAprioriSigma.append(accelerationAprioriSigma);
          }
          else {
            m_positionAprioriSigma.append(Isis::Null);
          }
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


  /**
   * Accesses the instrument position solve option.
   *
   * @return @b BundleObservationSolveSettings::InstrumentPositionSolveOption Returns the
   *     instrument position solve option
   */
  BundleObservationSolveSettings::InstrumentPositionSolveOption
      BundleObservationSolveSettings::instrumentPositionSolveOption() const {
    return m_instrumentPositionSolveOption;
  }


  /**
   * Accesses the degree of the polynomial fit to the original camera position (spkDegree).
   *
   * @return @b int Returns the degree of the original camera position polynomial.
   */
  int BundleObservationSolveSettings::spkDegree() const {
    return m_spkDegree;
  }


  /**
   * Accesses the degree of thecamera position polynomial being fit to in the bundle adjustment
   * (spkSolveDegree).
   *
   * @return @b int Returns the degree of the camera position polynomial in the bundle adjustment.
   */
  int BundleObservationSolveSettings::spkSolveDegree() const {
    return m_spkSolveDegree;
  }


  /**
   * Accesses the number of camera position coefficients in the solution.
   *
   * @return @b int Returns the number of camera position coefficients.
   */
  int BundleObservationSolveSettings::numberCameraPositionCoefficientsSolved() const {
    return m_numberCamPosCoefSolved;
  }


  /**
   * Whether or not the polynomial for solving will be fit over an existing Hermite spline
   *
   * @return @b bool Returns whether or not to fit the solve polynomial over an existing Hermite
   *                 spline
   */
  bool BundleObservationSolveSettings::solvePositionOverHermite() const {
    return m_solvePositionOverHermiteSpline;
  }


  /**
   * Accesses the a priori position sigmas.
   *
   * @return @b QList<double> Returns a QList of the a priori position sigmas
   */
  QList<double> BundleObservationSolveSettings::aprioriPositionSigmas() const {
    return m_positionAprioriSigma;
  }


  /**
   * Accesses the SpicePosition interpolation type for the spacecraft position
   *
   * @return @b SpicePosition::Source Returns the SpicePositon interpolation type for position
   */
  SpicePosition::Source BundleObservationSolveSettings::positionInterpolationType() const {
    return m_positionInterpolationType;
  }


  // =============================================================================================//
  // =============================================================================================//
  // =============================================================================================//


  /**
   * Saves this BundleObservationSolveSettings to an xml stream.
   *
   * @param stream A QXmlStreamWriter to write to
   * @param project Pointer to the current project
   *
   * @internal
   *   @todo Does xml stuff need project???
   */
  void BundleObservationSolveSettings::save(QXmlStreamWriter &stream,
                                            const Project *project) const {

    stream.writeStartElement("bundleObservationSolveSettings");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("instrumentId", instrumentId());

    // pointing related
    stream.writeStartElement("instrumentPointingOptions");
    stream.writeAttribute("solveOption",
                           instrumentPointingSolveOptionToString(m_instrumentPointingSolveOption));
    stream.writeAttribute("numberCoefSolved", toString(m_numberCamAngleCoefSolved));
    stream.writeAttribute("degree", toString(m_ckDegree));
    stream.writeAttribute("solveDegree", toString(m_ckSolveDegree));
    stream.writeAttribute("solveTwist", toString(m_solveTwist));
    stream.writeAttribute("solveOverExisting", toString(m_solvePointingPolynomialOverExisting));
    stream.writeAttribute("interpolationType", toString(m_pointingInterpolationType));

    stream.writeStartElement("aprioriPointingSigmas");
    for (int i = 0; i < m_anglesAprioriSigma.size(); i++) {
      if (IsSpecial(m_anglesAprioriSigma[i])) {
        stream.writeTextElement("sigma", "N/A");
      }
      else {
        stream.writeTextElement("sigma", toString(m_anglesAprioriSigma[i]));
      }
    }
    stream.writeEndElement();// end aprioriPointingSigmas
    stream.writeEndElement();// end instrumentPointingOptions

    // position related
    stream.writeStartElement("instrumentPositionOptions");
    stream.writeAttribute("solveOption",
                           instrumentPositionSolveOptionToString(m_instrumentPositionSolveOption));
    stream.writeAttribute("numberCoefSolved", toString(m_numberCamPosCoefSolved));
    stream.writeAttribute("degree", toString(m_spkDegree));
    stream.writeAttribute("solveDegree", toString(m_spkSolveDegree));
    stream.writeAttribute("solveOverHermiteSpline", toString(m_solvePositionOverHermiteSpline));
    stream.writeAttribute("interpolationType", toString(m_positionInterpolationType));

    stream.writeStartElement("aprioriPositionSigmas");
    for (int i = 0; i < m_positionAprioriSigma.size(); i++) {
      if (IsSpecial(m_positionAprioriSigma[i])) {
        stream.writeTextElement("sigma", "N/A");
      }
      else {
        stream.writeTextElement("sigma", toString(m_positionAprioriSigma[i]));
      }
    }
    stream.writeEndElement();// end aprioriPositionSigmas
    stream.writeEndElement(); // end instrumentPositionOptions

    stream.writeEndElement(); // end bundleObservationSolveSettings

  }


  /**
   * Constructs an XmlHandler for serialization.
   *
   * @param settings Pointer to the BundleObservationSolveSettings to handle
   * @param project Pointer to the current project
   *
   * @internal
   *   @todo Does xml stuff need project???
   */
  BundleObservationSolveSettings::XmlHandler::XmlHandler(BundleObservationSolveSettings *settings,
                                                         Project *project) {
    m_xmlHandlerObservationSettings = settings;
    m_xmlHandlerProject = project;  // TODO: does xml stuff need project???
    m_xmlHandlerCharacters = "";
  }


  /**
   * XmlHandler destructor.
   */
  BundleObservationSolveSettings::XmlHandler::~XmlHandler() {
    // do not delete this pointer... we don't own it, do we???
    // passed into StatCumProbDistDynCalc constructor as pointer
    // delete m_xmlHandlerProject;  // TODO: does xml stuff need project???
    m_xmlHandlerProject = NULL;
  }


  /**
   * @param namespaceURI
   * @param localName
   * @param qName
   * @param atts
   *
   * @return @b bool
   *
   * @internal
   *   @todo Document if we decide to use Xml handlers for serialization
   */
  bool BundleObservationSolveSettings::XmlHandler::startElement(const QString &namespaceURI,
                                                                const QString &localName,
                                                                const QString &qName,
                                                                const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "instrumentPointingOptions") {

        QString pointingSolveOption = atts.value("solveOption");
        if (!pointingSolveOption.isEmpty()) {
          m_xmlHandlerObservationSettings->m_instrumentPointingSolveOption
              = stringToInstrumentPointingSolveOption(pointingSolveOption);
        }

        QString numberCoefSolved = atts.value("numberCoefSolved");
        if (!numberCoefSolved.isEmpty()) {
          m_xmlHandlerObservationSettings->m_numberCamAngleCoefSolved = toInt(numberCoefSolved);
        }

        QString ckDegree = atts.value("degree");
        if (!ckDegree.isEmpty()) {
          m_xmlHandlerObservationSettings->m_ckDegree = toInt(ckDegree);
        }

        QString ckSolveDegree = atts.value("solveDegree");
        if (!ckSolveDegree.isEmpty()) {
          m_xmlHandlerObservationSettings->m_ckSolveDegree = toInt(ckSolveDegree);
        }

        QString solveTwist = atts.value("solveTwist");
        if (!solveTwist.isEmpty()) {
          m_xmlHandlerObservationSettings->m_solveTwist = toBool(solveTwist);
        }

        QString solveOverExisting = atts.value("solveOverExisting");
        if (!solveOverExisting.isEmpty()) {
          m_xmlHandlerObservationSettings->m_solvePointingPolynomialOverExisting =
              toBool(solveOverExisting);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_xmlHandlerObservationSettings->m_pointingInterpolationType =
              SpiceRotation::Source(toInt(interpolationType));
        }

      }
      else if (localName == "aprioriPointingSigmas") {
        m_xmlHandlerAprioriSigmas.clear();
      }
      else if (localName == "instrumentPositionOptions") {

        QString positionSolveOption = atts.value("solveOption");
        if (!positionSolveOption.isEmpty()) {
          m_xmlHandlerObservationSettings->m_instrumentPositionSolveOption
              = stringToInstrumentPositionSolveOption(positionSolveOption);
        }

        QString numberCoefSolved = atts.value("numberCoefSolved");
        if (!numberCoefSolved.isEmpty()) {
          m_xmlHandlerObservationSettings->m_numberCamPosCoefSolved = toInt(numberCoefSolved);
        }

        QString spkDegree = atts.value("degree");
        if (!spkDegree.isEmpty()) {
          m_xmlHandlerObservationSettings->m_spkDegree = toInt(spkDegree);
        }

        QString spkSolveDegree = atts.value("solveDegree");
        if (!spkSolveDegree.isEmpty()) {
          m_xmlHandlerObservationSettings->m_spkSolveDegree = toInt(spkSolveDegree);
        }

        QString solveOverHermiteSpline = atts.value("solveOverHermiteSpline");
        if (!solveOverHermiteSpline.isEmpty()) {
          m_xmlHandlerObservationSettings->m_solvePositionOverHermiteSpline =
              toBool(solveOverHermiteSpline);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_xmlHandlerObservationSettings->m_positionInterpolationType =
              SpicePosition::Source(toInt(interpolationType));
        }
      }
      else if (localName == "aprioriPositionSigmas") {
        m_xmlHandlerAprioriSigmas.clear();
      }
    }
    return true;
  }


  /**
   * @param ch
   *
   * @return @b bool
   *
   * @internal
   *   @todo Document if we use Xml handlers for serialization.
   */
  bool BundleObservationSolveSettings::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }


  /**
   * @param namespaceURI
   * @param localName
   * @param qName
   *
   * @return @b bool
   *
   * @internal
   *   @todo Document if we use Xml handlers for serialization.
   */
  bool BundleObservationSolveSettings::XmlHandler::endElement(const QString &namespaceURI,
                                                              const QString &localName,
                                                              const QString &qName) {
    if (!m_xmlHandlerCharacters.isEmpty()) {
      if (localName == "id") {
        m_xmlHandlerObservationSettings->m_id = NULL;
        m_xmlHandlerObservationSettings->m_id = new QUuid(m_xmlHandlerCharacters);
      }
      else if (localName == "instrumentId") {
        m_xmlHandlerObservationSettings->setInstrumentId(m_xmlHandlerCharacters);
      }
//    else if (localName == "bundleObservationSolveSettings") {
//      // end tag for this entire class... how to get out???
//      // call parse, as in Control List???
//    }
      else if (localName == "sigma") {
        m_xmlHandlerAprioriSigmas.append(m_xmlHandlerCharacters);
      }
      else if (localName == "aprioriPointingSigmas") {
        m_xmlHandlerObservationSettings->m_anglesAprioriSigma.clear();
        for (int i = 0; i < m_xmlHandlerAprioriSigmas.size(); i++) {
          if (m_xmlHandlerAprioriSigmas[i] == "N/A") {
            m_xmlHandlerObservationSettings->m_anglesAprioriSigma.append(Isis::Null);
          }
          else {
            m_xmlHandlerObservationSettings->m_anglesAprioriSigma.append(
                toDouble(m_xmlHandlerAprioriSigmas[i]));
          }
        }
      }
      else if (localName == "aprioriPositionSigmas") {
        m_xmlHandlerObservationSettings->m_positionAprioriSigma.clear();
        for (int i = 0; i < m_xmlHandlerAprioriSigmas.size(); i++) {
          if (m_xmlHandlerAprioriSigmas[i] == "N/A") {
            m_xmlHandlerObservationSettings->m_positionAprioriSigma.append(Isis::Null);
          }
          else {
            m_xmlHandlerObservationSettings->m_positionAprioriSigma.append(
                                                   toDouble(m_xmlHandlerAprioriSigmas[i]));
          }
        }
      }
      m_xmlHandlerCharacters = "";
    }
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
