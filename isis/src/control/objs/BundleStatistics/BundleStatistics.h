#ifndef BundleStatistics_h
#define BundleStatistics_h

/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2009/10/15 01:35:17 $
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

#include <QDataStream>
#include <QList>
#include <QPair>
#include <QString>
#include <QVector>

#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "BundleSettings.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Statistics.h" // ???

namespace Isis {
  class ControlNet;
  class CorrelationMatrix;
  class SerialNumberList;
  class StatCumProbDistDynCalc;
  class PvlGroup;
  /**
   * A utility class containing statistical results from a BundleAdjust solution. 
   *  
   * @author 2014-07-01 Jeannie Backer
   *
   * @internal
   *   @history 2014-07-01 Jeannie Backer - Original version
   *   @history 2014-07-14 Kimberly Oyama  Added support for correlation matrix.
   */
  class BundleStatistics {
    public:
      BundleStatistics();
      BundleStatistics(const BundleStatistics &other);
      ~BundleStatistics();
      BundleStatistics &operator=(const BundleStatistics &other);

      // mutators and computation methods
      bool computeBundleStatistics(SerialNumberList *m_pSnList, 
                                   ControlNet *m_pCnet, 
                                   bool errorPropagation,
                                   bool solveRadius);
      void maximumLikelihoodSetUp(
          QList< QPair< MaximumLikelihoodWFunctions::Model, double > > modelsWithQuantiles);
      void printMaximumLikelihoodTierInformation();
      void initializeResidualsProbabilityDistribution(unsigned int nodes = 20);
      void initializeProbabilityDistribution(unsigned int nodes = 20);
      void addResidualsProbabilityDistributionObservation(double obsValue);
      void addProbabilityDistributionObservation(double obsValue);
      void addProbabilityDistributionObservation(double obsValue, bool residuals);
      void incrementMaximumLikelihoodModelIndex();

      void incrementFixedPoints();
      int numberFixedPoints();
      void incrementHeldImages();
      int numberHeldImages();
      void incrementIgnoredPoints();
      int numberIgnoredPoints(); // currently unused ???
#if 0
      double computeRejectionLimit(ControlNet *p_Cnet,
                                   double outlierRejectionMultiplier,
                                   int numObservations);
#endif
      void setRejectionLimit(double rejectionLimit);
#if 0
      double computeResiduals(ControlNet *m_pCnet,
                              std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > pointWeights,
                              std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > pointCorrections,
                              boost::numeric::ublas::vector< double > image_Corrections,
                              std::vector< double > imageParameterWeights,
                              int numImagePartials,
                              int rank);
#endif
      void setRmsXYResiduals(double rx, double ry, double rxy);
      void setRmsRx(double rx);
      void setRmsRy(double ry);
      void setRmsRxy(double rxy);
#if 0
      bool flagOutliers(ControlNet *m_pCnet);
#endif
      void setNumberRejectedObservations(int numberObservations);
      void setNumberObservations(int numberObservations);
      void setNumberImageParameters(int numberParameters); // ??? this is the same value an m_nRank
      void resetNumberConstrainedPointParameters();
      void incrementNumberConstrainedPointParameters(int incrementAmount);
      void resetNumberConstrainedImageParameters();
      void incrementNumberConstrainedImageParameters(int incrementAmount);
      void setNumberUnknownParameters(int numberParameters);
      void computeDegreesOfFreedom();
      void computeSigma0(double dvtpv, BundleSettings::ConvergenceCriteria criteria);
      void setDegreesOfFreedom(double degreesOfFreedom);
      void setSigma0(double sigma0);
      void setElapsedTime(double time);
      void setElapsedTimeErrorProp(double time);
      void setConverged(bool converged); // or initialze method
      
      // Accessors...
      QVector< Statistics > rmsImageSampleResiduals() const;
      QVector< Statistics > rmsImageLineResiduals() const;
      QVector< Statistics > rmsImageResiduals() const;
      QVector< Statistics > rmsImageXSigmas() const;       // currently unused ???
      QVector< Statistics > rmsImageYSigmas() const;       // currently unused ???
      QVector< Statistics > rmsImageZSigmas() const;       // currently unused ???
      QVector< Statistics > rmsImageRASigmas() const;      // currently unused ???
      QVector< Statistics > rmsImageDECSigmas() const;     // currently unused ???
      QVector< Statistics > rmsImageTWISTSigmas() const;   // currently unused ???
      double  minSigmaLatitude() const;
      double  maxSigmaLatitude() const;
      double  minSigmaLongitude() const;
      double  maxSigmaLongitude() const;
      double  minSigmaRadius() const;
      double  maxSigmaRadius() const;
      QString maxSigmaLatitudePointId() const;
      QString minSigmaLatitudePointId() const;
      QString minSigmaLongitudePointId() const;
      QString maxSigmaLongitudePointId() const;
      QString minSigmaRadiusPointId() const;
      QString maxSigmaRadiusPointId() const;
      double rmsSigmaLat() const;
      double rmsSigmaLon() const;
      double rmsSigmaRad() const;
      double rmsRx();  // currently unused ???
      double rmsRy();  // currently unused ???
      double rmsRxy(); // currently unused ???
      double rejectionLimit() const;
      int numberRejectedObservations() const;
      int numberObservations() const;

      int numberImageParameters() const; // ??? this is the same value an m_nRank
      int numberConstrainedPointParameters() const;
      int numberConstrainedImageParameters() const;
      int numberUnknownParameters() const;
      int degreesOfFreedom() const;
      double sigma0() const;
      double elapsedTime() const;
      double elapsedTimeErrorProp() const;
      bool converged() const; // or initialze method
      
      int numberMaximumLikelihoodModels() const;
      int maximumLikelihoodModelIndex() const;
      StatCumProbDistDynCalc cumulativeProbabilityDistribution() const;
      StatCumProbDistDynCalc residualsCumulativeProbabilityDistribution() const;
      double maximumLikelihoodMedianR2Residuals() const;
      MaximumLikelihoodWFunctions maximumLikelihoodModelWFunc(int modelIndex) const;
      double maximumLikelihoodModelQuantile(int modelIndex) const;

      QList< QPair< MaximumLikelihoodWFunctions, double > > maximumLikelihoodModels() const;

      bool setNumberHeldImages(SerialNumberList m_pHeldSnList,
                               SerialNumberList *m_pSnList);

      PvlGroup pvlGroup(QString name = "BundleStatistics") const;

      CorrelationMatrix correlationMatrix();

    private:
      CorrelationMatrix *m_correlationMatrix;

// ???       Statistics m_statsx;                       //!<  x errors
// ???       Statistics m_statsy;                       //!<  y errors
// ???       Statistics m_statsrx;                      //!<  x residuals
// ???       Statistics m_statsry;                      //!<  y residuals
// ???       Statistics m_statsrxy;                     //!< xy residuals

      int m_numberFixedPoints;                //!< number of 'fixed' (ground) points (define)
      int m_numberIgnoredPoints;              //!< number of ignored points                  // currently set but unused ???
      int m_numberHeldImages;                 //!< number of 'held' images (define)          
      
      
      
            
      double m_rms_rx;  // set but unused ???                   //!< rms of x residuals
      double m_rms_ry;  // set but unused ???                   //!< rms of y residuals
      double m_rms_rxy; // set but unused ???                   //!< rms of all x and y residuals
      double m_rejectionLimit;             //!< current rejection limit
      int m_numberRejectedObservations;       //!< number of rejected image coordinate observations
      int m_numberObservations;               //!< number of image coordinate observations
      int m_numberImageParameters;            //!< number of image parameters
      int m_numberConstrainedPointParameters; //!< number of constrained point parameters
      int m_numberConstrainedImageParameters; //!< number of constrained image parameters
      int m_numberUnknownParameters;          //!< total number of parameters to solve for
      int m_degreesOfFreedom;           //!< degrees of freedom
      double m_sigma0;                     //!< std deviation of unit weight
      double m_elapsedTime;                //!< elapsed time for bundle
      double m_elapsedTimeErrorProp;       //!< elapsed time for error propagation
      bool m_converged;


      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // variables set in computeBundleStatistics()
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      QVector< Statistics > m_rmsImageSampleResiduals;
      QVector< Statistics > m_rmsImageLineResiduals;
      QVector< Statistics > m_rmsImageResiduals;
      QVector< Statistics > m_rmsImageXSigmas;     // unset and unused ???
      QVector< Statistics > m_rmsImageYSigmas;     // unset and unused ???
      QVector< Statistics > m_rmsImageZSigmas;     // unset and unused ???
      QVector< Statistics > m_rmsImageRASigmas;    // unset and unused ???
      QVector< Statistics > m_rmsImageDECSigmas;   // unset and unused ???
      QVector< Statistics > m_rmsImageTWISTSigmas; // unset and unused ???

      double  m_minSigmaLatitude;
      QString m_minSigmaLatitudePointId;

      double  m_maxSigmaLatitude;
      QString m_maxSigmaLatitudePointId;

      double  m_minSigmaLongitude;
      QString m_minSigmaLongitudePointId;

      double  m_maxSigmaLongitude;
      QString m_maxSigmaLongitudePointId;

      double  m_minSigmaRadius;
      QString m_minSigmaRadiusPointId;

      double  m_maxSigmaRadius;
      QString m_maxSigmaRadiusPointId;

      double m_rmsSigmaLat;               //!< rms of adjusted Latitude sigmas
      double m_rmsSigmaLon;               //!< rms of adjusted Longitude sigmas
      double m_rmsSigmaRad;               //!< rms of adjusted Radius sigmas

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // variables for maximum likelihood estimation
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      // ??? we can get rid of this variable by allowing m_wFunc/Quan to be a QList/QVector of variable size and checking the size...
      int m_numberMaximumLikelihoodModels;    /**< The number of maximum likelihood estimation 
                                                    models. Up to three different models can be used
                                                    in succession.*/
      MaximumLikelihoodWFunctions *m_wFunc[3]; /**< This class is used to reweight observations in 
                                                    order to achieve more robust parameter 
                                                    estimation, up to three different maximum 
                                                    likelihood estimation models can be used in
                                                    succession.*/
      double m_maximumLikelihoodQuan[3];       /**< Quantiles of the |residual| distribution to be
                                                    used for tweaking constants of the maximum 
                                                    probability models.*/
      int m_maximumLikelihoodIndex;            /**< This count keeps track of which stadge of the
                                                    maximum likelihood adjustment the bundle is
                                                    currently on.*/
      StatCumProbDistDynCalc *m_cumPro;        /**< This class will be used to calculate the
                                                    cumulative probability distribution of
                                                    |R^2 residuals|, quantiles of this distribution
                                                    are used to adjust the maximum likelihood
                                                    functions dynamically iteration by iteration.*/
      StatCumProbDistDynCalc *m_cumProRes;     /**< This class keeps track of the cumulative 
                                                    probability distribution of residuals
                                                    (in unweighted pixels), this is used for 
                                                    reporting, and not for computation.*/
      double m_maximumLikelihoodMedianR2Residuals; /**< Median of R^2 residuals.*/ 
       
  };
  // operators to read/write SparseBlockColumnMatrix to/from binary disk file
  // operator to write SparseBlockColumnMatrix to QDebug stream
  QDataStream&operator<<(QDataStream &stream, const BundleStatistics&);
  QDataStream&operator>>(QDataStream &stream, BundleStatistics&);
  QDebug operator<<(QDebug dbg, const BundleStatistics &bundleStatistics);
};
#endif // BundleStatistics_h
