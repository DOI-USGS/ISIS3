#include "BundleControlPoint.h"

#include <QDebug>

#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * constructor
   */
  BundleControlPoint::BundleControlPoint(ControlPoint *controlPoint) {
    m_controlPoint = controlPoint;

    // setup vector of BundleMeasures for this control point
    int numMeasures = controlPoint->GetNumMeasures();
    for (int i = 0; i < numMeasures; i++) {
      ControlMeasure *controlMeasure = controlPoint->GetMeasure(i);
      if (controlMeasure->IsIgnored()) {
        continue;
      }

      addMeasure(controlMeasure);      
    }

    // we should initialize these to Null like a priori sigmas? 
    m_corrections.clear();
    m_adjustedSigmas.clear();
    m_weights.clear();
    m_nicVector.clear();
    
    // initialize to Null for consistency with other bundle classes...
    m_aprioriSigmas.clear();
    m_aprioriSigmas[0] = Isis::Null;
    m_aprioriSigmas[1] = Isis::Null;
    m_aprioriSigmas[2] = Isis::Null;
  }


  /**
   * copy constructor
   */
  BundleControlPoint::BundleControlPoint(const BundleControlPoint &src) {
    copy(src);
  }


  /**
   * destructor
   */
  BundleControlPoint::~BundleControlPoint() {
    qDeleteAll(*this);
    clear();
  }


  /**
   * copy method
   */
  void BundleControlPoint::copy(const BundleControlPoint &src) {

    // sanity check
    qDeleteAll(*this);
    clear();

    m_controlPoint = src.m_controlPoint;

    int numMeasures = src.size();

    for(int i = 0; i < numMeasures; i++)
      append(new BundleMeasure(*(src.at(i))));

    m_corrections = src.m_corrections;
    m_aprioriSigmas = src.m_aprioriSigmas;
    m_adjustedSigmas = src.m_adjustedSigmas;
    m_weights = src.m_weights;
    m_nicVector = src.m_nicVector;
  }


  BundleMeasure *BundleControlPoint::addMeasure(ControlMeasure *controlMeasure) {

    BundleMeasure *bundleMeasure = new BundleMeasure(controlMeasure, this);

    append(bundleMeasure);

    return bundleMeasure;
  }


  void BundleControlPoint::setAdjustedSurfacePoint(SurfacePoint surfacePoint) {
    m_controlPoint->SetAdjustedSurfacePoint(surfacePoint);
  }


  void BundleControlPoint::setWeights(const BundleSettings *settings, double metersToRadians) {

    double d;

    double globalLatitudeAprioriSigma  = settings->globalLatitudeAprioriSigma();
    double globalLongitudeAprioriSigma = settings->globalLongitudeAprioriSigma();
    double globalRadiusAprioriSigma    = settings->globalRadiusAprioriSigma();

    if (m_controlPoint->GetType() == ControlPoint::Fixed) {
      m_weights[0] = 1.0e+50;
      m_weights[1] = 1.0e+50;
      m_weights[2] = 1.0e+50;
      // m_aprioriSigmas = Isis::Null by default
    }

    if (m_controlPoint->GetType() == ControlPoint::Free) {

      if (!IsNullPixel(globalLatitudeAprioriSigma)) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma * metersToRadians;
        m_weights[0] = 1.0 / (d * d);
      } // else m_aprioriSigma = Isis::Null
        // m_weights = 0.0
      
      if (!IsNullPixel(globalLongitudeAprioriSigma)) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma * metersToRadians;
        m_weights[1] = 1.0 / (d * d);
      } // else m_aprioriSigma = Isis::Null
        // m_weights = 0.0
      
      if (!settings->solveRadius()) {
        m_weights[2] = 1.0e+50;
      }
      else {
        if (!IsNullPixel(globalRadiusAprioriSigma)) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma * 0.001;
          m_weights[2] = 1.0 / (d * d);
        }
      }
    }

    if (m_controlPoint->GetType() == ControlPoint::Constrained) {
      
      if ( m_controlPoint->IsLatitudeConstrained() ) {
        m_aprioriSigmas[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatSigmaDistance().meters();
        m_weights[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatWeight();
      }
      else if (!IsNullPixel(globalLatitudeAprioriSigma)) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma * metersToRadians;
        m_weights[0] = 1.0 / (d * d);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0
      
      if ( m_controlPoint->IsLongitudeConstrained() ) {
        m_aprioriSigmas[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonSigmaDistance().meters();
        m_weights[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonWeight();
      }
      else if (!IsNullPixel(globalLongitudeAprioriSigma)) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma * metersToRadians;
        m_weights[1] = 1.0 / (d * d);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0
      
      if (!settings->solveRadius()) {
        // m_aprioriSigmas = Isis::Null
        m_weights[2] = 1.0e+50;
      }
      else {
        if ( m_controlPoint->IsRadiusConstrained() ) {
          m_aprioriSigmas[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusSigma().meters();
          m_weights[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusWeight();
        }
        else if (!IsNullPixel(globalRadiusAprioriSigma)) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma * 0.001;
          m_weights[2] = 1.0 / (d * d);
        } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
          // m_weights = 0.0
      }
    }
  }


  ControlPoint *BundleControlPoint::rawControlPoint() const {
    return m_controlPoint;
  }


  bool BundleControlPoint::isRejected() const {
    return m_controlPoint->IsRejected();
  }


  int BundleControlPoint::numberMeasures() const {
    return m_controlPoint->GetNumMeasures();
  }


  SurfacePoint BundleControlPoint::getAdjustedSurfacePoint() const {
    return m_controlPoint->GetAdjustedSurfacePoint();
  }


  QString BundleControlPoint::getId() const {
    return m_controlPoint->GetId();
  }


  // ??? why bounded vector ??? can we use linear algebra vector ??? 
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::corrections() {
    return m_corrections;
  }


  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::aprioriSigmas() {
    return m_aprioriSigmas;

  }


  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::weights() {
    return m_weights;
  }


  boost::numeric::ublas::bounded_vector<double, 3> &BundleControlPoint::nicVector() {
    return m_nicVector;
  }


  SparseBlockRowMatrix &BundleControlPoint::cholmod_QMatrix() {
    return m_cholmod_QMatrix;
  }


  QString BundleControlPoint::formatBundleOutputSummaryString(bool errorPropagation) const {

    int numRays        = numberMeasures(); // should this depend on the raw point, as written, or this->size()???
    int numGoodRays    = numRays - m_controlPoint->GetNumberOfRejectedMeasures();
    double residualRms = m_controlPoint->GetResidualRms();
    double lat         = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double lon         = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double rad         = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();

    QString pointType = m_controlPoint->GetPointTypeString().toUpper();

    QString output = QString("%1%2%3 of %4%5%6%7%8%9%10%11\n")
                             .arg(getId(), 16)
                             .arg(pointType, 15)
                             .arg(numGoodRays, 5)
                             .arg(numRays)
                             .arg(formatValue(residualRms, 6, 2))
                             .arg(formatValue(lat, 16, 8)) // deg
                             .arg(formatValue(lon, 16, 8)) // deg
                             .arg(formatValue(rad, 16, 8)) // km
                             .arg(formatLatitudeAdjustedSigmaString(16,8,errorPropagation))  // m
                             .arg(formatLongitudeAdjustedSigmaString(16,8,errorPropagation)) // m
                             .arg(formatRadiusAdjustedSigmaString(16,8,errorPropagation));   // m

    return output;
  }


  QString BundleControlPoint::formatBundleOutputDetailString(bool errorPropagation,
                                                             double RTM) const {

    int numRays     = numberMeasures();
    int numGoodRays = numRays - m_controlPoint->GetNumberOfRejectedMeasures();
    double lat      = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double lon      = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double rad      = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();

    // ??? corrections is always zero ??? never set in this class ???
    // point corrections and initial sigmas
    double cor_lat_dd = m_corrections(0) * RAD2DEG;                // lat correction, decimal degs
    double cor_lon_dd = m_corrections(1) * RAD2DEG;                // lon correction, decimal degs
    double cor_rad_m  = m_corrections(2) * 1000.0;                 // radius correction, meters


    double cor_lat_m = m_corrections(0) * RTM;                     // lat correction, meters
    double cor_lon_m = m_corrections(1) * RTM * cos(lat*DEG2RAD);  // lon correction, meters

    double latInit = Isis::Null;
    if (!IsSpecial(lat)) {
      latInit = lat - cor_lat_dd;
    }


    double lonInit = Isis::Null;
    if (!IsSpecial(lon)) {
      lonInit = lon - cor_lon_dd;
    }

    double radInit = Isis::Null;
    if (!IsSpecial(rad)) {
      radInit = rad - m_corrections(2); // km
    }

    QString pointType = m_controlPoint->GetPointTypeString().toUpper();

    QString output;

    output = QString(" Label: %1\nStatus: %2\n  Rays: %3 of %4\n")
                        .arg(getId())
                        .arg(pointType)
                        .arg(numGoodRays)
                        .arg(numRays);

    QString labels = "\n     Point         Initial               Total               Total        "
                     "      Final             Initial             Final\n"
                     "Coordinate          Value             Correction          Correction        "
                     "    Value             Accuracy          Accuracy\n"
                     "                 (dd/dd/km)           (dd/dd/km)           (Meters)         "
                     "  (dd/dd/km)          (Meters)          (Meters)\n";
    output += labels;

    output += QString("  LATITUDE%1%2%3%4%5%6\n")
                      .arg(formatValue(latInit, 17, 8))                               // deg
                      .arg(formatValue(cor_lat_dd, 21, 8))                            // deg
                      .arg(formatValue(cor_lat_m, 20, 8))                             // m 
                      .arg(formatValue(lat, 20, 8))                                   // deg
                      .arg(formatLatitudeAprioriSigmaString(18,8))                    // m 
                      .arg(formatLatitudeAdjustedSigmaString(18,8,errorPropagation)); // m 

    output += QString(" LONGITUDE%1%2%3%4%5%6\n")
                      .arg(formatValue(lonInit, 17, 8))                                // deg
                      .arg(formatValue(cor_lon_dd, 21, 8))                             // deg
                      .arg(formatValue(cor_lon_m, 20, 8))                              // m
                      .arg(formatValue(lon, 20, 8))                                    // deg
                      .arg(formatLongitudeAprioriSigmaString(18,8))                    // m
                      .arg(formatLongitudeAdjustedSigmaString(18,8,errorPropagation)); // m

    output += QString("    RADIUS%1%2%3%4%5%6\n\n")
                      .arg(formatValue(radInit, 17, 8))                             // km
                      .arg(formatValue(m_corrections(2), 21, 8))                    // km
                      .arg(formatValue(cor_rad_m, 20, 8))                           // m
                      .arg(formatValue(rad, 20, 8))                                 // km
                      .arg(formatRadiusAprioriSigmaString(18,8))                    // m
                      .arg(formatRadiusAdjustedSigmaString(18,8,errorPropagation)); // m

    return output;
  }


  QString BundleControlPoint::formatValue(double value, int fieldWidth, int precision) const {
    QString output;
    IsNullPixel(value) ? 
      output = QString("%1").arg("Null", fieldWidth) :
      output = QString("%1").arg(value, fieldWidth, 'f', precision);
    return output;
  }


  QString BundleControlPoint::formatAprioriSigmaString(int type, int fieldWidth, 
                                                       int precision) const {
    QString aprioriSigmaStr;
    double sigma = m_aprioriSigmas[type];
    if (IsNullPixel(sigma)) {
//    if (sigma <= 0) { // if globalAprioriSigma <= 0 (including IsNullPixel(sigma)), then m_aprioriSigmas = 0
      aprioriSigmaStr = QString("%1").arg("N/A", fieldWidth);
    }
    else {
      aprioriSigmaStr = QString("%1").arg(sigma, fieldWidth, 'f', precision);
    }
    return aprioriSigmaStr;
  }


  QString BundleControlPoint::formatLatitudeAprioriSigmaString(int fieldWidth, 
                                                               int precision) const {
    return formatAprioriSigmaString(0, fieldWidth, precision);
  }


  QString BundleControlPoint::formatLongitudeAprioriSigmaString(int fieldWidth, 
                                                                int precision) const {
    return formatAprioriSigmaString(1, fieldWidth, precision);
  }


  QString BundleControlPoint::formatRadiusAprioriSigmaString(int fieldWidth, int precision) const {
    return formatAprioriSigmaString(2, fieldWidth, precision);
  }


  QString BundleControlPoint::formatAdjustedSigmaString(int type, int fieldWidth, int precision,
                                                        bool errorPropagation) const {
    QString adjustedSigmaStr;

    if (!errorPropagation) {
      adjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
    }
    else {
      double sigma = Isis::Null;
      if (type == 0) {
        sigma = m_controlPoint->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
      }
      if (type == 1) {
        sigma = m_controlPoint->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
      }
      if (type == 2) {
        sigma = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();
      }
      if (IsNullPixel(sigma)) {
//    if (sigma <= 0) { // if globalAprioriSigma <= 0 (including IsNullPixel(sigma)), then m_aprioriSigmas = 0
        adjustedSigmaStr = QString("%1").arg("N/A", fieldWidth);
      }
      else {
        adjustedSigmaStr = QString("%1").arg(sigma, fieldWidth, 'f', precision);
      }
    }

    return adjustedSigmaStr;
  }


  QString BundleControlPoint::formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                bool errorPropagation) const {
    return formatAdjustedSigmaString(0, fieldWidth, precision, errorPropagation);
  }


  QString BundleControlPoint::formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                 bool errorPropagation) const {
    return formatAdjustedSigmaString(1, fieldWidth, precision, errorPropagation);
  }


  // TODO: what do we do if we're not solving for radius, how do we know that here??????????????
  // TODO: sigma is not == 0.0, if not solving for radius it's like something crazy e-22
  QString BundleControlPoint::formatRadiusAdjustedSigmaString(int fieldWidth, int precision,
                                                              bool errorPropagation) const {
    return formatAdjustedSigmaString(2, fieldWidth, precision, errorPropagation);
  }
}
