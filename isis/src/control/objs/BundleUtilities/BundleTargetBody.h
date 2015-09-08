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

#include <QObject>
#include <QSharedPointer>
#include <QStringList>

#include <boost/numeric/ublas/vector.hpp>

#include "set"

#include "Angle.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"

namespace Isis {

  class BundleSettings;
  class PvlObject;
  class Target;

  /**
   * @brief 
   *  
   * @ingroup ControlNetworks
   *
   * @author 2015-05-15 Ken Edmundson
   *
   * @internal
   *   @history 2015-05-15 Ken Edmundson - version 1.
   *
   */
  class BundleTargetBody : public QObject {

    Q_OBJECT

    public:
      // constructors
      BundleTargetBody();                            // default
      BundleTargetBody(Target *target);
      //BundleTargetBody(const BundleTargetBody &src); // copy
      ~BundleTargetBody();

      // copy (move to private????)
      //BundleTargetBody &operator=(const BundleTargetBody &src);
      //void copy(const BundleTargetBody &src);

      enum TargetRadiiSolveMethod {
        None = 0,                  //!< none
        Mean = 1,                  //!< mean radius
        All  = 2                   //!< all radii
      };

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
      boost::numeric::ublas::vector<double> &parameterWeights();
      boost::numeric::ublas::vector<double> &parameterCorrections();
      boost::numeric::ublas::vector<double> &parameterSolution();
      boost::numeric::ublas::vector<double> &aprioriSigmas();
      boost::numeric::ublas::vector<double> &adjustedSigmas();

      std::vector<Angle> poleRaCoefs();
      std::vector<Angle> poleDecCoefs();
      std::vector<Angle> pmCoefs();

      std::vector<Distance> radii();
      Distance meanRadius();

      // string format methods
      // TODO implement
      QString formatBundleOutputString(bool errorPropagation);
      QStringList parameterList();
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

      void applyParameterCorrections(boost::numeric::ublas::vector<double> corrections);

      double vtpv();

      Distance localRadius(const Latitude &lat, const Longitude &lon);

    private:
      TargetRadiiSolveMethod m_solveTargetBodyRadiusMethod;
      Distance m_aprioriRadiusA;
      Distance m_sigmaRadiusA;
      Distance m_aprioriRadiusB;
      Distance m_sigmaRadiusB;
      Distance m_aprioriRadiusC;
      Distance m_sigmaRadiusC;
      Distance m_aprioriMeanRadius;
      Distance m_sigmaMeanRadius;

      std::vector<Distance> m_radii;
      Distance m_meanRadius;

      std::vector<Angle> m_raPole;                   //!< pole ra quadratic polynomial coefficients
      std::vector<Angle> m_decPole;                  //!< pole dec quadratic polynomial coefficients
      std::vector<Angle> m_pm ;                      //!< pole pm quadratic polynomial coefficients

      std::set<int> m_parameterSolveCodes;  //!< target parameter solve codes (TODO: explain better)
                                            //!< ALSO, WHY DID I USE A std::set here? (there was a
                                            //!  good reason)
      QStringList m_parameterNamesList;     //!< list of all target parameters

      boost::numeric::ublas::vector<double> m_weights;         //!< parameter weights
      boost::numeric::ublas::vector<double> m_corrections;     //!< cumulative parameter corrections
      boost::numeric::ublas::vector<double> m_solution;        //!< parameter solution vector
      boost::numeric::ublas::vector<double> m_aprioriSigmas;   //!< a priori parameter sigmas
      boost::numeric::ublas::vector<double> m_adjustedSigmas;  //!< adjusted parameter sigmas
  };

  typedef QSharedPointer<BundleTargetBody> BundleTargetBodyQsp;
}

#endif // BundleTargetBody_h
