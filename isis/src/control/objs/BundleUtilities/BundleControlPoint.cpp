/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleControlPoint.h"

#include <QDebug>

// boost lib
#include <boost/numeric/ublas/vector_proxy.hpp>

// Isis lib
#include "ControlMeasure.h"
#include "Latitude.h"
#include "LinearAlgebra.h"
#include "Longitude.h"
#include "SparseBlockMatrix.h"
#include "SpecialPixel.h"

namespace Isis {


  /**
   * Constructs a BundleControlPoint object from a ControlPoint. Only the
   * non-ignored measures are added to the BundleControlPoint.
   * @param controlPoint Pointer to a ControlPoint that will be used to
   *                     construct this BundleControlPoint.
   */
  BundleControlPoint::BundleControlPoint(const BundleSettingsQsp bundleSettings,
                                         ControlPoint *controlPoint) {
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

    // initialize coordinate type from settings
    // m_coordType = SurfacePoint::Latitudinal;
    m_coordTypeReports = bundleSettings->controlPointCoordTypeReports();
    m_coordTypeBundle = bundleSettings->controlPointCoordTypeBundle();
    setWeights(bundleSettings);
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
    m_coordTypeReports = src.m_coordTypeReports;
    m_coordTypeBundle = src.m_coordTypeBundle;
    // *** TODO *** Why is the cholmodQMatrix not copied? Ask Ken
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

    // compute and store focal plane residuals in millimeters
    for (int i = 0; i < size(); i++) {
      at(i)->setFocalPlaneResidualsMillimeters();
    }
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
  void BundleControlPoint::setWeights(const BundleSettingsQsp settings) {

    double globalPointCoord1AprioriSigma = settings->globalPointCoord1AprioriSigma();
    double globalPointCoord2AprioriSigma = settings->globalPointCoord2AprioriSigma();
    double globalPointCoord3AprioriSigma = settings->globalPointCoord3AprioriSigma();

    if (m_controlPoint->GetType() == ControlPoint::Fixed) {
      m_weights[0] = 1.0e+50;
      m_weights[1] = 1.0e+50;
      m_weights[2] = 1.0e+50;
      // m_aprioriSigmas = Isis::Null by default
    }

    if (m_controlPoint->GetType() == ControlPoint::Free) {
      // Global sigmas are temporary and should not be stored in the control net.  Currently
      // global sigmas are always in meters.  Make sure unit of weights is 1/(var unit squared),
      // where var is a value being solved for in the adjustment.  Latitude and longitude are in
      // radians and everything else is in km.
      if (!IsSpecial(globalPointCoord1AprioriSigma)) {
        setSigmaWeightFromGlobals(globalPointCoord1AprioriSigma, 0);
      } // else m_aprioriSigma = Isis::Null
        // m_weights = 0.0
      if (!IsSpecial(globalPointCoord2AprioriSigma)) {
        m_aprioriSigmas[1] = globalPointCoord2AprioriSigma;
        setSigmaWeightFromGlobals(globalPointCoord2AprioriSigma, 1);
      }// else m_aprioriSigma = Isis::Null
       // m_weights = 0.0
      if (m_coordTypeBundle == SurfacePoint::Latitudinal && !settings->solveRadius()) {
        m_weights[2] = 1.0e+50;
      }
      else {
        if (!IsSpecial(globalPointCoord3AprioriSigma)) {
          setSigmaWeightFromGlobals(globalPointCoord3AprioriSigma, 2);
        }
      }
    }
    if (m_controlPoint->GetType() == ControlPoint::Constrained) {
      // *** Be careful...Is m_aprioriSigmas an output (for reports) or bundle variable? ***

      // Assuming the member variable sigmas are for output reports (internal use only) so use
      //   the report coordinate type to calculate.  All point sigmas are in meters.  Radius weights
      //   are in 1/km^2.  Make x/y/z weights the same because BundleAdjust works in km.
      //   Weights and corrections go into the bundle so use bundle coordinate type.
      if ( m_controlPoint->IsCoord1Constrained() ) {
        m_aprioriSigmas[0] =
          m_controlPoint->GetAprioriSurfacePoint().GetSigmaDistance
                      (m_coordTypeReports, SurfacePoint::One).meters();
        m_weights[0] =
             m_controlPoint->GetAprioriSurfacePoint().GetWeight(m_coordTypeBundle, SurfacePoint::One);
      }
      else if (!IsSpecial(globalPointCoord1AprioriSigma)) {
        setSigmaWeightFromGlobals(globalPointCoord1AprioriSigma, 0);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0

      if ( m_controlPoint->IsCoord2Constrained() ) {
        m_aprioriSigmas[1] =
          m_controlPoint->GetAprioriSurfacePoint().GetSigmaDistance
            (m_coordTypeReports, SurfacePoint::Two).meters();
        m_weights[1] =
          m_controlPoint->GetAprioriSurfacePoint().GetWeight(m_coordTypeBundle, SurfacePoint::Two);
      }
      else if (!IsSpecial(globalPointCoord2AprioriSigma)) {
        setSigmaWeightFromGlobals(globalPointCoord2AprioriSigma, 1);
      } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
        // m_weights = 0.0

      if (m_coordTypeBundle == SurfacePoint::Latitudinal && !settings->solveRadius()) {
        m_weights[2] = 1.0e+50;
      }
      else {
        if ( m_controlPoint->IsCoord3Constrained() ) {
          m_aprioriSigmas[2] =
            m_controlPoint->GetAprioriSurfacePoint().GetSigmaDistance
            (m_coordTypeReports, SurfacePoint::Three).meters();
          m_weights[2] = m_controlPoint->GetAprioriSurfacePoint().GetWeight
            (m_coordTypeBundle, SurfacePoint::Three);
        }
        else if (!IsSpecial(globalPointCoord3AprioriSigma)) {
          setSigmaWeightFromGlobals(globalPointCoord3AprioriSigma, 2);
        } // else not constrained and global sigma is Null, then  m_aprioriSigmas = Isis::Null
          // m_weights = 0.0
      }
    }
  }


  /**
   * Sets the member sigmas and weights from a global sigma..
   *
// *** TODO *** Should index be SurfacePoint::CoordIndex?
   * @param gSigma The global sigma to assign.
   * @param index The index of the point coordinate being set (0, 1, or 2).
   * @param cFactor The conversion factor applied to gSigma to match bundle variable units.
   *
    * @see formatAprioriSigmaString()
   */
  void BundleControlPoint::setSigmaWeightFromGlobals(double gSigma, int index) {
    m_aprioriSigmas[index] = gSigma;

    switch (index) {
      case 0:
        if (m_coordTypeBundle == SurfacePoint::Latitudinal) {
          m_aprioriSigmas[index] = gSigma;
          double sigmaRadians =
            m_controlPoint->GetAprioriSurfacePoint().MetersToLatitude(gSigma);  // m to radians
          if (sigmaRadians > DBL_EPSILON) {
            m_weights[index] = 1. / (sigmaRadians*sigmaRadians);  // 1/radians^2
          }
        }
        else if (m_coordTypeBundle == SurfacePoint::Rectangular) {
          double sigmakm = gSigma * .001;  // km
          if (sigmakm > DBL_EPSILON) {
            m_weights[0] = 1./ (sigmakm*sigmakm); // 1/km^2
          }
        }
        else {
          IString msg ="Unknown surface point coordinate type enum ["
            + toString(m_coordTypeBundle) + "]." ;
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
        break;
      case 1:
        // if (!IsSpecial(globalPointCoord2AprioriSigma)) {
        if (m_coordTypeBundle == SurfacePoint::Latitudinal) {
          double sigmaRadians =
            m_controlPoint->GetAprioriSurfacePoint().MetersToLongitude(gSigma);  // m to radians
          if (sigmaRadians > DBL_EPSILON) {
            m_weights[1] = 1. / (sigmaRadians*sigmaRadians);  // 1/radians^2
          }
        }
        else if (m_coordTypeBundle == SurfacePoint::Rectangular) {
          double sigmakm = gSigma * 0.001;  // km
          if (sigmakm > DBL_EPSILON) m_weights[1] = 1./ (sigmakm*sigmakm); // 1/km^2
        }
        else {
          IString msg ="Unknown surface point coordinate type enum ["
            + toString(m_coordTypeBundle) + "]." ;
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
        break;

      case 2:
      // Coordinate 2 is either latitudinal radius or rectangular z, both in m
      {
        double sigmakm = gSigma * .001;  // km
        m_weights[2] = 1./ (sigmakm*sigmakm); // 1/km^2
        }
        break;

      default:
          IString msg ="Unknown coordinate index [" + toString(index) + "]." ;
          throw IException(IException::Programmer, msg, _FILEINFO_);
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
   * Perform the matrix multiplication v2 = alpha ( Q x v1 ).
   *
   * @param alpha A constant multiplier.
   * @param sparseNormals The sparse block normal equations matrix.
   * @param v2 The output vector.
   * @param Q A sparse block matrix.
   * @param v1 A vector.
   */
  void BundleControlPoint::productAlphaAV(double alpha,
                                    SparseBlockMatrix &sparseNormals,
                                    LinearAlgebra::Vector &v1) {

    QMapIterator< int, LinearAlgebra::Matrix * > Qit(m_cholmodQMatrix);

    int subrangeStart, subrangeEnd;

    while ( Qit.hasNext() ) {
      Qit.next();

      int columnIndex = Qit.key();

      subrangeStart = sparseNormals.at(columnIndex)->startColumn();
      subrangeEnd = subrangeStart + Qit.value()->size2();

      m_nicVector += alpha * prod(*(Qit.value()),subrange(v1,subrangeStart,subrangeEnd));
    }
  }


  /**
   * Apply the parameter corrections to the bundle control point.
   *
   * @param imageSolution Image vector.
   * @param factor The unit conversion factor to use on lat and lon rad or x/y/z km.
   * @param target The BundleTargetBody.
   */
  void BundleControlPoint::applyParameterCorrections(LinearAlgebra::Vector imageSolution,
                                               SparseBlockMatrix &sparseNormals,
                                               const BundleTargetBodyQsp target) {
    if (!isRejected()) {

      // subtract product of Q and nj from NIC
      productAlphaAV(-1.0, sparseNormals, imageSolution);

      // update adjusted surface point appropriately for the coordinate type
      switch (m_coordTypeBundle) {
        case SurfacePoint::Latitudinal:
          updateAdjustedSurfacePointLatitudinally(target);
          break;
        case SurfacePoint::Rectangular:
          updateAdjustedSurfacePointRectangularly();
          break;
        default:
            IString msg ="Unknown surface point coordinate type enum ["
            + toString(m_coordTypeBundle) + "]." ;
            throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // sum and save corrections
      m_corrections(0) += m_nicVector[0];
      m_corrections(1) += m_nicVector[1];
      m_corrections(2) += m_nicVector[2];
    }
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


  /**
   * Accesses BundleControlPoint's coordinate type for reports.
   *
   * @return @b ControlPoint::CoordinateType The BundleControlPoint's coordinate type.
   *                                    See SurfacePoint for the options.
   *
   * @see ControlPoint::getCoordType()
   */
  SurfacePoint::CoordinateType BundleControlPoint::coordTypeReports() const{
    return m_coordTypeReports;
  }


  /**
   * Accesses BundleControlPoint's control point coordinate type for the bundle adjustment.
   *
   * @return @b ControlPoint::CoordinateType The BundleControlPoint's coordinate type.
   *                                    See SurfacePoint for the options.
   *
   * @see ControlPoint::getCoordTypeBundle()
   */
  SurfacePoint::CoordinateType BundleControlPoint::coordTypeBundle() const{
    return m_coordTypeBundle;
  }


// *** TODO *** Need to add bounded vectors to Linear Algebra class
  /**
   * Accesses the 3 dimensional ordered vector of correction values associated
   * with coord1, coord2, and coord 3 (latitude, longitude, and radius or X, Y, and Z.
   * A bounded vector is recommended because the size will always be 3.
   *
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of correction values.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::corrections() {
    return m_corrections;
  }


  /**
   * Accesses the 3 dimensional ordered vector of apriori sigmas (apriori
   * coordinate1, apriori coordinate2, apriori coordinate3).
   *
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of apriori sigmas.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::aprioriSigmas() {
    return m_aprioriSigmas;
    // Be careful about units.  Latitude and longitude sigmas are in radians.  Radius and X/Y/Z are
    // meters.

  }


  /**
   * Accesses the 3 dimensional ordered vector of adjusted sigmas (adjusted
   * coordinate1, adjusted coordinate2, adjusted coordinate3).
   *
   * @return @b boost::numeric::ublas::bounded_vector<double,3>& The vector of adjusted sigmas.
   */
  boost::numeric::ublas::bounded_vector< double, 3 > &BundleControlPoint::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Accesses the 3 dimensional ordered vector of weight values associated
   * with coordinate1, coordinate2, and coordinate3.
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
   * residual RMS, adjusted coordinate1, adjusted coordinate 2, and adjusted
   * coordinate 3 followed by the adjusted sigmas.  In the case of Latitudinal
   * coordinates, the coordinates will be adjusted latitude and longitude (in degrees),
   * the adjusted radius (in km), and the adjusted sigmas for latitude, longitude and
   * radius all in meters.  In the case of Rectangular coordinates, the coordinates
   * will be X, Y, Z (in km), and the adjusted sigmas for X, Y, and Z all in meters.
   *
   * @param errorPropagation Indicates whether error propagation was selected.
   *
   * @return @b QString The formatted output summary string.
   */
  QString BundleControlPoint::formatBundleOutputSummaryString(bool errorPropagation) const {

    int numRays        = numberOfMeasures(); // should this depend on the raw point, as written, or this->size()???
    int numGoodRays    = numRays - numberOfRejectedMeasures();
    double ResidualRms = residualRms();

    // Return generic set of control point coordinates as floating point values
    // Be careful because of the potential to confuse units.
    SurfacePoint::CoordUnits units = SurfacePoint::Degrees;
    if (m_coordTypeReports == SurfacePoint::Rectangular) units = SurfacePoint::Kilometers;
    double cpc1         = m_controlPoint->GetAdjustedSurfacePoint().GetCoord(m_coordTypeReports,
                                                  SurfacePoint::One, units);
    double cpc2         = m_controlPoint->GetAdjustedSurfacePoint().GetCoord(m_coordTypeReports,
                                                  SurfacePoint::Two, units);
    double cpc3         = m_controlPoint->GetAdjustedSurfacePoint().GetCoord(m_coordTypeReports,
                                                  SurfacePoint::Three, SurfacePoint::Kilometers);

    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();

    QString output = QString("%1%2%3 of %4%5%6%7%8%9%10%11\n")
                   .arg(id(), 16)
                   .arg(pointType, 15)
                   .arg(numGoodRays, 5)
                   .arg(numRays)
                   .arg(formatValue(ResidualRms, 6, 2))
                   .arg(formatValue(cpc1, 16, 8)) // deg            km
                   .arg(formatValue(cpc2, 16, 8)) // deg   OR    km
                   .arg(formatValue(cpc3, 16, 8)) // km            km
                   .arg(formatCoordAdjustedSigmaString(SurfacePoint::One, 16, 8, errorPropagation))  // m
                   .arg(formatCoordAdjustedSigmaString(SurfacePoint::Two, 16, 8, errorPropagation)) // m
                   .arg(formatCoordAdjustedSigmaString(SurfacePoint::Three, 16, 8, errorPropagation));   // m

    return output;
  }


  /**
   * Formats a detailed output string table for this BundleControlPoint.
   *
   * @param errorPropagation Indicates whether error propagation was selected.
   * @param RTM Conversion factor from radians to meters. Used to convert the
   *            latitude and longitude corrections to meters.
   * @param solveRadius A flag indicating to solve for each points individual radius.
   *            When solveRadius is false, the point radius is heavily weighted
   *
   * @return @b QString The formatted output detailed string.
   */
  QString BundleControlPoint::formatBundleOutputDetailString(bool errorPropagation,
                                                             bool solveRadius) const {
    QString output;

    switch (m_coordTypeReports) {
      case SurfacePoint::Latitudinal:
        output = formatBundleLatitudinalOutputDetailString(errorPropagation, solveRadius);
        break;
      case SurfacePoint::Rectangular:
        output = formatBundleRectangularOutputDetailString(errorPropagation);
        break;
      default:
         IString msg ="Unknown surface point coordinate type enum [" + toString(m_coordTypeBundle) + "]." ;
         throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    return output;
  }


  /**
   * Formats a detailed output string table for this Latitudinal BundleControlPoint.
   *
   * @param errorPropagation Indicates whether error propagation was selected.
   * @param RTM Conversion factor from radians to meters. Used to convert the
   *            latitude and longitude corrections to meters.
   * @param solveRadius A flag indicating to solve for each points individual radius.
   *            When solveRadius is false, the point radius is heavily weighted
   *
   * @return @b QString The formatted output detailed string.
   *
   * @internal
   *  @history 2017-08-24 Debbie A. Cook - Revised units of cor_rad to be km instead of meters
   *                                                     when outputting latitudinal corrections in degrees.  Fixed
   *                                                     coordinate type for rectPoint to be Displacement::Kilometers
   *                                                     instead of Distance::Kilometers.  Corrected conversions
   *                                                     of corrections in lat/longitudinal degrees to rectangular meters.
   *                                                     Fixed output of radius corrections in km.  It was outputting
   *                                                     Z corrections.
   */
  QString BundleControlPoint::formatBundleLatitudinalOutputDetailString(
                                                             bool errorPropagation,
                                                             bool solveRadius) const {

    int numRays     = numberOfMeasures();
    int numGoodRays = numRays - numberOfRejectedMeasures();
    double lat      = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
    double lon      = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
    double rad      = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();

    // Use the local radius in meters, rad*1000., to convert radians to meters now instead of the
    // target body equatorial radius DAC 09/17/2018
    double rtm = rad * 1000.;

    // Coordinate corrections are currently set in BundleAdjust::applyParameterCorrections in the
    //  coordTypeBundle coordinates (radians for Latitudinal coordinates and km for Rectangular).
    // point corrections and initial sigmas
    double cor_lat_dd = 0.;                // lat correction, decimal degs
    double cor_lon_dd = 0.;               // lon correction, decimal degs
    double cor_rad_km  = 0.;             // radius correction, kilometers
    double cor_lat_m = 0.;                 // lat correction, meters
    double cor_lon_m = 0.;                // lon correction, meters
    double cor_rad_m  = 0.;              // radius correction, meters
    double latInit = Isis::Null;
    double lonInit = Isis::Null;
    double radInit = Isis::Null;

    if (m_coordTypeBundle == SurfacePoint::Rectangular) {
      double x      = m_controlPoint->GetAdjustedSurfacePoint().GetX().kilometers();
      double xCor = m_corrections(0);  // km
      double y      = m_controlPoint->GetAdjustedSurfacePoint().GetY().kilometers();
      double yCor = m_corrections(1);  // km
      double z      = m_controlPoint->GetAdjustedSurfacePoint().GetZ().kilometers();
      double zCor = m_corrections(2);  // km

      if (!IsSpecial(x) && !IsSpecial(y) && !IsSpecial(z)) {
        SurfacePoint rectPoint(Displacement(x - xCor, Displacement::Kilometers),
                              Displacement(y - yCor, Displacement::Kilometers),
                              Displacement(z - zCor, Displacement::Kilometers));
        latInit = rectPoint.GetLatitude().degrees();
        lonInit = rectPoint.GetLongitude().degrees();
        radInit = rectPoint.GetLocalRadius().kilometers();
        if (!IsSpecial(lat)) {
          cor_lat_dd = (lat - latInit); // degrees
          cor_lat_m  =  cor_lat_dd * DEG2RAD * rtm;
        }
        if (!IsSpecial(lon)) {
          cor_lon_dd = (lon - lonInit); // degrees
          cor_lon_m  =  cor_lon_dd * DEG2RAD * rtm * cos(lat*DEG2RAD);  // lon corrections meters
        }
        if (!IsSpecial(rad)) {
          cor_rad_km  =  rad - radInit;
          cor_rad_m  =  cor_rad_km * 1000.;
        }
      }
    }
    else if (m_coordTypeBundle == SurfacePoint::Latitudinal) {
      cor_lat_dd = m_corrections(0) * RAD2DEG;                // lat correction, decimal degs
      cor_lon_dd = m_corrections(1) * RAD2DEG;               // lon correction, decimal degs
      cor_rad_m  = m_corrections(2) * 1000.0;                   // radius correction, meters

      cor_lat_m = m_controlPoint->GetAdjustedSurfacePoint().LatitudeToMeters(m_corrections(0));
      cor_lon_m = m_controlPoint->GetAdjustedSurfacePoint().LongitudeToMeters(m_corrections(1));
      cor_rad_km = m_corrections(2);

      if (!IsSpecial(lat)) {
        latInit = lat - cor_lat_dd;
      }

      if (!IsSpecial(lon)) {
        lonInit = lon - cor_lon_dd;
      }

      if (!IsSpecial(rad)) {
        radInit = rad - m_corrections(2); // km
      }
    }
    else {
      IString msg ="Unknown surface point coordinate type enum [" +
        toString(m_coordTypeBundle) + "]." ;
      throw IException(IException::Programmer, msg, _FILEINFO_);
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

    SurfacePoint::CoordIndex idx = SurfacePoint::One;
    output += QString("  LATITUDE%1%2%3%4%5%6\n")
        .arg(formatValue(latInit, 17, 8))                                         // deg
        .arg(formatValue(cor_lat_dd, 21, 8))                                 // deg
        .arg(formatValue(cor_lat_m, 20, 8))                                  // m
        .arg(formatValue(lat, 20, 8))                                              // deg
        .arg(formatCoordAprioriSigmaString(idx, 18,8, true))        // m
        .arg(formatCoordAdjustedSigmaString(idx, 18,8,errorPropagation)); // m

    idx = SurfacePoint::Two;
    output += QString(" LONGITUDE%1%2%3%4%5%6\n")
        .arg(formatValue(lonInit, 17, 8))                                         // deg
        .arg(formatValue(cor_lon_dd, 21, 8))                                 // deg
        .arg(formatValue(cor_lon_m, 20, 8))                                  // m
        .arg(formatValue(lon, 20, 8))                                              // deg
        .arg(formatCoordAprioriSigmaString(idx, 18,8, true))        // m
        .arg(formatCoordAdjustedSigmaString(idx, 18,8,errorPropagation)); // m

    idx = SurfacePoint::Three;
    output += QString("    RADIUS%1%2%3%4%5%6\n\n")
         .arg(formatValue(radInit, 17, 8))                             // km
         .arg(formatValue(cor_rad_km, 21, 8))                    // km
         .arg(formatValue(cor_rad_m, 20, 8))                      // m
         .arg(formatValue(rad, 20, 8))                                  // km
         .arg(formatCoordAprioriSigmaString(idx, 18,8, solveRadius))               // m
         .arg(formatCoordAdjustedSigmaString(idx, 18,8, errorPropagation));  // m

    return output;
  }


  /**
   * Formats a detailed output string table for this Rectangular BundleControlPoint.
   *
   * @param errorPropagation Indicates whether error propagation was selected.
   *
   * @return @b QString The formatted output detailed string.
   *
   * @internal
   *  @history 2017-08-24 Debbie A. Cook - Corrected units reported in comments to correctly report
   *                                                       km and also in label line.
   */
 QString BundleControlPoint::formatBundleRectangularOutputDetailString
                                                        (bool errorPropagation) const {
    int numRays     = numberOfMeasures();
    int numGoodRays = numRays - numberOfRejectedMeasures();
    double X         = m_controlPoint->GetAdjustedSurfacePoint().GetX().kilometers();
    double Y         = m_controlPoint->GetAdjustedSurfacePoint().GetY().kilometers();
    double Z         = m_controlPoint->GetAdjustedSurfacePoint().GetZ().kilometers();

    // Coordinate corrections are currently set in applyParameterCorrections.  Units depend on
    //  coordTypeBundle coordinates (radians for Latitudinal coordinates and km for Rectangular).
    // point corrections and initial sigmas.....
    double cor_X_m = 0.;                     // X correction, meters
    double cor_Y_m = 0.;                    // Y correction, meters
    double cor_Z_m = 0.;                    // Z correction, meters
    double XInit = Isis::Null;
    double YInit = Isis::Null;
    double ZInit = Isis::Null;

    if (m_coordTypeBundle == SurfacePoint::Latitudinal) {
      double lat      = m_controlPoint->GetAdjustedSurfacePoint().GetLatitude().degrees();
      double latcor = m_corrections(0) * RAD2DEG;
      double lon      = m_controlPoint->GetAdjustedSurfacePoint().GetLongitude().degrees();
      double loncor = m_corrections(1) * RAD2DEG;
      double rad      = m_controlPoint->GetAdjustedSurfacePoint().GetLocalRadius().kilometers();
      double radcor = m_corrections(2);

      if (!IsSpecial(lat) && !IsSpecial(lon) && !IsSpecial(rad)) {
        SurfacePoint latPoint(Latitude(lat-latcor, Angle::Degrees),
                              Longitude(lon-loncor, Angle::Degrees),
                              Distance(rad-radcor, Distance::Kilometers));
        XInit = latPoint.GetX().kilometers();
        YInit = latPoint.GetY().kilometers();
        ZInit = latPoint.GetZ().kilometers();
        cor_X_m = Isis::Null;
        if (!IsSpecial(X)) {
          cor_X_m = (X - XInit); // kilometers
        }
        cor_Y_m = Isis::Null;
        if (!IsSpecial(Y)) {
          cor_Y_m = (Y - YInit); // kilometers
        }
        cor_Z_m = Isis::Null;
        if (!IsSpecial(Z)) {
          cor_Z_m = (Z - ZInit); // kilometers
        }
      }
    }
    else if  (m_coordTypeBundle == SurfacePoint::Rectangular) {
      cor_X_m = m_corrections(0);                     // X correction, kilometers
      cor_Y_m = m_corrections(1);                     // Y correction, kilometers
      cor_Z_m = m_corrections(2);                    // Z correction, kilometers
      if (!IsSpecial(X)) {
        XInit = X - m_corrections(0); // km
      }
      if (!IsSpecial(Y)) {
        YInit = Y - m_corrections(1); // km
      }
      if (!IsSpecial(Z)) {
        ZInit = Z - m_corrections(2); // km
      }
    }
    else {
      IString msg ="Unknown surface point coordinate type enum [" +
        toString(m_coordTypeBundle) + "]." ;
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();

    QString output;

    output = QString(" Label: %1\nStatus: %2\n  Rays: %3 of %4\n")
                        .arg(id())
                        .arg(pointType)
                        .arg(numGoodRays)
                        .arg(numRays);

    QString labels =
                       "\n        Point         Initial              Total              Final        "
                       "     Initial              Final\n"
                       "   Coordinate         Value             Correction            Value         "
                       "    Accuracy          Accuracy\n"
                       "                    (km/km/km)             (km)           (km/km/km)         "
                       " (Meters)          (Meters)\n";
    output += labels;

    SurfacePoint::CoordIndex idx = SurfacePoint::One;
    output += QString(" BODY-FIXED-X%1%2%3%4%5\n")
            .arg(formatValue(XInit, 17, 8))                                       // km
            .arg(formatValue(cor_X_m, 20, 8))                                // km
            .arg(formatValue(X, 20, 8))                                           // km
            .arg(formatCoordAprioriSigmaString(idx, 18,8, true))   // m
            .arg(formatCoordAdjustedSigmaString(idx, 18,8,errorPropagation)); // m

    idx = SurfacePoint::Two;
    output += QString(" BODY-FIXED-Y%1%2%3%4%5\n")
            .arg(formatValue(YInit, 17, 8))                                        // km
            .arg(formatValue(cor_Y_m, 20, 8))                                 // km
            .arg(formatValue(Y, 20, 8))                                             // km
            .arg(formatCoordAprioriSigmaString(idx, 18, 8,  true))  // m
            .arg(formatCoordAdjustedSigmaString(idx, 18,8,errorPropagation)); // m

    idx = SurfacePoint::Three;
    output += QString(" BODY-FIXED-Z%1%2%3%4%5\n\n")
            .arg(formatValue(ZInit, 17, 8))                                        // km
            .arg(formatValue(cor_Z_m, 20, 8))                                 // km
            .arg(formatValue(Z, 20, 8))                                             // km
      // Set solveRadius to true to avoid limiting output information for Z.
      // Set radius does not really apply to rectangular coordinates.
            .arg(formatCoordAprioriSigmaString(idx, 18,8, true))            // m
            .arg(formatCoordAdjustedSigmaString(idx, 18,8,errorPropagation)); // m

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
   * @param index CoordIndex Coordinate index One: Latitude or X, Two:  Longitude or Y,
   *                                 or Three: Radius or Z
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param solveRadius A flag indicating to solve for each points individual radius.
   *            When solveRadius is false, the point radius is heavily weighted
   *
   * @return @b QString The formatted value, as a string.
   */
  QString BundleControlPoint::formatAprioriSigmaString(SurfacePoint::CoordIndex index,
                                                       int fieldWidth, int precision,
                                                       bool solveRadius) const {
    QString aprioriSigmaStr;
    QString pointType = ControlPoint::PointTypeToString(type()).toUpper();
    if (pointType == "CONSTRAINED" || !solveRadius) {
        pointType = "N/A";
    }
    double sigma = m_aprioriSigmas[int(index)];
    if (IsSpecial(sigma)) { // if globalAprioriSigma <= 0 (including Isis::Null), then m_aprioriSigmas = Null
      aprioriSigmaStr = QString("%1").arg(pointType, fieldWidth);
    }
    else {
      aprioriSigmaStr = QString("%1").arg(sigma, fieldWidth, 'f', precision);
    }
    return aprioriSigmaStr;
  }


  /**
   * Formats the apriori coordinate 1 (latitude or X) sigma value.
   *
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   *
   * @return @b QString The formatted apriori coordinate 1 sigma value, as a string.
   *
   * @see formatAprioriSigmaString()
   */
QString BundleControlPoint::formatCoordAprioriSigmaString(SurfacePoint::CoordIndex index,
                                                          int fieldWidth,
                                                          int precision,
                                                          bool solveRadius) const {
    return formatAprioriSigmaString(index, fieldWidth, precision, solveRadius);
  }


  /**
   * Formats the adjusted sigma value indicated by the given type code. If error
   * propagation is false or the selected sigma type was set to Null, then only
   * "N/A" will be returned.
   *
   * @param index CoordIndex  Coordinate index One: Latitude or X, Two:  Longitude or Y,
   *                                 or Three: Radius or Z
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   *
   * @return @b QString The formatted value, as a string.
   */
  QString BundleControlPoint::formatAdjustedSigmaString(SurfacePoint::CoordIndex index,
                                                        int fieldWidth, int precision,
                                                        bool errorPropagation) const {
    QString adjustedSigmaStr;

    if (!errorPropagation) {
      adjustedSigmaStr = QString("%1").arg("N/A",fieldWidth);
    }
    else {
      double sigma = Isis::Null;
      sigma = adjustedSurfacePoint().GetSigmaDistance(m_coordTypeReports, index).meters();

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
   * Formats the adjusted coordinate sigma value.
   *
   * @param index The coordinate index of the sigma to return.
   * @param fieldWidth The return string's field width.
   * @param precision The precision of the double to be saved off.
   * @param errorPropagation Indicates whether error propagation was selected.
   *
   * @return @b QString The formatted adjusted coordinate sigma value, as a string.
   *
   * @see formatAdjustedSigmaString()
   */
QString BundleControlPoint::formatCoordAdjustedSigmaString(SurfacePoint::CoordIndex index,
                                                                int fieldWidth, int precision,
                                                                bool errorPropagation) const {
    return formatAdjustedSigmaString(index, fieldWidth, precision, errorPropagation);
  }


  /**
   * Apply the parameter corrections to the bundle control point latitudinally.
   *
   * @param factor The unit conversion factor to use on lat & lon.  Radius is in km & converted to m
   * @param target The BundleTargetBody.
   */
  void BundleControlPoint::updateAdjustedSurfacePointLatitudinally
               (const BundleTargetBodyQsp target) {
      SurfacePoint surfacepoint = adjustedSurfacePoint();
      double pointLat = surfacepoint.GetLatitude().degrees();
      double pointLon = surfacepoint.GetLongitude().degrees();
      double pointRad = surfacepoint.GetLocalRadius().meters();

      // get point parameter corrections
      double latCorrection = m_nicVector[0];
      double lonCorrection = m_nicVector[1];
      double radCorrection = m_nicVector[2];

      // convert to degrees and apply
      pointLat += RAD2DEG * latCorrection;
      pointLon += RAD2DEG * lonCorrection;

      // Make sure updated values are still in valid range(0 to 360 for lon and -90 to 90 for lat)
      if (pointLat < -90.0) {
        pointLat = -180.0 - pointLat;
        pointLon = pointLon + 180.0;
      }
      if (pointLat > 90.0) {
        pointLat = 180.0 - pointLat;
        pointLon = pointLon + 180.0;
      }
      while (pointLon > 360.0) {
        pointLon = pointLon - 360.0;
      }
      while (pointLon < 0.0) {
        pointLon = pointLon + 360.0;
      }

      // convert to meters and apply
      pointRad += 1000. * radCorrection;

      // ken testing - if solving for target body mean radius, set radius to current
      // mean radius value
      // Only allow radius options for Latitudinal coordinates
      if (target && (target->solveMeanRadius() || target->solveTriaxialRadii()) ) {
        if (target->solveMeanRadius()) {
          surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                               Longitude(pointLon, Angle::Degrees),
                                               target->meanRadius());
        }
        else if (target->solveTriaxialRadii()) {
            Distance localRadius = target->localRadius(Latitude(pointLat, Angle::Degrees),
                                                   Longitude(pointLon, Angle::Degrees));
            surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                                 Longitude(pointLon, Angle::Degrees),
                                                 localRadius);
        }
      }
      else {
        surfacepoint.SetSphericalCoordinates(Latitude(pointLat, Angle::Degrees),
                                             Longitude(pointLon, Angle::Degrees),
                                             Distance(pointRad, Distance::Meters));
      }
      // Reset the point now that it has been updated
      setAdjustedSurfacePoint(surfacepoint);
  }


  /**
   * Apply the parameter corrections to the bundle control point rectangularly.
   *
   * @param factor The unit conversion factor to use on the coordinates.
   */
  void BundleControlPoint::updateAdjustedSurfacePointRectangularly() {
      SurfacePoint surfacepoint = adjustedSurfacePoint();
      double pointX = surfacepoint.GetX().meters();
      double pointY = surfacepoint.GetY().meters();
      double pointZ = surfacepoint.GetZ().meters();

      // get point parameter corrections
      double XCorrection = m_nicVector[0];
      double YCorrection = m_nicVector[1];
      double ZCorrection = m_nicVector[2];

      // Convert corrections to meters and apply
      pointX += 1000. * XCorrection;
      pointY += 1000. * YCorrection;
      pointZ += 1000. * ZCorrection;

      surfacepoint.SetRectangularCoordinates(Displacement(pointX, Displacement::Meters),
                                             Displacement(pointY, Displacement::Meters),
                                             Displacement(pointZ, Displacement::Meters));
      // Reset the point now that it has been updated
      setAdjustedSurfacePoint(surfacepoint);
  }


  /**
   * Compute vtpv of image measures (weighted sum of squares of measure residuals).
   *
   * @return double weighted sum of squares of measure residuals (vtpv).
   */
  double BundleControlPoint::vtpvMeasures() {

    double vtpv = 0.0;
    double weight = 0.0;
    double vx,vy;

    for (int i = 0; i < size(); i++) {
      BundleMeasureQsp measure = at(i);
      if (measure->isRejected()) {
        continue;
      }

      weight = measure->weight();
      vx = measure->xFocalPlaneResidual();
      vy = measure->yFocalPlaneResidual();

      vtpv += vx * vx * weight + vy * vy * weight;
    }

    return vtpv;
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained point residuals.
   *
   * @return double Weighted sum of squares of constrained point residuals.
   */
  double BundleControlPoint::vtpv() {
    double vtpv = 0.0;

    if ( m_weights(0) > 0.0 ) {
        vtpv += m_corrections(0) * m_corrections(0) * m_weights(0);
    }
    if ( m_weights(1) > 0.0 ) {
        vtpv += m_corrections(1) * m_corrections(1) * m_weights(1);
    }
    if ( m_weights(2) > 0.0 ) {
        vtpv += m_corrections(2) * m_corrections(2) * m_weights(2);
    }

    return vtpv;
  }
}
