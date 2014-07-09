#ifndef BundleObservationSolveSettings_h
#define BundleObservationSolveSettings_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/28 09:31:00 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QVector>

#include "BundleImage.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

namespace Isis {

  class BundleObservationSolveSettings {

    public:
      BundleObservationSolveSettings();
      ~BundleObservationSolveSettings();

      // copy constructor
      BundleObservationSolveSettings(const BundleObservationSolveSettings& src);

      // TODO: CHECK THIS!!! equals operator
      BundleObservationSolveSettings& operator=(const BundleObservationSolveSettings& src) {
        BundleObservationSolveSettings* fred = new BundleObservationSolveSettings();
        return *fred;
      }

      // copy method
      void copy(const BundleObservationSolveSettings& src);

      enum InstrumentPointingSolveOption {
        NoPointingFactors,          /**< Solve for none of the pointing factors.*/
        AnglesOnly,                 /**< Solve for pointing angles: right ascension, declination
                                         and, optionally, twist.*/
        AnglesVelocity,             //!< Solve for pointing angles and their angular velocities.
        AnglesVelocityAcceleration, /**< Solve for pointing angles, their velocities and their
                                         accelerations.*/
        AllPointingCoefficients     /**< Solve for all coefficients in the polynomials fit to the
                                         pointing angles.*/
      };
      static InstrumentPointingSolveOption stringToInstrumentPointingSolveOption(QString option);
      static QString instrumentPointingSolveOptionToString(InstrumentPointingSolveOption option);
      InstrumentPointingSolveOption instrumentPointingSolveOption() const;

      enum InstrumentPositionSolveOption {
        NoPositionFactors,            /**< Solve for none of the position factors.*/
        PositionOnly,                 /**< Solve for instrument positions only.*/
        PositionVelocity,             /**< Solve for instrument positions and velocities.*/
        PositionVelocityAcceleration, /**< Solve for instrument positions, velocities, and
                                           accelerations.*/
        AllPositionCoefficients       /**< Solve for all coefficients in the polynomials fit to
                                           the instrument positions.*/
      };
      static InstrumentPositionSolveOption stringToInstrumentPositionSolveOption(QString option);
      static QString instrumentPositionSolveOptionToString(InstrumentPositionSolveOption option);
      InstrumentPositionSolveOption instrumentPositionSolveOption() const;

      bool setFromPvl(PvlGroup& scParameterGroup);

      void setCKDegree(double ckDegree);
      void setCKSolveDegree(double solveCKDegree);
      void setInstrumentPointingSolveOption(InstrumentPointingSolveOption option);
      void setAnglesAprioriSigma(double anglesAprioriSigma);
      void setAngularVelocityAprioriSigma(double angularVelocityAprioriSigma);
      void setAngularAccelerationAprioriSigma(double angularAccelerationAprioriSigma);
      void setSolveTwist(bool solveTwist);
      void setSolvePolyOverPointing(bool solvePolynomialOverExisting);

      void setSPKDegree(double spkDegree);
      void setSPKSolveDegree(double spkSolveDegree);
      void setInstrumentPositionSolveOption(InstrumentPositionSolveOption option);
      void setSolvePolyOverHermite(bool positionOverHermite);
      void setPositionAprioriSigma(double positionAprioriSigma);
      void setVelocityAprioriSigma(double velocityAprioriSigma);
      void setAccelerationAprioriSigma(double accelerationAprioriSigma);

      QString instrumentId() const;
      bool solveTwist() const;
      bool solvePolyOverPointing() const;
      int ckDegree() const;
      int ckSolveDegree() const;
      SpiceRotation::Source pointingInterpolationType() const;

      bool solvePositionOverHermite() const;
      int spkDegree() const;
      int spkSolveDegree() const;
      SpicePosition::Source positionInterpolationType() const;

      int numberCameraAngleCoefficientsSolved() const;
      int numberCameraPositionCoefficientsSolved() const;

      QVector<double> aprioriPointingSigmas() const;
      QVector<double> aprioriPositionSigmas() const;

    private:
      QString m_instrumentId;               //!< spacecraft instrument id

      // pointing related parameters
      InstrumentPointingSolveOption m_instrumentPointingSolveOption;

      bool m_solveTwist;                          //!< solve for "twist" angle
      int m_ckDegree;                             //!< ck degree (define)
      int m_ckSolveDegree;                        //!< solve ck degree (define)
      int m_numberCamAngleCoefSolved;             //!< # of camera angle coefficients in solution
      bool m_solvePointingPolynomialOverExisting; //!< fit polynomial over existing pointing

      SpiceRotation::Source
          m_pointingInterpolationType;      //!< SpiceRotation interpolation type
                                            //!< Defined in SpiceRotation.cpp, these types are
                                            //!< 1) Spice: directly from kernels
                                            //!< 2) Nadir: Nadir pointing
                                            //!< 3) Memcache: from cached table
                                            //!< 4) PolyFunction: from nth degree polynomial
                                            //!< 5) PolyFunctionOverSpice: kernels plus nth degree
                                            //!<    polynomial

      // position related parameters
      InstrumentPositionSolveOption m_instrumentPositionSolveOption;

      int m_spkDegree;                       //!< spk degree (define)
      int m_spkSolveDegree;                  //!< solve spk degree (define)
      int m_numberCamPosCoefSolved;          //!< # of camera position coefficients in solution
      bool m_solvePositionOverHermiteSpline; //!< fit polynomial over existing Hermite spline

      SpicePosition::Source
          m_positionInterpolationType;       //!< SpicePosition interpolation types
                                             //!< Defined in SpicePosition.cpp, these types are
                                             //!< 1) Spice: read directly from kernels
                                             //!< 2) Memcache: read from cached table
                                             //!< 3) HermiteCache: read from splined table
                                             //!< 4) PolyFunction: calced from nth degree polynomial
                                             //!< 5) PolyFunctionOverHermiteConstant: read from
                                             //!<    splined table and adding nth degree polynomial

      QVector<double> m_anglesAprioriSigma;   //!< image position and pointing a priori sigmas. Size
      QVector<double> m_positionAprioriSigma; //!< the # of coefficients in the solution. A negative
                                              //!< value implies no weighting will be applied
  };
}

#endif // BundleObservationSolveSettings_h
