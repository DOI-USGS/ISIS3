#include "BundleObservationSolveSettings.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QUuid>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include <hdf5.h>
#include <hdf5_hl.h> // in the hdf5 library
#include <hdf5.h>
#include <H5Cpp.h>

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


#if 0
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
#endif


  /**
   * Copy constructor.
   */
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
  BundleObservationSolveSettings::BundleObservationSolveSettings(const BundleObservationSolveSettings &other) {

    m_id = NULL;
    m_id = new QUuid(other.m_id->toString());
     // TODO: add check to all copy constructors (verify other.xxx is not null) and operator= ??? or intit all variables in all constructors

    m_instrumentId = other.m_instrumentId;
    m_instrumentPointingSolveOption = other.m_instrumentPointingSolveOption;
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



  // TODO: CHECK THIS!!! equals operator
  BundleObservationSolveSettings 
      &BundleObservationSolveSettings::operator=(const BundleObservationSolveSettings &other) {
    if (&other != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(other.m_id->toString());
      
      m_instrumentId = other.m_instrumentId;
      
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
   * set bundle solve parameters for an observation
   */
  bool BundleObservationSolveSettings::setFromPvl(PvlGroup &scParameterGroup) {
    // reset defaults
    initialize();

    try {
      // group name must be instrument id
      setInstrumentId((QString)scParameterGroup.nameKeyword());

      // set up pointing settings
      InstrumentPointingSolveOption pointingOption
         = stringToInstrumentPointingSolveOption(scParameterGroup.findKeyword("CAMSOLVE")[0]);

      // If CKDEGREE is not specified, then a default of 2 is used
      int ckDegree = 2;
      if (scParameterGroup.hasKeyword("CKDEGREE")) {
        ckDegree = (int)(scParameterGroup.findKeyword("CKDEGREE"));
      }

      // If CKSOLVEDEGREE is not specified, then a default of 2 is used
      int ckSolveDegree = 2;
      if (scParameterGroup.hasKeyword("CKSOLVEDEGREE")) {
        ckSolveDegree = (int) (scParameterGroup.findKeyword("CKSOLVEDEGREE"));
      }

      // If TWIST is not specified, then a default of YES is used
      bool solveTwist = true;
      if (scParameterGroup.hasKeyword("TWIST")) {
        try {
          solveTwist = toBool(scParameterGroup.findKeyword("TWIST")[0]);
        } 
        catch (IException &e) {
          QString msg = "The TWIST parameter must be a valid boolean value.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      // If OVEREXISTING is not specified, then a default of NO is used
      bool solvePolynomialOverExisting = false;
      if (scParameterGroup.hasKeyword("OVEREXISTING")) {
        try {
          solvePolynomialOverExisting = toBool(scParameterGroup.findKeyword("OVEREXISTING")[0]);
        } 
        catch (IException &e) {
          QString msg = "OVEREXISTING parameter must be a valid boolean value.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      double anglesAprioriSigma = Isis::Null;
      double angularVelocityAprioriSigma = Isis::Null;
      double angularAccelerationAprioriSigma = Isis::Null;
      if (pointingOption != NoPointingFactors) {
        if (scParameterGroup.hasKeyword("CAMERA_ANGLES_SIGMA")) {
          anglesAprioriSigma = (double)(scParameterGroup.findKeyword("CAMERA_ANGLES_SIGMA"));
        }
        if (pointingOption != AnglesOnly) {
          if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA")) {
            angularVelocityAprioriSigma = 
                (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA"));
          }
          if (pointingOption != AnglesVelocity) {
            if (scParameterGroup.hasKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA")) {
              angularAccelerationAprioriSigma = 
                  (double)(scParameterGroup.findKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA"));
            }
          }
        }
      }

      setInstrumentPointingSettings(pointingOption, solveTwist, ckDegree, ckSolveDegree, 
                                    solvePolynomialOverExisting, anglesAprioriSigma, 
                                    angularVelocityAprioriSigma, angularAccelerationAprioriSigma);

      // Now set up position settings
      InstrumentPositionSolveOption positionOption
         = stringToInstrumentPositionSolveOption(scParameterGroup.findKeyword("SPSOLVE")[0]);

      //  If SPKDEGREE is not specified, then a default of 2 is used
      int spkDegree = 2;
      if (scParameterGroup.hasKeyword("SPKDEGREE")) {
        spkDegree = (int)(scParameterGroup.findKeyword("SPKDEGREE"));
      }

      // If SPKSOLVEDEGREE is not specified, then a default of 2 is used
      int spkSolveDegree = 2;
      if (scParameterGroup.hasKeyword("SPKSOLVEDEGREE")) {
        spkSolveDegree = (int)(scParameterGroup.findKeyword("SPKSOLVEDEGREE"));
      }

      // If OVERHERMITE is not specified, then a default of NO is used
      bool positionOverHermite = false;
      if (scParameterGroup.hasKeyword("OVERHERMITE")) {
        try {
          positionOverHermite = toBool(scParameterGroup.findKeyword("OVERHERMITE")[0]);
        } 
        catch (IException &e) {
          QString msg = "The OVERHERMITE parameter must be a valid boolean value.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      double positionAprioriSigma = Isis::Null;
      double velocityAprioriSigma = Isis::Null;
      double accelerationAprioriSigma = Isis::Null;
      if (positionOption != NoPositionFactors) {
        if (scParameterGroup.hasKeyword("SPACECRAFT_POSITION_SIGMA")) {
          positionAprioriSigma
             = (double)(scParameterGroup.findKeyword("SPACECRAFT_POSITION_SIGMA"));
        }
        if (positionOption != PositionOnly) {
          if (scParameterGroup.hasKeyword("SPACECRAFT_VELOCITY_SIGMA")) {
            velocityAprioriSigma = 
                (double)(scParameterGroup.findKeyword("SPACECRAFT_VELOCITY_SIGMA"));
          }
          if (positionOption != PositionVelocity) {
            if (scParameterGroup.hasKeyword("SPACECRAFT_ACCELERATION_SIGMA")) {
              accelerationAprioriSigma = 
                  (double)(scParameterGroup.findKeyword("SPACECRAFT_ACCELERATION_SIGMA"));
            }
          }
        }
      }
      setInstrumentPositionSettings(positionOption, spkDegree, spkSolveDegree,
                                    positionOverHermite, positionAprioriSigma,
                                    velocityAprioriSigma, accelerationAprioriSigma);
    } 
    catch (IException &e) {
      QString msg = "Unable to set bundle adjust options from the given PVL.";
      throw IException(e, IException::User, msg, _FILEINFO_);
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
      throw IException(IException::Unknown,
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
      throw IException(IException::Unknown,
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
      pvlName = instrumentId();
    }
    else {
      pvlName = name;
    }
    PvlObject pvl(pvlName);

    pvl += PvlKeyword("InstrumentPointingSolveOption", 
                      instrumentPointingSolveOptionToString(instrumentPointingSolveOption()));
    if (instrumentPointingSolveOption() > NoPointingFactors) {
      pvl += PvlKeyword("NumberAngleCoefficientsSolved", 
                        toString(numberCameraAngleCoefficientsSolved()));
      pvl += PvlKeyword("CKDegree", toString(ckDegree()));
      pvl += PvlKeyword("CKSolveDegree", toString(ckSolveDegree()));
      pvl += PvlKeyword("SolveTwist", toString(solveTwist()));
      pvl += PvlKeyword("SolvePointingPolynomialOverExisting", 
                        toString(solvePolyOverPointing())); 
      PvlKeyword angleSigmas("AngleAprioriSigmas");
      for (int i = 0; i < aprioriPointingSigmas().size(); i++) {
        if (IsSpecial(m_anglesAprioriSigma[i])) {
          angleSigmas.addValue("N/A");
        }
        else {
          angleSigmas.addValue(toString(m_anglesAprioriSigma[i]));
        }
      }
      pvl += angleSigmas;
      
      pvl += PvlKeyword("InstrumentPointingInterpolationType",
                      toString((int)pointingInterpolationType()));  // TODO: omit from pvl if pointing option == None ??? 
    }
    else {
      pvl += PvlKeyword("NumberAngleCoefficientsSolved", "N/A");
      pvl += PvlKeyword("CKDegree", "N/A");
      pvl += PvlKeyword("CKSolveDegree", "N/A");
      pvl += PvlKeyword("SolveTwist", "N/A");
      pvl += PvlKeyword("SolvePointingPolynomialOverExisting", "N/A");
      pvl += PvlKeyword("AngleAprioriSigmas", "N/A");
      pvl += PvlKeyword("InstrumentPointingInterpolationType", "N/A");
    }


    // position

    pvl += PvlKeyword("InstrumentPositionSolveOption", 
                      instrumentPositionSolveOptionToString(instrumentPositionSolveOption()));
    if (instrumentPositionSolveOption() > NoPositionFactors) {
      pvl += PvlKeyword("NumberPositionCoefficientsSolved", 
                        toString(numberCameraPositionCoefficientsSolved()));
      pvl += PvlKeyword("SPKDegree", toString(spkDegree()));
      pvl += PvlKeyword("SPKSolveDegree", toString(spkSolveDegree()));
      pvl += PvlKeyword("SolvePositionOverHermiteSpline", toString(solvePositionOverHermite()));
    
      PvlKeyword positionSigmas("PositionAprioriSigmas");
      for (int i = 0; i < aprioriPositionSigmas().size(); i++) {
        if (IsSpecial(m_positionAprioriSigma[i])) {
          positionSigmas.addValue("N/A");
        }
        else {
          positionSigmas.addValue(toString(m_positionAprioriSigma[i]));
        }
      }
      pvl += positionSigmas;
    
      pvl += PvlKeyword("InstrumentPositionInterpolationType",
                        toString((int)positionInterpolationType()));
    }
    else {
      pvl += PvlKeyword("NumberPositionCoefficientsSolved", "N/A");
      pvl += PvlKeyword("SPKDegree", "N/A");
      pvl += PvlKeyword("SPKSolveDegree", "N/A");
      pvl += PvlKeyword("SolvePositionOverHermiteSpline", "N/A");
      pvl += PvlKeyword("PositionAprioriSigmas", "N/A");
      pvl += PvlKeyword("InstrumentPositionInterpolationType", "N/A");
    }
    return pvl;
  }



  void BundleObservationSolveSettings::save(QXmlStreamWriter &stream, const Project *project) const {  // TODO: does xml stuff need project???

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



  BundleObservationSolveSettings::XmlHandler::XmlHandler(BundleObservationSolveSettings *settings, 
                                                         Project *project) {  // TODO: does xml stuff need project???
    m_xmlHandlerObservationSettings = settings;
    m_xmlHandlerProject = project;  // TODO: does xml stuff need project???
    m_xmlHandlerCharacters = "";
  }



  BundleObservationSolveSettings::XmlHandler::~XmlHandler() {
    // do not delete this pointer... we don't own it, do we??? passed into StatCumProbDistDynCalc constructor as pointer
    // delete m_xmlHandlerProject;  // TODO: does xml stuff need project???
    m_xmlHandlerProject = NULL;
  }
  


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
          m_xmlHandlerObservationSettings->m_solvePointingPolynomialOverExisting = toBool(solveOverExisting);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_xmlHandlerObservationSettings->m_pointingInterpolationType = SpiceRotation::Source(toInt(interpolationType));
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
          m_xmlHandlerObservationSettings->m_solvePositionOverHermiteSpline = toBool(solveOverHermiteSpline);
        }

        QString interpolationType = atts.value("interpolationType");
        if (!interpolationType.isEmpty()) {
          m_xmlHandlerObservationSettings->m_positionInterpolationType = SpicePosition::Source(toInt(interpolationType));
        }
      }
      else if (localName == "aprioriPositionSigmas") {
        m_xmlHandlerAprioriSigmas.clear();
      }
    }
    return true;
  }



  bool BundleObservationSolveSettings::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool BundleObservationSolveSettings::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
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



  QDataStream &BundleObservationSolveSettings::write(QDataStream &stream) const {

    stream << m_id->toString()
           << m_instrumentId
           << (qint32)m_instrumentPointingSolveOption
           << (qint32)m_numberCamAngleCoefSolved
           << (qint32)m_ckDegree
           << (qint32)m_ckSolveDegree
           << m_solveTwist
           << m_solvePointingPolynomialOverExisting
           << m_anglesAprioriSigma
           << (qint32)m_pointingInterpolationType
           << (qint32)m_instrumentPositionSolveOption
           << (qint32)m_numberCamPosCoefSolved 
           << (qint32)m_spkDegree
           << (qint32)m_spkSolveDegree
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
           >> numCamAngleCoefSolved
           >> ckDegree
           >> ckSolveDegree
           >> m_solveTwist
           >> m_solvePointingPolynomialOverExisting
           >> m_anglesAprioriSigma
           >> anglesInterpType
           >> positionSolveOption
           >> numCamPosCoefSolved
           >> spkDegree
           >> spkSolveDegree
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

#if 0
  /** 
   *  H5 compound data type uses the offesets from the QDataStream returned by
   *  the write(QDataStream &stream) method.
   */
  H5::CompType BundleObservationSolveSettings::compoundH5DataType() {

    H5::CompType compoundDataType((size_t)   );

    size_t offset = 0;

    compoundDataType.insertMember("InstrumentId", offset, H5::PredType::C_S1);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("InstrumentPointingSolveOption", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("NumCamAngleCoefSolved", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("CkDegree", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("CkSolveDegree", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("SolveTwist", offset, H5::PredType::NATIVE_HBOOL);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("SolvePointingPolynomialOverExisting", offset, H5::PredType::NATIVE_HBOOL);

    offset += sizeof(m_instrumentId);
???    compoundDataType.insertMember("AnglesAprioriSigma", offset, H5::PredType::NATIVE_DOUBLE);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("PointingInterpolationType", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("InstrumentPositionSolveOption", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_instrumentId);
    compoundDataType.insertMember("NumCamPosCoefSolved", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_numberCamPosCoefSolved);
    compoundDataType.insertMember("SpkDegree", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_spkDegree);
    compoundDataType.insertMember("SpkSolveDegree", offset, H5::PredType::NATIVE_INT);

    offset += sizeof(m_spkSolveDegree);
    compoundDataType.insertMember("SolvePositionOverHermiteSpline", offset, H5::PredType::NATIVE_HBOOL);

    offset += sizeof(m_solvePositionOverHermiteSpline);
???    compoundDataType.insertMember("PositionAprioriSigma", offset, H5::PredType::NATIVE_DOUBLE);

    offset += sizeof(m_positionAprioriSigma);
    compoundDataType.insertMember("PositionInterpolationType", offset, H5::PredType::NATIVE_INT);

    return compoundDataType;

  }
#endif
}
