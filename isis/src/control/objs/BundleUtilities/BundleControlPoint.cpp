#include "BundleControlPoint.h"

#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Latitude.h"
#include "Longitude.h"

namespace Isis {

  /**
   * constructor
   */
  BundleControlPoint::BundleControlPoint(ControlPoint *controlPoint) {
    m_controlPoint = controlPoint;

    // setup vector of BundleMeasures for this control point
    int nMeasures = controlPoint->GetNumMeasures();
    for (int i = 0; i < nMeasures; i++) {
      ControlMeasure* controlMeasure = controlPoint->GetMeasure(i);
      if (controlMeasure->IsIgnored())
        continue;

      addMeasure(controlMeasure);      
    }

    // initialize to 0.0
    m_corrections.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();
    m_weights.clear();
    m_nicVector.clear();
  }


  /**
   * copy constructor
   */
  BundleControlPoint::BundleControlPoint(const BundleControlPoint& src) {
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
  void BundleControlPoint::copy(const BundleControlPoint& src) {

    // sanity check
    qDeleteAll(*this);
    clear();

    m_controlPoint = src.m_controlPoint;

    int nMeasures = src.size();

    for(int i = 0; i < nMeasures; i++)
      append(new BundleMeasure(*(src.at(i))));

    m_corrections = src.m_corrections;
    m_aprioriSigmas = src.m_aprioriSigmas;
    m_adjustedSigmas = src.m_adjustedSigmas;
    m_weights = src.m_weights;
    m_nicVector = m_nicVector;
  }


  BundleMeasure *BundleControlPoint::addMeasure(ControlMeasure *controlMeasure) {

    BundleMeasure *bundleMeasure = new BundleMeasure(controlMeasure, this);

    append(bundleMeasure);

    return bundleMeasure;
  }


  bool BundleControlPoint::isRejected() {
    return m_controlPoint->IsRejected();
  }


  int BundleControlPoint::numberMeasures() {
    return m_controlPoint->GetNumMeasures();
  }


  SurfacePoint BundleControlPoint::getAdjustedSurfacePoint() const {
    return m_controlPoint->GetAdjustedSurfacePoint();
  }


  void BundleControlPoint::setAdjustedSurfacePoint(SurfacePoint surfacePoint) {
    m_controlPoint->SetAdjustedSurfacePoint(surfacePoint);
  }


  QString BundleControlPoint::getId() const {
    return m_controlPoint->GetId();
  }

  void BundleControlPoint::setWeights(const BundleSettings *settings, double metersToRadians) {

    double d;

    double globalLatitudeAprioriSigma = settings->globalLatitudeAprioriSigma();
    double globalLongitudeAprioriSigma =settings->globalLongitudeAprioriSigma();
    double globalRadiusAprioriSigma = settings->globalRadiusAprioriSigma();

    if (m_controlPoint->GetType() == ControlPoint::Fixed) {
      m_weights[0] = 1.0e+50;
      m_weights[1] = 1.0e+50;
      m_weights[2] = 1.0e+50;
    }
    else if (m_controlPoint->GetType() == ControlPoint::Free) {
      if ( globalLatitudeAprioriSigma > 0.0 ) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma*metersToRadians;
        m_weights[0] = 1.0/(d*d);
      }
      if ( globalLongitudeAprioriSigma > 0.0 ) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma*metersToRadians;
        m_weights[1] = 1.0/(d*d);
      }
      if (!settings->solveRadius())
        m_weights[2] = 1.0e+50;
      else {
        if ( globalRadiusAprioriSigma > 0.0 ) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma*0.001;
          m_weights[2] = 1.0/(d*d);
        }
      }
    }
    else if (m_controlPoint->GetType() == ControlPoint::Constrained) {
      if ( m_controlPoint->IsLatitudeConstrained() ) {
        m_aprioriSigmas[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatSigmaDistance().meters();
        m_weights[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatWeight();
      }
      else if ( globalLatitudeAprioriSigma > 0.0 ) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma*metersToRadians;
        m_weights[0] = 1.0/(d*d);
      }

      if ( m_controlPoint->IsLongitudeConstrained() ) {
        m_aprioriSigmas[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonSigmaDistance().meters();
        m_weights[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonWeight();
      }
      else if ( globalLongitudeAprioriSigma > 0.0 ) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma*metersToRadians;
        m_weights[1] = 1.0/(d*d);
      }

      if (!settings->solveRadius())
        m_weights[2] = 1.0e+50;
      else {
        if ( m_controlPoint->IsRadiusConstrained() ) {
          m_aprioriSigmas[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusSigma().meters();
          m_weights[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusWeight();
        }
        else if ( globalRadiusAprioriSigma > 0.0 ) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma*0.001;
          m_weights[2] = 1.0/(d*d);
        }
      }
    }
  }

  boost::numeric::ublas::bounded_vector< double, 3 >& BundleControlPoint::corrections() {
    return m_corrections;
  }

  boost::numeric::ublas::bounded_vector< double, 3 >& BundleControlPoint::aprioriSigmas() {
    return m_aprioriSigmas;

  }

  boost::numeric::ublas::bounded_vector< double, 3 >& BundleControlPoint::weights() {
    return m_weights;
  }

  ControlPoint *BundleControlPoint::getRawControlPoint() {
    return m_controlPoint;
  }

  QString BundleControlPoint::formatBundleOutputSummaryString(bool errorPropagation) {

    int nRays           = m_controlPoint->GetNumMeasures();
    int nGoodRays       = nRays - m_controlPoint->GetNumberOfRejectedMeasures();
    double dResidualRms = m_controlPoint->GetResidualRms();
    double dLat         = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double dLon         = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double dRadius      = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().meters();

    QString typeString = m_controlPoint->GetPointTypeString().toUpper();

    QString qStr = QString("%1%2%3 of %4%5%6%7%8%9%10%11\n").
        arg(m_controlPoint->GetId(), 16).
        arg(typeString, 15).
        arg(nGoodRays, 5).
        arg(nRays).
        arg(dResidualRms, 6, 'f', 2).
        arg(dLat, 16, 'f', 8).
        arg(dLon, 16, 'f', 8).
        arg(dRadius * 0.001, 16, 'f', 8).
        arg(formatLatitudeAdjustedSigmaString(16,8,errorPropagation)).
        arg(formatLongitudeAdjustedSigmaString(16,8,errorPropagation)).
        arg(formatRadiusAdjustedSigmaString(16,8,errorPropagation));

    return qStr;
  }


  QString BundleControlPoint::formatBundleOutputDetailString(bool errorPropagation,
                                                             double RTM) {

    int nRays           = m_controlPoint->GetNumMeasures();
    int nGoodRays       = nRays - m_controlPoint->GetNumberOfRejectedMeasures();
    double dLat         = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double dLon         = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double dRadius      = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().meters();

    // point corrections and initial sigmas
    double cor_lat_dd = m_corrections(0) * RAD2DEG;                // lat correction, decimal degs
    double cor_lon_dd = m_corrections(1) * RAD2DEG;                // lon correction, decimal degs
    double cor_rad_m  = m_corrections(2) * 1000.0;                 // radius correction, meters

    double cor_lat_m = m_corrections(0) * RTM;                     // lat correction, meters
    double cor_lon_m = m_corrections(1) * RTM * cos(dLat*DEG2RAD); // lon correction, meters

    double dLatInit = dLat - cor_lat_dd;
    double dLonInit = dLon - cor_lon_dd;
    double dRadiusInit = dRadius - (m_corrections(2) * 1000.0);

    QString typeString = m_controlPoint->GetPointTypeString().toUpper();

    QString finalqStr;

    finalqStr = QString(" Label: %1\nStatus: %2\n  Rays: %3 of %4\n").arg(m_controlPoint->GetId()).
        arg(typeString).
        arg(nGoodRays).
        arg(nRays);

    QString qStr = "\n     Point         Initial               Total               Total              Final             Initial             Final\n"
            "Coordinate          Value             Correction          Correction            Value             Accuracy          Accuracy\n"
            "                 (dd/dd/km)           (dd/dd/km)           (Meters)           (dd/dd/km)          (Meters)          (Meters)\n";
    finalqStr += qStr;

    finalqStr += QString("  LATITUDE%1%2%3%4%5%6\n").arg(dLatInit, 17, 'f', 8).
        arg(cor_lat_dd, 21, 'f', 8).
        arg(cor_lat_m, 20, 'f', 8).
        arg(dLat, 20, 'f', 8).
        arg(formatLatitudeAprioriSigmaString(18,8)).
        arg(formatLatitudeAdjustedSigmaString(18,8,errorPropagation));

    finalqStr += QString(" LONGITUDE%1%2%3%4%5%6\n").arg(dLonInit, 17, 'f', 8).
        arg(cor_lon_dd, 21, 'f', 8).
        arg(cor_lon_m, 20, 'f', 8).
        arg(dLon, 20, 'f', 8).
        arg(formatLongitudeAprioriSigmaString(18,8)).
        arg(formatLongitudeAdjustedSigmaString(18,8,errorPropagation));

    finalqStr += QString("    RADIUS%1%2%3%4%5%6\n\n").arg(dRadiusInit * 0.001, 17, 'f', 8).
        arg(m_corrections(2), 21, 'f', 8).
        arg(cor_rad_m, 20, 'f', 8).
        arg(dRadius * 0.001, 20, 'f', 8).
        arg(formatRadiusAprioriSigmaString(18,8)).
        arg(formatRadiusAdjustedSigmaString(18,8,errorPropagation));

    return finalqStr;
  }


  QString BundleControlPoint::formatLatitudeAprioriSigmaString(int fieldWidth, int precision) {
    QString qLatAprioriSigmaStr;
    if (m_aprioriSigmas(0) == 0.0)
      qLatAprioriSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else
      qLatAprioriSigmaStr = QString("%1").arg(m_aprioriSigmas(0), fieldWidth, 'f', precision);

    return qLatAprioriSigmaStr;
  }


  QString BundleControlPoint::formatLongitudeAprioriSigmaString(int fieldWidth, int precision) {
    QString qLonAprioriSigmaStr;
    if (m_aprioriSigmas(1) == 0.0)
      qLonAprioriSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else
      qLonAprioriSigmaStr = QString("%1").arg(m_aprioriSigmas(1), fieldWidth, 'f', precision);

    return qLonAprioriSigmaStr;
  }


  QString BundleControlPoint::formatRadiusAprioriSigmaString(int fieldWidth, int precision) {
    QString qRadAprioriSigmaStr;
    if (m_aprioriSigmas(2) == 0.0)
      qRadAprioriSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else
      qRadAprioriSigmaStr = QString("%1").arg(m_aprioriSigmas(2), fieldWidth, 'f', precision);

    return qRadAprioriSigmaStr;
  }


  QString BundleControlPoint::formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                bool errorPropagation) {
    QString qLatAdjustedSigmaStr;

    if (!errorPropagation)
      qLatAdjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else {
      double dSigmaLat = m_controlPoint->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters();
      qLatAdjustedSigmaStr = QString("%1").arg(dSigmaLat, fieldWidth, 'f', precision);
    }

    return qLatAdjustedSigmaStr;
  }


  QString BundleControlPoint::formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                 bool errorPropagation) {
    QString qLonAdjustedSigmaStr;

    if (!errorPropagation)
      qLonAdjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else {
      double dSigmaLon = m_controlPoint->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters();
      qLonAdjustedSigmaStr = QString("%1").arg(dSigmaLon, fieldWidth, 'f', precision);
    }

    return qLonAdjustedSigmaStr;
  }


  // TODO: what do we do if we're not solving for radius, how do we know that here?????????????????
  // TODO: dSigmaRad is not == 0.0, if not solving for radius it's like something crazy e-22
  QString BundleControlPoint::formatRadiusAdjustedSigmaString(int fieldWidth, int precision,
                                                              bool errorPropagation) {
    QString qRadAdjustedSigmaStr;
    double dSigmaRad = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters();

    if (!errorPropagation)
      qRadAdjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
    else {
      qRadAdjustedSigmaStr = QString("%1").arg(dSigmaRad, fieldWidth, 'f', precision);
    }

    return qRadAdjustedSigmaStr;
  }
}
