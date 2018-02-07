#ifndef FeatureNomenclature_h
#define FeatureNomenclature_h

#include <QObject>

// This is needed for the QVariant macro
#include <QMetaType>

class QDomDocument;
class QDomElement;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QString;
class QUrl;
class QWidget;

template <typename T> class QList;

namespace Isis {
  class Distance;
  class iTime;
  class Latitude;
  class Longitude;

  /**
   * @brief Feature nomenclature database querier
   *
   * This class queries the nomenclature database for features. To use this
   *   class, you create a blank instance. Then you connect to the
   *   featuresIdentified() signal. Finally, you call query with the appropriate
   *   input data. You can call query as many times as you want;
   *   featuresIdentified() will be emitted the same number of times. Finally,
   *   you can access the features with features().
   *
   * @author 2012-03-14 Steven Lambright
   *
   * @internal
   *   @history 2012-06-06 Steven Lambright and Kimbelry Oyama - Added an enumerator for the
   *                           approval status of each feature. Modified queryFeatures() to
   *                           ensure the correct longitude domain is used.
   *                           References #852. Fixes #892. Fixes #855.
   *   @history 2012-06-29 Steven Lambright and Kimberly Oyama - Separated queryFeatures() into:
   *                           1) queryFeatures(), which makes sure the longitude range is between
   *                           0 and 360 and that it is in the 360 domain, and calls the request
   *                           method, and 2) runQuery(), which makes the request. Fixes #958.
   *   @history 2018-01-05 Cole Neubauer - The URL for request was moved. Updated the URL to the
   *                           new URL and added an error to be thrown if it is moved again
   *                           Fixes #4963.
   */
  class FeatureNomenclature : public QObject {
      Q_OBJECT

    public:

      //!Enumeration of approval statuses
      enum IAUStatus {
        /**
         * When this status is assigned to a feature, there will be no status displayed and
         * the feature will not be shown if the IAU approved only checkbox is checked.
         */
        NoStatus,
        /**
         * When this status is assigned to a feature, the displayed status will be "Adopted
         * by the IAU" and the feature will always be shown.
         */
        Approved,
        /**
         * When this status is assigned to a feature, the displayed status will be "Dropped,
         * disallowed" and the feature will not be shown if the IAU approved only checkbox is
         * checked.
         */
        Dropped,
        /**
         * When this status is assigned to a feature, the displayed status will be "Never
         * approved by the IAU" and the feature will not be shown if the IAU approved only
         * checkbox checked.
         */
        Unapproved };

      FeatureNomenclature();
      FeatureNomenclature(const FeatureNomenclature &other);
      ~FeatureNomenclature();

      void queryFeatures(QString target,
                         Latitude startLat, Longitude startLon,
                         Latitude endLat, Longitude endLon);

      class Feature;
      QList<Feature> features() const;
      bool hasResult() const;

      void swap(FeatureNomenclature &other);
      FeatureNomenclature &operator=(const FeatureNomenclature &other);

      static bool featureDiameterGreaterThan(
          const FeatureNomenclature::Feature &lhs,
          const FeatureNomenclature::Feature &rhs);

      /**
       * @brief A named feature on a target
       *
       * This class encapsulates the idea of a single named feature on a target.
       *   The accessor methods will return empty strings/invalid data types if
       *   data is not present or available. These should be instantiated
       *   (originally) by the FeatureNomenclature class.
       *
       * @author 2012-03-22 Steven Lambright
       *
       * @internal
       *   @history 2012-06-06 Steven Lambright and Kimberly Oyama - Added approval status to the
       *                           list of characteristics of the feature and added the accessor
       *                           (status()). Fixes #852. Fixes #892.
       *   @history 2016-05-23 Ian Humphrey - Modified runQuery() to correctly send POST request
       *                           to find the features for the feature nomenclature tool (Qt5).
       */
      class Feature {
      public:
          Feature();
          Feature(QDomElement searchResultFeature, IAUStatus status);
          Feature(const Feature &other);
          ~Feature();

          QWidget *toWidget() const;

          QString id() const;
          QString name() const;
          QString cleanName() const;
          QString controlNet() const;
          QString displayName() const;
          QString target() const;
          QString system() const;
          Distance diameter() const;
          QString diameterString() const;
          Latitude centerLatitude() const;
          QString centerLatitudeString() const;
          Longitude centerLongitude() const;
          QString centerLongitudeString() const;
          Latitude northernLatitude() const;
          QString northernLatitudeString() const;
          Latitude southernLatitude() const;
          QString southernLatitudeString() const;
          Longitude easternLongitude() const;
          QString easternLongitudeString() const;
          Longitude westernLongitude() const;
          QString westernLongitudeString() const;
          QString originatingContinent() const;
          QString originatingEthnicity() const;
          QString approvalStatus() const;
          QString approvalDate() const;
          QString featureType() const;
          QString referenceString() const;
          QString origin() const;
          QString lastUpdated() const;
          QUrl referenceUrl() const;
          QString referenceUrlString() const;
          IAUStatus status() const;

          void swap(Feature &other);
          Feature &operator=(const Feature &rhs);

        private:
          QString getTagText(QString tagName) const;

        private:
          /**
           * This is the XML returned by the nomenclature DB. The accessors in
           *   this class all parse the XML to get their data on demand.
           */
          QDomElement * m_xmlRepresenation;
          //!The approval status of the feature
          IAUStatus  m_approvalStatus;
      };

    signals:
      /**
       * This is emitted when a query is completed. When emitted, the this
       *   pointer is provided.
       */
      void featuresIdentified(FeatureNomenclature *);

    private slots:
      void requestFinished(QNetworkReply *);

    private:
      void readSearchResults(QDomElement);
      void runQuery(QString target,
                         Latitude startLat, Longitude startLon,
                         Latitude endLat, Longitude endLon);

    private:
      QNetworkAccessManager *m_networkMgr; //!< Network manager does request
      QNetworkRequest *m_request; //!< Network request sent

      //! These are the features identified by the nomenclature database.
      QList<Feature> *m_features;
      //!The approval status of the feature from the database
      IAUStatus m_statusApproval;
      //!True if all queries have finished
      bool m_lastQuery;
  };

};

//! This allows Nomenclature Features to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::FeatureNomenclature::Feature);

#endif
