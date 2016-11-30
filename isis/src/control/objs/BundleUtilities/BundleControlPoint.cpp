#include "BundleControlPoint.h"

#include <QDebug>

#include "ControlMeasure.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

namespace Isis {


  /**
   * Constructs a BundleControlPoint object from a ControlPoint. Only the 
   * non-ignored measures are added to the BundleControlPoint. 
   *  
   * @param controlPoint Pointer to a ControlPoint that will be used to 
   *                     construct this BundleControlPoint.
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

    // initialize to 0.0
    m_corrections.clear();
    m_weights.clear();
    m_nicVector.clear();

    // initialize to Null for consistency with other bundle classes...
    m_aprioriSigmas.clear();
    m_aprioriSigmas[0] = Isis::Null;
    m_aprioriSigmas[1] = Isis::Null;
    m_aprioriSigmas[2] = Isis::Null;
    m_adjustedSigmas.clear();
    m_adjustedSigmas[0] = Isis::Null;
    m_adjustedSigmas[1] = Isis::Null;
    m_adjustedSigmas[2] = Isis::Null;
  }


  /**
   * Copy constructor. Constructs a BundleControlPoint object from an existing 
   * BundleControlPoint. 
   *  
   * @param src The BundleControlPoint to be copied.
   */
  BundleControlPoint::BundleControlPoint(const BundleControlPoint &src) {
    copy(src);
  }


  /**
   * Destructor for BundleControlPoint. 
   */
  BundleControlPoint::~BundleControlPoint() {
  }


  /**
   * Copies given BundleControlPoint to this BundleControlPoint. 
   *  
   * @param src The BundleControlPoint to be copied.
   */
  void BundleControlPoint::copy(const BundleControlPoint &src) {

    // sanity check
    clear();

    m_controlPoint = src.m_controlPoint;

    int numMeasures = src.size();

    for (int i = 0; i < numMeasures; i++)
      append(BundleMeasureQsp( new BundleMeasure(*(src.at(i))) ));

    m_corrections = src.m_corrections;
    m_aprioriSigmas = src.m_aprioriSigmas;
    m_adjustedSigmas = src.m_adjustedSigmas;
    m_weights = src.m_weights;
    m_nicVector = src.m_nicVector;
  }


  /**
   * Creates a BundleMeasure from the given ControlMeasure and appends it to 
   * this BundleControlPoint's measure list. 
   *  
   * @param controlMeasure The ControlMeasure to be converted.
   * 
   * @return @b BundleMeasure* A pointer to the new BundleMeasure.
   */
  BundleMeasureQsp BundleControlPoint::addMeasure(ControlMeasure *controlMeasure) {

    BundleMeasureQsp bundleMeasure = BundleMeasureQsp( new BundleMeasure(controlMeasure, this) );

    append(bundleMeasure);

    return bundleMeasure;
  }


  /**
   * Computes the residuals for this BundleControlPoint.
   *
   * @see ControlPoint::ComputeResiduals()
   */
  void BundleControlPoint::computeResiduals() {
    m_controlPoint->ComputeResiduals();
  }


  /**
   * Sets the adjusted surface point for this BundleControlPoint. 
   *  
   * @param surfacePoint The surface point to be set.
   */
  void BundleControlPoint::setAdjustedSurfacePoint(SurfacePoint surfacePoint) {
    m_controlPoint->SetAdjustedSurfacePoint(surfacePoint);
  }


  /**
   * Sets the number of rejected measures for this BundleControlPoint.
   *
   * @param numRejected Number of rejected measures.
   *
   * @see ControlPoint::SetNumberOfRejectedMeasures(int numRejected)
   */
  void BundleControlPoint::setNumberOfRejectedMeasures(int numRejected) {
    m_controlPoint->SetNumberOfRejectedMeasures(numRejected);
  }


  /**
   * Sets this BundleControlPoint to rejected or not rejected.
   *
   * @param reject True will set the BundleControlPoint to rejected.
   *
   * @see ControlPoint::SetRejected(bool reject)
   */
  void BundleControlPoint::setRejected(bool reject) {
    m_controlPoint->SetRejected(reject);
  }


  /**
   * Sets the weights using the given BundleSettings QSharedPointer and a 
   * conversion value for meters to radians. 
   *  
   * @param settings A QSharedPointer to BundleSettings object.
   * @param metersToRadians A double precision conversion factor.
   */
  void BundleControlPoint::setWeights(const BundleSettingsQsp settings, double metersToRadians) {

    double d;

    double globalLatitudeAprioriSigma = settings->globalLatitudeAprioriSigma();
    double globalLongitudeAprioriSigma = settings->globalLongitudeAprioriSigma();
    double globalRadiusAprioriSigma = settings->globalRadiusAprioriSigma();

    if (m_controlPoint->GetType() == ControlPoint::Fixed) {
      m_weights[0] = 1.0e+50;
      m_weights[1] = 1.0e+50;
      m_weights[2] = 1.0e+50;
      // m_aprioriSigmas = Isis::Null by default
    }
    if (m_controlPoint->GetType() == ControlPoint::Free) {
      if (!IsSpecial(globalLatitudeAprioriSigma)) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma*metersToRadians;
        m_weights[0] = 1.0/(d*d);
      } // else m_aprioriSigma = Isis::Null
        // m_weights = 0.0
      if (!IsSpecial(globalLongitudeAprioriSigma)) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma*metersToRadians;
        m_weights[1] = 1.0/(d*d);
      } // else m_aprioriSigma = Isis::Null
        // m_weights = 0.0
      if (!settings->solveRadius()) {
        m_weights[2] = 1.0e+50;
      }
      else {
        if (!IsSpecial(globalRadiusAprioriSigma)) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma*0.001;
          m_weights[2] = 1.0/(d*d);
        }
      }
    }
    if (m_controlPoint->GetType() == ControlPoint::Constrained) {
      if ( m_controlPoint->IsLatitudeConstrained() ) {
        m_aprioriSigmas[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatSigmaDistance().meters();
        m_weights[0] = m_controlPoint->GetAprioriSurfacePoint().GetLatWeight();
      }
      else if (!IsSpecial(globalLatitudeAprioriSigma)) {
        m_aprioriSigmas[0] = globalLatitudeAprioriSigma;
        d = globalLatitudeAprioriSigma*metersToRadians;
        m_weights[0] = 1.0/(d*d);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0

      if ( m_controlPoint->IsLongitudeConstrained() ) {
        m_aprioriSigmas[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonSigmaDistance().meters();
        m_weights[1] = m_controlPoint->GetAprioriSurfacePoint().GetLonWeight();
      }
      else if (!IsSpecial(globalLongitudeAprioriSigma)) {
        m_aprioriSigmas[1] = globalLongitudeAprioriSigma;
        d = globalLongitudeAprioriSigma*metersToRadians;
        m_weights[1] = 1.0/(d*d);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0

      if (!settings->solveRadius()) {
        m_weights[2] = 1.0e+50;
      }
      else {
        if ( m_controlPoint->IsRadiusConstrained() ) {
          m_aprioriSigmas[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusSigma().meters();
          m_weights[2] = m_controlPoint->GetAprioriSurfacePoint().GetLocalRadiusWeight();
        }
        else if (!IsSpecial(globalRadiusAprioriSigma)) {
          m_aprioriSigmas[2] = globalRadiusAprioriSigma;
          d = globalRadiusAprioriSigma*0.001;
          m_weights[2] = 1.0/(d*d);
        } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
          // m_weights = 0.0
      }
    }
  }


  /**
   * Resets the number of rejected measures for this BundleControlPoint to zero.
   *
   * @see ControlPoint::ZeroNumberOfRejectedMeasures()
   */
   void BundleControlPoint::zeroNumberOfRejectedMeasures() {
     m_controlPoint->ZeroNumberOfRejectedMeasures();
   }


  /**
   * Accessor for the raw ControlPoint object used for this BundleControlPoint.
   * 
   * @return @b ControlPoint* A pointer to the raw ControlPoint.
   */
  ControlPoint *BundleControlPoint::rawControlPoint() const {
    return m_controlPoint;
  }


  /**
   * Method used to determine whether this control point is rejected.
   * 
   * @return @b bool Indicates whether this control point is rejected.
   */
  bool BundleControlPoint::isRejected() const {
    return m_controlPoint->IsRejected();
  }


  /**
   * Accesses number of measures associated with this BundleControlPoint.
   * 
   * @return @b int The number of measures for this point.
   */
  int BundleControlPoint::numberOfMeasures() const {
    return this->size();
  }


  /**
   * Accesses the number of rejected measures for this BundleControlPoint.
   *
   * @return @b int Returns the number of rejected measures.
   *
   * @see ControlPoint::GetNumberOfRejectedMeasures()
   */
  int BundleControlPoint::numberOfRejectedMeasures() const {
    return m_controlPoint->GetNumberOfRejectedMeasures();
  }


  /**
   * Gets the root-mean-square (rms) of the BundleControlPoint's residuals.
   *
   * @return @b double Returns the rms of the residuals.
   *
   * @see ControlPoint::GetResidualRms()
   */
  double BundleControlPoint::residualRms() const {
    return m_controlPoint->GetResidualRms();
  }


  /**
   * Accesses the adjusted SurfacePoint associated with this BundleControlPoint.
   * 
   * @return @b SurfacePoint The adjusted surface point.
   */
  SurfacePoint BundleControlPoint::adjustedSurfacePoint() const {
    return m_controlPoint->GetAdjustedSurfacePoint();
  }


  /**
   * Accesses the Point ID associated with this BundleControlPoint. 
   * 
   * @return @b QString The ID for this point.
   */
  QString BundleControlPoint::id() const {
    return m_controlPoint->GetId();
  }


  /**
   * Accesses BundleControlPoint's type.
   * 
   * @return @b ControlPoint::PointType The BundleControlPoint's type.  Options are:
   *                                    Fixed = 0, Constrained = 1, Free = 2.
   * 
   * @see ControlPoint::GetType()
   */
  ControlPoint::PointType BundleControlPoint::type() const{
    return m_controlPoint->GetType();
  }


  // ??? why bounded vector ??? can we use linear algebra vector ??? 
  /**
   * Accesses the 3 dimensional ordered vector of correction values associated 
   * with latitude, longitude, and radius. 
   * 
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of correction values.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::corrections() {
    return m_corrections;
  }


  /**
   * Accesses the 3 dimenstional ordered vector of apriori sigmas (apriori 
   * latitude, apriori longitude, apriori radius). 
   * 
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of apriori sigmas.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::aprioriSigmas() {
    return m_aprioriSigmas;

  }


  /**
   * Accesses the 3 dimenstional ordered vector of adjusted sigmas (adjusted 
   * latitude, adjusted longitude, adjusted radius). 
   * 
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of adjusted sigmas.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Accesses the 3 dimensional ordered vector of weight values associated 
   * with latitude, longitude, and radius. 
   * 
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of weight values.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::weights() {
    return m_weights;
  }


  /**
   * Accesses the 3 dimensional ordered NIC vector. 
   * 
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The NIC vector.
   */
  boost::numeric::ublas::bounded_vector<double, 3> &BundleControlPoint::nicVector() {
    return m_nicVector;
  }


  /**
   * Accesses the CholMod matrix associated with this BundleControlPoint.
   * 
   * @return @b SparseBlockRowMatrix& The CholMod row matrix.
   */
  SparseBlockRowMatrix &BundleControlPoint::cholmodQMatrix() {
    return m_cholmodQMatrix;
  }


  /**
   * Formats an output summary string for this BundleControlPoint. This string 
   * includes ID, point type, number of rays from non-rejected measures, 
   * residual RMS, adjusted latitude and longitude (in degrees), adjusted radius
   * (in km), and the adjusted sigmas (for latitude, longitude and radius). 
   *  
   * @param errorPropagation Indicates whether error propagation was selected.
   * 
   * @return @b QString The formatted output summary string.
   */
  QString BundleControlPoint::formatBundleOutputSummaryString(bool errorPropagation) const {

    int numRays        = numberOfMeasures(); // should this depend on the raw point, as written, or this->size()???
    int numGoodRays    = numRays - numberOfRejectedMeasures();
    double ResidualRms = residualRms();
    double lat         = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double lon         = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double rad         = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();

    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();

    QString output = QString("%1%2%3 of %4%5%6%7%8%9%10%11\n")
                             .arg(id(), 16)
                             .arg(pointType, 15)
                             .arg(numGoodRays, 5)
                             .arg(numRays)
                             .arg(formatValue(ResidualRms, 6, 2))
                             .arg(formatValue(lat, 16, 8)) // deg
                             .arg(formatValue(lon, 16, 8)) // deg
                             .arg(formatValue(rad, 16, 8)) // km
                             .arg(formatLatitudeAdjustedSigmaString(16,8,errorPropagation))  // m
                             .arg(formatLongitudeAdjustedSigmaString(16,8,errorPropagation)) // m
                             .arg(formatRadiusAdjustedSigmaString(16,8,errorPropagation));   // m

    return output;
  }


  /**
   * Formats a detailed output string table for this BundleControlPoint. 
   *  
   * @param errorPropagation Indicates whether error propagation was selected.
   * @param RTM Conversion factor from radians to meters. Used to convert the 
   *            latitude and longitude corrections to meters.
   * 
   * @return @b QString The formatted output detailed string.
   */
  QString BundleControlPoint::formatBundleOutputDetailString(bool errorPropagation,
                                                             double RTM,
                                                             bool solveRadius) const {

    int numRays     = numberOfMeasures();
    int numGoodRays = numRays - numberOfRejectedMeasures();
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

    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();

    QString output;

    output = QString(" Label: %1\nStatus: %2\n  Rays: %3 of %4\n")
                        .arg(id())
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
                      .arg(formatRadiusAprioriSigmaString(18,8,solveRadius))        // m
                      .arg(formatRadiusAdjustedSigmaString(18,8,errorPropagation)); // m

    return output;
  }


  /**
   * Formats the given double precision value using the specified field width 
   * and precision. If the given value is special, then "Null" is returned. 
   *  
   * @param value The double value to be formattted.
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the given double value to be saved off.
   * 
   * @return @b QString The formatted value, as a string.
   */
  QString BundleControlPoint::formatValue(double value, int fieldWidth, int precision) const {
    QString output;
    IsSpecial(value) ? 
      output = QString("%1").arg("Null", fieldWidth) :
      output = QString("%1").arg(value, fieldWidth, 'f', precision);
    return output;
  }


  /**
   * Formats the apriori sigma value indicated by the given type code. If no 
   * sigma was set, then the string "N/A" will be returned. 
   *  
   * @param type Integer code that indicates which apriori sigma value will be 
   *             formatted. Latitude=0, Longitude=1, Radius=2.
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * 
   * @return @b QString The formatted value, as a string.
   */
  QString BundleControlPoint::formatAprioriSigmaString(int version, int fieldWidth,
                                                       int precision, bool solveRadius) const {
    QString aprioriSigmaStr;
    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();
    if (pointType == "CONSTRAINED"||!solveRadius) {
        pointType = "N/A";
    }
    double sigma = m_aprioriSigmas[version];
    if (IsSpecial(sigma)) { // if globalAprioriSigma <= 0 (including Isis::NUll), then m_aprioriSigmas = Null
      aprioriSigmaStr = QString("%1").arg(pointType, fieldWidth);
    }
    else {
      aprioriSigmaStr = QString("%1").arg(sigma, fieldWidth, 'f', precision);
    }
    return aprioriSigmaStr;
  }


  /**
   * Formats the apriori latitude sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * 
   * @return @b QString The formatted apriori latitude sigma value, as a string.
   *  
   * @see formatAprioriSigmaString() 
   */
  QString BundleControlPoint::formatLatitudeAprioriSigmaString(int fieldWidth, 
                                                               int precision) const {
    return formatAprioriSigmaString(0, fieldWidth, precision, true);
  }


  /**
   * Formats the apriori longitude sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * 
   * @return @b QString The formatted apriori longitude sigma value, as a string.
   *  
   * @see formatAprioriSigmaString() 
   */
  QString BundleControlPoint::formatLongitudeAprioriSigmaString(int fieldWidth, 
                                                                int precision) const {
    return formatAprioriSigmaString(1, fieldWidth, precision, true);
  }


  /**
   * Formats the apriori radius sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * 
   * @return @b QString The formatted apriori radius sigma value, as a string.
   *  
   * @see formatAprioriSigmaString() 
   */
  QString BundleControlPoint::formatRadiusAprioriSigmaString(int fieldWidth,
                                                             int precision,
                                                             bool solveRadius) const {
    return formatAprioriSigmaString(2, fieldWidth, precision, solveRadius);
  }


  /**
   * Formats the adjusted sigma value indicated by the given type code. If error 
   * propagation is false or the selected sigma type was set to Null, then only 
   * "N/A" will be returned. 
   *  
   * @param type Integer code that indicates which apriori sigma value will be 
   *             formatted. Latitude=0, Longitude=1, Radius=2.
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   * 
   * @return @b QString The formatted value, as a string.
   */
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
      if (IsSpecial(sigma)) {
        adjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
      }
      else {
        adjustedSigmaStr = QString("%1").arg(sigma, fieldWidth, 'f', precision);
      }
    }

    return adjustedSigmaStr;
  }


  /**
   * Formats the adjusted latitude sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   * 
   * @return @b QString The formatted adjusted latitude sigma value, as a string.
   *  
   * @see formatAdjustedSigmaString() 
   */
  QString BundleControlPoint::formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                bool errorPropagation) const {
    return formatAdjustedSigmaString(0, fieldWidth, precision, errorPropagation);
  }


  /**
   * Formats the adjusted longitude sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   * 
   * @return @b QString The formatted adjusted longitude sigma value, as a string.
   *  
   * @see formatAdjustedSigmaString() 
   */
  QString BundleControlPoint::formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                                 bool errorPropagation) const {
    return formatAdjustedSigmaString(1, fieldWidth, precision, errorPropagation);
  }


  /**
   * Formats the adjusted radius sigma value. 
   *  
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   * 
   * @return @b QString The formatted adjusted radius sigma value, as a string.
   *  
   * @see formatAdjustedSigmaString() 
   */
  QString BundleControlPoint::formatRadiusAdjustedSigmaString(int fieldWidth, int precision,
                                                              bool errorPropagation) const {
    return formatAdjustedSigmaString(2, fieldWidth, precision, errorPropagation);
  }
}
