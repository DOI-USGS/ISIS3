#ifndef BundleTargetBody_h
#define BundleTargetBody_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2015/5/15 08:00:00 $
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
   */
  class BundleTargetBody {

    public:
      // constructors
      BundleTargetBody();                            // default
      BundleTargetBody(Target *target);
      BundleTargetBody(const BundleTargetBody &src); // copy
      ~BundleTargetBody();

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

      bool solvePoleRA();
      bool solvePoleRAVelocity();
      bool solvePoleRAAcceleration();
      bool solvePoleDec();
      bool solvePoleDecVelocity();
      bool solvePoleDecAcceleration();
      bool solvePM();
      bool solvePMVelocity();
      bool solvePMAcceleration();
      bool solveTriaxialRadii();
      bool solveMeanRadius();

//      int numberPoleParameters();
//      int numberW0Parameters();
//      int numberWDotParameters();
      int numberRadiusParameters();
      int numberParameters();

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
