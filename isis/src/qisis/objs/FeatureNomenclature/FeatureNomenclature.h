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
class QUrl;
class QWidget;

template <typename T> class QList;

namespace Isis {
  class Distance;
  class iString;
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
   */
  class FeatureNomenclature : public QObject {
      Q_OBJECT

    public:
      FeatureNomenclature();
      FeatureNomenclature(const FeatureNomenclature &other);
      ~FeatureNomenclature();

      void queryFeatures(iString target,
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
       */
      class Feature {
        public:
          Feature();
          Feature(QDomElement searchResultFeature);
          Feature(const Feature &other);
          ~Feature();

          QWidget *toWidget() const;

          iString id() const;
          iString name() const;
          iString cleanName() const;
          iString controlNet() const;
          iString displayName() const;
          iString target() const;
          iString system() const;
          Distance diameter() const;
          iString diameterString() const;
          Latitude centerLatitude() const;
          iString centerLatitudeString() const;
          Longitude centerLongitude() const;
          iString centerLongitudeString() const;
          Latitude northernLatitude() const;
          iString northernLatitudeString() const;
          Latitude southernLatitude() const;
          iString southernLatitudeString() const;
          Longitude easternLongitude() const;
          iString easternLongitudeString() const;
          Longitude westernLongitude() const;
          iString westernLongitudeString() const;
          iString originatingContinent() const;
          iString originatingEthnicity() const;
          iString approvalStatus() const;
          iString approvalDate() const;
          iString featureType() const;
          iString referenceString() const;
          iString origin() const;
          iString lastUpdated() const;
          QUrl referenceUrl() const;
          iString referenceUrlString() const;

          void swap(Feature &other);
          Feature &operator=(const Feature &rhs);

        private:
          iString getTagText(iString tagName) const;

        private:
          /**
           * This is the XML returned by the nomenclature DB. The accessors in
           *   this class all parse the XML to get their data on demand.
           */
          QDomElement * m_xmlRepresenation;
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

    private:
      QNetworkAccessManager *m_networkMgr; //!< Network manager does request
      QNetworkRequest *m_request; //!< Network request sent

      //! These are the features identified by the nomenclature database.
      QList<Feature> *m_features;
  };

};

//! This allows Nomenclature Features to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::FeatureNomenclature::Feature);

#endif
