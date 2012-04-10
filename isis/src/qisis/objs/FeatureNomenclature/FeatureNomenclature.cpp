#include "IsisDebug.h"
#include "FeatureNomenclature.h"

#include <QDebug>
#include <QDomElement>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "Distance.h"
#include "IException.h"
#include "iString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"

namespace Isis {
  /**
   * Instantiate a feature nomenclature. This prepares to make network requests.
   */
  FeatureNomenclature::FeatureNomenclature() {
    m_networkMgr = NULL;
    m_request = NULL;
    m_features = NULL;

    m_networkMgr = new QNetworkAccessManager;

    connect(m_networkMgr, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(requestFinished(QNetworkReply *)));

    m_request = new QNetworkRequest;
    m_request->setUrl(
        QUrl("http://planetarynames.wr.usgs.gov/nomenclature/SearchResults"));
    m_request->setRawHeader("User-Agent",
                            "Mozilla/5.0 (X11; Linux i686; rv:6.0) "
                            "Gecko/20100101 Firefox/6.0");
    m_request->setHeader(QNetworkRequest::ContentTypeHeader,
                         "application/x-www-form-urlencoded");
  }


  /**
   * Copy a feature nomenclature. Data being queried currently will not be
   *   available in the copy, but ready results will be available.
   *
   * @param other The feature nomenclature to copy
   */
  FeatureNomenclature::FeatureNomenclature(const FeatureNomenclature &other) {
    m_networkMgr = NULL;
    m_request = NULL;
    m_features = NULL;

    m_networkMgr = new QNetworkAccessManager;

    connect(m_networkMgr, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(requestFinished(QNetworkReply *)));

    m_request = new QNetworkRequest;

    if (other.m_features)
      m_features = new QList<Feature>(*other.m_features);
  }


  /**
   * Frees allocated memory.
   */
  FeatureNomenclature::~FeatureNomenclature() {
    delete m_networkMgr;
    m_networkMgr = NULL;

    delete m_request;
    m_request = NULL;

    delete m_features;
    m_features = NULL;
  }


  /**
   * Query the nomenclature database for features inside the given range on the
   *   target. When the query is done, featuresIdentified() will be emitted.
   *
   * You can call query as many times as you'd like, but the results will be
   *   cumulative. Errors are reported in the form of a message box.
   *
   * @param target The target for which features might be present
   * @param startLat The minimum latitude of the ground range to search
   * @param startLon The minimum longitude of the ground range to search
   * @param endLat The maximum latitude of the ground range to search
   * @param endLon The maximum longitude of the ground range to search
   */
  void FeatureNomenclature::queryFeatures(iString target,
                                          Latitude startLat, Longitude startLon,
                                          Latitude endLat, Longitude endLon) {
    QUrl encodedFormData;

    // List of XML fields we want from the server
    encodedFormData.addQueryItem("additionalInfoColumn", "true");
    encodedFormData.addQueryItem("approvalDateColumn", "true");
    encodedFormData.addQueryItem("approvalStatusColumn", "true");
    encodedFormData.addQueryItem("centerLatLonColumn", "true");
    encodedFormData.addQueryItem("cleanFeatureNameColumn", "true");
    encodedFormData.addQueryItem("contEthColumn", "true");
    encodedFormData.addQueryItem("coordSystemColumn", "true");
    encodedFormData.addQueryItem("diameterColumn", "true");
    encodedFormData.addQueryItem("featureIDColumn", "true");
    encodedFormData.addQueryItem("featureNameColumn", "true");
    encodedFormData.addQueryItem("featureTypeCodeColumn", "true");
    encodedFormData.addQueryItem("featureTypeColumn", "true");
    encodedFormData.addQueryItem("lastUpdatedColumn", "true");
    encodedFormData.addQueryItem("latLonColumn", "true");
    encodedFormData.addQueryItem("originColumn", "true");
    encodedFormData.addQueryItem("quadColumn", "true");
    encodedFormData.addQueryItem("referenceColumn", "true");
    encodedFormData.addQueryItem("targetColumn", "true");

    // Data units
    encodedFormData.addQueryItem("is_0_360", "true");
    encodedFormData.addQueryItem("is_planetographic", "false");
    encodedFormData.addQueryItem("is_positive_east", "true");

    // Format parameters
    encodedFormData.addQueryItem("displayType", "XML");
    encodedFormData.addQueryItem("sort_asc", "true");
    encodedFormData.addQueryItem("sort_column", "name");

    // Search critera (required even if blank)
    encodedFormData.addQueryItem("approvalStatus", "");
    encodedFormData.addQueryItem("beginDate", "");
    encodedFormData.addQueryItem("continent", "");
    encodedFormData.addQueryItem("endDate", "");
    encodedFormData.addQueryItem("ethnicity", "");
    encodedFormData.addQueryItem("feature", "");
    encodedFormData.addQueryItem("featureType", "");
    encodedFormData.addQueryItem("minFeatureDiameter", "");
    encodedFormData.addQueryItem("maxFeatureDiameter", "");
    encodedFormData.addQueryItem("reference", "");
    encodedFormData.addQueryItem("system", "");

    encodedFormData.addQueryItem("target", target.UpCase().ToQt());
    encodedFormData.addQueryItem("easternLongitude",
                                 QString::number(endLon.degrees()));
    encodedFormData.addQueryItem("westernLongitude",
                                 QString::number(startLon.degrees()));
    encodedFormData.addQueryItem("northernLatitude",
                                 QString::number(endLat.degrees()));
    encodedFormData.addQueryItem("southernLatitude",
                                 QString::number(startLat.degrees()));

    m_networkMgr->post(*m_request, QByteArray(encodedFormData.encodedQuery()));
  }


  /**
   * This gives you the features found in all of the queries so far. If all of
   *   the queries are finished, then this list will contain all of the results.
   *   The list will be empty if no queries have succeeded so far.
   *
   * @return a cumulative list of all of the features found so far.
   */
  QList<FeatureNomenclature::Feature> FeatureNomenclature::features() const {
    QList<Feature> featureList;

    if (m_features)
      featureList = *m_features;

    return featureList;
  }


  /**
   * Test if any understandable results have been received from the nomenclature
   *   database.
   *
   * @return True if any queries have succeeded.
   */
  bool FeatureNomenclature::hasResult() const {
    return (m_features != NULL);
  }


  /**
   * Swap the instances *this and other. This cannot throw an exception.
   *
   * @param other The FeatureNomenclature to trade data with.
   */
  void FeatureNomenclature::swap(FeatureNomenclature &other) {
    std::swap(m_networkMgr, other.m_networkMgr);
    std::swap(m_request, other.m_request);
    std::swap(m_features, other.m_features);
  }


  /**
   * This takes on the data from rhs. Active queries will not be copied. This is
   *   exception safe.
   *
   * @param rhs The FeatureNomenclature on the right hand side of the '='
   * @return *this
   */
  FeatureNomenclature &FeatureNomenclature::operator=(
      const FeatureNomenclature &rhs) {
    FeatureNomenclature tmp(rhs);
    swap(tmp);
    return *this;
  }


  /**
   * Compare the diameter of two features. This is very useful for sorting with
   *   qSort(). If lhs > rhs, then this returns true. This considers valid data
   *   > invalid data.
   *
   * @param lhs The left hand side of the '>' operator
   * @param rhs The right hand side of the '>' operator
   * @return lhs > rhs
   */
  bool FeatureNomenclature::featureDiameterGreaterThan(
      const FeatureNomenclature::Feature &lhs,
      const FeatureNomenclature::Feature &rhs) {
    Distance lhsDiameter = lhs.diameter();
    Distance rhsDiameter = rhs.diameter();

    bool greaterThan = false;
    if (lhsDiameter.isValid() && rhsDiameter.isValid()) {
      greaterThan = (lhsDiameter > rhsDiameter);
    }
    else {
      greaterThan = lhsDiameter.isValid();
    }

    return greaterThan;
  }


  /**
   * Construct a feature with no data.
   */
  FeatureNomenclature::Feature::Feature() {
    m_xmlRepresenation = NULL;
  }


  /**
   * Construct a feature with the data encapsulated inside of the XML.
   */
  FeatureNomenclature::Feature::Feature(QDomElement searchResultFeature) {
    m_xmlRepresenation = NULL;
    m_xmlRepresenation = new QDomElement(searchResultFeature);
  }


  /**
   * Copy a feature. This is a shallow copy.
   *
   * @param other The feature to copy
   */
  FeatureNomenclature::Feature::Feature(const Feature &other) {
    m_xmlRepresenation = NULL;
    // QDomElement does a shallow copy.
    m_xmlRepresenation = new QDomElement(*other.m_xmlRepresenation);
  }


  /**
   * Clean up allocated memory by this feature.
   */
  FeatureNomenclature::Feature::~Feature() {
    delete m_xmlRepresenation;
    m_xmlRepresenation = NULL;
  }


  /**
   * This converts the data in this feature to a widget. All of the information
   *   returned by the server is encapsulated in this widget. There are no
   *   controls, this is a display only.
   *
   * @return A widget that displays all of the feature information
   */
  QWidget *FeatureNomenclature::Feature::toWidget() const {
    QWidget *widget = new QWidget;

    QGridLayout *layout = new QGridLayout;
    widget->setLayout(layout);

    int row = 0;

    QLabel *titleLabel = new QLabel("<h2>Feature Details</h2>");
    layout->addWidget(titleLabel, row, 0, 1, 2);
    row++;

    QList< QPair<iString, iString (FeatureNomenclature::Feature::*)() const> >
        displayValues;

    QPair<iString, iString (FeatureNomenclature::Feature::*)() const>
        displayValue;

    displayValue.first = "Feature Name:";
    displayValue.second = &FeatureNomenclature::Feature::displayName;
    displayValues.append(displayValue);

//     displayValue.first = "Feature Name (clean):";
//     displayValue.second = &FeatureNomenclature::Feature::cleanName;
//     displayValues.append(displayValue);

    displayValue.first = "Feature ID:";
    displayValue.second = &FeatureNomenclature::Feature::id;
    displayValues.append(displayValue);

    displayValue.first = "Target:";
    displayValue.second = &FeatureNomenclature::Feature::target;
    displayValues.append(displayValue);

    displayValue.first = "System:";
    displayValue.second = &FeatureNomenclature::Feature::system;
    displayValues.append(displayValue);

    displayValue.first = "Control Network:";
    displayValue.second = &FeatureNomenclature::Feature::controlNet;
    displayValues.append(displayValue);

    displayValue.first = "Diameter:";
    displayValue.second = &FeatureNomenclature::Feature::diameterString;
    displayValues.append(displayValue);

    displayValue.first = "Originating Continent:";
    displayValue.second =
        &FeatureNomenclature::Feature::originatingContinent;
    displayValues.append(displayValue);

    displayValue.first = "Originating Ethnicity:";
    displayValue.second =
        &FeatureNomenclature::Feature::originatingEthnicity;
    displayValues.append(displayValue);

    displayValue.first = "Approval Status:";
    displayValue.second =
        &FeatureNomenclature::Feature::approvalStatus;
    displayValues.append(displayValue);

    displayValue.first = "Feature Type:";
    displayValue.second =
        &FeatureNomenclature::Feature::featureType;
    displayValues.append(displayValue);

    displayValue.first = "Center Latitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::centerLatitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Center Longitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::centerLongitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Northern Latitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::northernLatitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Southern Latitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::southernLatitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Eastern Longitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::easternLongitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Western Longitude:";
    displayValue.second =
        &FeatureNomenclature::Feature::westernLongitudeString;
    displayValues.append(displayValue);

    displayValue.first = "Approval Date:";
    displayValue.second =
        &FeatureNomenclature::Feature::approvalDate;
    displayValues.append(displayValue);

    displayValue.first = "Last Updated:";
    displayValue.second =
        &FeatureNomenclature::Feature::lastUpdated;
    displayValues.append(displayValue);

    displayValue.first = "Reference:";
    displayValue.second =
        &FeatureNomenclature::Feature::referenceString;
    displayValues.append(displayValue);

    displayValue.first = "Origin:";
    displayValue.second =
        &FeatureNomenclature::Feature::origin;
    displayValues.append(displayValue);

    displayValue.first = "URL:";
    displayValue.second =
        &FeatureNomenclature::Feature::referenceUrlString;
    displayValues.append(displayValue);

    for (int i = 0; i < displayValues.count(); i++) {
      QLabel *titleLabel = new QLabel(displayValues[i].first.ToQt());
      QLabel *valueLabel = new QLabel( (this->*(displayValues[i].second))() );
      valueLabel->setOpenExternalLinks(true);
      valueLabel->setWordWrap(true);

      if (valueLabel->text() != "") {
        layout->addWidget(titleLabel, row, 0);
        layout->addWidget(valueLabel, row, 1);
      }
      else {
        delete titleLabel;
        delete valueLabel;
      }

      row++;
    }

    return widget;
  }


  /**
   * @return The feature ID (typically a number)
   */
  iString FeatureNomenclature::Feature::id() const {
    return getTagText("id");
  }


  /**
   * @return The feature name (not always compatible with iString)
   */
  iString FeatureNomenclature::Feature::name() const {
    return getTagText("name");
  }


  /**
   * @return The 'clean' feature name (non-ascii characters cleaned up)
   */
  iString FeatureNomenclature::Feature::cleanName() const {
    return getTagText("cleanName");
  }


  /**
   * @return The source control network for the feature position information.
   *           This is implied by what's currently available for targets, since
   *           the database does not return this information.
   */
  iString FeatureNomenclature::Feature::controlNet() const {
    iString targetStr = target();
    iString cnet = "";

    if (targetStr.UpCase() == "MOON")
      cnet = "LOLA";
    else if (targetStr.UpCase() == "MARS")
      cnet = "MDIM 2.1";
    else if (targetStr.UpCase() == "MERCURY")
      cnet = "Preliminary MESSENGER";

    return cnet;
  }


  /**
   * @return The recommended feature name to display to the users
   */
  iString FeatureNomenclature::Feature::displayName() const {
    iString nameString = name();
    iString cleanNameString = cleanName();

    iString displayNameString = nameString;

    if (nameString != cleanNameString)
      displayNameString = nameString + " (" + cleanNameString + ")";

    return displayNameString;
  }


  /**
   * @return The target name
   */
  iString FeatureNomenclature::Feature::target() const {
    return getTagText("target");
  }


  /**
   * @return The target's system (i.e. Target: Moon, System: Earth)
   */
  iString FeatureNomenclature::Feature::system() const {
    return getTagText("system");
  }


  /**
   * @return The feature's diameter
   */
  Distance FeatureNomenclature::Feature::diameter() const {
    Distance result;

    try {
      result = Distance((double)getTagText("diameter"),
                        Distance::Kilometers);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's diameter as a human readable string.
   */
  iString FeatureNomenclature::Feature::diameterString() const {
    return diameter().toString();
  }


  /**
   * @return The feature's center latitude.
   */
  Latitude FeatureNomenclature::Feature::centerLatitude() const {
    Latitude result;

    try {
      result = Latitude((double)getTagText("centerlatitude"), Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's center latitude as a human readable string.
   */
  iString FeatureNomenclature::Feature::centerLatitudeString() const {
    return centerLatitude().toString();
  }


  /**
   * @return The feature's center longitude.
   */
  Longitude FeatureNomenclature::Feature::centerLongitude() const {
    Longitude result;

    try {
      result = Longitude((double)getTagText("centerlongitude"),
                         Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's center longitude as a human readable string.
   */
  iString FeatureNomenclature::Feature::centerLongitudeString() const {
    return centerLongitude().toString();
  }


  /**
   * @return The feature's northernmost (max) latitude.
   */
  Latitude FeatureNomenclature::Feature::northernLatitude() const {
    Latitude result;

    try {
      result = Latitude((double)getTagText("northernLatitude"), Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's northernmost (max) latitude as a human readable
   *         string.
   */
  iString FeatureNomenclature::Feature::northernLatitudeString() const {
    return northernLatitude().toString();
  }


  /**
   * @return The feature's southernmost (min) latitude.
   */
  Latitude FeatureNomenclature::Feature::southernLatitude() const {
    Latitude result;

    try {
      result = Latitude((double)getTagText("southernLatitude"), Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's southernmost (min) latitude as a human readable
   *         string.
   */
  iString FeatureNomenclature::Feature::southernLatitudeString() const {
    return southernLatitude().toString();
  }


  /**
   * @return The feature's easternmost (max) longitude.
   */
  Longitude FeatureNomenclature::Feature::easternLongitude() const {
    Longitude result;

    try {
      result = Longitude((double)getTagText("easternLongitude"),
                         Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's easternmost (max) longitude as a human readable
   *         string.
   */
  iString FeatureNomenclature::Feature::easternLongitudeString() const {
    return easternLongitude().toString();
  }


  /**
   * @return The feature's westernmost (min) longitude.
   */
  Longitude FeatureNomenclature::Feature::westernLongitude() const {
    Longitude result;

    try {
      result = Longitude((double)getTagText("westernLongitude"),
                         Angle::Degrees);
    }
    catch (IException &) {
    }

    return result;
  }


  /**
   * @return The feature's westernmost (min) longitude as a human readable
   *         string.
   */
  iString FeatureNomenclature::Feature::westernLongitudeString() const {
    return westernLongitude().toString();
  }


  /**
   * @return The continent from which the person who named the feature
   *         originated.
   */
  iString FeatureNomenclature::Feature::originatingContinent() const {
    return getTagText("continent");
  }


  /**
   * @return The ethnicity of the person who named the feature.
   */
  iString FeatureNomenclature::Feature::originatingEthnicity() const {
    return getTagText("ethnicity");
  }


  /**
   * @return The IAU approval status of the feature.
   */
  iString FeatureNomenclature::Feature::approvalStatus() const {
    return getTagText("approvalStatus");
  }


  /**
   * @return The approval date of the feature.
   */
  iString FeatureNomenclature::Feature::approvalDate() const {
    return getTagText("approvalDate");
  }


  /**
   * @return The type of feature (for example, crater)
   */
  iString FeatureNomenclature::Feature::featureType() const {
    return getTagText("featuretype");
  }


  /**
   * @return The feature's reference (bibliography) information
   */
  iString FeatureNomenclature::Feature::referenceString() const {
    return getTagText("reference");
  }


  /**
   * @return The feature's origin
   */
  iString FeatureNomenclature::Feature::origin() const {
    return getTagText("origin");
  }


  /**
   * @return The feature's last updated time as a string.
   */
  iString FeatureNomenclature::Feature::lastUpdated() const {
    return getTagText("lastUpdated");
  }


  /**
   * @return The feature's online URL.
   */
  QUrl FeatureNomenclature::Feature::referenceUrl() const {
    return QUrl("http://planetarynames.wr.usgs.gov/Feature/" + id().ToQt());
  }


  /**
   * @return The feature's online URL as a HTML string (it is hyperlinked).
   */
  iString FeatureNomenclature::Feature::referenceUrlString() const {
    return "<a href='" + referenceUrl().toString() + "'>" +
             referenceUrl().toString() +
           "</a>";
  }


  /**
   * Swap the member data of this feature with another feature.
   *
   * @param other The feature to swap with.
   */
  void FeatureNomenclature::Feature::swap(Feature &other) {
    std::swap(m_xmlRepresenation, other.m_xmlRepresenation);
  }


  /**
   * Assign the values of this feature from the values of rhs.
   *
   * @param rhs The feature on the right hand side of the '=' operator
   * @return *this
   */
  FeatureNomenclature::Feature &FeatureNomenclature::Feature::operator=(
      const Feature &rhs) {
    Feature copy(rhs);
    swap(copy);
    return *this;
  }


  /**
   * Get the string value of an element of the XML. Returns an empty string if
   *   anything goes wrong.
   *
   * @param tagName the XML tag name, from the nomenclature server, of what you
   *        want
   * @return The data in the tag
   */
  iString FeatureNomenclature::Feature::getTagText(iString tagName) const {
    iString text;

    if (m_xmlRepresenation) {
      QDomNodeList nodes =
          m_xmlRepresenation->elementsByTagName(tagName.ToQt());

      if (nodes.count())
        text = nodes.at(0).toElement().text().trimmed();
    }

    return text;
  }


  /**
   * This is called when a query is done. This adds the features to our feature
   *   list and emits featuresIdentified().
   *
   * @param reply The server's response to our request, containing features in
   *     XML form.
   */
  void FeatureNomenclature::requestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {

      QString errorMsg;
      int errorLine;
      int errorCol;

      QDomDocument xmlResultDocument;
      if (xmlResultDocument.setContent(reply->readAll(),
                                       &errorMsg, &errorLine, &errorCol)) {
        for (QDomNode node = xmlResultDocument.firstChild();
             !node.isNull();
             node = node.nextSibling()) {
          QDomElement element = node.toElement();
          if (element.tagName() == "searchresults") {
            readSearchResults(element);
          }
        }
      }
      else {
      QMessageBox::warning(NULL, "Failed to read nomenclature database result",
                           "An error occurred when parsing the data sent back "
                           "from the nomenclature database. "
                           "The XML result was invalid. The parse is [" +
                           errorMsg + "] on line [" +
                           iString(errorLine).ToQt() +"], column [" +
                           iString(errorCol).ToQt() + "]");
      }
    }
    else {
      QMessageBox::warning(NULL, "Failed to query nomenclature database",
                           "An error occurred when querying the nomenclature "
                           "database for features that intersect the qeuried "
                           "ground range. Please make sure you have an active "
                           "internet connection. The error returned was [" +
                           reply->errorString() + "]");
    }

    reply->deleteLater();

    emit featuresIdentified(this);
  }


  /**
   * This is a helper method for requestFinished. This takes the search results
   *   and creates a Feature for each equivalent XML feature element.
   *
   * @param xmlSearchResults The searchresults XML from the nomenclature server.
   */
  void FeatureNomenclature::readSearchResults(QDomElement xmlSearchResults) {
    ASSERT(xmlSearchResults.tagName() == "searchresults");

    if (!m_features)
      m_features = new QList<Feature>;

    for (QDomNode node = xmlSearchResults.firstChild();
         !node.isNull();
         node = node.nextSibling()) {
      QDomElement element = node.toElement();

      if (element.tagName() == "feature") {

        m_features->append(Feature(element));
      }
    }
  }
}