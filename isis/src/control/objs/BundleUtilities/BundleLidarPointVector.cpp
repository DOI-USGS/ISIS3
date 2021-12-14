#include "BundleLidarPointVector.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#include "BundleTargetBody.h"
#include "IException.h"

namespace Isis {

  /**
   * Constructs an empty BundleLidarPointVector.
   */
  BundleLidarPointVector::BundleLidarPointVector() {
  }


  /**
   * Copy constructor.
   * 
   * @param src A reference to the BundleLidarPointVector to copy from.
   */
  BundleLidarPointVector::BundleLidarPointVector(const BundleLidarPointVector &src)
      :QVector<BundleLidarControlPointQsp>(src) {
  }


  /**
   * Destructor.
   *
   * Contained BundleControlPoints will remain until all shared pointers to them are deleted.
   */
  BundleLidarPointVector::~BundleLidarPointVector() {
    clear();
  }


  /**
   * Assignment operator.
   * 
   * Assigns the state of the source BundleLidarPointVector to this BundleLidarPointVector.
   * 
   * @param src The BundleLidarPointVector to assign from.
   * 
   * @return BundleLidarPointVector& A reference to this BundleLidarPointVector.
   */
  BundleLidarPointVector &BundleLidarPointVector::operator=(const BundleLidarPointVector &src) {
    if (&src != this) {
      QVector<BundleLidarControlPointQsp>::operator=(src);
    }
    return *this;
  }


  /**
   * Apply point parameter corrections.
   *
   * @param normalsMatrix Normal equations matrix.
   * @param imageSolution Current iteration solution vector for image parameters.
   *
   */
  void BundleLidarPointVector::applyParameterCorrections(SparseBlockMatrix &normalsMatrix,
                                                         LinearAlgebra::Vector &imageSolution,
                                                         const BundleTargetBodyQsp target) {
    for (int i = 0; i < size(); i++) {
      at(i)->applyParameterCorrections(imageSolution, normalsMatrix, target);
    }
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained point residuals.
   *
   * @return double Weighted sum of squares of constrained point residuals.
   */
  void BundleLidarPointVector::computeMeasureResiduals() {

    for (int i = 0; i < size(); i++) {
      at(i)->computeResiduals();
    }
  }


  /**
   * Compute vtpv of image measures (weighted sum of squares of measure residuals).
   *
   * @return double weighted sum of squares of measure residuals (vtpv).
   */
  double BundleLidarPointVector::vtpvMeasureContribution() {
    double vtpv = 0.0;

    for (int i = 0; i < size(); i++) {
      vtpv += at(i)->vtpvMeasures();
    }

    return vtpv;
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained point residuals.
   *
   * @return double Weighted sum of squares of constrained point residuals.
   */
  double BundleLidarPointVector::vtpvContribution() {
    double vtpvControl = 0.0;

    for (int i = 0; i < size(); i++) {
      vtpvControl += at(i)->vtpv();
    }

    return vtpvControl;
  }


  /**
   * Compute vtpv of lidar range constraints.
   *
   * @return double vtpvRange of lidar range constraints.
   */
  double BundleLidarPointVector::vtpvRangeContribution() {
    double vtpvRange = 0.0;

    for (int i = 0; i < size(); i++) {
        vtpvRange += at(i)->vtpvRangeContribution();
    }

    return vtpvRange;
  }

}
