#include "BundleLidarControlPoint.h"

// qt lib
#include <QDebug>

// std lib
#include <vector>

// boost lib
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

// Isis lib
#include "BundleLidarControlPoint.h"
#include "BundleLidarRangeConstraint.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "SpecialPixel.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Constructs a BundleLidarControlPoint object from a LidarControlPoint. Only the
   * non-ignored measures are added to the BundleLidarControlPoint.
   *
   * @param bundleSettings BundleSettings.
   *
   * @param controlPoint Pointer to a ControlPoint that will be used to
   *                     construct this BundleLidarControlPoint.
   *
   * TODO: is the typecast expensive?
   */
  BundleLidarControlPoint::BundleLidarControlPoint(const BundleSettingsQsp bundleSettings,
                                                   LidarControlPointQsp lidarControlPoint)
    : BundleControlPoint(bundleSettings, (ControlPoint*)  lidarControlPoint.data()) {

    m_lidarControlPoint = lidarControlPoint;
  }


  /**
   * Initialize range constraints.
   *
   */
  void BundleLidarControlPoint::initializeRangeConstraints() {

    QStringList simultaneousSerialNumbers = m_lidarControlPoint->snSimultaneous();
    for (int i= 0; i < size(); i++) {
      BundleMeasureQsp measure = at(i);
      if (!simultaneousSerialNumbers.contains(measure->cubeSerialNumber())) {
          continue;
      }

      m_rangeConstraints.append(BundleLidarRangeConstraintQsp
                                (new BundleLidarRangeConstraint(m_lidarControlPoint, measure)));
    }
  }


  /**
   * TODO: implement?
   * Copy constructor.
   *
   * @param src The BundleLidarControlPoint to be copied.
   */
//  BundleLidarControlPoint::BundleLidarControlPoint(const BundleLidarControlPoint &src) {
//    : BundleControlPoint(((ControlPoint) src) {
//    copy(src);
//  }


  /**
   * Destructor.
   */
  BundleLidarControlPoint::~BundleLidarControlPoint() {
  }


  /**
   * Copies given BundleLidarControlPoint to this BundleLidarControlPoint.
   *
   * @param src The BundleLidarControlPoint to be copied.
   */
  void BundleLidarControlPoint::copy(const BundleLidarControlPoint &src) {

    BundleControlPoint::copy((BundleControlPoint) src);
  }


  /**
   * Computes the residuals for this BundleLidarControlPoint.
   *
   */
  void BundleLidarControlPoint::computeResiduals() {
    m_lidarControlPoint->ComputeResiduals();

    // compute and store focal plane residuals in millimeters
    for (int i = 0; i < size(); i++) {
      at(i)->setFocalPlaneResidualsMillimeters();
    }
  }


  /**
   * Apply the parameter corrections to the lidar range.
   */
  void BundleLidarControlPoint::applyParameterCorrections(
        LinearAlgebra::Vector imageSolution,
        SparseBlockMatrix &sparseNormals,
        const BundleTargetBodyQsp target) {
    // Call parent class method to apply point corrections
    BundleControlPoint::applyParameterCorrections(imageSolution, sparseNormals, target);

    for (int i = 0; i < m_rangeConstraints.size(); i++) {
      m_lidarControlPoint->setRange(m_rangeConstraints[i]->rangeComputed());
      // The updated range sigma cannot be computed because the bundle does not
      // solve for the range, it simply re-computes it each iteration
      // m_lidarControlPoint->setRangeSigma(m_rangeConstraints[i]->rangeAdjustedSigma());
    }
  }


  /**
   * Applies range constraint between image and lidar point acquired simultaneously.
   *
   * @param normalsMatrix Bundle Adjustment normal equations matrix.
   * @param N22 Normal equation matrix for the point.
   * @param N12 Normal equations block between image and point.
   * @param n1 Right hand side vector for the images and target body.
   * @param n2 Right hand side vector for the point.
   *
   * @return int Number of constraints successfully applied.
   */
  int BundleLidarControlPoint::applyLidarRangeConstraints(SparseBlockMatrix &normalsMatrix,
                                                         LinearAlgebra::MatrixUpperTriangular& N22,
                                                         SparseBlockColumnMatrix& N12,
                                                         LinearAlgebra::VectorCompressed& n1,
                                                         LinearAlgebra::Vector& n2) {
    int constraintsApplied = 0;

    // loop over range constraints
    for (int i = 0; i < m_rangeConstraints.size(); i++) {
      if (m_rangeConstraints.at(i)->applyConstraint(normalsMatrix, N22, N12, n1, n2)) {
        constraintsApplied++;
      }
    }

    return constraintsApplied;
  }


  /**
   * Returns Weighted sum of squares of range residuals for this point.
   *
   * @return double Weighted sum of squares of range residuals for this point.
   *
   */
  double BundleLidarControlPoint::vtpvRangeContribution() {
    double vtpv = 0.0;
    for (int i = 0; i < m_rangeConstraints.size(); i++) {
      vtpv += m_rangeConstraints.at(i)->vtpv();
    }

    return vtpv;
  }


  /**
   * Returns number of range constraints between this lidar point & images acquired simultaneously.
   *
   * @return int Number of range constraints
   *
   */
  int BundleLidarControlPoint::numberRangeConstraints() {
    return m_rangeConstraints.size();
  }


  /**
   * Returns range constraint at index n.
   *
   * @param int Index of range constraint to return.
   *
   * @return BundleLidarRangeConstraintQsp QSharedPointer to range constraint at index n.
   *
   */
  BundleLidarRangeConstraintQsp BundleLidarControlPoint::rangeConstraint(int n) {
    return m_rangeConstraints.at(n);
  }


  /**
   * Returns range between this point and ???
   *
   * @return double range.
   *
   */
  double BundleLidarControlPoint::range() {
    return m_lidarControlPoint->range();
  }


  /**
   * Returns sigma of observed range.
   *
   * @return double Sigma of observed range.
   *
   */
  double BundleLidarControlPoint::sigmaRange() {
    return m_lidarControlPoint->sigmaRange();
  }
}
