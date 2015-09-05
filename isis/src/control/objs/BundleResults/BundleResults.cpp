  #include "BundleResults.h"
  #include <QDataStream>

#include <QDebug>
#include <QString>
#include <QtGlobal> // qMax()
#include <QUuid>
#include <QXmlStreamWriter>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <H5Cpp.h>
#include <hdf5_hl.h>
#include <hdf5.h>

#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "CorrelationMatrix.h"
#include "Distance.h"
#include "FileName.h"
#include "IString.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "StatCumProbDistDynCalc.h"
#include "Statistics.h"
#include "XmlStackedHandlerReader.h"

using namespace boost::numeric::ublas;

namespace Isis {

  BundleResults::BundleResults(QObject *parent) : QObject(parent) {

    initialize();

    m_id = new QUuid(QUuid::createUuid());
    m_correlationMatrix = new CorrelationMatrix;
    m_cumPro = new StatCumProbDistDynCalc;
    m_cumProRes = new StatCumProbDistDynCalc;

    // residual prob distribution is calculated even if there is no maximum likelihood estimation.
    // so set up the solver to have a node at every percent of the distribution
    initializeResidualsProbabilityDistribution(101);

  }



  /**
   * Construct this BundleSettings object from XML.
   *
   * @param bundleSettingsFolder Where this settings XML resides - /work/.../projectRoot/images/import1
   * @param xmlReader An XML reader that's up to an <bundleSettings/> tag.
   * @param parent The Qt-relationship parent
   */
  BundleResults::BundleResults(Project *project, XmlStackedHandlerReader *xmlReader,
                                 QObject *parent) : QObject(parent) {   // TODO: does xml stuff need project???

    initialize();

    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));

  }


  BundleResults::BundleResults(H5::CommonFG &locationObject, QString locationName) {
    openH5Group(locationObject, locationName);
  }


  BundleResults::BundleResults(const BundleResults &src)
      : m_id(new QUuid(src.m_id->toString())),
        m_correlationMatrix(new CorrelationMatrix(*src.m_correlationMatrix)),
        m_numberFixedPoints(src.m_numberFixedPoints),
        m_numberIgnoredPoints(src.m_numberIgnoredPoints),
        m_numberHeldImages(src.m_numberHeldImages),
        m_rmsXResiduals(src.m_rmsXResiduals),
        m_rmsYResiduals(src.m_rmsYResiduals),
        m_rmsXYResiduals(src.m_rmsXYResiduals),
        m_rejectionLimit(src.m_rejectionLimit), 
        m_numberObservations(src.m_numberObservations),
        m_numberRejectedObservations(src.m_numberRejectedObservations),
        m_numberUnknownParameters(src.m_numberUnknownParameters),
        m_numberImageParameters(src.m_numberImageParameters),
        m_numberConstrainedImageParameters(src.m_numberConstrainedImageParameters),
        m_numberConstrainedPointParameters(src.m_numberConstrainedPointParameters),
        m_numberConstrainedTargetParameters(src.m_numberConstrainedTargetParameters),
        m_degreesOfFreedom(src.m_degreesOfFreedom),
        m_sigma0(src.m_sigma0),
        m_elapsedTime(src.m_elapsedTime),
        m_elapsedTimeErrorProp(src.m_elapsedTimeErrorProp),
        m_converged(src.m_converged),
        m_rmsImageSampleResiduals(src.m_rmsImageSampleResiduals),
        m_rmsImageLineResiduals(src.m_rmsImageLineResiduals),
        m_rmsImageResiduals(src.m_rmsImageResiduals),
        m_rmsImageXSigmas(src.m_rmsImageXSigmas),    
        m_rmsImageYSigmas(src.m_rmsImageYSigmas),    
        m_rmsImageZSigmas(src.m_rmsImageZSigmas),    
        m_rmsImageRASigmas(src.m_rmsImageRASigmas),   
        m_rmsImageDECSigmas(src.m_rmsImageDECSigmas),  
        m_rmsImageTWISTSigmas(src.m_rmsImageTWISTSigmas),
        m_minSigmaLatitudeDistance(src.m_minSigmaLatitudeDistance),
        m_maxSigmaLatitudeDistance(src.m_maxSigmaLatitudeDistance),
        m_minSigmaLongitudeDistance(src.m_minSigmaLongitudeDistance),
        m_maxSigmaLongitudeDistance(src.m_maxSigmaLongitudeDistance),
        m_minSigmaRadiusDistance(src.m_minSigmaRadiusDistance),
        m_maxSigmaRadiusDistance(src.m_maxSigmaRadiusDistance),
        m_minSigmaLatitudePointId(src.m_minSigmaLatitudePointId),
        m_maxSigmaLatitudePointId(src.m_maxSigmaLatitudePointId),
        m_minSigmaLongitudePointId(src.m_minSigmaLongitudePointId),
        m_maxSigmaLongitudePointId(src.m_maxSigmaLongitudePointId),
        m_minSigmaRadiusPointId(src.m_minSigmaRadiusPointId),
        m_maxSigmaRadiusPointId(src.m_maxSigmaRadiusPointId),
        m_rmsSigmaLatitudeStats(src.m_rmsSigmaLatitudeStats),
        m_rmsSigmaLongitudeStats(src.m_rmsSigmaLongitudeStats),
        m_rmsSigmaRadiusStats(src.m_rmsSigmaRadiusStats),
        m_maximumLikelihoodFunctions(src.m_maximumLikelihoodFunctions),
        m_maximumLikelihoodIndex(src.m_maximumLikelihoodIndex),
        m_cumPro(new StatCumProbDistDynCalc(*src.m_cumPro)),
        m_cumProRes(new StatCumProbDistDynCalc(*src.m_cumProRes)), 
        m_maximumLikelihoodMedianR2Residuals(src.m_maximumLikelihoodMedianR2Residuals) {

  }



  BundleResults::~BundleResults() {
    
    delete m_id;
    m_id = NULL;

    delete m_correlationMatrix;
    m_correlationMatrix = NULL;

    delete m_cumPro;
    m_cumPro = NULL;

    delete m_cumProRes;
    m_cumProRes = NULL;

  }

  

  BundleResults &BundleResults::operator=(const BundleResults &src) {

    if (&src != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(src.m_id->toString());

      delete m_correlationMatrix;
      m_correlationMatrix = NULL;
      m_correlationMatrix = new CorrelationMatrix(*src.m_correlationMatrix);

      m_numberFixedPoints = src.m_numberFixedPoints;
      m_numberIgnoredPoints = src.m_numberIgnoredPoints;
      m_numberHeldImages = src.m_numberHeldImages;
      m_rmsXResiduals = src.m_rmsXResiduals;
      m_rmsYResiduals = src.m_rmsYResiduals;
      m_rmsXYResiduals = src.m_rmsXYResiduals;
      m_rejectionLimit = src.m_rejectionLimit;
      m_numberObservations = src.m_numberObservations;
      m_numberRejectedObservations = src.m_numberRejectedObservations;
      m_numberUnknownParameters = src.m_numberUnknownParameters;
      m_numberImageParameters = src.m_numberImageParameters;
      m_numberConstrainedImageParameters = src.m_numberConstrainedImageParameters;
      m_numberConstrainedPointParameters = src.m_numberConstrainedPointParameters;
      m_numberConstrainedTargetParameters = src.m_numberConstrainedTargetParameters;
      m_degreesOfFreedom = src.m_degreesOfFreedom;
      m_sigma0 = src.m_sigma0;
      m_elapsedTime = src.m_elapsedTime;
      m_elapsedTimeErrorProp = src.m_elapsedTimeErrorProp;
      m_converged = src.m_converged;
      m_rmsImageSampleResiduals = src.m_rmsImageSampleResiduals;
      m_rmsImageLineResiduals = src.m_rmsImageLineResiduals;
      m_rmsImageResiduals = src.m_rmsImageResiduals;
      m_rmsImageXSigmas = src.m_rmsImageXSigmas;
      m_rmsImageYSigmas = src.m_rmsImageYSigmas;
      m_rmsImageZSigmas = src.m_rmsImageZSigmas;
      m_rmsImageRASigmas = src.m_rmsImageRASigmas;
      m_rmsImageDECSigmas = src.m_rmsImageDECSigmas;
      m_rmsImageTWISTSigmas = src.m_rmsImageTWISTSigmas;
      m_minSigmaLatitudeDistance = src.m_minSigmaLatitudeDistance;
      m_maxSigmaLatitudeDistance = src.m_maxSigmaLatitudeDistance;
      m_minSigmaLongitudeDistance = src.m_minSigmaLongitudeDistance;
      m_maxSigmaLongitudeDistance = src.m_maxSigmaLongitudeDistance;
      m_minSigmaRadiusDistance = src.m_minSigmaRadiusDistance;
      m_maxSigmaRadiusDistance = src.m_maxSigmaRadiusDistance;
      m_minSigmaLatitudePointId = src.m_minSigmaLatitudePointId;
      m_maxSigmaLatitudePointId = src.m_maxSigmaLatitudePointId;
      m_minSigmaLongitudePointId = src.m_minSigmaLongitudePointId;
      m_maxSigmaLongitudePointId = src.m_maxSigmaLongitudePointId;
      m_minSigmaRadiusPointId = src.m_minSigmaRadiusPointId;
      m_maxSigmaRadiusPointId = src.m_maxSigmaRadiusPointId;
      m_rmsSigmaLatitudeStats = src.m_rmsSigmaLatitudeStats;
      m_rmsSigmaLongitudeStats = src.m_rmsSigmaLongitudeStats;
      m_rmsSigmaRadiusStats = src.m_rmsSigmaRadiusStats;
      m_maximumLikelihoodFunctions = src.m_maximumLikelihoodFunctions;
      m_maximumLikelihoodIndex = src.m_maximumLikelihoodIndex;

      delete m_cumPro;
      m_cumPro = NULL;
      m_cumPro = new StatCumProbDistDynCalc(*src.m_cumPro);

      delete m_cumProRes;
      m_cumProRes = NULL;
      m_cumProRes = new StatCumProbDistDynCalc(*src.m_cumProRes);

      m_maximumLikelihoodMedianR2Residuals = src.m_maximumLikelihoodMedianR2Residuals;
    }
    return *this;
  }


  void BundleResults::initialize() {
    m_id = NULL;
    m_correlationMatrix = NULL;

    m_numberFixedPoints = 0; // set in BA constructor->init->fillPointIndexMap
    m_numberIgnoredPoints = 0; // set in BA constructor->init->fillPointIndexMap


    // set in BundleAdjust init()
    m_numberHeldImages = 0;

    // members set while computing bundle stats
    m_rmsImageSampleResiduals.clear();
    m_rmsImageLineResiduals.clear();
    m_rmsImageResiduals.clear();
    m_rmsImageXSigmas.clear();
    m_rmsImageYSigmas.clear();
    m_rmsImageZSigmas.clear();
    m_rmsImageRASigmas.clear();
    m_rmsImageDECSigmas.clear();
    m_rmsImageTWISTSigmas.clear();

    // initialize lat/lon/rad boundaries
    m_minSigmaLatitudeDistance.setMeters(1.0e+12);
    m_maxSigmaLatitudeDistance.setMeters(0.0);
    m_minSigmaLongitudeDistance.setMeters(1.0e+12);
    m_maxSigmaLongitudeDistance.setMeters(0.0);;
    m_minSigmaRadiusDistance.setMeters(1.0e+12);
    m_maxSigmaRadiusDistance.setMeters(0.0);
    m_minSigmaLatitudePointId = "";
    m_maxSigmaLatitudePointId = "";
    m_minSigmaLongitudePointId = "";
    m_maxSigmaLongitudePointId = "";
    m_minSigmaRadiusPointId = "";
    m_maxSigmaRadiusPointId = "";

    m_rmsSigmaLatitudeStats = 0.0;
    m_rmsSigmaLongitudeStats = 0.0;
    m_rmsSigmaRadiusStats = 0.0;


    // set by compute residuals
    m_rmsXResiduals = 0.0;
    m_rmsYResiduals = 0.0;
    m_rmsXYResiduals = 0.0;

    // set by compute rejection limit
    m_rejectionLimit = 0.0;
    
    // set by flag outliers    
    m_numberRejectedObservations = 0;

    // set by formNormalEquations_CHOLMOD, formNormalEquations_SPECIALK, or solve
    m_numberObservations = 0;
    m_numberImageParameters = 0; 

// ??? unused variable ???    m_numberHeldPoints = 0;

    // set by formNormalEquations_CHOLMOD, formNormalEquations_SPECIALK, or
    // setParameterWeights (i.e. solve)
    m_numberConstrainedPointParameters = 0;
    m_numberConstrainedImageParameters = 0;
    m_numberConstrainedTargetParameters = 0;

    // set by initialize, formNormalEquations_CHOLMOD, formNormalEquations_SPECIALK, or solve
    m_numberUnknownParameters = 0;

    // solve and solve cholesky
    m_degreesOfFreedom = -1;
    m_sigma0 = 0.0;
    m_elapsedTime = 0.0;
    m_elapsedTimeErrorProp = 0.0;
    m_converged = false; // or initialze method

    m_cumPro = NULL;
    m_maximumLikelihoodIndex = 0;
    m_maximumLikelihoodMedianR2Residuals = 0.0;
    m_maximumLikelihoodFunctions.clear();
    m_cumProRes = NULL;

  }



  void BundleResults::resizeSigmaStatisticsVectors(int numberImages) {
      m_rmsImageXSigmas.resize(numberImages);
      m_rmsImageYSigmas.resize(numberImages);
      m_rmsImageZSigmas.resize(numberImages);
      m_rmsImageRASigmas.resize(numberImages);
      m_rmsImageDECSigmas.resize(numberImages);
      m_rmsImageTWISTSigmas.resize(numberImages);
  }


#if 0
  void BundleResults::setRmsImageResidualLists(QVector<Statistics> rmsImageLineResiduals,
                                                  QVector<Statistics> rmsImageSampleResiduals,
                                                  QVector<Statistics> rmsImageResiduals) {
    m_rmsImageLineResiduals = rmsImageLineResiduals.toList();// QList??? jigsaw apptest gives - ASSERT failure in QList<T>::operator[]: "index out of range",
    m_rmsImageSampleResiduals = rmsImageSampleResiduals.toList();
    m_rmsImageResiduals = rmsImageResiduals.toList();
  }
#endif


  void BundleResults::setRmsImageResidualLists(QList<Statistics> rmsImageLineResiduals,
                                                  QList<Statistics> rmsImageSampleResiduals,
                                                  QList<Statistics> rmsImageResiduals) {
    m_rmsImageLineResiduals = rmsImageLineResiduals;
    m_rmsImageSampleResiduals = rmsImageSampleResiduals;
    m_rmsImageResiduals = rmsImageResiduals;
  }



  void BundleResults::setSigmaLatitudeRange(Distance minLatDist, Distance maxLatDist,
                                               QString minLatPointId, QString maxLatPointId) {
    m_minSigmaLatitudeDistance = minLatDist;
    m_maxSigmaLatitudeDistance = maxLatDist;
    m_minSigmaLatitudePointId  = minLatPointId;
    m_maxSigmaLatitudePointId  = maxLatPointId;
  }



  void BundleResults::setSigmaLongitudeRange(Distance minLonDist, Distance maxLonDist,
                                                QString minLonPointId, QString maxLonPointId) {
    m_minSigmaLongitudeDistance = minLonDist;
    m_maxSigmaLongitudeDistance = maxLonDist;
    m_minSigmaLongitudePointId  = minLonPointId;
    m_maxSigmaLongitudePointId  = maxLonPointId;
  }



  void BundleResults::setSigmaRadiusRange(Distance minRadDist, Distance maxRadDist,
                                             QString minRadPointId, QString maxRadPointId) {
    m_minSigmaRadiusDistance = minRadDist;
    m_maxSigmaRadiusDistance = maxRadDist;
    m_minSigmaRadiusPointId  = minRadPointId;
    m_maxSigmaRadiusPointId  = maxRadPointId;
  }



  void BundleResults::setRmsFromSigmaStatistics(double rmsFromSigmaLatStats,
                                                   double rmsFromSigmaLonStats,
                                                   double rmsFromSigmaRadStats) {
    m_rmsSigmaLatitudeStats = rmsFromSigmaLatStats;
    m_rmsSigmaLongitudeStats = rmsFromSigmaLonStats;
    m_rmsSigmaRadiusStats = rmsFromSigmaRadStats;
  }



  /** 
  * This method steps up the maximum likelihood estimation solution.  Up to three successive
  * solutions models are available.
  */
  void BundleResults::maximumLikelihoodSetUp(
      QList< QPair< MaximumLikelihoodWFunctions::Model, double > > modelsWithQuantiles) {


    // reinitialize variables if this setup has already been called
    m_maximumLikelihoodIndex = 0;
    m_maximumLikelihoodMedianR2Residuals = 0.0;

    // residual prob distribution is calculated even if there is no maximum likelihood estimation.
    // set up the solver to have a node at every percent of the distribution
    m_cumProRes = NULL;
    m_cumProRes = new StatCumProbDistDynCalc;
    initializeResidualsProbabilityDistribution(101);

    // if numberMaximumLikelihoodModels > 0, then MaximumLikeliHood Estimation is being used.
    for (int i = 0; i < modelsWithQuantiles.size(); i++) {

      // if maximum likelihood is being used, the cum prob calculator is initialized.
      if (i == 0) {
        m_cumPro = NULL;
        m_cumPro = new StatCumProbDistDynCalc;
        // set up the solver to have a node at every percent of the distribution
        initializeProbabilityDistribution(101);
      }

      // set up the w functions for the maximum likelihood estimation
      m_maximumLikelihoodFunctions.append(
          qMakePair(MaximumLikelihoodWFunctions(modelsWithQuantiles[i].first), 
                    modelsWithQuantiles[i].second));

    }
    

    //maximum likelihood estimation tiered solutions requiring multiple convergeances are support,
    // this index keeps track of which tier the solution is in
    m_maximumLikelihoodIndex = 0;
  }



  void BundleResults::printMaximumLikelihoodTierInformation() {
    printf("Maximum Likelihood Tier: %d\n", m_maximumLikelihoodIndex);
    if (numberMaximumLikelihoodModels() > m_maximumLikelihoodIndex) {
      // if maximum likelihood estimation is being used
      // at the end of every iteration
      // reset the tweaking contant to the desired quantile of the |residual| distribution
      double quantile = m_maximumLikelihoodFunctions[m_maximumLikelihoodIndex].second;
      double tc = m_cumPro->value(quantile);
      m_maximumLikelihoodFunctions[m_maximumLikelihoodIndex].first.setTweakingConstant(tc);
      //  print meadians of residuals
      m_maximumLikelihoodMedianR2Residuals = m_cumPro->value(0.5);
      printf("Median of R^2 residuals:  %lf\n", m_maximumLikelihoodMedianR2Residuals);

      //restart the dynamic calculation of the cumulative probility distribution of |R^2 residuals| --so it will be up to date for the next iteration
      initializeProbabilityDistribution(101);
    }
  }



  void BundleResults::initializeProbabilityDistribution(unsigned int nodes) {
    m_cumPro->setQuantiles(nodes);
  }



  void BundleResults::initializeResidualsProbabilityDistribution(unsigned int nodes) {
    m_cumProRes->setQuantiles(nodes);
  }



  void BundleResults::addProbabilityDistributionObservation(double observationValue) {
    m_cumPro->addObs(observationValue);
  }



  void BundleResults::addResidualsProbabilityDistributionObservation(double observationValue) {
    m_cumProRes->addObs(observationValue);
  }



  void BundleResults::incrementMaximumLikelihoodModelIndex() {
    m_maximumLikelihoodIndex++;
  }



  void BundleResults::incrementFixedPoints() {
    m_numberFixedPoints++;
  }



  int BundleResults::numberFixedPoints() const {
    return m_numberFixedPoints;
  }



  void BundleResults::incrementHeldImages() {
    m_numberHeldImages++;
  }



  int BundleResults::numberHeldImages() const {
    return m_numberHeldImages;
  }



  void BundleResults::incrementIgnoredPoints() {
    m_numberIgnoredPoints++;
  }


  int BundleResults::numberIgnoredPoints() const {
    return m_numberIgnoredPoints;
  }



  void BundleResults::setRmsXYResiduals(double rx, double ry, double rxy) {
    m_rmsXResiduals = rx;
    m_rmsYResiduals = ry;
    m_rmsXYResiduals = rxy;
  }



  void BundleResults::setRejectionLimit(double rejectionLimit) {
    m_rejectionLimit = rejectionLimit;
  }



  void BundleResults::setNumberRejectedObservations(int numberRejectedObservations) {
    m_numberRejectedObservations = numberRejectedObservations;
  }



  void BundleResults::setNumberObservations(int numberObservations) {
    m_numberObservations = numberObservations;
  }



  void BundleResults::setNumberImageParameters(int numberParameters) {
    m_numberImageParameters = numberParameters;
  }



  void BundleResults::resetNumberConstrainedPointParameters() {
    m_numberConstrainedPointParameters = 0;
  }



  void BundleResults::incrementNumberConstrainedPointParameters(int incrementAmount) {
    m_numberConstrainedPointParameters += incrementAmount;
  }



  void BundleResults::resetNumberConstrainedImageParameters() {
    m_numberConstrainedImageParameters = 0;
  }



  void BundleResults::incrementNumberConstrainedImageParameters(int incrementAmount) {
    m_numberConstrainedImageParameters += incrementAmount;
  }



  void BundleResults::resetNumberConstrainedTargetParameters() {
    m_numberConstrainedTargetParameters = 0;
  }



  void BundleResults::incrementNumberConstrainedTargetParameters(int incrementAmount) {
    m_numberConstrainedTargetParameters += incrementAmount;
  }



  void BundleResults::setNumberUnknownParameters(int numberParameters) {
    m_numberUnknownParameters = numberParameters;
  }



  void BundleResults::computeDegreesOfFreedom() {
    m_degreesOfFreedom = m_numberObservations
                         + m_numberConstrainedPointParameters
                         + m_numberConstrainedImageParameters
                         + m_numberConstrainedTargetParameters
                         - m_numberUnknownParameters;
  }



  void BundleResults::computeSigma0(double dvtpv, BundleSettings::ConvergenceCriteria criteria) {
    computeDegreesOfFreedom();

    if (m_degreesOfFreedom > 0) {
      m_sigma0 = dvtpv / m_degreesOfFreedom;
    }
    else if (m_degreesOfFreedom == 0 && criteria == BundleSettings::ParameterCorrections) {
      m_sigma0 = dvtpv;
    }
    else {
      QString msg = "Computed degrees of freedom [" + toString(m_degreesOfFreedom)
                    + "] is invalid.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    m_sigma0 = sqrt(m_sigma0);
  }



  void BundleResults::setDegreesOfFreedom(double degreesOfFreedom) { // old sparse
    m_degreesOfFreedom = degreesOfFreedom;
  }



  void BundleResults::setSigma0(double sigma0) { // old sparse
    m_sigma0 = sigma0;
  }



  void BundleResults::setElapsedTime(double time) {
    m_elapsedTime = time;
  }



  void BundleResults::setElapsedTimeErrorProp(double time) {
    m_elapsedTimeErrorProp = time;
  }



  void BundleResults::setConverged(bool converged) {
    m_converged = converged;
  }



  //************************* Accessors **********************************************************//
  QList<Statistics> BundleResults::rmsImageSampleResiduals() const {
    return m_rmsImageSampleResiduals;
  }



  QList<Statistics> BundleResults::rmsImageLineResiduals() const {
    return m_rmsImageLineResiduals;
  }



  QList<Statistics> BundleResults::rmsImageResiduals() const {
    return m_rmsImageResiduals;
  }



  QVector<Statistics> BundleResults::rmsImageXSigmas() const {
    return m_rmsImageXSigmas;
  }



  QVector<Statistics> BundleResults::rmsImageYSigmas() const {
    return m_rmsImageYSigmas;
  }



  QVector<Statistics> BundleResults::rmsImageZSigmas() const {
    return m_rmsImageZSigmas;
  }



  QVector<Statistics> BundleResults::rmsImageRASigmas() const {
    return m_rmsImageRASigmas;
  }



  QVector<Statistics> BundleResults::rmsImageDECSigmas() const {
    return m_rmsImageDECSigmas;
  }



  QVector<Statistics> BundleResults::rmsImageTWISTSigmas() const {
    return m_rmsImageTWISTSigmas;
  }



  Distance BundleResults::minSigmaLatitudeDistance() const {
    return m_minSigmaLatitudeDistance;
  }



  Distance BundleResults::maxSigmaLatitudeDistance() const {
    return m_maxSigmaLatitudeDistance;
  }



  Distance BundleResults::minSigmaLongitudeDistance() const {
    return m_minSigmaLongitudeDistance;
  }



  Distance BundleResults::maxSigmaLongitudeDistance() const {
    return m_maxSigmaLongitudeDistance;
  }



  Distance BundleResults::minSigmaRadiusDistance() const {
    return m_minSigmaRadiusDistance;
  }



  Distance BundleResults::maxSigmaRadiusDistance() const {
    return m_maxSigmaRadiusDistance;
  }



  QString BundleResults::minSigmaLatitudePointId() const {
    return m_minSigmaLatitudePointId;
  }



  QString BundleResults::maxSigmaLatitudePointId() const {
    return m_maxSigmaLatitudePointId;
  }



  QString BundleResults::minSigmaLongitudePointId() const {
    return m_minSigmaLongitudePointId;
  }



  QString BundleResults::maxSigmaLongitudePointId() const {
    return m_maxSigmaLongitudePointId;
  }



  QString BundleResults::minSigmaRadiusPointId() const {
    return m_minSigmaRadiusPointId;
  }



  QString BundleResults::maxSigmaRadiusPointId() const {
    return m_maxSigmaRadiusPointId;
  }



  double BundleResults::sigmaLatitudeStatisticsRms() const {
    return m_rmsSigmaLatitudeStats;
  }



  double BundleResults::sigmaLongitudeStatisticsRms() const {
    return m_rmsSigmaLongitudeStats;
  }



  double BundleResults::sigmaRadiusStatisticsRms() const {
    return m_rmsSigmaRadiusStats;
  }



  double BundleResults::rmsRx() const {
    return m_rmsXResiduals;
  }



  double BundleResults::rmsRy() const {
    return m_rmsYResiduals;
  }



  double BundleResults::rmsRxy() const {
    return m_rmsXYResiduals;
  }



  double BundleResults::rejectionLimit() const {
    return m_rejectionLimit;
  }



  int BundleResults::numberRejectedObservations() const {
    return m_numberRejectedObservations;
  }



  int BundleResults::numberObservations() const {
    return m_numberObservations;
  }



  int BundleResults::numberImageParameters() const {
    return m_numberImageParameters;
  }



  int BundleResults::numberConstrainedPointParameters() const {
    return m_numberConstrainedPointParameters;
  }



  int BundleResults::numberConstrainedImageParameters() const {
    return m_numberConstrainedImageParameters;
  }



  int BundleResults::numberConstrainedTargetParameters() const {
    return m_numberConstrainedTargetParameters;
  }



  int BundleResults::numberUnknownParameters() const {
    return m_numberUnknownParameters;
  }



  int BundleResults::degreesOfFreedom() const {
    return m_degreesOfFreedom;
  }



  double BundleResults::sigma0() const {
    return m_sigma0;
  }



  double BundleResults::elapsedTime() const {
    return m_elapsedTime;
  }



  double BundleResults::elapsedTimeErrorProp() const {
    return m_elapsedTimeErrorProp;
  }



  bool BundleResults::converged() const {
    return m_converged;
  }



  int BundleResults::numberMaximumLikelihoodModels() const {
    return m_maximumLikelihoodFunctions.size();
  }



  int BundleResults::maximumLikelihoodModelIndex() const {
    return m_maximumLikelihoodIndex;
  }



  StatCumProbDistDynCalc BundleResults::cumulativeProbabilityDistribution() const {
    return *m_cumPro;
  }



  StatCumProbDistDynCalc BundleResults::residualsCumulativeProbabilityDistribution() const {
    return *m_cumProRes;
  }



  double BundleResults::maximumLikelihoodMedianR2Residuals() const {
    return m_maximumLikelihoodMedianR2Residuals;
  }



  MaximumLikelihoodWFunctions BundleResults::maximumLikelihoodModelWFunc(int modelIndex) const {
    return m_maximumLikelihoodFunctions[modelIndex].first;
  }



  double BundleResults::maximumLikelihoodModelQuantile(int modelIndex) const {
    return m_maximumLikelihoodFunctions[modelIndex].second;
  }



//  QList< QPair< MaximumLikelihoodWFunctions, double > >
//      BundleResults::maximumLikelihoodModels() const {
//    return m_maximumLikelihoodFunctions;
//  }



  PvlObject BundleResults::pvlObject(QString name) const {

    PvlObject pvl(name);

    pvl += PvlKeyword("NumberFixedPoints", toString(numberFixedPoints()));
    pvl += PvlKeyword("NumberIgnoredPoints", toString(numberIgnoredPoints()));
    pvl += PvlKeyword("NumberHeldImages", toString(numberHeldImages()));
    pvl += PvlKeyword("RMSResidualX", toString(rmsRx()));
    pvl += PvlKeyword("RMSResidualY", toString(rmsRy()));
    pvl += PvlKeyword("RMSResidualXY", toString(rmsRxy()));
    pvl += PvlKeyword("RejectionLimit", toString(rejectionLimit()));
    pvl += PvlKeyword("NumberRejectedObservations", toString(numberRejectedObservations()));
    pvl += PvlKeyword("NumberObservations", toString(numberObservations()));
    pvl += PvlKeyword("NumberImageParameters", toString(numberImageParameters()));
    pvl += PvlKeyword("NumberConstrainedPointParameters", toString(numberConstrainedPointParameters()));
    pvl += PvlKeyword("NumberConstrainedImageParameters", toString(numberConstrainedImageParameters()));
    pvl += PvlKeyword("NumberConstrainedTargetParameters", toString(numberConstrainedTargetParameters()));
    pvl += PvlKeyword("NumberUnknownParameters", toString(numberUnknownParameters()));
    pvl += PvlKeyword("DegreesOfFreedom", toString(degreesOfFreedom()));
    pvl += PvlKeyword("Sigma0", toString(sigma0()));
    pvl += PvlKeyword("ElapsedTime", toString(elapsedTime()));
    pvl += PvlKeyword("ElapsedTimeErrorProp", toString(elapsedTimeErrorProp()));
    pvl += PvlKeyword("Converged", toString(converged()));
#if 0
    // loop through these ??? what value to store???
    pvl += PvlKeyword("RmsImageSampleResidualsSize", toString(m_rmsImageSampleResiduals.size());
    pvl += PvlKeyword("RmsImageLineResidualsSize",   toString(m_rmsImageLineResiduals.size());
    pvl += PvlKeyword("RmsImageResidualsSize",       toString(m_rmsImageResiduals.size());
    pvl += PvlKeyword("RmsImageXSigmasSize",         toString(m_rmsImageXSigmas.size());
    pvl += PvlKeyword("RmsImageYSigmasSize",         toString(m_rmsImageYSigmas.size());
    pvl += PvlKeyword("RmsImageZSigmasSize",         toString(m_rmsImageZSigmas.size());
    pvl += PvlKeyword("RmsImageRASigmasSize",        toString(m_rmsImageRASigmas.size());
    pvl += PvlKeyword("RmsImageDECSigmasSize",       toString(m_rmsImageDECSigmas.size());
    pvl += PvlKeyword("RmsImageTWISTSigmasSize",     toString(m_rmsImageTWISTSigmas.size());
#endif 
    pvl += PvlKeyword("MinSigmaLatitude", toString(minSigmaLatitudeDistance().meters()));
    pvl += PvlKeyword("MinSigmaLatitudePointId", minSigmaLatitudePointId());
    pvl += PvlKeyword("MaxSigmaLatitude", toString(maxSigmaLatitudeDistance().meters()));
    pvl += PvlKeyword("MaxSigmaLatitudePointId", maxSigmaLatitudePointId());
    pvl += PvlKeyword("MinSigmaLongitude", toString(minSigmaLongitudeDistance().meters()));
    pvl += PvlKeyword("MinSigmaLongitudePointId", minSigmaLongitudePointId());
    pvl += PvlKeyword("MaxSigmaLongitude", toString(maxSigmaLongitudeDistance().meters()));
    pvl += PvlKeyword("MaxSigmaLongitudePointId", maxSigmaLongitudePointId());
    pvl += PvlKeyword("MinSigmaRadius", toString(minSigmaRadiusDistance().meters()));
    pvl += PvlKeyword("MinSigmaRadiusPointId", minSigmaRadiusPointId());
    pvl += PvlKeyword("MaxSigmaRadius", toString(maxSigmaRadiusDistance().meters()));
    pvl += PvlKeyword("MaxSigmaRadiusPointId", maxSigmaRadiusPointId());
    pvl += PvlKeyword("RmsSigmaLat", toString(sigmaLatitudeStatisticsRms()));
    pvl += PvlKeyword("RmsSigmaLon", toString(sigmaLongitudeStatisticsRms()));
    pvl += PvlKeyword("RmsSigmaRad", toString(sigmaRadiusStatisticsRms()));
    pvl += PvlKeyword("NumberMaximumLikelihoodModels", toString(numberMaximumLikelihoodModels()));
    if (numberMaximumLikelihoodModels() > 0) {

      PvlKeyword models("MaximumLikelihoodModels");
      PvlKeyword quantiles("MaximumLikelihoodQuantiles"); 
      
      for (int i = 0; i < m_maximumLikelihoodFunctions.size(); i++) {
        models.addValue(MaximumLikelihoodWFunctions::modelToString(
                            m_maximumLikelihoodFunctions[i].first.model()));
        quantiles.addValue(toString(m_maximumLikelihoodFunctions[i].second));
      }
      pvl += models;
      pvl += quantiles;
      pvl += PvlKeyword("MaximumLikelihoodMedianR2Residuals", 
                          toString(m_maximumLikelihoodMedianR2Residuals));
    }

    if (m_correlationMatrix) {
      pvl += correlationMatrix().pvlObject();
    }
    else {
      pvl += PvlKeyword("CorrelationMatrix", "None");
    }

    return pvl;
  }



  /**
   * Accessor for the Correlation Matrix.
   *
   * @return The correlation matrix.
   */
  CorrelationMatrix BundleResults::correlationMatrix() const {
    if (m_correlationMatrix) {
      return *m_correlationMatrix;
    }
    else {
      throw IException(IException::Unknown, 
                       "Correlation matrix for this bundle is NULL.",
                       _FILEINFO_);
    }
  }



  /**
   * Set the covariance file name for the matrix used to calculate the correlation matrix.
   *
   * @param name Name of the file used to store the covariance matrix.
   */
  void BundleResults::setCorrMatCovFileName(FileName name) {
    correlationMatrix();// throw error if null
    m_correlationMatrix->setCovarianceFileName(name);
  }



  /**
   * Set the images and their associated parameters of the correlation matrix.
   *
   * @param imgsAndParams The qmap with all the images and parameters used for this bundle.
   */
  void BundleResults::setCorrMatImgsAndParams(QMap<QString, QStringList> imgsAndParams) {
    correlationMatrix();// throw error if null
    m_correlationMatrix->setImagesAndParameters(imgsAndParams);
  }



  void BundleResults::save(QXmlStreamWriter &stream, const Project *project) const {   // TODO: does xml stuff need project???

    stream.writeStartElement("bundleResults");
    stream.writeTextElement("id", m_id->toString());
 
//    stream.writeTextElement("instrumentId", m_instrumentId);

    stream.writeStartElement("correlationMatrix");
    stream.writeAttribute("correlationFileName", correlationMatrix().correlationFileName().expanded()); 
    stream.writeAttribute("covarianceFileName", correlationMatrix().covarianceFileName().expanded()); 
    stream.writeStartElement("imagesAndParameters");
    QMapIterator<QString, QStringList> imgParamIt(*correlationMatrix().imagesAndParameters());
    while (imgParamIt.hasNext()) {
      imgParamIt.next();
      stream.writeStartElement("image"); 
      stream.writeAttribute("id", imgParamIt.key());
      QStringList parameters = imgParamIt.value();
      for (int i = 0; i < parameters.size(); i++) {
        stream.writeTextElement("parameter", parameters[i]);
      }
      stream.writeEndElement(); // end image
      
    }
    stream.writeEndElement(); // end images and parameters
    stream.writeEndElement(); // end correlationMatrix
    
    stream.writeStartElement("generalStatisticsValues");
    stream.writeTextElement("numberFixedPoints", toString(numberFixedPoints()));
    stream.writeTextElement("numberIgnoredPoints", toString(numberIgnoredPoints()));
    stream.writeTextElement("numberHeldImages", toString(numberHeldImages()));
    stream.writeTextElement("rejectionLimit", toString(rejectionLimit()));
    stream.writeTextElement("numberRejectedObservations", toString(numberRejectedObservations()));
    stream.writeTextElement("numberObservations", toString(numberObservations()));
    stream.writeTextElement("numberImageParameters", toString(numberImageParameters()));
    stream.writeTextElement("numberConstrainedPointParameters", toString(numberConstrainedPointParameters()));
    stream.writeTextElement("numberConstrainedImageParameters", toString(numberConstrainedImageParameters()));
    stream.writeTextElement("numberConstrainedTargetParameters", toString(numberConstrainedTargetParameters()));
    stream.writeTextElement("numberUnknownParameters", toString(numberUnknownParameters()));
    stream.writeTextElement("degreesOfFreedom", toString(degreesOfFreedom()));
    stream.writeTextElement("sigma0", toString(sigma0()));
    stream.writeTextElement("converged", toString(converged()));
    stream.writeEndElement(); // end generalStatisticsValues

    stream.writeStartElement("rms");
    stream.writeStartElement("residuals");
    stream.writeAttribute("x", toString(rmsRx())); 
    stream.writeAttribute("y", toString(rmsRy())); 
    stream.writeAttribute("xy", toString(rmsRxy())); 
    stream.writeEndElement(); // end residuals element
    stream.writeStartElement("sigmas");
    stream.writeAttribute("lat", toString(sigmaLatitudeStatisticsRms())); 
    stream.writeAttribute("lon", toString(sigmaLongitudeStatisticsRms())); 
    stream.writeAttribute("rad", toString(sigmaRadiusStatisticsRms())); 
    stream.writeEndElement(); // end sigmas element

    stream.writeStartElement("imageResidualsLists");
    stream.writeStartElement("residualsList");
    stream.writeAttribute("listSize", toString(rmsImageResiduals().size())); 
    for (int i = 0; i < m_rmsImageResiduals.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageResiduals[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end residuals list
    stream.writeStartElement("sampleList");
    stream.writeAttribute("listSize", toString(rmsImageSampleResiduals().size())); 
    for (int i = 0; i < m_rmsImageSampleResiduals.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageSampleResiduals[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end sample residuals list

    stream.writeStartElement("lineList");
    stream.writeAttribute("listSize", toString(rmsImageLineResiduals().size())); 
    for (int i = 0; i < m_rmsImageLineResiduals.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageLineResiduals[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end line residuals list
    stream.writeEndElement(); // end image residuals lists

    stream.writeStartElement("imageSigmasLists");
    stream.writeStartElement("xSigmas");
    stream.writeAttribute("listSize", toString(rmsImageXSigmas().size())); 
    for (int i = 0; i < m_rmsImageXSigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageXSigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    
    stream.writeEndElement(); // end x sigma list

    stream.writeStartElement("ySigmas");
    stream.writeAttribute("listSize", toString(rmsImageYSigmas().size())); 
    for (int i = 0; i < m_rmsImageYSigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageYSigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end y sigma list

    stream.writeStartElement("zSigmas");
    stream.writeAttribute("listSize", toString(rmsImageZSigmas().size())); 
    for (int i = 0; i < m_rmsImageZSigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageZSigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end z sigma list

    stream.writeStartElement("raSigmas");
    stream.writeAttribute("listSize", toString(rmsImageRASigmas().size())); 
    for (int i = 0; i < m_rmsImageRASigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageRASigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end ra sigma list

    stream.writeStartElement("decSigmas");
    stream.writeAttribute("listSize", toString(rmsImageDECSigmas().size())); 
    for (int i = 0; i < m_rmsImageDECSigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageDECSigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end dec sigma list

    stream.writeStartElement("twistSigmas");
    stream.writeAttribute("listSize", toString(rmsImageTWISTSigmas().size())); 
    for (int i = 0; i < m_rmsImageTWISTSigmas.size(); i++) {
      stream.writeStartElement("statisticsItem");
      m_rmsImageTWISTSigmas[i].save(stream, project);
      stream.writeEndElement(); // end statistics item
    }
    stream.writeEndElement(); // end twist sigma list
    stream.writeEndElement(); // end sigmas lists
    stream.writeEndElement(); // end rms

    stream.writeStartElement("elapsedTime");
    stream.writeAttribute("time", toString(elapsedTime())); 
    stream.writeAttribute("errorProp", toString(elapsedTimeErrorProp())); 
    stream.writeEndElement(); // end elapsed time

    stream.writeStartElement("minMaxSigmas");
    stream.writeStartElement("minLat");
    stream.writeAttribute("value", toString(minSigmaLatitudeDistance().meters())); 
    stream.writeAttribute("pointId", minSigmaLatitudePointId()); 
    stream.writeEndElement();
    stream.writeStartElement("maxLat");
    stream.writeAttribute("value", toString(maxSigmaLatitudeDistance().meters())); 
    stream.writeAttribute("pointId", maxSigmaLatitudePointId()); 
    stream.writeEndElement();
    stream.writeStartElement("minLon");
    stream.writeAttribute("value", toString(minSigmaLongitudeDistance().meters())); 
    stream.writeAttribute("pointId", minSigmaLongitudePointId()); 
    stream.writeEndElement();
    stream.writeStartElement("maxLon");
    stream.writeAttribute("value", toString(maxSigmaLongitudeDistance().meters())); 
    stream.writeAttribute("pointId", maxSigmaLongitudePointId()); 
    stream.writeEndElement();
    stream.writeStartElement("minRad");
    stream.writeAttribute("value", toString(minSigmaRadiusDistance().meters())); 
    stream.writeAttribute("pointId", minSigmaRadiusPointId()); 
    stream.writeEndElement();
    stream.writeStartElement("maxRad");
    stream.writeAttribute("value", toString(maxSigmaRadiusDistance().meters())); 
    stream.writeAttribute("pointId", maxSigmaRadiusPointId()); 
    stream.writeEndElement();
    stream.writeEndElement(); // end minMaxSigmas

    // call max likelihood setup from startElement to fill the rest of these values... 
    stream.writeStartElement("maximumLikelihoodEstimation");
    stream.writeAttribute("numberModels", toString(numberMaximumLikelihoodModels())); 
    stream.writeAttribute("maximumLikelihoodIndex", toString(maximumLikelihoodModelIndex())); 
    stream.writeAttribute("maximumLikelihoodMedianR2Residuals", toString(maximumLikelihoodMedianR2Residuals())); 

    stream.writeStartElement("cumulativeProbabilityCalculator");
    cumulativeProbabilityDistribution().save(stream, project);
    stream.writeEndElement(); // end cumulativeProbabilityCalculator

    stream.writeStartElement("residualsCumulativeProbabilityCalculator");
    residualsCumulativeProbabilityDistribution().save(stream, project);
    stream.writeEndElement(); // end residualsCumulativeProbabilityCalculator

    for (int i = 0; i < numberMaximumLikelihoodModels(); i++) {
      stream.writeStartElement("model");
      stream.writeAttribute("modelNumber", toString(i+1)); 
      stream.writeAttribute("modelSelection", 
                          MaximumLikelihoodWFunctions::modelToString(m_maximumLikelihoodFunctions[i].first.model()));
      stream.writeAttribute("tweakingConstant", toString(m_maximumLikelihoodFunctions[i].first.tweakingConstant())); 
      stream.writeAttribute("quantile", toString(m_maximumLikelihoodFunctions[i].second));
      stream.writeEndElement(); // end this model
    }
    stream.writeEndElement(); // end maximumLikelihoodEstimation
    stream.writeEndElement(); // end bundleResults
  }



  BundleResults::XmlHandler::XmlHandler(BundleResults *statistics, Project *project) {   // TODO: does xml stuff need project???
    m_xmlHandlerBundleResults = NULL;
    m_xmlHandlerProject = NULL;

    m_xmlHandlerBundleResults = statistics;
    m_xmlHandlerProject = project;   // TODO: does xml stuff need project???
    m_xmlHandlerCharacters = "";

    m_xmlHandlerResidualsListSize = 0;
    m_xmlHandlerSampleResidualsListSize = 0;
    m_xmlHandlerLineResidualsListSize = 0;
    m_xmlHandlerXSigmasListSize = 0;
    m_xmlHandlerYSigmasListSize = 0;
    m_xmlHandlerZSigmasListSize = 0;
    m_xmlHandlerRASigmasListSize = 0;
    m_xmlHandlerDECSigmasListSize = 0;
    m_xmlHandlerTWISTSigmasListSize = 0;
    m_xmlHandlerStatisticsList.clear();

  }



  BundleResults::XmlHandler::~XmlHandler() {
    // do not delete this pointer... we don't own it, do we??? passed into StatCumProbDistDynCalc constructor as pointer
    // delete m_xmlHandlerProject;    // TODO: does xml stuff need project???
    m_xmlHandlerProject = NULL;
    
    // delete m_xmlHandlerBundleResults;
    // m_xmlHandlerBundleResults = NULL;
    
  }
  


  bool BundleResults::XmlHandler::startElement(const QString &namespaceURI, 
                                                  const QString &localName,
                                                  const QString &qName,
                                                  const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
        
      if (qName == "correlationMatrix") {

        m_xmlHandlerBundleResults->m_correlationMatrix = NULL;
        m_xmlHandlerBundleResults->m_correlationMatrix = new CorrelationMatrix();

        QString correlationFileName = atts.value("correlationFileName");
        if (!correlationFileName.isEmpty()) {
          FileName correlationFile(correlationFileName);
          m_xmlHandlerBundleResults->m_correlationMatrix->setCorrelationFileName(correlationFile);
        }

        QString covarianceFileName = atts.value("covarianceFileName");
        if (!covarianceFileName.isEmpty()) {
          FileName covarianceFile(covarianceFileName);
          m_xmlHandlerBundleResults->m_correlationMatrix->setCovarianceFileName(covarianceFile);
        }

      }
      else if (qName == "image") {
        QString correlationMatrixImageId = atts.value("id");
        if (!correlationMatrixImageId.isEmpty()) {
          m_xmlHandlerCorrelationImageId = correlationMatrixImageId;
        }
      }
      else if (qName == "residuals") {
        
        QString rx = atts.value("x");
        if (!rx.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsXResiduals = toDouble(rx);
        }

        QString ry = atts.value("y");
        if (!ry.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsYResiduals = toDouble(ry);
        }

        QString rxy = atts.value("xy");
        if (!rxy.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsXYResiduals = toDouble(rxy);
        }

      }
      else if (qName == "sigmas") {

        QString lat = atts.value("lat");
        if (!lat.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsSigmaLatitudeStats = toDouble(lat);
        }

        QString lon = atts.value("lon");
        if (!lon.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsSigmaLongitudeStats = toDouble(lon);
        }

        QString rad = atts.value("rad");
        if (!rad.isEmpty()) {
          m_xmlHandlerBundleResults->m_rmsSigmaRadiusStats = toDouble(rad);
        }

      }
      else if (qName == "residualsList") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerResidualsListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "sampleList") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerSampleResidualsListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "lineList") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerLineResidualsListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "xSigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerXSigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "ySigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerYSigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "zSigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerZSigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "raSigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerRASigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "decSigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerDECSigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "twistSigmas") {

        QString listSizeStr = atts.value("listSize");
        if (!listSizeStr.isEmpty()) {
          m_xmlHandlerTWISTSigmasListSize = toInt(listSizeStr);
        }

      }
      else if (qName == "statisticsItem") {
        // add statistics object to the xml handler's current statistics list.
        m_xmlHandlerStatisticsList.append(
            new Statistics(m_xmlHandlerProject, reader()));
      }
      else if (qName == "elapsedTime") {

        QString time = atts.value("time");
        if (!time.isEmpty()) {
          m_xmlHandlerBundleResults->m_elapsedTime = toDouble(time);
        }

        QString errorProp = atts.value("errorProp");
        if (!errorProp.isEmpty()) {
          m_xmlHandlerBundleResults->m_elapsedTimeErrorProp = toDouble(errorProp);
        }

      }
// ???      else if (qName == "minMaxSigmaDistances") {
// ???        QString units = atts.value("units");
// ???        if (!QString::compare(units, "meters", Qt::CaseInsensitive)) {
// ???          QString msg = "Unable to read BundleResults xml. Sigma distances must be "
// ???                        "provided in meters.";
// ???          throw IException(IException::Io, msg, _FILEINFO_);
// ???        }
// ???      }
      else if (qName == "minLat") {

        QString minLat = atts.value("value");
        if (!minLat.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaLatitudeDistance.setMeters(toDouble(minLat));
        }

        QString minLatPointId = atts.value("pointId");
        if (!minLatPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaLatitudePointId = minLatPointId;
        }

      }
      else if (qName == "maxLat") {

        QString maxLat = atts.value("value");
        if (!maxLat.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaLatitudeDistance.setMeters(toDouble(maxLat));
        }

        QString maxLatPointId = atts.value("pointId");
        if (!maxLatPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaLatitudePointId = maxLatPointId;
        }

      }
      else if (qName == "minLon") {

        QString minLon = atts.value("value");
        if (!minLon.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaLongitudeDistance.setMeters(toDouble(minLon));
        }

        QString minLonPointId = atts.value("pointId");
        if (!minLonPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaLongitudePointId = minLonPointId;
        }

      }
      else if (qName == "maxLon") {

        QString maxLon = atts.value("value");
        if (!maxLon.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaLongitudeDistance.setMeters(toDouble(maxLon));
        }

        QString maxLonPointId = atts.value("pointId");
        if (!maxLonPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaLongitudePointId = maxLonPointId;
        }

      }
      else if (qName == "minRad") {

        QString minRad = atts.value("value");
        if (!minRad.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaRadiusDistance.setMeters(toDouble(minRad));
        }

        QString minRadPointId = atts.value("pointId");
        if (!minRadPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_minSigmaRadiusPointId = minRadPointId;
        }

      }
      else if (qName == "maxRad") {

        QString maxRad = atts.value("value");
        if (!maxRad.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaRadiusDistance.setMeters(toDouble(maxRad));
        }

        QString maxRadPointId = atts.value("pointId");
        if (!maxRadPointId.isEmpty()) {
          m_xmlHandlerBundleResults->m_maxSigmaRadiusPointId = maxRadPointId;
        }

      }
      else if (qName == "maximumLikelihoodEstimation") {

        QString maximumLikelihoodIndex = atts.value("maximumLikelihoodIndex");
        if (!maximumLikelihoodIndex.isEmpty()) {
          m_xmlHandlerBundleResults->m_maximumLikelihoodIndex = toInt(maximumLikelihoodIndex);
        }

        QString maximumLikelihoodMedianR2Residuals = atts.value("maximumLikelihoodMedianR2Residuals");
        if (!maximumLikelihoodMedianR2Residuals.isEmpty()) {
          m_xmlHandlerBundleResults->m_maximumLikelihoodMedianR2Residuals = toDouble(maximumLikelihoodMedianR2Residuals);
        }

      }
      else if (qName == "model") {
        QString model = atts.value("modelSelection");
        QString tweakingConstant = atts.value("tweakingConstant");
        QString quantile = atts.value("quantile");
        bool validModel = true;
        if (model.isEmpty())            validModel = false;
        if (tweakingConstant.isEmpty()) validModel = false;
        if (quantile.isEmpty())         validModel = false;
        if (validModel) {
          m_xmlHandlerBundleResults->m_maximumLikelihoodFunctions.append(
              qMakePair(MaximumLikelihoodWFunctions(
                            MaximumLikelihoodWFunctions::stringToModel(model),
                            toDouble(tweakingConstant)),
                        toDouble(quantile)));
        }
      }
      else if (qName == "cumulativeProbabilityCalculator") {
        m_xmlHandlerBundleResults->m_cumPro = NULL;
        m_xmlHandlerBundleResults->m_cumPro = new StatCumProbDistDynCalc(m_xmlHandlerProject, reader());
      }
      else if (qName == "residualsCumulativeProbabilityCalculator") {
        m_xmlHandlerBundleResults->m_cumProRes = NULL;
        m_xmlHandlerBundleResults->m_cumProRes = new StatCumProbDistDynCalc(m_xmlHandlerProject, reader());
      }
    }
    return true;
  }



  bool BundleResults::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool BundleResults::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {

    if (!m_xmlHandlerCharacters.isEmpty()) {
      if (qName == "id") {
        m_xmlHandlerBundleResults->m_id = NULL;
        m_xmlHandlerBundleResults->m_id = new QUuid(m_xmlHandlerCharacters);
      }
//      else if (qName == "instrumentId") {
//        m_xmlHandlerBundleResults->m_instrumentId = m_xmlHandlerCharacters;
//      }
      if (qName == "parameter") {
        // add the parameter to the current list
        m_xmlHandlerCorrelationParameterList.append(m_xmlHandlerCharacters);
      }
      if (qName == "image") {
        // add this image and its parameters to the map
        if (m_xmlHandlerCorrelationImageId != "") {
          m_xmlHandlerCorrelationMap.insert(m_xmlHandlerCorrelationImageId, 
                                            m_xmlHandlerCorrelationParameterList);
        }
        m_xmlHandlerCorrelationImageId = "";
        m_xmlHandlerCorrelationParameterList.clear();

      }
      if (qName == "imagesAndParameters") {
        // set the map after all images and parameters have been added
        if (!m_xmlHandlerCorrelationMap.isEmpty()) {
          m_xmlHandlerBundleResults->setCorrMatImgsAndParams(m_xmlHandlerCorrelationMap);
        }
      }
      else if (qName == "numberFixedPoints") {
        m_xmlHandlerBundleResults->m_numberFixedPoints = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberIgnoredPoints") {
        m_xmlHandlerBundleResults->m_numberIgnoredPoints = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberHeldImages") {
        m_xmlHandlerBundleResults->m_numberHeldImages = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "rejectionLimit") {
        m_xmlHandlerBundleResults->m_rejectionLimit = toDouble(m_xmlHandlerCharacters);
      }
      else if (qName == "numberRejectedObservations") {
        m_xmlHandlerBundleResults->m_numberRejectedObservations = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberObservations") {
        m_xmlHandlerBundleResults->m_numberObservations = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberImageParameters") {
        m_xmlHandlerBundleResults->m_numberImageParameters = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberConstrainedPointParameters") {
        m_xmlHandlerBundleResults->m_numberConstrainedPointParameters = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberConstrainedImageParameters") {
        m_xmlHandlerBundleResults->m_numberConstrainedImageParameters = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberConstrainedTargetParameters") {
        m_xmlHandlerBundleResults->m_numberConstrainedTargetParameters = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "numberUnknownParameters") {
        m_xmlHandlerBundleResults->m_numberUnknownParameters = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "degreesOfFreedom") {
        m_xmlHandlerBundleResults->m_degreesOfFreedom = toInt(m_xmlHandlerCharacters);
      }
      else if (qName == "sigma0") {
        m_xmlHandlerBundleResults->m_sigma0 = toDouble(m_xmlHandlerCharacters);
      }
      else if (qName == "converged") {
        m_xmlHandlerBundleResults->m_converged = toBool(m_xmlHandlerCharacters);
      }
      // copy the xml handler's statistics list to the appropriate bundle statistics list
      else if (qName == "residualsList") {
        // ??? if (m_xmlHandlerResidualsListSize != m_xmlHandlerStatisticsList.size()) { // do this check or assume the xml is valid???
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid residualsList", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageResiduals.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "sampleList") {
        // ??? if (m_xmlHandlerSampleResidualsListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid sampleList", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageSampleResiduals.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "lineList") {
        // ??? if (m_xmlHandlerLineResidualsListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid lineList", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageLineResiduals.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "xSigmas") {
        // ??? if (m_xmlHandlerXSigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid xSigmas", _FILEINFO_); ???
        // }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageXSigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "ySigmas") {
        // ??? if (m_xmlHandlerYSigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid ySigmas", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageYSigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "zSigmas") {
        // ??? if (m_xmlHandlerZSigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid zSigmas", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageZSigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "raSigmas") {
        // ??? if (m_xmlHandlerRASigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid raSigmas", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageRASigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "decSigmas") {
        // ??? if (m_xmlHandlerDECSigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid decSigmas", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageDECSigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
      else if (qName == "twistSigmas") {
        // ??? if (m_xmlHandlerTWISTSigmasListSize != m_xmlHandlerStatisticsList.size()) {
        // ???   throw IException(IException::Unknown, 
        // ???                    "Unable to read xml file. Invalid twistSigmas", _FILEINFO_);
        // ??? }
        for (int i = 0; i < m_xmlHandlerStatisticsList.size(); i++) {
          m_xmlHandlerBundleResults->m_rmsImageTWISTSigmas.append(m_xmlHandlerStatisticsList[i]);
        }
        m_xmlHandlerStatisticsList.clear();
      }
    }
    m_xmlHandlerCharacters = "";
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }



  QDataStream &BundleResults::write(QDataStream &stream) const {
    stream << m_id->toString()
           << *m_correlationMatrix
           << (qint32)m_numberFixedPoints
           << (qint32)m_numberIgnoredPoints
           << (qint32)m_numberHeldImages
           << m_rmsXResiduals << m_rmsYResiduals << m_rmsXYResiduals
           << m_rejectionLimit
           << (qint32)m_numberObservations
           << (qint32)m_numberRejectedObservations
           << (qint32)m_numberUnknownParameters
           << (qint32)m_numberImageParameters
           << (qint32)m_numberConstrainedImageParameters
           << (qint32)m_numberConstrainedPointParameters
           << (qint32)m_numberConstrainedTargetParameters
           << (qint32)m_degreesOfFreedom
           << m_sigma0
           << m_elapsedTime << m_elapsedTimeErrorProp
           << m_converged
           << m_rmsImageSampleResiduals << m_rmsImageLineResiduals
           << m_rmsImageResiduals
           << m_rmsImageXSigmas << m_rmsImageYSigmas << m_rmsImageZSigmas
           << m_rmsImageRASigmas << m_rmsImageDECSigmas << m_rmsImageTWISTSigmas
           << m_minSigmaLatitudeDistance.meters()
           << m_maxSigmaLatitudeDistance.meters()
           << m_minSigmaLongitudeDistance.meters()
           << m_maxSigmaLongitudeDistance.meters()
           << m_minSigmaRadiusDistance.meters()
           << m_maxSigmaRadiusDistance.meters()
           << m_minSigmaLatitudePointId        
           << m_maxSigmaLatitudePointId        
           << m_minSigmaLongitudePointId       
           << m_maxSigmaLongitudePointId       
           << m_minSigmaRadiusPointId          
           << m_maxSigmaRadiusPointId          
           << m_rmsSigmaLatitudeStats << m_rmsSigmaLongitudeStats << m_rmsSigmaRadiusStats
           << m_maximumLikelihoodFunctions
           << (qint32)m_maximumLikelihoodIndex << *m_cumPro << *m_cumProRes
           << m_maximumLikelihoodMedianR2Residuals;
    return stream;
  }



  QDataStream &BundleResults::read(QDataStream &stream) {
    QString id;
    CorrelationMatrix correlationMatrix;
    qint32 numberFixedPoints, numberIgnoredPoints, numberHeldImages, numberRejectedObservations,
           numberObservations, numberImageParameters, numberConstrainedPointParameters,
           numberConstrainedImageParameters, numberConstrainedTargetParameters,
           numberUnknownParameters, degreesOfFreedom, maximumLikelihoodIndex;
    double minSigmaLatitudeDistance, maxSigmaLatitudeDistance, minSigmaLongitudeDistance,
           maxSigmaLongitudeDistance, minSigmaRadiusDistance, maxSigmaRadiusDistance;   
    StatCumProbDistDynCalc cumPro;
    StatCumProbDistDynCalc cumProRes;

    stream >> id;
    stream >> correlationMatrix;
    stream >> numberFixedPoints;
    stream >> numberIgnoredPoints;
    stream >> numberHeldImages;
    stream >> m_rmsXResiduals >> m_rmsYResiduals >> m_rmsXYResiduals;
    stream >> m_rejectionLimit;
    stream >> numberObservations;
    stream >> numberRejectedObservations;
    stream >> numberUnknownParameters;
    stream >> numberImageParameters;
    stream >> numberConstrainedImageParameters;
    stream >> numberConstrainedPointParameters;
    stream >> numberConstrainedTargetParameters;
    stream >> degreesOfFreedom;
    stream >> m_sigma0;
    stream >> m_elapsedTime >> m_elapsedTimeErrorProp;
    stream >> m_converged;
    stream >> m_rmsImageSampleResiduals >> m_rmsImageLineResiduals;
    stream >> m_rmsImageResiduals;
    stream >> m_rmsImageXSigmas >> m_rmsImageYSigmas >> m_rmsImageZSigmas;
    stream >> m_rmsImageRASigmas >> m_rmsImageDECSigmas >> m_rmsImageTWISTSigmas;
    stream >> minSigmaLatitudeDistance;
    stream >> maxSigmaLatitudeDistance;
    stream >> minSigmaLongitudeDistance;
    stream >> maxSigmaLongitudeDistance;
    stream >> minSigmaRadiusDistance;
    stream >> maxSigmaRadiusDistance;
    stream >> m_minSigmaLatitudePointId;   
    stream >> m_maxSigmaLatitudePointId;   
    stream >> m_minSigmaLongitudePointId;  
    stream >> m_maxSigmaLongitudePointId;  
    stream >> m_minSigmaRadiusPointId;     
    stream >> m_maxSigmaRadiusPointId;     
    stream >> m_rmsSigmaLatitudeStats >> m_rmsSigmaLongitudeStats >> m_rmsSigmaRadiusStats;
    stream >> m_maximumLikelihoodFunctions;
    stream >> maximumLikelihoodIndex;
    stream >> cumPro >> cumProRes;
    stream >> m_maximumLikelihoodMedianR2Residuals;

    m_id = NULL;
    m_id = new QUuid(id);

    m_correlationMatrix = NULL;
    m_correlationMatrix = new CorrelationMatrix(correlationMatrix);

    m_numberFixedPoints                 = (int)numberFixedPoints;
    m_numberIgnoredPoints               = (int)numberIgnoredPoints;
    m_numberHeldImages                  = (int)numberHeldImages;
    m_numberRejectedObservations        = (int)numberRejectedObservations;
    m_numberObservations                = (int)numberObservations;
    m_numberImageParameters             = (int)numberImageParameters;
    m_numberConstrainedPointParameters  = (int)numberConstrainedPointParameters;
    m_numberConstrainedImageParameters  = (int)numberConstrainedImageParameters;
    m_numberConstrainedTargetParameters = (int)numberConstrainedTargetParameters;
    m_numberUnknownParameters           = (int)numberUnknownParameters;
    m_degreesOfFreedom                  = (int)degreesOfFreedom;
    m_maximumLikelihoodIndex            = (int)maximumLikelihoodIndex;

    m_minSigmaLatitudeDistance.setMeters(minSigmaLatitudeDistance); 
    m_maxSigmaLatitudeDistance.setMeters(maxSigmaLatitudeDistance); 
    m_minSigmaLongitudeDistance.setMeters(minSigmaLongitudeDistance);
    m_maxSigmaLongitudeDistance.setMeters(maxSigmaLongitudeDistance);
    m_minSigmaRadiusDistance.setMeters(minSigmaRadiusDistance);   
    m_maxSigmaRadiusDistance.setMeters(maxSigmaRadiusDistance);   

    m_cumPro = NULL;
    m_cumPro = new StatCumProbDistDynCalc(cumPro);

    m_cumProRes = NULL;
    m_cumProRes = new StatCumProbDistDynCalc(cumProRes);
    
    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const BundleResults &bundleResults) {
    return bundleResults.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, BundleResults &bundleResults) {
    return bundleResults.read(stream);
  } 
  
  
    
  /**
   * Saves an hdf5 group
   */  
  void BundleResults::createH5Group(H5::CommonFG &locationObject, QString locationName) const {
    try {
      // Try block to detect exceptions raised by any of the calls inside it
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
  //      H5::Exception::dontPrint();

        // create a results group to add to the given H5 object
        QString resultsGroupName = locationName + "/BundleResults"; 
        H5::Group resultsGroup = locationObject.createGroup(resultsGroupName.toAscii());

        // use H5S_SCALAR data space type for single valued spaces
        H5::DataSpace spc(H5S_SCALAR);
        Attribute att;

        /* 
         * Add string attributes as predefined data type H5::PredType::C_S1 (string)
         */ 
        H5::StrType strDataType;
        int stringSize = 0; 

        //TODO: finish Correlation Matrix
        //Create a dataset with compression
  //    m_correlationMatrix->createH5Group(resultsGroup, resultsGroupName);
        QString correlationFileName = correlationMatrix().correlationFileName().expanded();
        stringSize = qMax(correlationFileName.length(), 1);
        strDataType = H5::StrType(H5::PredType::C_S1, stringSize); 
        att = resultsGroup.createAttribute("correlationFileName", strDataType, spc);
        att.write(strDataType, correlationFileName.toStdString());

        QString covarianceFileName = correlationMatrix().covarianceFileName().expanded();
        stringSize = qMax(covarianceFileName.length(), 1);
        strDataType = H5::StrType(H5::PredType::C_S1, stringSize); 
        att = resultsGroup.createAttribute("covarianceFileName", strDataType, spc);
        att.write(strDataType, covarianceFileName.toStdString());
        // TODO: table???
        // correlationMatrix().imagesAndParameters()???
        // QMapIterator<QString, QStringList> a list of images with their
        // corresponding parameters...


        /* 
         * Add integer attributes as predefined data type H5::PredType::NATIVE_INT
         */ 
        att = resultsGroup.createAttribute("numberFixedPoints", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberFixedPoints);

        att = resultsGroup.createAttribute("numberIgnoredPoints", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberIgnoredPoints);

        att = resultsGroup.createAttribute("numberHeldImages", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberHeldImages);

        att = resultsGroup.createAttribute("numberObservations", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberObservations);

        att = resultsGroup.createAttribute("numberRejectedObservations", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberRejectedObservations);

        att = resultsGroup.createAttribute("numberImageParameters", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberImageParameters);

        att = resultsGroup.createAttribute("numberConstrainedPointParameters", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberConstrainedPointParameters);

        att = resultsGroup.createAttribute("numberConstrainedImageParameters", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberConstrainedImageParameters);

        att = resultsGroup.createAttribute("numberUnknownParameters", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_numberUnknownParameters);

        att = resultsGroup.createAttribute("degreesOfFreedom", 
                                            H5::PredType::NATIVE_INT,
                                            spc);
        att.write(H5::PredType::NATIVE_INT, &m_degreesOfFreedom);

        /* 
         * Add double attributes as predefined data type H5::PredType::NATIVE_DOUBLE
         */ 
        att = resultsGroup.createAttribute("rejectionLimit", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rejectionLimit);

        att = resultsGroup.createAttribute("sigma0", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_sigma0);

        att = resultsGroup.createAttribute("elapsedTime", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_elapsedTime);

        att = resultsGroup.createAttribute("elapsedTimeErrorProp", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_elapsedTimeErrorProp);

        // todo: put rms in their own table/dataset/group???
        att = resultsGroup.createAttribute("rmsXResiduals", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsXResiduals);

        att = resultsGroup.createAttribute("rmsYResiduals", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsYResiduals);

        att = resultsGroup.createAttribute("rmsXYResiduals", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsXYResiduals);

        att = resultsGroup.createAttribute("rmsSigmaLatitudeStats", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaLatitudeStats);

        att = resultsGroup.createAttribute("rmsSigmaLongitudeStats", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaLongitudeStats);

        att = resultsGroup.createAttribute("rmsSigmaRadiusStats", 
                                            H5::PredType::NATIVE_DOUBLE,
                                            spc);
        att.write(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaRadiusStats);

        /*
         * Add bool attributes as predefined data type H5::PredType::NATIVE_HBOOL
         */ 
        att = resultsGroup.createAttribute("converged", 
                                            H5::PredType::NATIVE_HBOOL,
                                            spc);
        int converged = (int)m_converged;
        att.write(H5::PredType::NATIVE_HBOOL, &converged);
        /*
         * Add Statistics lists as data sets
         */ 
        QString dataSetName;
        H5::DataSet dataSet;
        hsize_t dims[1];
        H5::CompType compoundDataType = Statistics::compoundH5DataType();


        // IMAGE LINE RESIDUALS LIST
        {
          int listLength = 1;
          if (!m_rmsImageLineResiduals.isEmpty()) {
            listLength = m_rmsImageLineResiduals.size();
          }

          // Set the data space dimension to be the number of Statistics elements in this data set
          dims[0] = (hsize_t)listLength;
          H5::DataSpace dataSetSpace(1, dims);

          dataSetName = resultsGroupName + "/RmsImageLineResidualsStatistics"; 
          dataSet = resultsGroup.createDataSet(dataSetName.toAscii(),
                                               compoundDataType,
                                               dataSetSpace);

          QByteArray byteArray;
          QDataStream stream(&byteArray, QIODevice::WriteOnly);
          stream.setByteOrder(QDataStream::LittleEndian);
          for (int i = 0; i < listLength; i++) {
            stream << m_rmsImageLineResiduals[i];
          }
          char *buf = byteArray.data();
          dataSet.write(buf, compoundDataType);
          dataSet.close();
        }

        // IMAGE SAMPLE RESIDUALS LIST
        {
          int listLength = 1;
          if (!m_rmsImageSampleResiduals.isEmpty()) {
            listLength = m_rmsImageSampleResiduals.size();
          }

          // Set the data space dimension to be the number of Statistics elements in this data set
          dims[0] = (hsize_t)listLength;
          H5::DataSpace dataSetSpace(1, dims);

          dataSetName = resultsGroupName + "/RmsImageSampleResidualsStatistics"; 
          dataSet = resultsGroup.createDataSet(dataSetName.toAscii(),
                                               compoundDataType,
                                               dataSetSpace);

          QByteArray byteArray;
          QDataStream stream(&byteArray, QIODevice::WriteOnly);
          stream.setByteOrder(QDataStream::LittleEndian);
          for (int i = 0; i < listLength; i++) {
            stream << m_rmsImageSampleResiduals[i];
          }
          char *buf = byteArray.data();
          dataSet.write(buf, compoundDataType);
          dataSet.close();
        }

        // IMAGE RESIDUALS LIST
        {
          int listLength = 1;
          if (!m_rmsImageResiduals.isEmpty()) {
            listLength = m_rmsImageResiduals.size();
          }

          // Set the data space dimension to be the number of Statistics elements in this data set
          dims[0] = (hsize_t)listLength;
          H5::DataSpace dataSetSpace(1, dims);

          dataSetName = resultsGroupName + "/RmsImageResidualsStatistics"; 
          dataSet = resultsGroup.createDataSet(dataSetName.toAscii(),
                                               compoundDataType,
                                               dataSetSpace);

          QByteArray byteArray;
          QDataStream stream(&byteArray, QIODevice::WriteOnly);
          stream.setByteOrder(QDataStream::LittleEndian);
          for (int i = 0; i < listLength; i++) {
            stream << m_rmsImageResiduals[i];
          }
          char *buf = byteArray.data();
          dataSet.write(buf, compoundDataType);
          dataSet.close();
        }

      }  // end of try block
      // catch failure caused by the Attribute operations
      catch( H5::AttributeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 ATTRIBUTE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSet operations
      catch( H5::DataSetIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SET exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataSpace operations
      catch( H5::DataSpaceIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA SPACE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the DataType operations
      catch( H5::DataTypeIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 DATA TYPE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the H5File operations
      catch( H5::FileIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 FILE exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      // catch failure caused by the Group operations
      catch( H5::GroupIException error ) {
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GROUP exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GENERAL exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown, 
                       "Unable to save bundle results information to an HDF5 group.",
                       _FILEINFO_);
    }
      
  }

  void BundleResults::openH5Group(H5::CommonFG &locationObject, QString locationName) {
    try {
      // Try block to detect exceptions raised by any of the calls inside it
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
  //      H5::Exception::dontPrint();

        // create a results group to add to the given H5 object
        QString resultsGroupName = locationName + "/BundleResults"; 
        H5::Group resultsGroup = locationObject.openGroup(resultsGroupName.toAscii());

        Attribute att;

        //TODO: finish Correlation Matrix
        //Create a dataset with compression
  //    m_correlationMatrix->openH5Group(resultsGroup, resultsGroupName);

        /* 
         * read string atts as predefined data type H5::PredType::C_S1 (string)
         */ 
        H5::StrType strDataType;
        H5std_string strAttValue;

        att = resultsGroup.openAttribute("correlationFileName");
        strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
        att.read(strDataType, strAttValue);
        m_correlationMatrix->setCorrelationFileName(QString::fromStdString(strAttValue));

        att = resultsGroup.openAttribute("covarianceFileName");
        strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
        att.read(strDataType, strAttValue);
        m_correlationMatrix->setCovarianceFileName(QString::fromStdString(strAttValue));

        // TODO: table??? data set???
        // correlationMatrix().imagesAndParameters()???
        // QMapIterator<QString, QStringList> a list of images with their
        // corresponding parameters...


        /* 
         * read int attributes as predefined data type H5::PredType::NATIVE_INT
         */ 
        att = resultsGroup.openAttribute("numberFixedPoints");
        att.read(H5::PredType::NATIVE_INT, &m_numberFixedPoints);

        att = resultsGroup.openAttribute("numberIgnoredPoints");
        att.read(H5::PredType::NATIVE_INT, &m_numberIgnoredPoints);

        att = resultsGroup.openAttribute("numberHeldImages");
        att.read(H5::PredType::NATIVE_INT, &m_numberHeldImages);

        att = resultsGroup.openAttribute("numberObservations");
        att.read(H5::PredType::NATIVE_INT, &m_numberObservations);

        att = resultsGroup.openAttribute("numberRejectedObservations");
        att.read(H5::PredType::NATIVE_INT, &m_numberRejectedObservations);

        att = resultsGroup.openAttribute("numberImageParameters");
        att.read(H5::PredType::NATIVE_INT, &m_numberImageParameters);

        att = resultsGroup.openAttribute("numberConstrainedImageParameters");
        att.read(H5::PredType::NATIVE_INT, &m_numberConstrainedImageParameters);

        att = resultsGroup.openAttribute("numberConstrainedPointParameters");
        att.read(H5::PredType::NATIVE_INT, &m_numberConstrainedPointParameters);

        att = resultsGroup.openAttribute("numberUnknownParameters");
        att.read(H5::PredType::NATIVE_INT, &m_numberUnknownParameters);

        att = resultsGroup.openAttribute("degreesOfFreedom");
        att.read(H5::PredType::NATIVE_INT, &m_degreesOfFreedom);

        /* 
         * read double attributes as predefined data type H5::PredType::NATIVE_DOUBLE
         */ 
        att = resultsGroup.openAttribute("rejectionLimit");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rejectionLimit);

        att = resultsGroup.openAttribute("sigma0");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_sigma0);

        att = resultsGroup.openAttribute("elapsedTime");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_elapsedTime);

        att = resultsGroup.openAttribute("elapsedTimeErrorProp");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_elapsedTimeErrorProp);

        // todo: put rms in their own table/dataset/group???
        att = resultsGroup.openAttribute("rmsXResiduals");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsXResiduals);

        att = resultsGroup.openAttribute("rmsYResiduals");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsYResiduals);

        att = resultsGroup.openAttribute("rmsXYResiduals");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsXYResiduals);

        att = resultsGroup.openAttribute("rmsSigmaLatitudeStats");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaLatitudeStats);

        att = resultsGroup.openAttribute("rmsSigmaLongitudeStats");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaLongitudeStats);

        att = resultsGroup.openAttribute("rmsSigmaRadiusStats");
        att.read(H5::PredType::NATIVE_DOUBLE, &m_rmsSigmaRadiusStats);

        /* 
         * read bool attributes as predefined data type H5::PredType::NATIVE_HBOOL
         */ 
        int boolAttValue = 0;
        att = resultsGroup.openAttribute("converged");
        att.read(H5::PredType::NATIVE_HBOOL, &boolAttValue);
        m_converged = (bool)boolAttValue;

        /*
         * read data sets of Statistics objects
         */ 
        QString dataSetName = "";
        H5::DataSet dataSet;
        H5::CompType compoundDataType = Statistics::compoundH5DataType();

        // IMAGE LINE RESIDUALS LIST
        {
          dataSetName = resultsGroupName + "/RmsImageLineResidualsStatistics";
          herr_t status = H5Gget_objinfo(resultsGroup.getId(), dataSetName.toAscii(), 0, NULL);
          if (status != 0) {
            // group DNE...
            qDebug() << "didn't find or couldn't read stats list.";//???
          }
          try {

            // if this doesn't throw an error, then the group exists???
            H5G_stat_t info;
            resultsGroup.getObjinfo(dataSetName.toAscii(), info);

            dataSet = resultsGroup.openDataSet(dataSetName.toAscii());
            H5::DataSpace dataSetSpace = dataSet.getSpace();

            char statsList[dataSet.getStorageSize()];
            dataSet.read(statsList, compoundDataType);
    
            int listLength = dataSetSpace.getSimpleExtentNpoints();
            int statsSize = compoundDataType.getSize();
            for (int i = 0; i < listLength; i++) {
              QByteArray byteArray(&(statsList[i*statsSize]), statsSize);
              QDataStream stream(&byteArray, QIODevice::ReadOnly);
              stream.setByteOrder(QDataStream::LittleEndian);
      
              Statistics tempStats;
              stream >> tempStats;
              m_rmsImageLineResiduals.append(tempStats);
            }
    
          } 
          catch (H5::GroupIException groupError) {
            // don't do anything???
          }
        }
          
        // IMAGE SAMPLE RESIDUALS LIST
        {
          dataSetName = resultsGroupName + "/RmsImageSampleResidualsStatistics"; 
          herr_t status = H5Gget_objinfo(resultsGroup.getId(), dataSetName.toAscii(), 0, NULL);
          if (status != 0) {
            // group DNE...
            qDebug() << "didn't find or couldn't read stats list.";
          }
          try {

            // if this doesn't throw an error, then the group exists???
            H5G_stat_t info;
            resultsGroup.getObjinfo(dataSetName.toAscii(), info);

            dataSet = resultsGroup.openDataSet(dataSetName.toAscii());
            H5::DataSpace dataSetSpace = dataSet.getSpace();

            char statsList[dataSet.getStorageSize()];
            dataSet.read(statsList, compoundDataType);
    
            int listLength = dataSetSpace.getSimpleExtentNpoints();
            int statsSize = compoundDataType.getSize();
            for (int i = 0; i < listLength; i++) {
              QByteArray byteArray(&(statsList[i*statsSize]), statsSize);
              QDataStream stream(&byteArray, QIODevice::ReadOnly);
              stream.setByteOrder(QDataStream::LittleEndian);
      
              Statistics tempStats;
              stream >> tempStats;
              m_rmsImageLineResiduals.append(tempStats);
            }
    
          } 
          catch (H5::GroupIException groupError) {
            // don't do anything???
          }
        }
        // IMAGE RESIDUALS LIST
        {
          dataSetName = resultsGroupName + "/RmsImageResidualsStatistics"; 
          herr_t status = H5Gget_objinfo(resultsGroup.getId(), dataSetName.toAscii(), 0, NULL);
          if (status != 0) {
            // group DNE...
            qDebug() << "didn't find or couldn't read stats list.";//???
          }
          try {

            // if this doesn't throw an error, then the group exists???
            H5G_stat_t info;
            resultsGroup.getObjinfo(dataSetName.toAscii(), info);

            dataSet = resultsGroup.openDataSet(dataSetName.toAscii());
            H5::DataSpace dataSetSpace = dataSet.getSpace();

            char statsList[dataSet.getStorageSize()];
            dataSet.read(statsList, compoundDataType);
    
            int listLength = dataSetSpace.getSimpleExtentNpoints();
            int statsSize = compoundDataType.getSize();
            for (int i = 0; i < listLength; i++) {
              QByteArray byteArray(&(statsList[i*statsSize]), statsSize);
              QDataStream stream(&byteArray, QIODevice::ReadOnly);
              stream.setByteOrder(QDataStream::LittleEndian);
      
              Statistics tempStats;
              stream >> tempStats;
              m_rmsImageLineResiduals.append(tempStats);
            }
    
          } 
          catch (H5::GroupIException groupError) {
            // don't do anything???
          }
        }
      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 GENERAL exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown, 
                       "Unable to read bundle results information to an HDF5 group.",
                       _FILEINFO_);
    }
  }
}
