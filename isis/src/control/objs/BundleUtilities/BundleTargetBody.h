#ifndef BundleTargetBody_h
#define BundleTargetBody_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <set>

#include <QSharedPointer>
#include <QStringList>

#include <boost/numeric/ublas/vector.hpp>

#include "Angle.h"
#include "Distance.h"
#include "Latitude.h"
#include "LinearAlgebra.h"
#include "Longitude.h"

namespace Isis {

  class BundleSettings;
  class PvlObject;
  class Target;

  /**
   * This class is used to represent a target body in a bundle and how to solve for it.
   *
   * @ingroup ControlNetworks
   *
   * @author 2015-05-15 Ken Edmundson
   *
   * @internal
   *   @history 2015-05-15 Ken Edmundson - version 1.
   *   @histroy 2016-07-13 Jesse Mapel - Updated documentation and coding standards, and added
   *                           testing in preparation for merging from IPCE to ISIS.
   *                           Fixes #4079.
   *   @history 2016-08-10 Jeannie Backer - Replaced boost vector with Isis::LinearAlgebra::Vector.
   *                           References #4163.
   *   @history 2016-08-18 Jesse Mapel - Changed to no longer inherit from QObject.  Fixes #4192.
   *   @history 2016-08-23 Ian Humphrey - The applyParameterCorrections() method now throws its
   *                           last exception. Updated unit test to test that exception.
   *                           Fixes #4153.
   *   @history 2018-11-13 Jesse Mapel - Made methods used by BundleSettings virtual
   *                           so they can be override by mock testing objects.
   */
  class BundleTargetBody {

    public:
      // constructors
      BundleTargetBody();                            // default
      BundleTargetBody(Target *target);
      BundleTargetBody(const BundleTargetBody &src); // copy
      virtual ~BundleTargetBody();

      BundleTargetBody &operator=(const BundleTargetBody &src);

      //! Enumeration that defines how to solve for target radii.
      enum TargetRadiiSolveMethod {
        None = 0,                  //!< Solve for none.
        Mean = 1,                  //!< Solve for mean radius.
        All  = 2                   //!< Solve for all radii.
      };

      //! Enumeration that defines what BundleTargetBody can solve for.
      enum TargetSolveCodes { PoleRA              = 0,
                              VelocityPoleRA      = 1,
                              AccelerationPoleRA  = 2,
                              PoleDec             = 3,
                              VelocityPoleDec     = 4,
                              AccelerationPoleDec = 5,
                              PM                  = 6,
                              VelocityPM          = 7,
                              AccelerationPM      = 8,
                              TriaxialRadiusA     = 9,
                              TriaxialRadiusB     = 10,
                              TriaxialRadiusC     = 11,
                              MeanRadius          = 12
                            };

      void setSolveSettings(std::set<int> targetParameterSolveCodes,
                            Angle aprioriPoleRA, Angle sigmaPoleRA, Angle aprioriVelocityPoleRA,
                            Angle sigmaVelocityPoleRA, Angle aprioriPoleDec, Angle sigmaPoleDec,
                            Angle aprioriVelocityPoleDec, Angle sigmaVelocityPoleDec,
                            Angle aprioriPM, Angle sigmaPM, Angle aprioriVelocityPM,
                            Angle sigmaVelocityPM, TargetRadiiSolveMethod solveRadiiMethod,
                            Distance aprioriRadiusA, Distance sigmaRadiusA, Distance aprioriRadiusB,
                            Distance sigmaRadiusB, Distance aprioriRadiusC, Distance sigmaRadiusC,
                            Distance aprioriMeanRadius, Distance sigmaMeanRadius);

      static TargetRadiiSolveMethod stringToTargetRadiiOption(QString option);
      static QString targetRadiiOptionToString(TargetRadiiSolveMethod targetRadiiSolveMethod);

      bool readFromPvl(PvlObject &tbPvlObject);

      // mutators
      //

      // accessors
      LinearAlgebra::Vector &parameterWeights();
      LinearAlgebra::Vector &parameterCorrections();
      LinearAlgebra::Vector &parameterSolution();
      LinearAlgebra::Vector &aprioriSigmas();
      LinearAlgebra::Vector &adjustedSigmas();

      std::vector<Angle> poleRaCoefs();
      std::vector<Angle> poleDecCoefs();
      std::vector<Angle> pmCoefs();

      std::vector<Distance> radii();
      Distance meanRadius();

      // string format methods
      QString formatBundleOutputString(bool errorPropagation);
      QStringList parameterList();
      // TODO implement
      QString formatValue(double value, int fieldWidth, int precision) const;
      QString formatAprioriSigmaString(int type, int fieldWidth, int precision) const;
      QString formatPolePositionAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatW0AprioriSigmaString(int fieldWidth, int precision) const;
      QString formatWDotAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatRadiusAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatPolePositionAdjustedSigmaString(int fieldWidth, int precision) const;
      QString formatW0AdjustedSigmaString(int fieldWidth, int precision) const;
      QString formatWDotAdjustedSigmaString(int fieldWidth, int precision) const;
      QString formatRadiusAdjustedSigmaString(int fieldWidth, int precision) const;

      virtual bool solvePoleRA();
      virtual bool solvePoleRAVelocity();
      virtual bool solvePoleRAAcceleration();
      virtual bool solvePoleDec();
      virtual bool solvePoleDecVelocity();
      virtual bool solvePoleDecAcceleration();
      virtual bool solvePM();
      virtual bool solvePMVelocity();
      virtual bool solvePMAcceleration();
      virtual bool solveTriaxialRadii();
      virtual bool solveMeanRadius();

//      int numberPoleParameters();
//      int numberW0Parameters();
//      int numberWDotParameters();
      int numberRadiusParameters();
      virtual int numberParameters();

      void applyParameterCorrections(LinearAlgebra::Vector corrections);

      double vtpv();

      Distance localRadius(const Latitude &lat, const Longitude &lon);

    private:
      TargetRadiiSolveMethod m_solveTargetBodyRadiusMethod; //!< Which radii will be solved for.
      Distance m_aprioriRadiusA;                            //!< Apriori Radius A.
      Distance m_sigmaRadiusA;                              //!< Apriori Radius A Sigma.
      Distance m_aprioriRadiusB;                            //!< Apriori Radius B.
      Distance m_sigmaRadiusB;                              //!< Apriori Radius B Sigma.
      Distance m_aprioriRadiusC;                            //!< Apriori Radius C.
      Distance m_sigmaRadiusC;                              //!< Apriori Radius C Sigma.
      Distance m_aprioriMeanRadius;                         //!< Apriori Mean Radius.
      Distance m_sigmaMeanRadius;                           //!< Apriori Mean Radius Sigma.

      std::vector<Distance> m_radii;                 //!< Adjusted triaxial radii values.
      Distance m_meanRadius;                         //!< Adjusted mean radius value.

      std::vector<Angle> m_raPole;                   //!< pole ra quadratic polynomial coefficients
      std::vector<Angle> m_decPole;                  //!< pole dec quadratic polynomial coefficients
      std::vector<Angle> m_pm ;                      //!< pole pm quadratic polynomial coefficients

      std::set<int> m_parameterSolveCodes;  /**< Target parameter solve codes.  Stored as a set to
                                                 ensure they are always in the correct order. **/
      QStringList m_parameterNamesList;     //!< List of all target parameters.

      LinearAlgebra::Vector m_weights;         //!< Parameter weights.
      LinearAlgebra::Vector m_corrections;     //!< Cumulative parameter corrections.
      LinearAlgebra::Vector m_solution;        //!< Parameter solution vector.
      LinearAlgebra::Vector m_aprioriSigmas;   //!< A priori parameter sigmas.
      LinearAlgebra::Vector m_adjustedSigmas;  //!< Adjusted parameter sigmas.
  };

  //! Definition for BundleTargetBodyQsp, a QSharedPointer to a BundleTargetBody.
  typedef QSharedPointer<BundleTargetBody> BundleTargetBodyQsp;
}

#endif // BundleTargetBody_h
