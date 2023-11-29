#ifndef BundleObservationSolveSettings_h
#define BundleObservationSolveSettings_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

#include <csm.h>

#include "SpicePosition.h"
#include "SpiceRotation.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;
  /**
   * This class is used to modify and manage solve settings for 1 to many BundleObservations. These
   * settings indicate how any associated observations should be solved.
   *
   * @ingroup ControlNetworks
   *
   * @author 2014-07-09 Ken Edmundson
   *
   * @internal
   *   @history 2014-07-09 Ken Edmundson - Original version.
   *   @history 2014-07-23 Jeannie Backer - Replace QVectors with QLists. Moved copy
   *                           constructor above destructor. Moved operator= implementation
   *                           to cpp file. Added methods to set/get instrument Id. Created
   *                           setInstrumentPointingSettings() and
   *                           setInstrumentPositionSettings() methods to check for variable
   *                           dependencies before setting. Reordered so that all position
   *                           methods were together and all pointing methods were together.
   *                           Updated documentation. Added QDataStream >> and << operators
   *                           and read/write methods. Added unitTest for BundleUtilities
   *                           BundleObservationSolveSettings class.
   *   @history 2014-07-25 Jeannie Backer - For enums < 4, set solve degrees one less than
   *                           enum value.
   *   @history 2014-12-02 Jeannie Backer - Undo last modification. While it makes sense, it
   *                           was causing jigsaw to fail. Brought test coverage of this
   *                           class to 99.403% scope.
   *   @history 2016-08-03 Jesse Mapel - Added BundleObservationSolveSettingsQsp definition.
   *                           Fixes #4150.
   *   @history 2016-08-03 Ian Humphrey - Updated documentation and reviewed coding standards.
   *                           Fixes #4078.
   *   @history 2016-08-18 Jesse Mapel - Changed to no longer inherit from QObject.  Fixes #4192.
   *   @history 2016-10-13 Ian Humphrey - Added m_observationNumbers, addObservationNumber(), and
   *                           observationNumbers() members to associate multiple observation types
   *                           with these settings. Removed the setFromPvl() method. When re-
   *                           implemented, it should be put in jigsaw. References #4293.
   *   @history 2017-04-24 Ian Humphrey - Removed pvlObject(). Fixes #4797.
   *   @history 2018-06-21 Ian Humphrey - Added removeObservationNumber() to be able to remove an
   *                           observation number from a BundleObservationSolveSettings.
   *                           References #497.
   *   @history 2018-06-26 Tyler Wilson - Added support for adding an arbitrary number of
   *                           additional apriori sigma values in setInstrumentPositionSettings/
   *                           setInstrumentPointingSettings beyond position/velocity/acceleration.
   *                           References #497.
   *   @history 2019-12-19 Aaron Giroux - Added new constructor to construct a BOSS from
   *                           a PVLGroup.
   *                           References #3369.
   *   @todo Figure out why solve degree and num coefficients does not match solve option.
   *   @todo Determine whether xml stuff needs a Project pointer.
   */

class BundleObservationSolveSettings {

    public:
      BundleObservationSolveSettings();
      BundleObservationSolveSettings(const BundleObservationSolveSettings &src);
      BundleObservationSolveSettings(const PvlGroup &scParameterGroup);
      ~BundleObservationSolveSettings();
      BundleObservationSolveSettings &operator=(const BundleObservationSolveSettings &src);
      void initialize();

      void setInstrumentId(QString instrumentId);
      QString instrumentId() const;
      void addObservationNumber(QString observationNumber);
      bool removeObservationNumber(QString observationNumber);
      QSet<QString> observationNumbers() const;

      //! Options for how to solve for CSM parameters.
      enum CSMSolveOption {
        NoCSMParameters = 0, /**< Do not solve for CSM parameters.*/
        Set             = 1, /**< Solve for all CSM parameters belonging to a specific set.*/
        Type            = 2, /**< Solve for all CSM parameters of a specific type.*/
        List            = 3  /**< Solve for an explicit list of CSM parameters.*/
      };

      static CSMSolveOption stringToCSMSolveOption(QString option);
      static QString csmSolveOptionToString(CSMSolveOption option);
      static csm::param::Set stringToCSMSolveSet(QString set);
      static QString csmSolveSetToString(csm::param::Set set);
      static csm::param::Type stringToCSMSolveType(QString type);
      static QString csmSolveTypeToString(csm::param::Type type);
      void setCSMSolveSet(csm::param::Set set);
      void setCSMSolveType(csm::param::Type type);
      void setCSMSolveParameterList(QStringList list);
      CSMSolveOption csmSolveOption() const;
      csm::param::Set csmParameterSet() const;
      csm::param::Type csmParameterType() const;
      QStringList csmParameterList() const;

      //! Options for how to solve for instrument pointing.
      enum InstrumentPointingSolveOption {
        NoPointingFactors          = 0, /**< Solve for none of the pointing factors.*/
        AnglesOnly                 = 1, /**< Solve for pointing angles: right ascension, declination
                                             and, optionally, twist.*/
        AnglesVelocity             = 2, //!< Solve for pointing angles and their angular velocities.
        AnglesVelocityAcceleration = 3, /**< Solve for pointing angles, their velocities and their
                                             accelerations.*/
        AllPointingCoefficients    = 4  /**< Solve for all coefficients in the polynomials fit to
                                             the pointing angles.*/
      };

      static InstrumentPointingSolveOption stringToInstrumentPointingSolveOption(QString option);
      static QString instrumentPointingSolveOptionToString(InstrumentPointingSolveOption option);
      void setInstrumentPointingSettings(InstrumentPointingSolveOption option,
                                         bool solveTwist,
                                         int ckDegree = 2,
                                         int ckSolveDegree = 2,
                                         bool solvePolynomialOverExisting = false,
                                         double anglesAprioriSigma = -1.0,
                                         double angularVelocityAprioriSigma = -1.0,
                                         double angularAccelerationAprioriSigma = -1.0,
                                         QList<double> * additionalPointingSigmas=nullptr);
      InstrumentPointingSolveOption instrumentPointingSolveOption() const;
      bool solveTwist() const;
      int ckDegree() const;
      int ckSolveDegree() const;
      int numberCameraAngleCoefficientsSolved() const;
      bool solvePolyOverPointing() const;
      QList<double> aprioriPointingSigmas() const;
      SpiceRotation::Source pointingInterpolationType() const;



      //! Options for how to solve for instrument position
      enum InstrumentPositionSolveOption {
        NoPositionFactors            = 0, /**< Solve for none of the position factors.*/
        PositionOnly                 = 1, /**< Solve for instrument positions only.*/
        PositionVelocity             = 2, /**< Solve for instrument positions and velocities.*/
        PositionVelocityAcceleration = 3, /**< Solve for instrument positions, velocities, and
                                               accelerations.*/
        AllPositionCoefficients      = 4  /**< Solve for all coefficients in the polynomials fit to
                                               the instrument positions.*/
      };

      static InstrumentPositionSolveOption stringToInstrumentPositionSolveOption(QString option);
      static QString instrumentPositionSolveOptionToString(InstrumentPositionSolveOption option);
      void setInstrumentPositionSettings(InstrumentPositionSolveOption option,
                                         int spkDegree = 2,
                                         int spkSolveDegree = 2,
                                         bool positionOverHermite = false,
                                         double positionAprioriSigma = -1.0,
                                         double velocityAprioriSigma = -1.0,
                                         double accelerationAprioriSigma = -1.0,
                                         QList<double> * additionalPositionSigmas=nullptr);
      InstrumentPositionSolveOption instrumentPositionSolveOption() const;
      int spkDegree() const;
      int spkSolveDegree() const;
      int numberCameraPositionCoefficientsSolved() const;
      bool solvePositionOverHermite() const;
      QList<double> aprioriPositionSigmas() const;
      SpicePosition::Source positionInterpolationType() const;

      // TODO: does xml stuff need project???
      void save(QXmlStreamWriter &stream, const Project *project) const;

      /**
       * A unique ID for this object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;
      QString m_instrumentId;               //!< The spacecraft instrument id for this observation.
      QSet<QString> m_observationNumbers;  //!< Associated observation numbers for these settings.

      // CSM related parameters
      CSMSolveOption m_csmSolveOption;  //!< How the CSM solution is specified
      csm::param::Set m_csmSolveSet;    /**< The CSM parameter set to solve for. Only valid
                                             if the solve option is Set.*/
      csm::param::Type m_csmSolveType;  /**< The CSM parameter type to solve for. Only valid
                                             if the solve option is Type.*/
      QStringList m_csmSolveList;       /**< The names of the CSM parameters to solve for. Only
                                             valid if the solve option is List.*/

      // pointing related parameters
      //! Option for how to solve for instrument pointing.
      InstrumentPointingSolveOption m_instrumentPointingSolveOption;
      int m_numberCamAngleCoefSolved;             /**< The number of camera angle coefficients in
                                                       solution.*/
      int m_ckDegree;                             /**< Degree of the polynomial fit to the original
                                                       camera angles. **/
      int m_ckSolveDegree;                        /**< Degree of the camera angles polynomial being
                                                       fit to in the bundle adjustment. **/
      bool m_solveTwist;                          //!< Solve for "twist" angle.
      bool m_solvePointingPolynomialOverExisting; /**< The polynomial will be fit over the existing
                                                       pointing polynomial.*/
      QList<double> m_anglesAprioriSigma; /**< The image position a priori sigmas.The size of the
                                               list is equal to the number of coefficients in the
                                               solution. An Isis::Null value implies no weighting
                                               will be applied.*/
      SpiceRotation::Source
          m_pointingInterpolationType;    /**< SpiceRotation interpolation type.
                                               Defined in SpiceRotation.cpp, these types are:
                                               1) Spice: directly from kernels,
                                               2) Nadir: Nadir pointing,
                                               3) Memcache: from cached table,
                                               4) PolyFunction: from nth degree polynomial,
                                               5) PolyFunctionOverSpice: kernels plus nth degree
                                                  polynomial.*/

      // position related parameters
      //! Option for how to solve for instrument position
      InstrumentPositionSolveOption m_instrumentPositionSolveOption;
      int m_numberCamPosCoefSolved;          /**< The number of camera position coefficients in the
                                                  solution.*/
      int m_spkDegree;                       /**< Degree of the polynomial fit to the original
                                                  camera position. **/
      int m_spkSolveDegree;                  /**< Degree of the camera position polynomial being
                                                  fit to in the bundle adjustment. **/
      bool m_solvePositionOverHermiteSpline; /**< The polynomial will be fit over an existing
                                                  Hermite spline.*/
      QList<double> m_positionAprioriSigma; /**< The instrument pointing a priori sigmas. The
                                                  size of the list is equal to the number of
                                                  coefficients in the solution. An Isis:Null value
                                                  implies no weighting will be applied.*/
      SpicePosition::Source
          m_positionInterpolationType;      /**< SpicePosition interpolation types.
                                                 Defined in SpicePosition.cpp, these types are:
                                                 1) Spice: read directly from kernels,
                                                 2) Memcache: read from cached table,
                                                 3) HermiteCache: read from splined table,
                                                 4) PolyFunction: calced from nth degree polynomial,
                                                 5) PolyFunctionOverHermiteConstant: read from
                                                    splined table and adding nth degree
                                                    polynomial.*/

  };
  //!  Definition for BundleObservationSolveSettingsQsp, a QSharedPointer to a
  //!< BundleObservationSolveSettings object.
  typedef QSharedPointer<BundleObservationSolveSettings> BundleObservationSolveSettingsQsp;
};

#endif // BundleObservationSolveSettings_h
