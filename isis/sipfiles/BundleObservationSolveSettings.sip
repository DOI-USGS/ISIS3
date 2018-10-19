
%Import QtCore/QtCoremod.sip
%Import QtXml/QtXmlmod.sip

namespace Isis{
  class BundleObservationSolveSettings {
    %TypeHeaderCode
    #include "BundleObservationSolveSettings.h"
    %End
  public:

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

    BundleObservationSolveSettings()  throw(Isis::IException);
    InstrumentPositionSolveOption stringToInstrumentPositionSolveOption(QString option) throw(Isis::IException);
    InstrumentPointingSolveOption stringToInstrumentPointingSolveOption(QString option) throw(Isis::IException);

    void setInstrumentPositionSettings(InstrumentPositionSolveOption option,
                                       int spkDegree = 2,
                                       int spkSolveDegree = 2,
                                       bool positionOverHermite = false,
                                       double positionAprioriSigma = -1.0,
                                       double velocityAprioriSigma = -1.0,
                                       double accelerationAprioriSigma = -1.0)  throw(Isis::IException) /KeywordArgs="Optional"/;


   void setInstrumentPointingSettings(InstrumentPointingSolveOption option,
                                      bool solveTwist,
                                      int ckDegree = 2,
                                      int ckSolveDegree = 2,
                                      bool solvePolynomialOverExisting = false,
                                      double anglesAprioriSigma = -1.0,
                                      double angularVelocityAprioriSigma = -1.0,
                                      double angularAccelerationAprioriSigma = -1.0) throw(Isis::IException) /KeywordArgs="Optional"/;



  };
};
