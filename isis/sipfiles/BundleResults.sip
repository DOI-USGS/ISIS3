%Include Statistics.sip
%Include type_conversions.sip


namespace Isis {
  class BundleResults : public QObject {
    %TypeHeaderCode
      #include "BundleResults.h"
    %End

    public:
      BundleResults(const Isis::BundleResults &src);
      void initialize();

      void resizeSigmaStatisticsVectors(int numberImages);
      // void setRmsImageResidualLists(QList<Isis::Statistics> rmsImageLineResiduals,
      //                               QList<Isis::Statistics> rmsImageSampleResiduals,
      //                               QList<Isis::Statistics> rmsImageResiduals);
      // void setRmsImageResidualLists(QVector<Isis::Statistics> rmsImageLineResiduals,
      //                               QVector<Isis::Statistics> rmsImageSampleResiduals,
      //                               QVector<Isis::Statistics> rmsImageResiduals);
      // void setSigmaLatitudeRange(Distance minLatDist, Distance maxLatDist,
      //                            QString minLatPointId, QString maxLatPointId);
      // void setSigmaLongitudeRange(Distance minLonDist, Distance maxLonDist,
      //                             QString minLonPointId, QString maxLonPointId);
      // void setSigmaRadiusRange(Distance minRadDist, Distance maxRadDist,
      //                          QString minRadPointId, QString maxRadPointId);
      void setRmsFromSigmaStatistics(double rmsFromSigmaLatStats,
                                     double rmsFromSigmaLonStats,
                                     double rmsFromSigmaRadStats);

      void printMaximumLikelihoodTierInformation() throw(Isis::IException);
      void incrementMaximumLikelihoodModelIndex() throw(Isis::IException);

      void incrementFixedPoints() throw(Isis::IException);
      int numberFixedPoints() const throw(Isis::IException);
      void incrementHeldImages() throw(Isis::IException);
      int numberHeldImages() const throw(Isis::IException);
      void incrementIgnoredPoints() throw(Isis::IException);
      int numberIgnoredPoints() const throw(Isis::IException); // currently unused ???
      void setRejectionLimit(double rejectionLimit) throw(Isis::IException);
      void setRmsXYResiduals(double rx, double ry, double rxy) throw(Isis::IException);

      void setNumberRejectedObservations(int numberObservations) throw(Isis::IException);
      void setNumberObservations(int numberObservations) throw(Isis::IException);
      void setNumberImageParameters(int numberParameters) throw(Isis::IException); // ??? this is the same value an m_nRank
      void resetNumberConstrainedPointParameters() throw(Isis::IException);
      void incrementNumberConstrainedPointParameters(int incrementAmount) throw(Isis::IException);
      void resetNumberConstrainedImageParameters() throw(Isis::IException);
      void incrementNumberConstrainedImageParameters(int incrementAmount) throw(Isis::IException);
      void resetNumberConstrainedTargetParameters() throw(Isis::IException);
      void incrementNumberConstrainedTargetParameters(int incrementAmount) throw(Isis::IException);
      void setNumberUnknownParameters(int numberParameters) throw(Isis::IException);
      void computeDegreesOfFreedom() throw(Isis::IException);

      void computeSigma0(double dvtpv, Isis::BundleSettings::ConvergenceCriteria criteria) throw(Isis::IException);
      void setDegreesOfFreedom(double degreesOfFreedom) throw(Isis::IException);
      void setSigma0(double sigma0) throw(Isis::IException);
      void setElapsedTime(double time) throw(Isis::IException);
      void setElapsedTimeErrorProp(double time) throw(Isis::IException);
      void setRadiansToMeters(double rtm) throw(Isis::IException);
      void setConverged(bool converged) throw(Isis::IException); // or initialze method
      // void setBundleControlPoints(QVector<BundleControlPointQsp> controlPoints);
      // void setOutputControlNet(QSharedPointer<Isis::ControlNet> outNet);
      void setIterations(int iterations) throw(Isis::IException);
      // void setObservations(BundleObservationVector observations) throw(Isis::IException);

      QList<Isis::Statistics> rmsImageSampleResiduals() const throw(Isis::IException);
      QList<Isis::Statistics> rmsImageLineResiduals() const throw(Isis::IException);
      QList<Isis::Statistics> rmsImageResiduals() const throw(Isis::IException);
      QVector<Isis::Statistics> rmsImageXSigmas() const throw(Isis::IException);       // currently unused ???
      QVector<Isis::Statistics> rmsImageYSigmas() const throw(Isis::IException);       // currently unused ???
      QVector<Isis::Statistics> rmsImageZSigmas() const throw(Isis::IException);       // currently unused ???
      QVector<Isis::Statistics> rmsImageRASigmas() const throw(Isis::IException);      // currently unused ???
      QVector<Isis::Statistics> rmsImageDECSigmas() const throw(Isis::IException);     // currently unused ???
      QVector<Isis::Statistics> rmsImageTWISTSigmas() const throw(Isis::IException);   // currently unused ???

      // Distance minSigmaLatitudeDistance() const throw(Isis::IException);
      // Distance maxSigmaLatitudeDistance() const throw(Isis::IException);
      // Distance minSigmaLongitudeDistance() const throw(Isis::IException);
      // Distance maxSigmaLongitudeDistance() const throw(Isis::IException);
      // Distance minSigmaRadiusDistance() const throw(Isis::IException);
      // Distance maxSigmaRadiusDistance() const throw(Isis::IException);

      QString maxSigmaLatitudePointId() const throw(Isis::IException);
      QString minSigmaLatitudePointId() const throw(Isis::IException);
      QString minSigmaLongitudePointId() const throw(Isis::IException);
      QString maxSigmaLongitudePointId() const throw(Isis::IException);
      QString minSigmaRadiusPointId() const throw(Isis::IException);
      QString maxSigmaRadiusPointId() const throw(Isis::IException);
      double sigmaLatitudeStatisticsRms() const throw(Isis::IException);
      double sigmaLongitudeStatisticsRms() const throw(Isis::IException);
      double sigmaRadiusStatisticsRms() const throw(Isis::IException);
      double rmsRx() const throw(Isis::IException);  // currently unused ???
      double rmsRy() const throw(Isis::IException);  // currently unused ???
      double rmsRxy() const throw(Isis::IException); // currently unused ???
      double rejectionLimit() const throw(Isis::IException);
      double radiansToMeters() const throw(Isis::IException);
      int numberRejectedObservations() const throw(Isis::IException);
      int numberObservations() const throw(Isis::IException);

      int numberImageParameters() const throw(Isis::IException); // ??? this is the same value an m_nRank
      int numberConstrainedPointParameters() const throw(Isis::IException);
      int numberConstrainedImageParameters() const throw(Isis::IException);
      int numberConstrainedTargetParameters() const throw(Isis::IException);
      int numberUnknownParameters() const throw(Isis::IException);
      int degreesOfFreedom() const throw(Isis::IException);
      double sigma0() const throw(Isis::IException);
      double elapsedTime() const throw(Isis::IException);
      double elapsedTimeErrorProp() const throw(Isis::IException);
      bool converged() const throw(Isis::IException); // or initialze method

      QVector<QSharedPointer<Isis::BundleControlPoint> > &bundleControlPoints();
      QSharedPointer<Isis::ControlNet> outputControlNet() const throw(Isis::IException);

      int iterations() const throw(Isis::IException);

      int numberMaximumLikelihoodModels() const throw(Isis::IException);
      int maximumLikelihoodModelIndex() const throw(Isis::IException);
      double maximumLikelihoodMedianR2Residuals() const throw(Isis::IException);
      double maximumLikelihoodModelQuantile(int modelIndex) const throw(Isis::IException);

    };

};
