#ifndef BundleResults_h
#define BundleResults_h

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
// Qt Library
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QVector>

// Isis Library
#include "BundleControlPoint.h"
#include "BundleObservationVector.h"
#include "BundleSettings.h"
#include "ControlNet.h"
#include "Distance.h"
#include "MaximumLikelihoodWFunctions.h"
#include "PvlObject.h"
#include "Statistics.h" // ???
#include "XmlStackedHandler.h"

// Qt Library
class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  // Isis Library
  class ControlNet;
  class CorrelationMatrix;
  class FileName;
  class Project;// ??? does xml stuff need project???
  class PvlObject;
  class SerialNumberList;
  class StatCumProbDistDynCalc;
  class XmlStackedHandlerReader;

  /**
   * A container class for statistical results from a BundleAdjust solution.
   *
   * @author 2014-07-01 Jeannie Backer
   *
   * @internal
   *   @history 2014-07-01 Jeannie Backer - Original version
   *   @history 2014-07-14 Kimberly Oyama - Added support for correlation matrix.
   *   @history 2014-07-16 Jeannie Backer - Changed pvlGroup() to pvlObject()
   *   @history 2014-07-23 Jeannie Backer - Added QDataStream operators (<< and >>) and read/write
   *                           methods. Initialize m_cumProRes in the constructor since this
   *                           variable is used regardless of whether maximum likelihood estimation
   *                           is used.
   *   @history 2015-09-03 Jeannie Backer - Added preliminary hdf5 read/write capabilities. Renamed
   *                           member variables to make names more descriptive.
   *   @history 2015-10-14 Jeffrey Covington - Declared BundleResults as a Qt
   *                           metatype for use with QVariant.
   *   @history 2016-07-01 Jesse Mapel - Updated documentation and testing in preparation for
   *                           merging from IPCE into ISIS. Fixes #3975.
   *   @history 2016-08-10 Jeannie Backer - Replaced boost vector with Isis::LinearAlgebra::Vector.
   *                           References #4163.
   *   @history 2016-08-15 Jesse Mapel - Added iteration count, radians to meters conversion,
   *                           observation vector, bundle control point vector, and output control
   *                           network for write methods in BundleSolutionInfo.  Fixes #4159.
   *   @history 2017-04-24 Ian Humphrey - Removed pvlObject() method. Commented out m_id serialization
   *                           for save() (causes segfault in unit test for empty xml). Fixes #4797.
   *   @history 2017-04-27 J Bonn - Updated serialization code and tests.
   */
  class BundleResults : public QObject {
    Q_OBJECT
    public:
      BundleResults(QObject *parent = 0);
      // TODO: does xml stuff need project???
      BundleResults(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
      BundleResults(const BundleResults &src);
      ~BundleResults();
      BundleResults &operator=(const BundleResults &src);
      void initialize();

      // mutators and computation methods
      void resizeSigmaStatisticsVectors(int numberImages);
      void setRmsImageResidualLists(QList<Statistics> rmsImageLineResiduals,
                                    QList<Statistics> rmsImageSampleResiduals,
                                    QList<Statistics> rmsImageResiduals);
      void setRmsImageResidualLists(QVector<Statistics> rmsImageLineResiduals,
                                    QVector<Statistics> rmsImageSampleResiduals,
                                    QVector<Statistics> rmsImageResiduals);
      void setSigmaLatitudeRange(Distance minLatDist, Distance maxLatDist,
                                 QString minLatPointId, QString maxLatPointId);
      void setSigmaLongitudeRange(Distance minLonDist, Distance maxLonDist,
                                  QString minLonPointId, QString maxLonPointId);
      void setSigmaRadiusRange(Distance minRadDist, Distance maxRadDist,
                               QString minRadPointId, QString maxRadPointId);
      void setRmsFromSigmaStatistics(double rmsFromSigmaLatStats,
                                     double rmsFromSigmaLonStats,
                                     double rmsFromSigmaRadStats);
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
      int numberFixedPoints() const;
      void incrementHeldImages();
      int numberHeldImages() const;
      void incrementIgnoredPoints();
      int numberIgnoredPoints() const; // currently unused ???
#if 0
      double computeRejectionLimit(ControlNet *p_Cnet,
                                   double outlierRejectionMultiplier,
                                   int numObservations);
#endif
      void setRejectionLimit(double rejectionLimit);
#if 0
      double computeResiduals(
               ControlNet *pCnet,
               std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > pointWeights,
               std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > pointCorrections,
               LinearAlgebra::Vector image_Corrections,
               std::vector< double > imageParameterWeights,
               int numImagePartials,
               int rank);
#endif
      void setRmsXYResiduals(double rx, double ry, double rxy);
#if 0
      bool flagOutliers(ControlNet *pCnet);
#endif
      void setNumberRejectedObservations(int numberObservations);
      void setNumberObservations(int numberObservations);
      void setNumberImageParameters(int numberParameters); // ??? this is the same value an m_nRank
      void resetNumberConstrainedPointParameters();
      void incrementNumberConstrainedPointParameters(int incrementAmount);
      void resetNumberConstrainedImageParameters();
      void incrementNumberConstrainedImageParameters(int incrementAmount);
      void resetNumberConstrainedTargetParameters();
      void incrementNumberConstrainedTargetParameters(int incrementAmount);
      void setNumberUnknownParameters(int numberParameters);
      void computeDegreesOfFreedom();
      void computeSigma0(double dvtpv, BundleSettings::ConvergenceCriteria criteria);
      void setDegreesOfFreedom(double degreesOfFreedom);
      void setSigma0(double sigma0);
      void setElapsedTime(double time);
      void setElapsedTimeErrorProp(double time);
      void setRadiansToMeters(double rtm);
      void setConverged(bool converged); // or initialze method
      void setBundleControlPoints(QVector<BundleControlPointQsp> controlPoints);
      void setOutputControlNet(ControlNetQsp outNet);
      void setIterations(int iterations);
      void setObservations(BundleObservationVector observations);

      // Accessors...
      QList<Statistics> rmsImageSampleResiduals() const;
      QList<Statistics> rmsImageLineResiduals() const;
      QList<Statistics> rmsImageResiduals() const;
      QVector<Statistics> rmsImageXSigmas() const;       // currently unused ???
      QVector<Statistics> rmsImageYSigmas() const;       // currently unused ???
      QVector<Statistics> rmsImageZSigmas() const;       // currently unused ???
      QVector<Statistics> rmsImageRASigmas() const;      // currently unused ???
      QVector<Statistics> rmsImageDECSigmas() const;     // currently unused ???
      QVector<Statistics> rmsImageTWISTSigmas() const;   // currently unused ???
      Distance minSigmaLatitudeDistance() const;
      Distance maxSigmaLatitudeDistance() const;
      Distance minSigmaLongitudeDistance() const;
      Distance maxSigmaLongitudeDistance() const;
      Distance minSigmaRadiusDistance() const;
      Distance maxSigmaRadiusDistance() const;
      QString maxSigmaLatitudePointId() const;
      QString minSigmaLatitudePointId() const;
      QString minSigmaLongitudePointId() const;
      QString maxSigmaLongitudePointId() const;
      QString minSigmaRadiusPointId() const;
      QString maxSigmaRadiusPointId() const;
      double sigmaLatitudeStatisticsRms() const;
      double sigmaLongitudeStatisticsRms() const;
      double sigmaRadiusStatisticsRms() const;
      double rmsRx() const;  // currently unused ???
      double rmsRy() const;  // currently unused ???
      double rmsRxy() const; // currently unused ???
      double rejectionLimit() const;
      double radiansToMeters() const;
      int numberRejectedObservations() const;
      int numberObservations() const;

      int numberImageParameters() const; // ??? this is the same value an m_nRank
      int numberConstrainedPointParameters() const;
      int numberConstrainedImageParameters() const;
      int numberConstrainedTargetParameters() const;
      int numberUnknownParameters() const;
      int degreesOfFreedom() const;
      double sigma0() const;
      double elapsedTime() const;
      double elapsedTimeErrorProp() const;
      bool converged() const; // or initialze method
      QVector<BundleControlPointQsp> &bundleControlPoints();
      ControlNetQsp outputControlNet() const;
      int iterations() const;
      const BundleObservationVector &observations() const;

      int numberMaximumLikelihoodModels() const;
      int maximumLikelihoodModelIndex() const;
      StatCumProbDistDynCalc cumulativeProbabilityDistribution() const;
      StatCumProbDistDynCalc residualsCumulativeProbabilityDistribution() const;
      double maximumLikelihoodMedianR2Residuals() const;
      MaximumLikelihoodWFunctions maximumLikelihoodModelWFunc(int modelIndex) const;
      double maximumLikelihoodModelQuantile(int modelIndex) const;

      QList< QPair< MaximumLikelihoodWFunctions, double > > maximumLikelihoodModels() const;

      bool setNumberHeldImages(SerialNumberList pHeldSnList,
                               SerialNumberList *pSnList);

      // Correlation Matrix accessors for cnetsuite and mutators for bundle adjust.
      CorrelationMatrix correlationMatrix() const;
      void setCorrMatCovFileName(FileName name);
      void setCorrMatImgsAndParams(QMap<QString, QStringList> imgsAndParams);

      // TODO: does xml stuff need project???
      void save(QXmlStreamWriter &stream, const Project *project) const;


    private:
      /**
       * This class is an XmlHandler used to read and write BundleResults objects
       * from and to XML files.  Documentation will be updated when it is decided
       * if XML support will remain.
       *
       * @author 2014-07-28 Jeannie Backer
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          // TODO: does xml stuff need project???
          XmlHandler(BundleResults *statistics, Project *project);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          BundleResults *m_xmlHandlerBundleResults;
          Project *m_xmlHandlerProject;   // TODO: does xml stuff need project???
          QString m_xmlHandlerCharacters;
          int m_xmlHandlerResidualsListSize;
          int m_xmlHandlerSampleResidualsListSize;
          int m_xmlHandlerLineResidualsListSize;
          int m_xmlHandlerXSigmasListSize;
          int m_xmlHandlerYSigmasListSize;
          int m_xmlHandlerZSigmasListSize;
          int m_xmlHandlerRASigmasListSize;
          int m_xmlHandlerDECSigmasListSize;
          int m_xmlHandlerTWISTSigmasListSize;
          QList<Statistics *> m_xmlHandlerStatisticsList;
          StatCumProbDistDynCalc *m_xmlHandlerCumProCalc;

          QString m_xmlHandlerCorrelationImageId;
          QStringList m_xmlHandlerCorrelationParameterList;
          QMap<QString, QStringList> m_xmlHandlerCorrelationMap;
      };

      CorrelationMatrix *m_correlationMatrix; //!< The correlation matrix from the BundleAdjust.


      int m_numberFixedPoints;                //!< number of 'fixed' (ground) points (define)
      // Currently set but unused
      int m_numberIgnoredPoints;              //!< number of ignored points
      int m_numberHeldImages;                 //!< number of 'held' images (define)

      // The following three members are set but unused.
      double m_rmsXResiduals;                 //!< rms of x residuals
      double m_rmsYResiduals;                 //!< rms of y residuals
      double m_rmsXYResiduals;                //!< rms of all x and y residuals

      double m_rejectionLimit;                //!< current rejection limit
      // TODO:??? reorder read/write data stream, init, copy constructor, operator=
      int m_numberObservations;                //!< number of image coordinate observations
      int m_numberRejectedObservations;        //!< number of rejected image coordinate observations
      int m_numberUnknownParameters;           //!< total number of parameters to solve for
      int m_numberImageParameters;             //!< number of image parameters
      int m_numberConstrainedImageParameters;  //!< number of constrained image parameters
      int m_numberConstrainedPointParameters;  //!< number of constrained point parameters
      int m_numberConstrainedTargetParameters; //!< number of constrained target parameters
      int m_degreesOfFreedom;                  //!< degrees of freedom
      double m_sigma0;                         //!< std deviation of unit weight
      double m_elapsedTime;                    //!< elapsed time for bundle
      double m_elapsedTimeErrorProp;           //!< elapsed time for error propagation
      double m_radiansToMeters;                //!< radian to meters conversion factor for the body
      bool m_converged;

      // Variables for output methods in BundleSolutionInfo

      QVector<BundleControlPointQsp> m_bundleControlPoints; /**< The vector of BundleControlPoints
                                                                 from BundleAdjust.  Equivalent to
                                                                 the output control net minus
                                                                 ignored points and measures.
                                                                 The contained points and members
                                                                 hold pointers to the points
                                                                 and measures in the output
                                                                 control net.*/
      ControlNetQsp m_outNet;                               /**< The output control net from
                                                                 BundleAdjust.*/
      int m_iterations;                                     /**< The number of iterations taken
                                                                 by BundleAdjust.*/
      BundleObservationVector m_observations;               /**< The vector of BundleObservations
                                                                 from BundleAdjust.*/


      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // variables set in computeBundleResults()
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // QList??? jigsaw apptest gives - ASSERT failure in QList<T>::operator[]: "index out of range",
      QList<Statistics> m_rmsImageSampleResiduals;/**< List of RMS image sample residual statistics
                                                       for each image in the bundle  */
      QList<Statistics> m_rmsImageLineResiduals;  /**< List of RMS image line residual statistics
                                                       for each image in the bundle  */
      QList<Statistics> m_rmsImageResiduals;      /**< RMS image sample and line residual statistics
                                                       for each image in the bundle  */

      //!< The root mean square image x sigmas.
      QVector<Statistics> m_rmsImageXSigmas;     // unset and unused ???
      //!< The root mean square image y sigmas.
      QVector<Statistics> m_rmsImageYSigmas;     // unset and unused ???
      //!< The root mean square image z sigmas.
      QVector<Statistics> m_rmsImageZSigmas;     // unset and unused ???
      //!< The root mean square image right ascension sigmas.
      QVector<Statistics> m_rmsImageRASigmas;    // unset and unused ???
      //!< The root mean square image declination sigmas.
      QVector<Statistics> m_rmsImageDECSigmas;   // unset and unused ???
      //!< The root mean square image twist sigmas.
      QVector<Statistics> m_rmsImageTWISTSigmas; // unset and unused ???

      Distance m_minSigmaLatitudeDistance;  //!< The minimum sigma latitude distance.
      Distance m_maxSigmaLatitudeDistance;  //!< The maximum sigma latitude distance.
      Distance m_minSigmaLongitudeDistance; //!< The minimum sigma longitude distance.
      Distance m_maxSigmaLongitudeDistance; //!< The maximum sigma longitude distance.
      Distance m_minSigmaRadiusDistance;    //!< The minimum sigma radius distance.
      Distance m_maxSigmaRadiusDistance;    //!< The maximum sigma radius distance.

      QString m_minSigmaLatitudePointId;    //!< The minimum sigma latitude point id.
      QString m_maxSigmaLatitudePointId;    //!< The maximum sigma latitude point id.
      QString m_minSigmaLongitudePointId;   //!< The minimum sigma longitude point id.
      QString m_maxSigmaLongitudePointId;   //!< The maximum sigma longitude point id.
      QString m_minSigmaRadiusPointId;      //!< The minimum sigma radius point id.
      QString m_maxSigmaRadiusPointId;      //!< The maximum sigma radius point id.

      double m_rmsSigmaLatitudeStats;       //!< rms of adjusted Latitude sigmas
      double m_rmsSigmaLongitudeStats;      //!< rms of adjusted Longitude sigmas
      double m_rmsSigmaRadiusStats;         //!< rms of adjusted Radius sigmas

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // variables for maximum likelihood estimation
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      //!< The maximum likelihood models and their quantiles.
      QList< QPair< MaximumLikelihoodWFunctions, double > > m_maximumLikelihoodFunctions;

      /**< The number of maximum likelihood estimation
      models. Up to three different models can be used
      in succession.*/
      /**< This class is used to reweight observations in
           order to achieve more robust parameter
           estimation, up to three different maximum
           likelihood estimation models can be used in
           succession.*/
      /**< Quantiles of the |residual| distribution to be
           used for tweaking constants of the maximum
           probability models.*/
      int m_maximumLikelihoodIndex;            /**< This count keeps track of which stage of the
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

};

Q_DECLARE_METATYPE(Isis::BundleResults);

#endif // BundleResults_h
